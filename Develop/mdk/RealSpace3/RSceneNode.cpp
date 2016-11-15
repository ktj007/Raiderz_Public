#include "stdafx.h"
#include "RSceneManager.h"
#include "RDevice.h"
#include "RSceneNodeController.h"
#include "RCameraSceneNode.h"
#include "REffectActor.h"
#include "RDebugPassRenderer.h"

namespace rs3 {

//////////////////////////////////////////////////////////////////////////
//
// Traverse( RSceneNodeUpdateVisitor ) Update 순서 :
//		OnPreUpdate
//		Animation & local TM Update
//		World TM Update
//		OnUpdate
//		Controller(s) Update 
//
//		children update
//
//		OnPostUpdate
//
//////////////////////////////////////////////////////////////////////////
bool RSceneNodeUpdateVisitor::OnPreOrderVisit( RSceneNode* pSceneNode )
{
	if(!pSceneNode->m_bUsableSceneNode)
		return false;

	RPFC_THISFUNC;

	// 이미 이번 프레임에서 업데이트가 되어있다면 다시 실행하지 않는다
	if( pSceneNode->m_nUpdatedFrame == REngine::GetDevice().GetFrameCount() )
		return false;

	pSceneNode->m_nUpdatedFrame = REngine::GetDevice().GetFrameCount();

	pSceneNode->OnPreUpdate();
	pSceneNode->UpdateSceneNodeControllers( RSNCTRL_PREUPDATE );

	RPFC_B("RSceneNode::OnUpdate");
	pSceneNode->OnUpdate();
	RPFC_E;
	pSceneNode->UpdateSceneNodeControllers( RSNCTRL_UPDATE );

	// controller 들에 의해 tm 이 변경될수도 있다.
	RPFC_B("update all hierarchy dependent values");

	pSceneNode->UpdateAllParentDependentValues();

	RPFC_E;

	return true;
}

void RSceneNodeUpdateVisitor::OnPostOrderVisit( RSceneNode* pSceneNode )
{
	RPFC_THISFUNC;
	pSceneNode->OnPostUpdate();
	pSceneNode->UpdateSceneNodeControllers( RSNCTRL_POSTUPDATE );
}

bool RSceneNodeUpdateTransformVisitor::OnPreOrderVisit( RSceneNode* pSceneNode )
{
	if(!pSceneNode->m_bUsableSceneNode)
		return false;

	// TODO: 트랜스폼을 변경하는 컨트롤러만 호출 되게 수정해야 함.
	pSceneNode->UpdateSceneNodeControllers( RSNCTRL_PREUPDATE );
	pSceneNode->UpdateTransform();
	
	return true;
}

//////////////////////////////////////////////////////////////////////////
// RSceneNode
MImplementRootRTTI(RSceneNode);

//기본이 생성하자마자 사용가능 - 옵션으로 Async한 사용
RSceneNode::RSceneNode(RRENDERPASS eRenderPass)
: m_eRenderPass(eRenderPass)
, m_pParent(NULL)
, m_bVisible(true)
, m_bOccluded(false)
, m_bUsableSceneNode(true)
, m_bAttached(false)
, m_bPrimaryCamRenderable(true)
, m_fVisibility(1.f)
, m_fVisibilityResult(1.f)
, m_fNoiseRef(1.f)
, m_fNoiseRefResult(1.f)
, m_scale(1.f,1.f,1.f)
, m_pUserData(NULL)
, m_bCullingNode(true)
, m_nManagingSceneNodeCount(0)
, m_pSceneNodeGroupOwner(NULL)
, m_nUpdatedFrame(0)
, m_bAutoUpdate(false)
, m_bAutoUpdateRegistered(false)
, m_bRegisteredCandidate(false)
, m_dwSceneCount(0)
, m_dwRefreshedTime(0)				// 090716, OZ
, m_bUseForPhysiqueBone(false)		// 피직에 사용되는 본의 용도인지
, m_fSBasecale(0)
, m_pRootScene(NULL)
, m_bNeedUpdateBoundingVolume(true)
{
	ALIGNED16_MALLOC(m_matLocal);
	ALIGNED16_MALLOC(m_matWorld);
	ALIGNED16_MALLOC(m_matWorldPrev);

	m_matWorld->MakeIdentity();
	m_matWorldPrev->MakeIdentity();
	m_matLocal->MakeIdentity();

	m_aabb.Initialize();
}

bool RSceneNode::RSceneNodeChildrenCleanUpVisitor::OnPreOrderVisit(RSceneNode* pSceneNode )
{
	// 요청자가 자기자신이면 검사하지 않는다.
	if (pSceneNode == m_pRequestSceneNode)
		return true;

	// 요청자가 자식을 관리하는 경우라면
	if (m_pRequestSceneNode->m_nManagingSceneNodeCount > 0)
	{
		// 소속 그룹이 없다면, 지워주지 않고 다음자식도 순회하지 않는다.
		if (pSceneNode->m_pSceneNodeGroupOwner == NULL)
		{
			m_vecNodeToChangeParent.push_back(pSceneNode);
			return false;
		}

		// 그룹에 존재하면, 이 씬노드가 지워질때 같이 지워준다.
		return true;
	}

	// 자식을 관리하지 않는경우, 지우지 않을것이므로
	// 소속에 변화도 주지 않고 그 하위노드도 검색할 필요없다.
	_ASSERT( m_pRequestSceneNode->m_nManagingSceneNodeCount == 0 );
	m_vecNodeToChangeParent.push_back(pSceneNode);
	return false;
}

void RSceneNode::Clear()
{
	if (m_children.empty() == false)
	{
		RSceneNodeChildrenCleanUpVisitor visitor;
		visitor.m_pRequestSceneNode = this;
		Traverse(&visitor);

		for(std::vector< RSceneNode* >::iterator itr = visitor.m_vecNodeToChangeParent.begin();
			itr != visitor.m_vecNodeToChangeParent.end(); ++itr)
		{
			RSceneNode* pSceneNode = *itr;

			// 부모로부터 제거
			pSceneNode->RemoveFromParent();

			// 상위부모가 있다면 상위로 붙여주기
			if (m_pParent)
				m_pParent->AddChild(pSceneNode);
		}
	}

	#ifdef _DEBUG
	if (m_nManagingSceneNodeCount == 0)
	{
		//관리되지 않는 씬노드라면, CleanUp시에 다 떨어져나가서 자식이 없어야 한다.
		_ASSERT(m_children.empty() == true);
	}
	#endif

	// 자식노드들 제거. leaf 부터 제거하므로 RSceneNodeChildrenCleanUpVisitor을 두번 수행하지는 않는다.
	DeleteManagingChildren();

	// 부모로부터 떨어져나오기
	RemoveFromParent();

	// controller 들을 제거
	for( int i = 0; i < RSNCTRL_MAX; ++i )
	{
		for( RSCENENODECONTROLLERLIST::iterator it = m_sceneNodeControllers[i].begin(); it != m_sceneNodeControllers[i].end(); )
		{
			RSceneNodeController* pController = *it;
			pController->m_pSceneNode = NULL;
			if( pController->QueryAttribute(RCA_MANAGED) )	// 관리되는 콘트롤러는 삭제
			{
				pController->OnRemove();
				delete pController;
				it = m_sceneNodeControllers[i].erase(it);
			}else
				++it;
		}
	}

	_ASSERT( m_children.empty() == true );

	ALIGNED16_FREE(m_matLocal);
	ALIGNED16_FREE(m_matWorld);
	ALIGNED16_FREE(m_matWorldPrev);
}

RSceneNode::~RSceneNode()
{
	//////////////////////////////////////////////////////////////////////////
 	// 지우기 전에 신에서 제거해주어야 합니다

	// 그렇지 않으면, 하위클래스의 소멸자를 거쳐 온 상태이기 때문에 
	// 씬그래프에서 빠질때 하위 클래스의 OnRemove 가 제대로 호출되지 않아 위험
 	_ASSERT(m_pParent==NULL);

	// 업데이트 등록을 했다면 unregister 도 해주셔야 합니다. 뻗습니다.
	_ASSERT(!m_bAutoUpdateRegistered);

	Clear();
}

void RSceneNode::DeleteManagingChildren()
{
	while(m_children.empty() == false)
	{
		RSceneNode* pChild = m_children.back();
		pChild->DeleteManagingChildren();

		// leaf
		_ASSERT( pChild->m_children.empty() == true );

		RemoveChild(pChild);
		delete pChild;

		// next
		//pNode->m_children.pop_back();
	}
}

bool RSceneNode::AddChild(RSceneNode *pChild)
{
	class RSceneNodeAddVisitor : public RSceneNodeVisitor
	{
		RSceneManager* m_pManager;
	public:
		RSceneNodeAddVisitor(RSceneManager* pManager) : m_pManager(pManager) { }

		virtual bool			OnPreOrderVisit(RSceneNode* pSceneNode )
		{
			_ASSERT(NULL != pSceneNode->GetParent());
			_ASSERT(NULL != pSceneNode->GetParent()->GetRootSceneNode());
	
			RWorldSceneNode *pRootSceneNode = pSceneNode->GetParent()->GetRootSceneNode();
			pSceneNode->SetRootSceneNode(pRootSceneNode);
			pRootSceneNode->RegisterRenderCandidate( pSceneNode);

			if(pSceneNode->m_bAutoUpdate)
				pSceneNode->RegisterAutoUpdate();

			pSceneNode->OnAddScene();
			pSceneNode->m_bAttached = true;
			return true;
		}
		virtual void			OnPostOrderVisit( RSceneNode* pSceneNode ){}
	};

	assert(pChild!=NULL);
	assert(pChild!=this);

	m_children.push_back(pChild);
	pChild->SetParent(this);

	pChild->OnAdd();
	pChild->NeedUpdateTransform();
	pChild->NeedUpdateBoundingVolume();

	if(m_bAttached)
	{
		RSceneNodeAddVisitor addVisitor(REngine::GetSceneManagerPtr());
		pChild->Traverse(&addVisitor);
	}

	return true;
}

bool RSceneNode::RemoveChild(RSceneNode *pChild)
{
	class RSceneNodeRemoveVisitor : public RSceneNodeVisitor
	{
		RSceneManager* m_pManager;
	public:
		RSceneNodeRemoveVisitor(RSceneManager* pManager) : m_pManager(pManager) { }

		virtual bool			OnPreOrderVisit(RSceneNode* pSceneNode )	{	return true;	}
		virtual void			OnPostOrderVisit(RSceneNode* pSceneNode )
		{
			_ASSERT(NULL != pSceneNode->GetRootSceneNode());

			pSceneNode->GetRootSceneNode()->UnregisterRenderCandidate(pSceneNode);
			if(pSceneNode->m_bAutoUpdate)
				pSceneNode->UnregisterAutoUpdate();
			pSceneNode->OnRemoveScene();
			pSceneNode->m_bAttached = false;
		}
	};

	for ( RSceneNodeList::reverse_iterator ri = m_children.rbegin(); ri != m_children.rend(); ++ri) {
		RSceneNode* pNode = *ri;
		if(pNode==pChild)
		{
			if(m_bAttached)
			{
				_ASSERT(true == pChild->IsAttached() );
				RSceneNodeRemoveVisitor removeVisitor(REngine::GetSceneManagerPtr());
				pChild->Traverse(&removeVisitor);
			}

			_ASSERT(pChild->m_bAutoUpdateRegistered==false);
			pChild->OnRemove();

			m_children.erase( (++ri).base() );
			pChild->m_pParent = NULL;
			if (pChild->m_pSceneNodeGroupOwner == this)
			{
				pChild->m_pSceneNodeGroupOwner = NULL;
				--m_nManagingSceneNodeCount;
			}
			_ASSERT(pChild->m_pSceneNodeGroupOwner == NULL);
			return true;
		}
	}
	return false;
}

bool RSceneNode::RemoveFromParent()
{
	if(!m_pParent)
		return false;

	m_pParent->RemoveChild(this);
	return true;
}

/*
void RSceneNode::UpdateRotation()	// dir,up,right 로 부터 quaternion rotation 을 업데이트 ( 일관성 유지를 위해 )
{
	RMatrix mat;
	mat(0,0) = m_right.x;
	mat(1,0) = m_right.y;
	mat(2,0) = m_right.z;
	mat(3,0) = 0;

	mat(0,1) = m_direction.x;
	mat(1,1) = m_direction.y;
	mat(2,1) = m_direction.z;
	mat(3,1) = 0;

	mat(0,2) = m_up.x;
	mat(1,2) = m_up.y;
	mat(2,2) = m_up.z;
	mat(3,2) = 0;

	mat(0,3) = 0;
	mat(1,3) = 0;
	mat(2,3) = 0;
	mat(3,3) = 1;

//	m_rotation = mat;
}
*/

void RSceneNode::SetTransform(const RMatrix& tm)
{ 
	(*m_matLocal) = tm;

	m_scale.x = RVector(tm._11,tm._12,tm._13).Length();
	m_scale.y = RVector(tm._21,tm._22,tm._23).Length();
	m_scale.z = RVector(tm._31,tm._32,tm._33).Length();

	// normalize
	float invx = 1.f/m_scale.x;
	m_matLocal->_11*=invx;m_matLocal->_12*=invx;m_matLocal->_13*=invx;
	float invy = 1.f/m_scale.y;
	m_matLocal->_21*=invy;m_matLocal->_22*=invy;m_matLocal->_23*=invy;
	float invz = 1.f/m_scale.z;
	m_matLocal->_31*=invz;m_matLocal->_32*=invz;m_matLocal->_33*=invz;

	NeedUpdateTransform();
}

void RSceneNode::SetTransform(const RVector& pos, const RVector& dir, const RVector& up)
{
	m_matLocal->SetLocalMatrix(pos,dir,up);

	NeedUpdateTransform();
}

void RSceneNode::SetDirection(const RVector& dir, const RVector& up)
{
	RVector _dir = Normalize(dir);
	RVector _up = Normalize(up);
	RVector _right = Normalize(_dir.CrossProduct(_up));
	_up = CrossProduct( _right, _dir);

	m_matLocal->_11 = _right.x;	m_matLocal->_12 = _right.y;	m_matLocal->_13 = _right.z;
	m_matLocal->_21 = _dir.x;	m_matLocal->_22 = _dir.y;	m_matLocal->_23 = _dir.z;
	m_matLocal->_31 = _up.x;	m_matLocal->_32 = _up.y;	m_matLocal->_33 = _up.z;

	NeedUpdateTransform();
}

void RSceneNode::SetRotation(const RQuaternion& quat)
{
//	RMatrix mat = quat.Conv2Matrix();
// 	m_matLocal._11 = mat._11;m_matLocal._12 = mat._12;m_matLocal._13 = mat._13;
// 	m_matLocal._21 = mat._21;m_matLocal._22 = mat._22;m_matLocal._23 = mat._23;
// 	m_matLocal._31 = mat._31;m_matLocal._32 = mat._32;m_matLocal._33 = mat._33;

#ifdef USING_RSC_PHYSIQUE_QT
	if( IsUseForPhysiqueBone())
		m_qRotLocal = quat;
#endif

	// org...
	quat.SetMatrixRotation((*m_matLocal));

	NeedUpdateTransform();
}

////////////////////////////////////////////////////////////////

void RSceneNode::AddController(RSceneNodeController* pController)
{
	_ASSERT( pController->GetType() < RSNCTRL_MAX );
	RSCENENODECONTROLLERLIST& controllerList = m_sceneNodeControllers[pController->GetType()];
	if( find( controllerList.begin(), controllerList.end(), pController ) != controllerList.end() )
		return;

	controllerList.push_back(pController);
	pController->m_pSceneNode = this;
	pController->m_bEnable = true;
	pController->OnAdd();
}

bool RSceneNode::RemoveController( RSceneNodeController* pController )
{
	_ASSERT( pController->GetType() < RSNCTRL_MAX );
	RSCENENODECONTROLLERLIST& controllerList = m_sceneNodeControllers[pController->GetType()];
	for( RSCENENODECONTROLLERLIST::iterator i = controllerList.begin(); i != controllerList.end(); ++i )
	{
		if( *i == pController )
		{
			pController->OnRemove();
			pController->m_bEnable = false;
			controllerList.erase(i);
			return true;
		}
	}
	return false;
}

RSceneNodeController* RSceneNode::GetFirstControllerByName( RSNCTRL_TYPE eType, const char* szControllerName)
{
	RPFC_THISFUNC;
	RSCENENODECONTROLLERLIST& ls = m_sceneNodeControllers[eType];
	for (RSCENENODECONTROLLERLIST::iterator itr = ls.begin(); itr != ls.end(); ++itr)
	{
		std::string& rControllerName = (*itr)->m_strControllerName;
		if (rControllerName.empty() == false && rControllerName == szControllerName)
		{
			return *itr;
		}
	}
	return NULL;
}

void RSceneNode::UpdateSceneNodeControllers( RSNCTRL_TYPE type, RCameraSceneNode* pCamera /*= NULL*/ )
{
	RPFC_THISFUNC;
	RSceneNodeController* pController;

	for( RSCENENODECONTROLLERLIST::iterator i = m_sceneNodeControllers[type].begin(); i != m_sceneNodeControllers[type].end(); ++i )
	{
		pController = *i;
		if( pController->IsEnable() )
			pController->Update( pCamera );
	}
}

void RSceneNode::ReNewContollers()
{
	for( int i = 0; i < RSNCTRL_MAX; ++i )
	{
		for( RSCENENODECONTROLLERLIST::iterator it = m_sceneNodeControllers[i].begin(); it != m_sceneNodeControllers[i].end(); ++it )
		{
			(*it)->ReNew();
		}
	}
}

void RSceneNode::EnableControllers(bool bEnable)
{
	for( int i = 0; i < RSNCTRL_MAX; ++i )
	{
		for( RSCENENODECONTROLLERLIST::iterator it = m_sceneNodeControllers[i].begin(); it != m_sceneNodeControllers[i].end(); ++it )
		{
			(*it)->Enable(bEnable);
		}
	}
}

bool RSceneNode::IsNeedHierarchicalUpdate()
{
	return ( QueryAttribute(RSNA_NO_HIERARCHY_UPDATE) == false );
}

void RSceneNode::NeedUpdateAllParentDependentValues()
{
	NeedUpdateTransform();
	NeedUpdateVisibility();
	NeedUpdateNoiseRef();
}

void RSceneNode::UpdateAllParentDependentValues()
{
	UpdateTransform();
	UpdateVisibility();
	UpdateNoiseRef();
}

bool RSceneNode::UpdateTransform()
{
//	RPFC_THISFUNC;
//	RPFC_B("RSceneNode::UpdateTransform");

	// 한 프레임 안에서 로직상에 의해 갱신을 여러번 할 수도 있다. 갱신 시간을 비교하여 같으면 안하도록 하자. - 090716, OZ
	// 무조건 그러면 그럼 뭔가 이상하다.. 이전 매트릭스 값만 해주자 우선..
	if( REngine::GetTimer().GetTime() != m_dwRefreshedTime)
	{
		m_dwRefreshedTime = REngine::GetTimer().GetTime();
		
		(*m_matWorldPrev) = (*m_matWorld);
	}
	else
	{
		// do notthing
		//return false;
	}

	bool bNeedHierarchicalUpdate = GetParent() && IsNeedHierarchicalUpdate();
	int parentUpdatedID = 0;
	if( bNeedHierarchicalUpdate )
	{
		parentUpdatedID = GetParent()->m_TransformUpdateContext.GetUpdatedID();
	}
	bool bNeedUpdate = m_TransformUpdateContext.IsNeedUpdate( bNeedHierarchicalUpdate, parentUpdatedID );

	if(bNeedUpdate)
	{
		// World TM 업데이트

		// 매트릭스를 사용 하는지 쿼터니언과 이동 값을 사용 하는지에 따라 계산 분기 
#ifdef USING_RSC_PHYSIQUE_QT
		if( IsUseForPhysiqueBone())
		{
			bool bWorldMatCalculated = false;
				// PhysiqueBone일 경우만 쿼터니언과 이동값이 잘 셋팅 되어 있을 것이다.
				// 그러므로 PhysiqueBone이 아닐 경우 .. 우짜지? 쿼터니언을 셋팅해줘? 아님 로컬을 매트릭스 계산 뒤 뽑아줘?
				// 부모는 많은 것들이 공유하므로 로컬을 TM변환을 통해 뽑아주자
				if(!m_pParent->IsUseForPhysiqueBone())
				{
					RMatrix matR;
					RMatrix matLocal = m_qRotLocal.Conv2Matrix();
					matLocal._41 = m_vTranLocal.x; 
					matLocal._42 = m_vTranLocal.y; 
					matLocal._43 = m_vTranLocal.z;
					RMatrix matScale;
					matScale.MakeIdentity();
					matScale.SetScale( m_vScaleLocal);	
					matScale.MultiplyTo( matLocal, matR);
					matR.MultiplyTo(m_pParent->GetWorldTransform(), (*m_matWorld));
					// 월드 매트릭스 아래서 또 계산 안하게..
					bWorldMatCalculated = true;
 					m_matWorld->Decompose( m_vScaleWorld, m_vTranWorld, m_qRotWorld);
				}
				else
				{
					// W = L * PW
					// ( RotP * (TransL * ScaleP)) + TranP;
					m_vTranWorld = (m_pParent->GetWorldRotation() * (m_vTranLocal * m_pParent->GetWorldScale().z)) + m_pParent->GetWorldTranslation();		
					m_qRotWorld = m_qRotLocal * m_pParent->GetWorldRotation();
					//m_vScaleWorld = (m_pParent->GetWorldRotation() * m_vScaleLocal) * m_pParent->GetWorldScale();
					m_vScaleWorld.x = m_vScaleLocal.x * m_pParent->GetWorldScale().x;
					m_vScaleWorld.y = m_vScaleLocal.y * m_pParent->GetWorldScale().y;
					m_vScaleWorld.z = m_vScaleLocal.z * m_pParent->GetWorldScale().z;

				}
			
				m_vTranWorld = m_vTranLocal;
				m_qRotWorld = m_qRotLocal;
				m_vScaleWorld = m_vScaleLocal;

			if(!bWorldMatCalculated)
			{
				// IsUseForPhysiqueBone이라도 매트릭스를 만들어 놓는다. 물체를 마운트 한다던가 하는게 ... 
				RMatrix matR = m_qRotWorld.Conv2Matrix();
				matR._41 = m_vTranWorld.x; 
				matR._42 = m_vTranWorld.y; 
				matR._43 = m_vTranWorld.z;
				RMatrix matScale;
				matScale.MakeIdentity();
				matScale.SetScale( m_vScaleWorld);
				matScale.MultiplyTo( matR, (*m_matWorld));
			}
		}
		else
#endif
		{
			RMatrixA16 matLocalScale, matLocal;
			matLocalScale.SetScaleMatrix(m_scale);
			// matLocal = matLocalScale * m_matLocal;
			matLocalScale.MultiplyTo((*m_matLocal), matLocal);

			if ( bNeedHierarchicalUpdate )
			{
				// m_matWorld = matLocal * m_pParent->GetWorldTransform();y

				matLocal.MultiplyTo(m_pParent->GetWorldTransform(), (*m_matWorld));	// 속도때문에
			}
			else
			{
				(*m_matWorld) = matLocal;
			}
		}
		
		OnUpdateTransform();

		// 트랜스폼이 업데이트 되면 BoundingVolume도 업데이트 되어야 한다.
		NeedUpdateBoundingVolume();
	}

//	RPFC_E;

	return bNeedUpdate;
}

void RSceneNode::Message(char* szMsg, void* param)
{
	OnMessage(szMsg, param);
}

bool RSceneNode::LoadCommonProperty(MXmlElement &element)
{
	char szBuffer[256];
	RVector pos(0,0,0),dir(0,1,0),up(0,0,1),scale(1,1,1);

	const char *pName = element.Attribute("name");
	if ( pName )
		SetNodeName(pName);

	MXmlElement* pElement;
	pElement = element.FirstChildElement("POSITION");
	if ( pElement && _Contents(szBuffer, pElement) )
		sscanf(szBuffer,"%f %f %f",&pos.x,&pos.y,&pos.z);

	pElement = element.FirstChildElement("DIRECTION");
	if ( pElement && _Contents(szBuffer, pElement) )
		sscanf(szBuffer,"%f %f %f",&dir.x,&dir.y,&dir.z);

	pElement = element.FirstChildElement("SCALE");
	if ( pElement && _Contents(szBuffer, pElement) )
		sscanf(szBuffer,"%f %f %f",&scale.x,&scale.y,&scale.z);

	pElement = element.FirstChildElement("UP");
	if ( pElement && _Contents(szBuffer, pElement))
		sscanf(szBuffer,"%f %f %f",&up.x,&up.y,&up.z);

	SetTransform(pos,dir,up);
	SetScale(scale);

	return true;
}

bool RSceneNode::Pick(RPICKINFO& pickInfo)
{
	if(!GetVisible() || GetOcclued() ) return false;

	bool bPicked = false;

	RPICKINFO thisPickInfo;
	thisPickInfo.inPickOrigin = pickInfo.inPickOrigin;
	thisPickInfo.inPickTo = pickInfo.inPickTo;
	thisPickInfo.inMethod = pickInfo.inMethod;

	if(OnPick(thisPickInfo)) {
		float fDistSq = (thisPickInfo.inPickOrigin - thisPickInfo.outPosition).LengthSq();
		if(fDistSq < pickInfo.outDistanceSq) {
			pickInfo.outNode = this;
			pickInfo.outPosition = thisPickInfo.outPosition;
			pickInfo.outDistanceSq = fDistSq;
			bPicked = true;
		}
	}


	for ( RSceneNodeList::iterator itor = m_children.begin(); itor != m_children.end(); ++itor) {
		RSceneNode* pChild = *itor;
		bPicked |= pChild->Pick(pickInfo);
	}

	return bPicked;
}

const RVector& RSceneNode::GetScale() const 
{ 
	return m_scale;
}

void RSceneNode::Traverse( RSceneNodeVisitor* pVisitor )
{
	RPFC_THISFUNC;
	if ( pVisitor->OnPreOrderVisit( this) )
	{
		for ( RSceneNodeList::const_iterator itor = m_children.begin(); itor != m_children.end(); ++itor)
			(*itor)->Traverse(pVisitor);

		pVisitor->OnPostOrderVisit( this);
	}
}


void RSceneNode::ExclusionTraverseRefMesh( RSceneNodeConstVisitor* pVisitor ) const
{
	if ( pVisitor->OnPreOrderVisit( this) )
	{
		for ( RSceneNodeList::const_iterator itor = m_children.begin(); itor != m_children.end(); ++itor)
		{
			if( strcmp( (*itor)->GetNodeName().c_str(), "RefferenceMesh") == 0 )
				continue;
			
			(*itor)->Traverse(pVisitor);
		}

		pVisitor->OnPostOrderVisit( this);
	}	
}


void RSceneNode::Traverse( RSceneNodeConstVisitor* pVisitor ) const
{
	if ( pVisitor->OnPreOrderVisit( this) )
	{
		for ( RSceneNodeList::const_iterator itor = m_children.begin(); itor != m_children.end(); ++itor)
			(*itor)->Traverse(pVisitor);

		pVisitor->OnPostOrderVisit( this);
	}
}


void RSceneNode::SerializeToVector( RSerializeTest pSerailTestFunc, std::vector< RSceneNode* >& refSerialedVector ) const
{
	for ( RSceneNodeList::const_iterator itor = m_children.begin(); itor != m_children.end(); ++itor)
	{
		RSceneNode* pChild = *itor;
		if( (*pSerailTestFunc)( pChild ) )
			refSerialedVector.push_back( pChild );

		pChild->SerializeToVector( pSerailTestFunc, refSerialedVector );
	}
}

// 리턴값은 등록되어 업데이트 되는 상태인지 여부
bool RSceneNode::RegisterAutoUpdate()
{
	_ASSERT(NULL != GetRootSceneNode());

	if(m_bAutoUpdateRegistered) return true;

	m_itrAutoUpdateItr = GetRootSceneNode()->RegisterAutoUpdate(this);
	_ASSERT((*m_itrAutoUpdateItr).pSceneNode == this);
	m_bAutoUpdateRegistered = true;
	return true;
}

void RSceneNode::UnregisterAutoUpdate()
{
	_ASSERT(NULL != GetRootSceneNode());
	if(!m_bAutoUpdateRegistered) return;

	_ASSERT((*m_itrAutoUpdateItr).pSceneNode == this);
	GetRootSceneNode()->UnregisterAutoUpdate(m_itrAutoUpdateItr );
	m_bAutoUpdateRegistered = false;
}

void RSceneNode::SetAutoUpdate(bool bEnable)
{
	if(m_bAutoUpdate==bEnable) return;

	// 신에 더해져있는 상태이면 업데이트 목록에 등록한다
	if(m_bAttached)
	{
		if(bEnable)
			RegisterAutoUpdate();
		else
			UnregisterAutoUpdate();
	}
	// 신에 더해져있지 않은 상태이면 상태만 갱신해두면
	// 실제로 신에 더해질때 등록된다

	m_bAutoUpdate = bEnable;
}

void RSceneNode::SetRootSceneNode( RWorldSceneNode* p)
{
	m_pRootScene = p;

	for( RSceneNodeList::iterator i = m_children.begin(); i != m_children.end(); ++i)
	{
		RSceneNode* pNode = *i;
		if( pNode)
		{
			// 매번 하는게 컴터가 힘들어 할 수도 있으니, 루트 씬 노드가 설정 되어 있지 않는 경우에만 호출을 하여 그 하위까지 트리 탐색을 하도록 한다.
			if(!pNode->GetRootSceneNode())
				pNode->SetRootSceneNode( p);
		}
	}
}

bool RSceneNode::OnUpdateSphere()
{
	if( false == m_aabb.IsValid() )		return false;
	
	m_sphere.center = m_aabb.GetCenter();
	m_sphere.radius = m_aabb.GetRadius();

	return true;
}


void RSceneNode::DebugRenderSphere( DWORD dwColor ,float plusRadius)
{
	RMatrix matWorld = RMatrix::IDENTITY;	
	matWorld.SetTranslation(GetSphere().center);
	float fRadius = GetSphere().radius + plusRadius;
	REngine::GetSceneManager().GetPassRenderer<RDebugPassRenderer>()->AddDebugSphere(matWorld,fRadius,0.001f,dwColor);
}

void RSceneNode::OnAddScene()
{
	_ASSERT( false == IsAttached() );	// 월드씬에 삽입이 안된 씬노드만 월드씬에 삽입될 수 있다.
}

void RSceneNode::OnRemoveScene()
{
	_ASSERT( true == IsAttached() );	// 월드씬에 삽입이 된 씬노드만 월드씬에서 삭제될 수 있다.
}



bool RSceneNode::UpdateSpatialTree()
{
	if( false == UseSpatialTree() || false == IsAttached() || false == IsCullingNode() )	return false;
	GetRootSceneNode()->MoveSceneNodeInBVH(this);
	return true;
}

void RSceneNode::UpdateBoundingVolume()
{
	OnUpdateAABB();
	OnUpdateSphere();
	UpdateSpatialTree();

	m_bNeedUpdateBoundingVolume = false;
}

bool RSceneNode::UseSpatialTree( void )
{
	if( GetUsingPass() < RRP_CULL_BEGIN || GetUsingPass() > RRP_CULL_END )		return false;
	return true;
}

void RSceneNode::SetParent( RSceneNode* pParent )
{
	m_pParent = pParent;
}

void RSceneNode::UpdateVisibility()
{
	_ASSERT(m_fVisibility>=0.0f);

	bool bNeedHierarchicalUpdate = GetParent() && IsNeedHierarchicalUpdate();
	int nParentUpdatedID = 0;
	if( bNeedHierarchicalUpdate )
	{
		nParentUpdatedID = GetParent()->m_VisibilityUpdateContext.GetUpdatedID();
	}
	if( m_VisibilityUpdateContext.IsNeedUpdate( bNeedHierarchicalUpdate, nParentUpdatedID ) )
	{
		if( bNeedHierarchicalUpdate )
			m_fVisibilityResult = GetParent()->GetVisibilityResult() * m_fVisibility;
		else
			m_fVisibilityResult = m_fVisibility;
	}
}

void RSceneNode::UpdateNoiseRef()
{
	_ASSERT(m_fNoiseRef>=0.0f);

	bool bNeedHierarchicalUpdate = GetParent() && IsNeedHierarchicalUpdate();
	int nParentUpdatedID = 0;
	if( bNeedHierarchicalUpdate )
	{
		nParentUpdatedID = GetParent()->m_NoiseRefUpdateContext.GetUpdatedID();
	}
	if( m_NoiseRefUpdateContext.IsNeedUpdate( bNeedHierarchicalUpdate, nParentUpdatedID ) )
	{
		if( bNeedHierarchicalUpdate )
			m_fNoiseRefResult = GetParent()->GetNoiseRefResult() * m_fNoiseRef;
		else
			m_fNoiseRefResult = m_fNoiseRef;
	}
}

void RSceneNode::AddToRenderingList( RRenderingList* pRenderingList )
{
	pRenderingList->Add(this,GetUsingPass());
}

void RSceneNode::RemoveFromRenderingList( RRenderingList* pRenderingList )
{
	pRenderingList->Remove(this,GetUsingPass());
}

RVector RSceneNode::GetScaleFromWorldTransform()
{
	RVector vScale;
	vScale.x = RVector(m_matWorld->_11,m_matWorld->_12,m_matWorld->_13).Length();
	vScale.y = RVector(m_matWorld->_21,m_matWorld->_22,m_matWorld->_23).Length();
	vScale.z = RVector(m_matWorld->_31,m_matWorld->_32,m_matWorld->_33).Length();

	return vScale;
}

void RSceneNode::PreRender( RCameraSceneNode *pCamera )
{
	RPFC_THISFUNC;
	UpdateSceneNodeControllers(RSNCTRL_PRERENDER, pCamera);
	OnPreRender(pCamera);
}

void RSceneNode::PostRender( RCameraSceneNode *pCamera )
{
	RPFC_THISFUNC;
	OnPostRender(pCamera);
}


void RSceneNode::AddAABB( RBoundingBox* _pAABB )
{
	_pAABB->Add( m_aabb );
}

// user property support
void RSceneNode::PushUserProperty(const string& strPropertyName, const string& strValue)
{
	m_vecUserProperty.push_back( pair< string, string> ( strPropertyName, strValue ) );
}

void RSceneNode::RemoveUserProperty(unsigned int index)
{
	if( index >= m_vecUserProperty.size() ) { _ASSERT(FALSE); return; }
	m_vecUserProperty.erase( m_vecUserProperty.begin() + index );
}

static string NULLSTRING;

const string& RSceneNode::GetUserPropertyName(unsigned int index) const
{
	if( index >= m_vecUserProperty.size() ) { _ASSERT(FALSE); return NULLSTRING; }
	return m_vecUserProperty[index].first;
}

const string& RSceneNode::GetUserPropertyValue(unsigned int index) const
{
	if( index >= m_vecUserProperty.size() ) { _ASSERT(FALSE); return NULLSTRING; }
	return m_vecUserProperty[index].second;
}

void RSceneNode::SetUserPropertyValue(unsigned int index, const string& strValue)
{
	if( index >= m_vecUserProperty.size() ) { _ASSERT(FALSE); return; }
	m_vecUserProperty[index].second = strValue;
}


}