#include "stdafx.h"
#include <deque>

#include "RMeshMgr.h"
#include "RCameraSceneNode.h"
#include "RMeshNode.h"
#include "RSkeleton.h"
#include "RRenderHelper.h"
#include "RMeshCollision.h"
#include "MCollision.h"
#include "RCollisionTreeBuilder.h"
#include "RShadowStrategy.h"
#include "RActor.h"
#include "RActorNodePhysique.h"
#include "RActorNodeRenderStrategy.h"
#include "RShaderCompositeManager.h"
#include "RShaderEffectController.h"
#include "RMaterialResource.h"
#include "RAnimationResource.h"
#include "RClothActorNode.h"
#include "RRenderingStrategy.h"
#include "RActorCollision.h"
#include "RPostEffectManager.h"
#include "RPostEffectDistortionParameter.h"
#include "RLODProcessorBase.h"

namespace rs3 {

MImplementRTTI(RActor, RSceneNode);
MImplementRTTI(RActorMLeaf, RActor);


//////////////////////////////////////////////////////////////////////////
// 
RActor::RActorNodeBuilder::RActorNodeBuilder(RActor* pActor) 
{
	m_pActor = pActor;

	m_vecAddMeshList.clear();
	m_vecDeleteList.clear();
};
RActor::RActorNodeBuilder::~RActorNodeBuilder() 
{
	for( size_t i = 0; i < m_vecAddMeshList.size(); ++i )
		delete m_vecAddMeshList[i];

	m_vecAddMeshList.clear();
	m_vecDeleteList.clear();
};

void RActor::RActorNodeBuilder::DeleteActorNode(RActorNode* pActorNode)
{
	m_vecDeleteList.push_back(pActorNode);
}

void RActor::RActorNodeBuilder::DeleteActorNode(const char* szNodeName)
{
	RActorNode* pActorNode = m_pActor->GetActorNode(szNodeName);
	if( pActorNode == NULL )
		mlog("Failed DeleteActorNode 해당 엑터노드를 찾을 수 없습니다!!\n");

	m_vecDeleteList.push_back(pActorNode);
}

void RActor::RActorNodeBuilder::AddMeshNode(RMeshNode* pMeshNode, RActorNode* pParent, const char* szActorNodeName, bool bBackground)
{
	RADDMESHNODE* pNewMeshNode = new RADDMESHNODE();

	pNewMeshNode->pMeshNode			= pMeshNode;
	pNewMeshNode->pParentNode		= pParent;
	pNewMeshNode->bBackground		= bBackground;
	pNewMeshNode->szActorNodeName	= szActorNodeName;

	m_vecAddMeshList.push_back(pNewMeshNode);
}

void RActor::RActorNodeBuilder::Build()
{
	//////////////////////////////////////////////////////////////////////////
	// DELETE
	std::vector<RActorNode*> vecActorNodesToDelete;

	for( size_t i = 0; i < m_vecDeleteList.size(); ++i )
	{
		std::vector<RActorNode*> vecCurrentNodesToDelete;
		RActorNode* pActorNode = m_vecDeleteList[i];
	
		// 자식을 포함한 모든 삭제할 노드를 수집한다
		CActorNodeDeletionVisitor visitor(m_pActor);
		pActorNode->Traverse(&visitor);
		vecCurrentNodesToDelete = visitor.GetDeleteNodes();
		
	//	std::vector<RActorNode*>::iterator itr;
		for( size_t j = 0; j < vecCurrentNodesToDelete.size(); ++j )
			vecActorNodesToDelete.push_back( vecCurrentNodesToDelete[j] );
	}

	// 한번에 모두 지운다
	m_pActor->DeleteActorNode( vecActorNodesToDelete,  false);
	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////
	// ADD
	m_pActor->AddMeshNode(m_vecAddMeshList);
	
	// Rebuild Actor
	m_pActor->CreateRenderInfoAndNotifyToController();

	// 애니메이션 재빌드
	m_pActor->RebuildAnimationOnActorNodeChanged();
}

bool RActor::AddMeshNode(std::vector<RActorNodeBuilder::RADDMESHNODE*>& vecActorNode)
{
	std::vector<RActorNodeBuilder::RADDMESHNODE*>::iterator itr;
	for( itr = vecActorNode.begin(); itr != vecActorNode.end(); ++itr )
	{
		const char* szNewActorNodeName = (*itr)->szActorNodeName ? (*itr)->szActorNodeName : (*itr)->pMeshNode->GetName();

		// 새로 만든다
		RActorNode* pActorNode = NewActorNode((*itr)->pMeshNode);
		pActorNode->m_strNodeName = szNewActorNodeName;

		if((*itr)->pParentNode)
			AddActorNode(pActorNode, (*itr)->pParentNode);
		else
			AddActorNode(pActorNode, this);

		pActorNode->Create();
		bool bOk = pActorNode->OnCreateDeviceDependentPart();
		_ASSERT(bOk);

		pActorNode->OnCreateMaterials((*itr)->bBackground);
	}

	return true;
}

// Builder 에서 호출 되는 액터 리스트 삭제 함수
bool RActor::DeleteActorNode(std::vector<RActorNode*> &vecActorNode, bool bNotifyToController)
{
	std::vector<RActorNode*>::reverse_iterator itrRev;
	for( itrRev = vecActorNode.rbegin(); itrRev != vecActorNode.rend(); ++itrRev )
	{
		RActorNode* pActorNode = (*itrRev);
		if( pActorNode  == NULL )	// 같은 목록의 참조로 이미 지워졌을때
			continue;

		vector<RActorNode*>::iterator itrVec;
		itrVec = remove( m_actorNodes.begin(), m_actorNodes.end(), pActorNode  ); 
		m_actorNodes.erase( itrVec, m_actorNodes.end() ); 

		stdext::hash_map<string,RActorNode*>::iterator itrMap;
		itrMap = m_actorNodeMap.find( pActorNode->GetNodeName() );
		m_actorNodeMap.erase( itrMap );

		DeleteActorNodeAndNotifyToController(pActorNode, bNotifyToController);

		pActorNode->Destroy();
		delete pActorNode;
	}

	return true;
}


//////////////////////////////////////////////////////////////////////////
// RActor
RActor::RActor(RRENDERPASS eRenderPass) : RSceneNode(eRenderPass)
, m_pListener(NULL)
, m_pShaderEffectController(NULL)
, m_vMultiplyColor(1, 1, 1, 1)
, m_bExistCollisionTree(true)
, m_bUsingActorPassRenderer(true)
, m_bAutoAnimationEnable(false)
, m_bUsingEnvironmentTiling(true)
, m_bAlpha2PassRendering(false)
, m_bSceneActor(false)
, m_nBGWKPriorityOffset(0)
//, m_pBackgroundWorkUnit(NULL)
//, m_bUsingActorPassRenderer(false)
, m_bUseShade(true)
, m_bIsMaxAniAABB(false)
, m_pAutoAnimation(NULL)
, m_matInvWorld(RMatrix::IDENTITY)
, m_nResultTransformUpdatedFrame(0)
, m_nAnimationFrameUpdatedFrame(0)
, m_nUpdateForRenderRefCount(0)
, m_fDistanceToCamera(0)
, m_bVertexMotionBlur(false)
, m_bEdge(false)
, m_bPEEdge( false )
, m_bPEEdgeProj( false )
, m_bAccumMotionBlur(false)
, m_nPixelMotionBlur(PMB_NONE)
, m_fMotionBlurBasisFrame(0)
, m_bUsingVertexAnimation(false)
, m_pDistortionParam(NULL)
, m_bUseDoubleShadow(false)
, m_bAOEnable(true)
, m_bShadowReceiver(false)
, m_pEnvironmentTexture( NULL )
{
	m_pMesh = NULL;
	m_pLowPolyMesh = NULL;

	// transition controller
// 	m_pTransitionController = new RBlendAnimationController();
// 	m_pTransitionController->m_pActor = this;
 	m_animationController.m_pActor = this;
	m_animationController.GetFrameController().m_pActor = this;

	// collision
	m_pCollision = NULL;
	memset( m_szEnvTexName, 0, sizeof(m_szEnvTexName) );
	SetUsable(false);

	/// LOD 사용
	CreateLODContext( LODP_FOG | LODP_PROJECTION);
}

RActor::~RActor() 
{
	// destroy
	Destroy();

	if(m_pMesh)
	{
		REngine::GetSceneManager().ReleaseResource( m_pMesh );
		m_pMesh = NULL;
	}
}

void RActor::DeleteManagingChildren()
{
	Destroy();

	// 나머지 딸린 노드들에 대해서는 SceneNode의 기본 정책을 따른다
	RSceneNode::DeleteManagingChildren();
}

void RActor::Destroy()
{
	// Clear(); // 딸린 노드들을 지움	-> 딸린노드를 관리하자

	// RActor가 소유하는 controller
	m_animationController.Destroy();
//	m_pTransitionController->Destroy();

	SAFE_DELETE(m_pShaderEffectController);

	// parent 가 앞쪽에 있기 때문에 뒤에서부터 지운다
	for(int i=(int)m_actorNodes.size()-1; i>=0; --i)
	{
		m_actorNodes[i]->Destroy();
	}

	// destory 하는 과정에서 이펙트가 사라질때 actornode 에 기록하는 부분이 있어서 destroy/delete 를 분리하였다.
	// TODO: 리팩토링 하면서 없애자.
	for(int i=(int)m_actorNodes.size()-1; i>=0; --i)
	{
		m_actorNodes[i]->RemoveFromParent();
		delete m_actorNodes[i];
	}

	m_actorNodes.clear();
	//m_sortedRenderableActorNode.clear();	// http://dev:8181/projects/engine/ticket/1093의 문제1 땜빵코드
	m_actorNodeMap.clear();

	if(m_pCollision)
	{
		m_pCollision->Destroy();
		delete m_pCollision;
		m_pCollision = NULL;
	}
	
	m_bUsingActorPassRenderer = true;
	SetAABB( RBoundingBox::INVALID );

	m_pAutoAnimation = NULL;

	SetDistortion( false );

	EmptyEnvironment();
}

RActorNode* RActor::NewActorNode(RMeshNode* pMNode)
{
	RActorNode *pNewNode = NULL;
	
	if( pMNode->isClothMesh() )
	{
		std::string fileName = pMNode->m_pParentMesh->GetName();
		unsigned int index = fileName.find("elu");
		fileName.replace(index,index+2,"clt");

		pNewNode = RClothActorNode::Create(pMNode->m_Name,fileName);

	}
	else if(pMNode->isPhysique())
	{
		pNewNode = new RActorNodePhysiqueGPU();	// TODO: cpu physique
	}
	
	if( pNewNode == NULL )
	{
		pNewNode = new RActorNodeMLeaf( ACTOR_NODE_COMMON );
	}

	pNewNode->m_pMeshNode = pMNode;
	pNewNode->m_strNodeName = pMNode->m_Name;
	pNewNode->AddAttribute(RSNA_DO_NOT_SAVE);

	return pNewNode;
}
void RActor::CreateDeviceIndependentPart(RMesh* pMesh)
{
	INIT_TCHECK("/check_actor_createtime");
	_ASSERT(pMesh);

	// 충돌체
	// by pok. 충돌체가 메시가 생성될때 생성되는것이 이상한거 같다. 09.02.09
	BEGIN_TCHECK;
	_ASSERT(m_pCollision == NULL);
	m_pCollision = new RActorCollision;
	END_TCHECK("RActor::CreateDeviceIndependentPart, new RActorCollision");

	// 메시에 의한 기본 충돌체
	bool bNeedDefaultCollsion = ! ( (NULL != pMesh->m_pMeshCollision) && (false == pMesh->m_pMeshCollision->empty()) );
	if(bNeedDefaultCollsion)
	{
		BEGIN_TCHECK;
		// GetStaticSkeletonBoundingBox()에 필요한 m_matBase 등은
		// RMesh의 Fill 단계에서 처리되므로, AsyncLoading이어도 상관없다.
		RBox b;
		if( true == pMesh->GetStaticSkeletonBoundingBox(b) )
		{
			RVector c = b.GetCenter();

			float rad, bot, top;

			rad = b.maxx - c.x;
			if (rad < b.maxy - c.y)
				rad = b.maxy - c.y;

			rad *= 1.2f;

			bot = rad;
			if (bot > c.z)
				bot = c.z;

			top = b.maxz - rad;
			if (top < c.z)
				top = c.z;

			top += top * 0.1f;

			m_pCollision->Add(MCollision::CAPSULE, MCollision::MESHTYPE_NORMAL, rad, RVector(0, 0, bot), RVector(0, 0, top), RMatrix::IDENTITY, this);
		}
		END_TCHECK("RActor::CreateDeviceIndependentPart, m_pCollision->Add");
	}

	// 메시에 적혀있는 테이블로부터 ActorNode 만들기
	
	int nSize = pMesh->m_NodeTable.size();
	for(int i=0;i<nSize;++i)
	{
		RMeshNode* pMeshNode = pMesh->m_NodeTable[i];

		RSceneNode* pParent = NULL;
		if(pMeshNode->m_pParent)
			pParent = m_actorNodeMap[pMeshNode->m_pParent->GetName()];
		else
			pParent = this;

		RActorNode* pActorNode = NewActorNode(pMeshNode);

		AddActorNode(pActorNode,pParent);

	}

	// 더해진 액터노드 생성
	// pMesh->m_NodeTable.size()과 m_actorNode의 nSize는 같다.
	BEGIN_TCHECK;
	{
		for(int i=0;i<nSize;++i)
			m_actorNodes[i]->Create();
	}
	END_TCHECK("RActor::CreateDeviceIndependentPart, RActorNode::Create");

	// Build Rendering Information
	BEGIN_TCHECK;
	CreateRenderInfoAndNotifyToController();
	END_TCHECK("RActor::CreateDeviceIndependentPart, CreateRenderInfoAndNotifyToController");
}

void RActor::CreateDeviceDependentPart(RMesh* pMesh)
{
	INIT_TCHECK("/check_actor_createtime");

	// actor 노드중에 디바이스 리소스를 생성해야하는것은 생성
	BEGIN_TCHECK;
	for( vector< RActorNode* >::iterator itr = m_actorNodes.begin(); itr != m_actorNodes.end(); ++itr )
	{
		RActorNode* pActorNode = *itr;
		bool bCreated = pActorNode->OnCreateDeviceDependentPart();
		_ASSERT(bCreated);
	}
	END_TCHECK("RActor::CreateDeviceDependentPart, pActorNode->OnCreateDeviceDependentPart()");

	BEGIN_TCHECK;
	// Collision Attachment 처리
//	m_pCollision->Build(this);

	// Alpha2Pass 렌더링 여부
	SetAlpha2PassRendering(pMesh->m_isAlpha2PassRendering);

	// 초기 부모 의존적인 변수 업데이트
	NeedUpdateAllParentDependentValues();
	UpdateAllParentDependentValues();

	//UpdateSpatialTree();

	// Scene Actor가 아닌 Actor만 ShaderEffect를 적용.
	if( !m_bSceneActor && strncmp( GetNodeIDString(), RSID_ACTOR, strlen(RSID_ACTOR) ) == 0 )
	{
		m_pShaderEffectController = new RShaderEffectController;
		AddController( m_pShaderEffectController );
	}
	END_TCHECK("RActor::CreateDeviceDependentPart, ETC");
}

void RActor::CreateMaterials(bool bBackground)
{
	for( vector< RActorNode* >::iterator itr = m_actorNodes.begin(); itr != m_actorNodes.end(); ++itr )
	{
		RActorNode* pActorNode = *itr;
		bool bCreated = pActorNode->OnCreateMaterials(bBackground);
		_ASSERT(bCreated);
	}
}

void RActor::AddToBVH()
{
#ifdef BVH_CAMERA
	if( IsAttached() && GetRootSceneNode() )	//컬링리스트에 다시 등록
	{
		GetRootSceneNode()->AddSceneNodeToBVH(this);
	}
#endif
}

void RActor::RemoveFromBVH()
{
#ifdef BVH_CAMERA
	if( IsAttached() && GetRootSceneNode() )	//컬링리스트에서 지우기
	{
		GetRootSceneNode()->RemoveSceneNodeFromBVH(this);
	}
#endif
}

void RActor::ChangeMatetial(string strActorNode, string strSrcMtrl, string strDstMtrl, float fBlendingTime)
{
	RPFC_THISFUNC;

	RemoveFromBVH();

	RPFC_B("ChangeMatetial_CORE");
	RActorNode* pActorNode = GetActorNode(strActorNode.c_str());
	pActorNode->ChangeMtrlSet(strSrcMtrl, strDstMtrl, fBlendingTime);
	RPFC_E;

	AddToBVH();
}

void RActor::ChangeMatetial(string strActorNode, int nSrcMtrl, int nDstMtrl, float fBlendingTime)
{
	RemoveFromBVH();

	RActorNode* pActorNode = GetActorNode(strActorNode.c_str());
	pActorNode->ChangeMtrlSet(nSrcMtrl, nDstMtrl, fBlendingTime);

	AddToBVH();
}

void RActor::ChangeMatetial(string strActorNode, int nSrcMtrl, string strDstMtrl, float fBlendingTime)
{
	RemoveFromBVH();

	RActorNode* pActorNode = GetActorNode(strActorNode.c_str());
	pActorNode->ChangeMtrlSet(nSrcMtrl, strDstMtrl, fBlendingTime);

	AddToBVH();
}

void RActor::ChangeMatetial(int nActorNodeIndex, string strSrcMtrl, string strDstMtrl, float fBlendingTime)
{
	RemoveFromBVH();

	RActorNode *pActorNode = GetActorNodes()[nActorNodeIndex];
	pActorNode->ChangeMtrlSet(strSrcMtrl, strDstMtrl, fBlendingTime);

	AddToBVH();
}

void RActor::ChangeMatetial(int nActorNodeIndex, int nSrcMtrl, int nDstMtrl, float fBlendingTime)
{
	RemoveFromBVH();

	RActorNode *pActorNode = GetActorNodes()[nActorNodeIndex];
	pActorNode->ChangeMtrlSet(nSrcMtrl, nDstMtrl, fBlendingTime);

	AddToBVH();
}

void RActor::ChangeMatetial(int nActorNodeIndex, int nSrcMtrl, string strDstMtrl, float fBlendingTime)
{
	RemoveFromBVH();

	RActorNode *pActorNode = GetActorNodes()[nActorNodeIndex];
	pActorNode->ChangeMtrlSet(nSrcMtrl, strDstMtrl, fBlendingTime);

	AddToBVH();
}

void RActor::SetIlluminationScale(const float fScale)
{ 
	for( vector< RActorNode* >::iterator itr = m_actorNodes.begin(); itr != m_actorNodes.end(); ++itr )
	{
		RActorNode* pActorNode = *itr;
		pActorNode->SetIlluminationScale(fScale);
	} 
}

void RActor::SetIlluminationScale(string strActorNode, const float fScale)
{ 
	RActorNode* pActorNode = GetActorNode(strActorNode.c_str());
	if(pActorNode == NULL)
		return;

	pActorNode->SetIlluminationScale(fScale);
}

float RActor::GetIlluminationScale(string strActorNode)
{
	RActorNode* pActorNode = GetActorNode(strActorNode.c_str());
	if(pActorNode == NULL)
		return 0.0f;

	return pActorNode->GetIlluminationScale(); 
}

int RActor::CalcPriority()
{
	int nPriority(0);
	if (m_bSceneActor)
		nPriority = RBGWK_SLEEP - RBGWK_HIGH_OFFSET;
	else
	{
		RCameraSceneNode* pCamera = REngine::GetSceneManager().GetPrimaryCamera();
		if (pCamera)
		{
			float fDistance = pCamera->GetWorldPosition().DistanceTo( GetWorldPosition());

			if (fDistance > DEFAULT_DISTANCE_FOR_BACKGROUND_PRIORITY)
			{
				return RBGWK_SLEEP - m_nBGWKPriorityOffset;
			}
		}

		if (nPriority != RBGWK_SLEEP)
		{
			nPriority -= m_nBGWKPriorityOffset;
		}
	}
	return nPriority;
}

void RActor::OnLoadingComplete(RResource* pResource)
{
	if( m_pMesh != pResource )		return;

	if(!m_pMesh->IsLoadingSucceeded()) 
	{
		mlog("Failed to create an actor %s.\n", m_pMesh->GetName().c_str() );
		return;
	}

	// 우선 메시의 스태틱 AABB를 기본 로컬 AABB로 셋팅해준다.
	SetLocalAABB( &m_pMesh->GetStaticBBox() );
	
	if( m_bAutoAnimationEnable && m_pAutoAnimation == NULL)	//AutoAnimaion
	{
		string strAnimationName( MGetPureFileName(m_pMesh->GetName()) );
		m_pAutoAnimation = m_pMesh->GetAnimationMgr()->GetAnimation(strAnimationName.c_str());
	}

	Load();
	UpdateAnimationReferenceNodes();	// 애니메이션이 없는 physique 모델인경우도 있으므로 한번 초기화
	OnActorLoadComplete();
}

bool RActor::Load()
{
	RPFC_THISFUNC;
	INIT_TCHECK("/check_actor_createtime");

	BEGIN_TCHECK;
	CreateDeviceIndependentPart(m_pMesh);
	END_TCHECK("RActor::Load, CreateDeviceIndependentPart");

	BEGIN_TCHECK;
	CreateDeviceDependentPart(m_pMesh);
	END_TCHECK("RActor::Load, CreateDeviceDependentPart");

	BEGIN_TCHECK;
	CreateMaterials(m_bBackgroundCreation);
	END_TCHECK("RActor::Load, CreateMaterials");

	SetUsable(true);

	return true;
}

void RActor::OnActorLoadComplete()
{
	RPFC_THISFUNC;
	// 액터 로딩이 끝나면 로딩 의존적인 컨트롤러들이 활성화 되게 해준다.
	for( RSceneNodeList::iterator i = m_children.begin(); i != m_children.end(); ++i)
	{
		RSceneNode* pNode = *i;
		pNode->EnableControllers(true);
	}

	PreCreateShader();
	LinkLODMetricInfo();
}

void RActor::LinkLODMetricInfo()
{
	if( m_pMesh && m_pMesh->GetLODMetricInfo())
	{
		CreateLODContext( LODP_DISTANCE);
		SetMetricInfo( m_pMesh->GetLODMetricInfo());
	}
}

void RActor::OnAutoAnimationLoaded()
{
	PlayAutoAnimationRandom();
}

void RActor::PlayAutoAnimationRandom()
{
	// 자동 애니메이션 플레이
	if(m_pAutoAnimation)
	{
		SetAnimation( m_pAutoAnimation );
		// 다만 모두가 같지 않도록 임의의 프레임으로 설정
		// 애니메이션 프레임이 RandomNumber(int,int) 허용치를 넘는 경우가 있어서 float으로 값을 얻는다.
		int nRandomFrame = (int)RandomNumber( 0.f, (float)m_pAutoAnimation->GetResource()->GetMaxFrame() );
		m_animationController.SetFrame( nRandomFrame );
	}
}

bool RActor::Create(RMesh* pMesh, bool bBackgroundCreation, bool bLoadMonitoring)
{
//	mlog("RActor::Create %s %x \n",pMesh->GetName().c_str(), this);

	if(!pMesh)
		return false;

	INIT_TCHECK("/check_actor_createtime");

	// 이미 create된 것은 destroy를 불러주고 다시 create해야한다
	_ASSERT( m_pMesh==NULL );
	m_pMesh = pMesh;
	m_bBackgroundCreation = bBackgroundCreation;

	BEGIN_TCHECK;
	bool bRet = REngine::GetSceneManagerPtr()->AddLoadingList( m_pMesh, bBackgroundCreation, this );
	END_TCHECK("RActor::Create, AddLoadingList");
	return bRet;
}


bool RActor::Create(const char* szMeshName, bool bBackgroundCreation)
{
	bool bLoadMonitoring = false;
	if( bBackgroundCreation && CalcPriority() != RBGWK_SLEEP )
		bLoadMonitoring = true;

	RMesh* pMesh = REngine::GetSceneManager().CreateResource< RMesh >( szMeshName, bLoadMonitoring );

	return Create(pMesh, bBackgroundCreation, bLoadMonitoring);
}

void RActor::SetDistortion(bool bDistortion)
{
	if(IsDistortion()==bDistortion)
		return;

	RemoveFromBVH();

	if( bDistortion )
	{
		if( NULL == m_pDistortionParam )
			m_pDistortionParam = new RDistortionParameter;
	}
	else
	{
		SAFE_DELETE( m_pDistortionParam );
	}

	AddToBVH();
}

bool RActor::IsDistortion()
{
	return ( m_pDistortionParam ? true : false );
}

bool RActor::IsDistortionRenderable()
{
	if( IsDistortion() )
		return m_pDistortionParam->bRender;

	return true;
}

RDistortionParameter* RActor::GetDistortionParam()
{
	return m_pDistortionParam;
}

void RActor::SetDistortionParam(RDistortionParameter* param)
{
	if(IsDistortion()==false)
		return;

	_ASSERT(param);

	*m_pDistortionParam = *param;
}

void RActor::SetUseDoubleShadow(bool bUse)
{
	RemoveFromBVH();

	m_bUseDoubleShadow = bUse;

	AddToBVH();
}

// 툴에서 요청하는 리로드는 결국 액터에서 다시 해주어야 할 것 같다. - 090608, OZ
// 리로드 되게 어케 구성하지....
bool RActor::ReLoadMesh()	// 메시의 데이터만 갱신하면 안되고 액터 자체를 갱신 시켜야 한다. - 090608, OZ
{
	// 데이터들을 지웠다가
	bool bUsingActorPassRenderer = m_bUsingActorPassRenderer;
	// Destroy()는 DeleteManagingChildren()에서 호출
	DeleteManagingChildren();
	// Destory에서 m_bUsingActorPassRenderer를 T로 강제화 한다.
	// DEstory에서 이를 제거하면 어떤 영향을 끼칠 지 확인 되지 않았으므로
	// 수동으로 복원 시켜버린다
	m_bUsingActorPassRenderer = bUsingActorPassRenderer;

	string meshFileName;
	if(m_pMesh)
	{
		meshFileName = m_pMesh->GetName();
		REngine::GetSceneManager().ReleaseResource( m_pMesh );
		m_pMesh = NULL;
	}

	// collision
	m_pCollision = NULL;

	// 액터의 이름이 메시의 이름이므로 이로 다시 구성
	// 리로드는 주로 툴에서 사용 목적이므로 백그라운드 로딩은 고려 X
	Create( meshFileName.c_str(), false);

	return true;
}


void RActor::Play() 
{
	m_animationController.Play();
	m_AutoUpdateContext.m_bPlayingAnimation = true;

	SetAutoUpdate(m_AutoUpdateContext.CheckAutoUpdateContext());
}

void RActor::Stop() 
{
	m_animationController.Stop();

	for(int i=0;i<GetActorNodeCount();++i) {
		RActorNode* pNode = m_actorNodes[i];
		pNode->ResetTransform();
//		pNode->m_pAnimationController=NULL;
	}

	m_AutoUpdateContext.m_bPlayingAnimation = false;
	SetAutoUpdate(m_AutoUpdateContext.CheckAutoUpdateContext());
}

void RActor::Pause() 
{
	m_animationController.Pause();

	m_AutoUpdateContext.m_bPlayingAnimation = false;
	SetAutoUpdate(m_AutoUpdateContext.CheckAutoUpdateContext());
}

void RActor::RenderSingle( RCameraSceneNode* pCurrentCamera, RRENDERINGMETHOD eMothod)
{
	RPFC_THISFUNC;
	RActorNodeRenderStrategyMStageInferred RenderStrategy;

	// 액터 노드 그리기
	for(size_t i=0;i<m_sortedRenderableActorNode.size(); ++i)
	{
		RActorNode* pActorNode = m_sortedRenderableActorNode[i];
		pActorNode->RenderSingle( pCurrentCamera, &RenderStrategy);
	}
}

void RActor::RenderSingleAlpha2Pass(  RCameraSceneNode* pCurrentCamera, RRENDERINGMETHOD eMothod)
{
	RPFC_THISFUNC;
	//------------------------------------------------------------------------------
	//Render Depth
	for( size_t i = 0; i < m_sortedRenderableActorNode.size(); ++i )
	{
		RActorNode* pActorNode = m_sortedRenderableActorNode[i];

		RActorNodeRenderStrategyAlphaDepthOnly RendeStrategy;
		pActorNode->RenderSingle( pCurrentCamera, &RendeStrategy );
	}	
	//------------------------------------------------------------------------------



	//------------------------------------------------------------------------------
	//Render normally
	for( size_t i = 0; i < m_sortedRenderableActorNode.size(); ++i )
	{
		RActorNode* pActorNode = m_sortedRenderableActorNode[i];
		
		RActorNodeRenderStrategyAlpha2PassMStage RendeStrategy;
		pActorNode->RenderSingle( pCurrentCamera, &RendeStrategy);
	}
	//------------------------------------------------------------------------------
}

void RActor::RenderMeshCollision()
{
	m_pCollision->RenderCollisionObject();
}

void RActor::RenderClothCollision()
{
	m_pCollision->RenderCollisionObject( MCollision::MESHTYPE_CLOTH );
}

void RActor::OnUpdateTransform()
{
	GetWorldTransform().GetInverse(&m_matInvWorld);	// world 의 inverse 를 구해둔다. 렌더링때 쓰인다

#ifdef USING_RSC_PHYSIQUE_QT
	//if( IsUseForPhysiqueBone())	// 이조건을 붙이면 케릭터가 제대로 랜더링 되지 않는다.. 왜??
	// ActorNode가 아니라 Actor이니까 ㅡㅡ;; Bone으로 쓰이는 것은 ActorNode이다. - 090724, OZ
	m_matInvWorld.Decompose( m_vInvWorldScale, m_vInvWorldTran, m_qInvWorldRot);
#endif
}

void RActor::OnUpdateAABB()
{
	UpdateAABB();
}

void RActor::UpdateAABB()
{
	if(m_bIsMaxAniAABB == true)
	{
		RBoundingBox aabbWorld;
		TransformBox(&aabbWorld, m_aabbLocal, GetWorldTransform());
		SetAABB( aabbWorld );
	}
	else // EXPORTER_ANI_VER8 이하 포멧
	{
		// TODO : ani 파일 포멧 변환이 완료 되고 나면 없앨것
		SetAABB( ForceUpdateAABB() );// Actor와 ActorNode의 AABB를 다시 수집한다.
	}
}

// 기본적으로 Actor AABB는 Animation별로 Static한 AABB를 사용하고 ActorNode의 AABB는 수집하지 않는다.
// 정밀한 Actor의 AABB나 ActorNode의 AABB 정보가 필요할때는 ForceUpdateAABB를 호출해 AABB를 수집한다.
// bBillboard : BillBoard Node의 AABB를 원점과 가장먼 정점으로 회전시킬수 있는 가장 큰 최대 AABB 박스를 구축한다.
RBoundingBox RActor::ForceUpdateAABB(bool bBillboard/* = false*/)
{
	RBoundingBox box;
	box.Initialize();

	for(int i=0;i<GetActorNodeCount();++i)
	{
		RActorNode* pNode = m_actorNodes[i];

		pNode->NeedUpdateTransform();
		pNode->UpdateTransform();
		RMeshNode *pMeshNode = pNode->m_pMeshNode;

		// 참조되는 뼈라면 노드위치 기반으로 바운딩박스를 구한다
		if(	//pMeshNode->GetFlag(RM_FLAG_BONE_MESH) &&	// 참조되는 뼈가 bone 이 아닌경우도 있네요.
			pNode->GetBoneRefCount()
			)
		{
			const RBoundingBox &nodeLocalAABB = pMeshNode->m_boundingBox;
			// 좋은 코드는 아니지만, 일단 이렇게 해서 아무 데이터도 없는 node를 판별한다.
			if ( nodeLocalAABB.vmin.x == FLT_MAX )
				continue;

			RBoundingBox nodeWorldAABB;
			TransformBox(&nodeWorldAABB, nodeLocalAABB, pNode->GetWorldTransform());
			box.Add(nodeWorldAABB);
		}
		else
		{
			if(!pMeshNode || !pMeshNode->isRenderAble())
				continue;

			// 뼈이외의 노드, aabb 기반으로 대충 빠르게 구한다.
			const RBox& nodelLocalAABB = pNode->m_pMeshNode->m_boundingBox;
			RBox nodeWorldAABB;
			TransformBox(&nodeWorldAABB, nodelLocalAABB, (*pNode->m_matWorld));
			pNode->SetAABB( nodeWorldAABB );
			box.Add(pNode->GetAABB());
		}

		if( bBillboard && pNode->m_pMeshNode->m_nAlign != RMA_NONE )
		{
			RVector3 vCenter = pNode->GetAABB().GetCenter();
			RVector3 vRadius;
			vRadius.x = vCenter.x < 0 ? box.minx : box.maxx;
			vRadius.y = vCenter.y < 0 ? box.miny : box.maxy;
			vRadius.z = vCenter.z < 0 ? box.minz : box.maxz;
			float fBoxRadius = vRadius.Length() * 0.5f;

			RBox aabb( RVector3(-fBoxRadius, -fBoxRadius, -fBoxRadius), RVector3(fBoxRadius, fBoxRadius, fBoxRadius) );
			box.Add(aabb);
		}
	}

	return box;
}

void RActor::UpdateColCapsule()
{
	m_vecColCapsule.clear();
}

void RActor::UpdateAnimationFrameAndEvent()
{
	RPFC_THISFUNC;

	// 중복실행 방지
	if(m_nAnimationFrameUpdatedFrame == REngine::GetDevice().GetFrameCount())
		return;
	m_nAnimationFrameUpdatedFrame = REngine::GetDevice().GetFrameCount();

	// 애니메이션 프레임 업데이트
	if( m_bAutoAnimationEnable && m_pAutoAnimation && m_pAutoAnimation->GetResource()->IsLoadingSucceeded() )
	{
		if( m_animationController.GetAnimation() == NULL )
			OnAutoAnimationLoaded();
	}

	if(m_animationController.GetAnimation() ) 
	{
		RAnimationFrameController& frameController = m_animationController.GetFrameController();
		frameController.Update( NULL );

		// 애니메이션 정지되고 블렌딩도 정지되면 AutoUpdateContext를 확인해봄
		if( m_animationController.IsPlayDone() )
		{
			m_AutoUpdateContext.m_bPlayingAnimation = false;
			SetAutoUpdate(m_AutoUpdateContext.CheckAutoUpdateContext());
		}
	}
}

void RActor::OnUpdate()
{
	RPFC_THISFUNC;
	if(!m_pMesh) return;
	UpdateAnimationFrameAndEvent();
}


void RActor::AddActorNode(RActorNode* pActorNode, RSceneNode* pParent)
{
	pActorNode->m_pActor = this;

	//////////////////////////////////////////////////////////////////////////
	//
	// - 테이블에 등록
	// - 부모와의 계층
	// - 액터노드에 더해짐 알려주기
	//
	//////////////////////////////////////////////////////////////////////////

	// vector table 에 넣기
	// vector table은 계층순서로 정렬되어있다고 가정된다.
	m_actorNodes.push_back(pActorNode);

	// map table에 넣기
	if(m_actorNodeMap.find(pActorNode->GetNodeName()) != m_actorNodeMap.end())
	{
		mlog_tool("duplicated actor node name ( %s ) node : mesh %s \n",pActorNode->GetNodeName().c_str(), pActorNode->m_pMeshNode->m_Name.c_str());
	}
	m_actorNodeMap.insert(RACTORNODEMAP::value_type(pActorNode->GetNodeName(),pActorNode));

//	_ASSERT(pActorNode->GetName()!="Bone_stick_01");

	// 부모에 더해주기
	pActorNode->ResetTransform();
	if(pParent)
	{
		pParent->AddChild(pActorNode);
		pParent->AddToSceneNodeGroup(pActorNode);
	}

	_ASSERT(pParent);

	// 액터노드 더해짐 알리기
//	pActorNode->OnAddedToActor();
}

RActorNode* RActor::AddMeshNode(RMeshNode* pMeshNode, RActorNode* pParentNode, const char* szActorNodeName, bool bBackground, bool bNotifyToController)
{
//	TIMEPROFILE_THIS_FUNCTION;

	const char* szNewActorNodeName = szActorNodeName ? szActorNodeName : pMeshNode->GetName();

	RActorNode* pActorNode = GetActorNode(szNewActorNodeName);
	if(pActorNode)	// 있으면 삭제
		DeleteActorNode(pActorNode);

	// 새로 만든다
	pActorNode = NewActorNode(pMeshNode);
	pActorNode->m_strNodeName = szNewActorNodeName;

	if(pParentNode)
		AddActorNode(pActorNode ,pParentNode);
	else
		AddActorNode(pActorNode ,this);

	pActorNode->Create();
	bool bOk = pActorNode->OnCreateDeviceDependentPart();
	_ASSERT(bOk);

	pActorNode->OnCreateMaterials(bBackground);

	// Extern 여부는 AddMeshNode를 호출하는 쪽에서 확인하여 처리한다.
	/*if(pActorNode->IsExtern())
		pActorNode->m_pMeshNode->m_pBaseMesh->AddRef();*/

	CreateRenderInfoAndNotifyToController(bNotifyToController);

	// 애니메이션 재빌드
	//m_animationController.RebuildLODBoneSets();
	RebuildAnimationOnActorNodeChanged();

	return pActorNode ;
}

bool RActor::DeleteActorNode(RActorNode* pActorNode, bool bNotifyToController)
{
//	TIMEPROFILE_THIS_FUNCTION;

	_ASSERT( pActorNode != NULL);

	// 딸린 액터 노드 분류
	CActorNodeDeletionVisitor visitor(this);
	pActorNode->Traverse(&visitor);
	
	std::vector<RActorNode*>			vecActorNodesToDelete;
	std::vector<RActorNode*>::iterator itrActor;

	vecActorNodesToDelete = visitor.GetDeleteNodes();

	// Traverse가 std::vector로 순서가 보장되게 되었고
	// 자식부터 탐색하기 때문에 앞에서 부터 순서대로 지우면 된다!

	// 레퍼런스 카운트 등 맞추기 위해 destroy 먼저 호출
	for( itrActor = vecActorNodesToDelete.begin(); itrActor != vecActorNodesToDelete.end(); ++itrActor )
	{
		RActorNode* pDeleteActorNode = *itrActor;
		pDeleteActorNode->Destroy();
	}

	// 실제로 삭제
	for( itrActor = vecActorNodesToDelete.begin(); itrActor != vecActorNodesToDelete.end(); ++itrActor )
	{
		RActorNode* pDeleteActorNode = *itrActor;

		vector<RActorNode*>::iterator itrVec;
		itrVec = remove( m_actorNodes.begin(), m_actorNodes.end(), pDeleteActorNode ); 
		m_actorNodes.erase( itrVec, m_actorNodes.end() ); 

		stdext::hash_map<string,RActorNode*>::iterator itrMap;
		itrMap = m_actorNodeMap.find(pDeleteActorNode->GetNodeName());
		m_actorNodeMap.erase( itrMap );

		// 전체 갱신이 아니라 지우기와 통지만 한다.
		//CreateRenderInfoAndNotifyToController();
		DeleteActorNodeAndNotifyToController(pDeleteActorNode, bNotifyToController);

		// 지울 액터 노드 실제로 지워주기
		// 액터노드를 지울때 액터노드가 더미 액터노드등을 자신의 소멸자에서 지울수 있으므로 액터 테이블이 재구성된후 실제로 지워준다.
		delete pDeleteActorNode;
	}

	RebuildAnimationOnActorNodeChanged();

	return true;
}

bool RActor::DeleteActorNode( const char* szNodeName, bool bNotifyToController )
{
	RActorNode* pActorNode = GetActorNode(szNodeName);
	if (pActorNode)
	{
		return DeleteActorNode(pActorNode, bNotifyToController);
	}
	return false;
}

bool RActor::SetAnimation( const char* szAnimation )
{
	if(!m_pMesh)
		return false;

	RAnimation* pAniSet = m_pMesh->GetAnimationMgr()->GetAnimation(szAnimation);
	if(!pAniSet)
		return false;

	return SetAnimation( pAniSet );
}

bool RActor::SetAnimation( RAnimation* pAnimation )
{
	if( !pAnimation) 
		return false;

	if( false == pAnimation->GetResource()->IsLoadingSucceeded() )
		return false;

	if( 0 == pAnimation->GetResource()->GetAniNodeCount())
		return false;

	RPFC_THISFUNC;

	RPLAYINFO playInfo;
	m_animationController.SetAnimation(pAnimation, &playInfo );

	SetLocalAABB( pAnimation->GetResource()->GetMaxAniBoundingBox() );

	Play();

	return true;
}

RAnimation* RActor::GetAnimation(const char *szAnimationName)	// 애니메이션을 얻어낸다
{
	if(!m_pMesh)
		return NULL;

	return m_pMesh->GetAnimationMgr()->GetAnimation(szAnimationName);
}

void RActor::SetSpeed(float s)
{
	m_animationController.GetFrameController().SetSpeed( s );
}

float RActor::GetSpeed()
{
	return m_animationController.GetFrameController().GetSpeed();
}

bool RActor::IsPlayDone()
{
	return m_animationController.IsPlayDone();
}

bool RActor::IsOncePlayDone()
{
	return m_animationController.IsOncePlayDone();
}

RAnimation* RActor::GetAnimation()
{
	return m_animationController.GetAnimation();
}

void RActor::OnRestore()
{
//	m_RenderQuery.Create( RGetDevice() );
}

void RActor::OnInvalidate()
{
//	m_RenderQuery.Destroy();
}

// 액터를 새로 만들때 재구축 한다.
void RActor::CreateRenderInfoAndNotifyToController(bool _bNotifyToController)
{
	TIMEPROFILE_THIS_FUNCTION;

	//////////////////////////////////////
	// 0. 컬링리스트에서 지우기
	// 1. sort node
	// 2. make lod bone set
	// 3. 컬링리스트에 다시 등록
	//////////////////////////////////////

	RemoveFromBVH();

	////////////////////////////////////////////////////////////////////////////////////////////////////
	// sortnodes
	//
	m_sortedRenderableActorNode.clear();

	deque<RActorNode*> deqNodes, deqPhysiqueNodes, deqAlphaNodes, deqPhysiqueAlphaNodes;

	int nActorNodeSize = GetActorNodeCount();
	for(int i=0;i< nActorNodeSize;++i)
	{
		RActorNode* pNode = m_actorNodes[i];
//		mlog("%d , %s \n",i,pNode->m_pMeshNode->m_Name.c_str());
		_ASSERT(pNode);
		if(!pNode->m_pMeshNode->isRenderAble())
			continue;

		if(pNode->m_pMeshNode->hasAlphaMaterial())
		{
			if(pNode->m_pMeshNode->isHWPhysique())
				deqPhysiqueAlphaNodes.push_front(pNode);
				//m_alphaPhysiqueNodes.push_front(pNode);
			else
				//m_alphaNodes.push_back(pNode);
				deqAlphaNodes.push_back(pNode);
		}
		else if(pNode->m_pMeshNode->isHWPhysique())
			//m_physiqueMeshNodes.push_back(pNode);
			deqPhysiqueNodes.push_back(pNode);
		else
			//m_nonPhysiqueMeshNodes.push_back(pNode);
			deqNodes.push_back(pNode);
	}

	// physique - phsyique_alpha - non_physique - non_phsyique_alpha
	// 알파를 사용하는 노드를 뒤로 밀어넣는다
	m_sortedRenderableActorNode.insert(m_sortedRenderableActorNode.end(), deqPhysiqueNodes.begin(), deqPhysiqueNodes.end() );
	m_sortedRenderableActorNode.insert(m_sortedRenderableActorNode.end(), deqPhysiqueAlphaNodes.begin(), deqPhysiqueAlphaNodes.end() );
	m_sortedRenderableActorNode.insert(m_sortedRenderableActorNode.end(), deqNodes.begin(), deqNodes.end() );
	m_sortedRenderableActorNode.insert(m_sortedRenderableActorNode.end(), deqAlphaNodes.begin(), deqAlphaNodes.end() );

	// 애니메이션에 필요한 뼈를 모은다
	// 필요한지 여부는 액터노드가 생성될떄 필요한 뼈의 boneRefCount 를 올려놨기 때문에 
	// boneRefCount 를 조사하여 0이 아닌지 조사하면 알수 있다.
	m_animationReferencedNodes.clear();
	for(size_t i=0;i<m_actorNodes.size();++i) 
	{
		RActorNode* pNode = m_actorNodes[i];
		if(pNode->GetBoneRefCount()>0)
		{
			m_animationReferencedNodes.push_back(pNode);
		}
	}

	// controller 재구성
	if (_bNotifyToController)
		ReNewContollers();


	//3. 컬링리스트에 다시 등록
	AddToBVH();

	PreCreateShader();
}

// 이미 있는 상태에서 지우기만 한다.
void RActor::DeleteActorNodeAndNotifyToController(RActorNode* pActorNode, bool _bNotifyToController)
{
	RemoveFromBVH();

	vector<RActorNode*>::iterator itrVec;
	itrVec = remove( m_sortedRenderableActorNode.begin(), m_sortedRenderableActorNode.end(), pActorNode ); 
	m_sortedRenderableActorNode.erase( itrVec, m_sortedRenderableActorNode.end() ); 

	itrVec = remove( m_animationReferencedNodes.begin(), m_animationReferencedNodes.end(), pActorNode ); 
	m_animationReferencedNodes.erase( itrVec, m_animationReferencedNodes.end() ); 

	// controller 재구성
	if (_bNotifyToController)
		ReNewContollers();

	AddToBVH();
}

bool RActor::CreateFromXML(MXmlElement &element, bool bBackgroundCreation)
{
	char szBuffer[256];

	MXmlElement *pElement = element.FirstChildElement("FILENAME");
	_Contents(szBuffer,pElement);
	string meshFileName(szBuffer);

	if(element.FirstChildElement("COLLISION_FREE") )
	{
		m_bExistCollisionTree = false;
	}

	// TODO: CreateFromXML로 로딩하는 액터는 씬액터라고 가정합니다.
	m_bSceneActor = true;

	if(element.FirstChildElement("ANI") )
	{
		m_bAutoAnimationEnable = true;
		m_AutoUpdateContext.m_bMustAutoUpdate = true;
		SetAutoUpdate(m_AutoUpdateContext.CheckAutoUpdateContext());
	}

	if( MXmlElement *pDistortion = element.FirstChildElement("DISTORTION") )
	{
		SetDistortion( true );
		m_pDistortionParam->CreateToXML(*pDistortion);
		m_pDistortionParam->LoadNormalMap(bBackgroundCreation);
	}
	else
		SetDistortion( false );


	if( element.FirstChildElement("SHADOWRECEIVER") )
		m_bShadowReceiver = true;

//	SetUsable(!bBackgroundCreation);
	return Create(meshFileName.c_str(), bBackgroundCreation);

}

string RActor::GetMeshName()
{
	if (m_pMesh == NULL) return string("");
	std::string strFileName(m_pMesh->GetName());
	int token_path = strFileName.rfind("/") + 1;
	return strFileName.substr(token_path, strFileName.length() - token_path);
}

void RActor::SaveToXML(MXmlElement &element)
{
	if(!m_pMesh) return;

	// 저장할 내용 : 메쉬 이름, 애니메이션,프레임 등등
	MXmlElement *pElement = new MXmlElement("FILENAME");

	string strMeshName = GetMeshName();
	strMeshName = MGetPureFileNameExt( strMeshName );
	_SetContents(pElement, strMeshName );

	element.LinkEndChild(pElement);

	if( m_bExistCollisionTree == false )
	{
		MXmlElement *pCollisionFreeElement = new MXmlElement("COLLISION_FREE");
		element.LinkEndChild( pCollisionFreeElement );
	}

	if( m_bAutoAnimationEnable )
	{
		MXmlElement *pAutoAnimationElement = new MXmlElement("ANI");
		element.LinkEndChild( pAutoAnimationElement );
	}

	if( IsDistortion() )
	{
		MXmlElement *pElement = new MXmlElement("DISTORTION");
		element.LinkEndChild(pElement);

		m_pDistortionParam->SaveToXML(*pElement);
	}

	if( IsShadowReceiver() )
	{
		MXmlElement *pElement = new MXmlElement("SHADOWRECEIVER");
		element.LinkEndChild(pElement);
	}

}

RActorNode* RActor::GetActorNode(const char* szNodeName)
{
	stdext::hash_map<string,RActorNode*>::iterator itr = m_actorNodeMap.find(string(szNodeName));
	if(itr!=m_actorNodeMap.end())
		return itr->second;
	return NULL;
}

bool RActor::GetNodePosition(char* nodename, RVector& vPos)
{
	RActorNode* pActorNode = GetActorNode(nodename);
	if(!pActorNode) return false;
	pActorNode->ForceUpdateAnimation();

	vPos.x = pActorNode->m_matLocal->_41;
	vPos.y = pActorNode->m_matLocal->_42;
	vPos.z = pActorNode->m_matLocal->_43;

	return true;
}

void RActor::RenderSkeleton()
{
	if(!m_pMesh) return;

	RDevice *pDevice = REngine::GetDevicePtr();

	pDevice->ShaderOff();
	pDevice->SetLighting(false);
	pDevice->SetTexture(0,R_NONE);
	pDevice->SetTexture(1,R_NONE);
	pDevice->SetTextureStageState( 0, RTSS_COLOROP, RTOP_SELECTARG1);
	pDevice->SetTextureStageState( 0, RTSS_COLORARG1, RTA_TFACTOR);
	pDevice->SetTextureStageState( 1, RTSS_COLOROP, RTOP_DISABLE );
	pDevice->SetTextureStageState( 1, RTSS_ALPHAOP, RTOP_DISABLE );

	pDevice->SetTransform(RST_WORLD,RMatrix::IDENTITY);
	pDevice->SetFvF(D3DFVF_XYZ | D3DFVF_DIFFUSE);

	if(m_pMesh->m_nFileVersion>=EXPORTER_MESH_VER10)
	{
		for(unsigned int i=0;i<m_actorNodes.size();++i)
		{
			RActorNode* pNode = m_actorNodes[i];
			if(pNode->m_pMeshNode->GetFlag(RM_FLAG_BONE_MESH))
			{
				if(pNode->GetBoneRefCount()==0)
					pNode->ForceUpdateAnimation();

				if(pNode->m_pMeshNode->m_nParentNodeID!=-1 && !pNode->QueryAttribute(RSNA_NO_DEBUG_RENDERING))
				{
					RActorNode* pParent = GetActorNodes()[pNode->m_pMeshNode->m_nParentNodeID];

					RVector curBone = RVector(0,0,0) * pNode->GetWorldTransform();
					RVector parentBone = RVector(0,0,0) * pParent->GetWorldTransform();

					const float fSize = 2.f;
					RBoundingBox bbox = pNode->m_pMeshNode->m_boundingBox;
					RRenderHelper::RenderBox( pNode->m_matResult * pNode->m_pActor->GetWorldTransform(), bbox,m_dwBoneColor[i]);

					DWORD color = 0xffffffff;

					RRenderHelper::RenderLine(RMatrix::IDENTITY,curBone,parentBone,color);					
					RRenderHelper::RenderAxis(pNode->GetWorldTransform(),10);
				}
			}
		}
	}else
	{
		for(int i=0;i<RBIPID_END;++i)
		{
			RBIPID bipid = (RBIPID)i;
			RBIPID bipparentid = RSkeleton::GetBipParent((RBIPID)i);

			if(RSkeleton::GetBipParent((RBIPID)i)==RBIPID_UNKNOWN) continue;
	//		RAnimationNode* pANode = m_pAnimationNodeTable[i];
	//		if(!pANode) continue;

			RActorNode* pActorNode = GetActorNode( RSkeleton::GetBipName(bipid) );
			if(!pActorNode) continue;

			RVector curBone = RVector(0,0,0) * pActorNode->GetWorldTransform();
			RVector parentBone = RVector(0,0,0) * GetActorNode( RSkeleton::GetBipName(RSkeleton::GetBipParent(bipid)) )->GetWorldTransform();

			const float fSize = 2.f;
			RBoundingBox box;
			box.vmax = curBone + RVector(fSize,fSize,fSize);
			box.vmin = curBone - RVector(fSize,fSize,fSize);
			RRenderHelper::RenderBox(RMatrix::IDENTITY,box,m_dwBoneColor[i]);

			if(RSkeleton::GetBipParent((RBIPID)i)==RBIPID_ROOT) continue;	// pelvis & prop1

			DWORD color = 0xffffffff;

			if(i==RBIPID_RFOREARM) color = 0xffff0000;
			if(i==RBIPID_PELVIS) color = 0xff00ff00;

			RRenderHelper::RenderLine(RMatrix::IDENTITY,curBone,parentBone,color);
		}
	}
}


bool RActor::OnPick(RPICKINFO& pickInfo)
{
//	return false;	// 잠시보류

	if(!GetAABB().ContainsLineSegment(pickInfo.inPickOrigin,pickInfo.inPickTo)) 
		return false;

	bool bPicked = false;
	for(int i=0;i<GetActorNodeCount();++i) 
	{
		RActorNode* pNode = m_actorNodes[i];
		if(!pNode->m_pMeshNode->isRenderAble()) 
			continue;
		bPicked |= pNode->Pick(pickInfo);
	}

	return bPicked;
}

bool RActor::PickActorNode(RPICKINFO& pickInfo)
{
	return OnPick(pickInfo);
}

void RActor::SetFrame(unsigned int nFrame)					// 특정프레임으로 세팅
{
	m_animationController.GetFrameController().SetFrame( min(nFrame,m_animationController.GetFrameController().GetMaxFrame() ) );
}

/// update for render 가 이미 호출 되었는지 여부
bool RActor::CheckAlreadyUpdatedForRender()
{
	return ( m_nResultTransformUpdatedFrame == REngine::GetDevice().GetFrameCount() );
}

void RActor::UpdateForRender()
{
	// TODO: 정리해서 없애자
#ifdef _DEBUG
	if( m_bUsingActorPassRenderer )
		_ASSERT( GetUsingPass()==RRP_ACTOR );
	else
		_ASSERT( GetUsingPass()!=RRP_ACTOR );
#endif

	// 같은 프레임에서 두번 업데이트 되지 않는다.
	if( !CheckAlreadyUpdatedForRender() )
	{
		RPFC_THISFUNC;

		m_nResultTransformUpdatedFrame = REngine::GetDevice().GetFrameCount();

		if( IsUsingVertexAnimation() ||  (NULL != m_animationController.GetAnimation()) )
		{
 			UpdateAnimation();				// 기본 애니메이션 업데이트
		}

		UpdateAnimationReferenceNodes();	// 최종 행렬 계산

		// Collision Update
		// UpdateTransformAndVertex 이후 시점이어야 한다.
		if(m_pCollision)
			m_pCollision->Update(this);
	}
}


void RActor::UpdateAnimationReferenceNodes()
{
	RPFC_THISFUNC;
	for(size_t i=0;i<m_animationReferencedNodes.size();++i)
	{
		RActorNode* pActorNode = m_animationReferencedNodes[i];
		pActorNode->UpdateAllParentDependentValues();	// 계층 업데이트
		pActorNode->UpdateFinalResultTransforms();
	}
}


void RActor::OnPreRender( RCameraSceneNode* pCurrentCamera)
{
	RPFC_THISFUNC;

	_ASSERT( pCurrentCamera);

	if( GetVisible() == false || GetOcclued() )
		return;

	UpdateForRender();

	for(size_t i=0;i< m_sortedRenderableActorNode.size() ;++i)
	{
		RActorNode* pActorNode = m_sortedRenderableActorNode[i];
		if( IsRenderable( pActorNode ) )
		{
			pActorNode->PreRender(pCurrentCamera);
			pActorNode->UpdateTransformAndVertex();		// ActorNode 단위 업데이트(행렬 등)
		}
	}
}

void RActor::OnRender( RCameraSceneNode* pCurrentCamera, int nSubMaterialID, RRENDERINGMETHOD eMothod)
{
	RPFC_THISFUNC;
	_ASSERT( pCurrentCamera);
	RenderSingle( pCurrentCamera, eMothod);
}

void RActor::RenderShadowMap( RCameraSceneNode* pCurrentCamera, RShadowStrategy* pShadowStrategy, RActorNodeRenderStrategy* pRenderStrategy )
{
	if(!m_bUsingActorPassRenderer)
		return;

	RPFC_THISFUNC;

	for(size_t i=0; i<m_sortedRenderableActorNode.size(); ++i)
	{
		RActorNode* pActorNode = m_sortedRenderableActorNode[i];

		if(pActorNode->CheckDoRender()==false)
			continue;

		pActorNode->RenderShadowMap(pCurrentCamera, pShadowStrategy, pRenderStrategy);
	}
}

void RActor::UpdateAnimation()
{
	RPFC_THISFUNC;

	m_animationController.Update(NULL);

	UpdateMaterialAnimation();
}

void RActor::UpdateMaterialAnimation()
{
	RPFC_THISFUNC;
	// material uv animation update
	// RMaterial Node 를 Texture Matrix로 ..
	if(!m_pMesh) return;

	int nMtrlCnt = m_pMesh->GetMaterialResourceCount();
	for ( int i = 0; i<nMtrlCnt; ++i )
	{
		// 08.04.21
		RMaterial *pMtrl = m_pMesh->GetMaterialResource(i)->m_pMaterial;
		if ( pMtrl->IsUseUVAnimation() )
		{
			int nFrame = m_animationController.GetFrameController().GetFrame();
			pMtrl->UpdateUVAnimation(nFrame);
		}
	}
}


bool RActor::IsRenderable( RActorNode* pActorNode )
{
	RMeshNode* pMeshNode = pActorNode->m_pMeshNode;
	if(pMeshNode == NULL)
		return false;

	if( // 메시속성에 의해
		pMeshNode->isRenderAble()
		&& pMeshNode->m_pVb && pMeshNode->m_pIb
		&& pMeshNode->m_pMtrlTableInfo
		// 액터노드 속성에 의해
		&& pActorNode->GetVisibilityResult() > 0.0f && pActorNode->GetVisible()
		// 액터 속성에 의해
		&& GetNoiseRefResult() > 0.0f
	)
	{
		return true;
	}

	return false;
}

int RActor::GetFrame()
{
	return m_animationController.GetFrameController().GetFrame();
}

const RMatrix& RActor::GetWorldInverseTransform()	// 월드의 역행렬 
{
	return m_matInvWorld;
}


const RQuaternion& RActor::GetWorldInverseRotation()	
{
#ifdef USING_RSC_PHYSIQUE_QT
	return m_qInvWorldRot;
#else
	RVector3 vInvWorldScale, vInvWorldTran;
	static RQuaternion qInvWorldRot;
	m_matInvWorld.Decompose( vInvWorldScale, vInvWorldTran, qInvWorldRot);
	return qInvWorldRot;
#endif
}

const RVector3& RActor::GetWorldInverseTranslation()	
{
#ifdef USING_RSC_PHYSIQUE_QT
	return m_vInvWorldTran;
#else
	RVector3 vInvWorldScale;
	static RVector3 vInvWorldTran;
	RQuaternion qInvWorldRot;
	m_matInvWorld.Decompose( vInvWorldScale, vInvWorldTran, qInvWorldRot);
	return vInvWorldTran;
#endif
}

const RVector3& RActor::GetWorldInverseScale()
{
#ifdef USING_RSC_PHYSIQUE_QT
	return m_vInvWorldScale;
#else
	static RVector3 vInvWorldScale;
	RVector3 vInvWorldTran;
	RQuaternion qInvWorldRot;
	m_matInvWorld.Decompose( vInvWorldScale, vInvWorldTran, qInvWorldRot);
	return vInvWorldScale;
#endif
}

vector<RActorNode*>& RActor::GetAnimationReferrencedNodes()
{
	return m_animationReferencedNodes;
}

vector<RActorNode*>& RActor::GetSortedRenderableNodes()
{
	return m_sortedRenderableActorNode;
}

void RActor::CollectStaticCollisionPolygons(RSceneNodePolygonCollector* pCollector) const
{
	RActor::CollectStaticCollisionPolygonsDo(pCollector, RCollisionTriangle::MESH);
}

void RActor::CollectStaticCollisionPolygonsDo(RSceneNodePolygonCollector* pCollector, DWORD dwAttribute) const
{
	// tool 에서는 toolactor의 것이 사용된다. 따라서, 이 코드는 아마 사용되지 않을 것임.
//	if(!GetVisible()) 
//		return;

	if(!GetCollisionCreationEnable())
		return;

	if(m_pMesh->m_nFileVersion<EXPORTER_MESH_VER12)
	{
		string strMessage = string(m_pMesh->GetName()) + "이전파일입니다. 익스포터버그로 충돌정보를 만들수 없습니다. 다시 익스포트 부탁해요.";
		MessageBox(NULL,strMessage.c_str(),"I'm sorry.",MB_OK);
		return;
	}

	// 실제 메시 정보를 pTree
	for(int m=0;m<GetActorNodeCount();++m)
	{
		RActorNode* pActorNode = const_cast< RActor* >(this)-> GetActorNodes()[m];
		//if(!pActorNode->GetVisible())
		//	bVisible = false;

		RMeshNode* pMeshNode = pActorNode->m_pMeshNode;
		// 보이는건 상관없이 충돌 속성이 있으면 출력해준다.
		if( ( pMeshNode->FindUserProperty("nocollision")) || ( false == pMeshNode->isCollisionMesh() ) )
			continue;

		RMatrix matWorld = pMeshNode->m_matBase * GetWorldTransform();

		for(int i=0;i<pMeshNode->m_nFaceCnt;++i)
		{
			RMeshPolygonData* pPolygonData = &pMeshNode->m_pPolygonTable[i];

			// 마테리얼이 없는 메시는 충돌 메시로 사용하지 않는다.
			const RMaterialResource* pMaterialResource = m_pMesh->GetMaterialResource(pPolygonData->mtrl_id);
			if( pMaterialResource == NULL )
				continue;

			RMaterial* pMaterial = pMaterialResource->m_pMaterial;
			if(pMaterial->CheckFlag(RM_FLAG_ADDITIVE)) 
				continue;
			
			for(int j=0;j<(pPolygonData->nVertices-2);++j)
			{
				RVector pos[3];
				pos[0] = pMeshNode->m_pPointTable[pPolygonData->index[0].p] * matWorld;
				pos[1] = pMeshNode->m_pPointTable[pPolygonData->index[j+1].p] * matWorld;
				pos[2] = pMeshNode->m_pPointTable[pPolygonData->index[j+2].p] * matWorld;

				pCollector->OnAddPolygon(pos[0],pos[1],pos[2], dwAttribute, 0, 0);
			}
		}
	}
}

void RActor::LoadAutoAnimation()
{
	string strAnimationName( MGetPureFileName(m_pMesh->GetName()) );
	m_pAutoAnimation = m_pMesh->GetAnimationMgr()->GetAnimation(strAnimationName.c_str());

	if ( m_pAutoAnimation != NULL && m_pAutoAnimation->GetResource()->IsLoadingSucceeded() )
	{
		//이미 로딩이 되어있으면 다시 로딩 걸어 줄 필요가 없다.
		return;
	}

	string anifilename = m_pMesh->GetName() + ".ani";

	//로딩 걸어줌
	m_pAutoAnimation = m_pMesh->GetAnimationMgr()->Add(strAnimationName.c_str(), anifilename.c_str(), m_pMesh->IsBackgroundCreated(), false, this ) ;
}

void RActor::SetForceAnimationControlPlayDoneForTest( bool bPlayDone )
{
	m_animationController.GetFrameController().SetForceAnimationControlPlayDoneForTest( bPlayDone );
}

void RActor::SetForceAnimationControlLoopForTest( bool bLoop )
{
	m_animationController.GetFrameController().SetLoop( bLoop );
}

bool RActor::IsLoadingCompleted()
{
	if( m_pMesh && !m_pMesh->IsLoadingCompleted() ) return false;

	for( vector< RActorNode* >::iterator itr = m_actorNodes.begin(); itr != m_actorNodes.end(); ++itr )
	{
		RActorNode* pActorNode = *itr;
		if (pActorNode)
		{
			if (pActorNode->IsLoadingCompleted() == false) 
				return false;
		}
	}

	if( m_pMesh && !m_pMesh->GetAnimationMgr()->IsLoadingCompleted() ) return false;
	return true;
}

bool RActor::SetLocalAABB( const RBoundingBox* pAABB )
{
	if( pAABB )
	{
		m_bIsMaxAniAABB = true;
		m_aabbLocal = *pAABB;
		NeedUpdateBoundingVolume();
		return true;
	}

	m_bIsMaxAniAABB = false;
	return false;
}

void RActor::GetSamplingNodeData( const char* szNodeName, unsigned int nStartFrame, unsigned int nEndFrame, int nSampleInterval, std::vector< RMatrix >& _rOUt )
{
	RActorNode * pActorNode = GetActorNode(szNodeName);
	if(pActorNode)
	{
		// 현재 프레임 가져오기
		int nCurrentFrame = GetFrame();

		// 시작 프레임이 총 프레임보다 크면 에러...
		if(nStartFrame > m_animationController.GetFrameController().GetMaxFrame() )
			return;

		// 샘플링
		for(unsigned int n = nStartFrame; n < nEndFrame; n += nSampleInterval)
		{
			SetFrame(n);

			pActorNode->ForceUpdateAnimation();
			_rOUt.push_back(pActorNode->GetWorldTransform());
		}

		// 마지막 프레임을 샘플링 못하므로 여기서 처리한다.
		SetFrame(nEndFrame);
		pActorNode->ForceUpdateAnimation();
		_rOUt.push_back(pActorNode->GetWorldTransform());

		// 프레임을 원상태로 돌린다.
		SetFrame(nCurrentFrame);
		pActorNode->ForceUpdateAnimation();
	}
}

bool RActor::IsUsingVertexAnimation()
{
	return m_bUsingVertexAnimation;
}

void RActor::SetUseVertexAnimation( bool bUse )
{
	m_bUsingVertexAnimation = bUse;
}

void RActor::RebuildAnimationOnActorNodeChanged()
{
	// 애니메이션 리빌드
// 	m_animationController.RebuildOnActorNodeChanged();
	m_animationController.ReNew();
}

void RActor::PreCreateShader()
{
	for( size_t i = 0 ; i < m_sortedRenderableActorNode.size(); ++i )
	{
		m_sortedRenderableActorNode[i]->PreCreateShader( REngine::GetConfig().m_bBackgroundCreateShaderComposite );
	}
}

bool RActor::HasClothActorNode( void )
{
	for(unsigned int i=0;i<m_actorNodes.size();++i)
	{
		if( true == m_actorNodes[i]->m_pMeshNode->isClothMesh() )	return true;
	}
	return false;
}


void RActor::EmptyEnvironment()
{
	memset( m_szEnvTexName, 0, sizeof(m_szEnvTexName) );
	REngine::GetDevice().DeleteTexture(m_pEnvironmentTexture);
	m_pEnvironmentTexture = R_NONE;
}


void RActor::SetEnvironmentTexture( const char* _szEnvTexName )
{
	if( strcmp( m_szEnvTexName, _szEnvTexName ) == 0 )
		return;

	strncpy( m_szEnvTexName, _szEnvTexName, strlen( _szEnvTexName ) + 1 );

	RTexture* pTexture = REngine::GetDevice().CreateTextureForBuildLater(_szEnvTexName, RTF_LINEAR, 
		RCT_CLAMP | RCT_AUTOGENMIPMAP | RCT_CUBEMAP, RFMT_A8R8G8B8, false);

	_ASSERT( pTexture );
	m_pEnvironmentTexture = pTexture;
}


void RActor::AddToRenderingList( RRenderingList* pRenderingList )
{
	if( !RActor::IsLoadingCompleted() )
	{
		pRenderingList->AddNullActor(this);
		return;
	}

	if( !m_bUsingActorPassRenderer )
	{
		pRenderingList->Add(this,GetUsingPass());
		return;
	}

	if( m_bVertexMotionBlur )	// 해당 속성의 집합에 추가한다.
		pRenderingList->Add( this, RRP_VBLUR);
	if( m_nPixelMotionBlur )
		pRenderingList->Add( this, RRP_PBLUR);
	if( m_bAccumMotionBlur )
		pRenderingList->Add( this, RRP_ABLUR);
	if( m_bEdge )
		pRenderingList->Add( this, RRP_EDGE);
	if( m_bPEEdge )
		pRenderingList->Add( this, RRP_POSTEFFECT_EDGE);


	if( m_bAlpha2PassRendering ) // 깨지지 않는 알파2패스 렌더링
	{
		pRenderingList->AddAlpha2PassActor(this);
		return;
	}

	if(IsDistortion())
		pRenderingList->Add(this, RRP_DISTORTION);


	for(size_t i=0;i< m_sortedRenderableActorNode.size() ;++i)
	{
		RActorNode* pActorNode = m_sortedRenderableActorNode[i];

		string nodeName = pActorNode->m_pMeshNode->GetName();
		if(nodeName.find("LOD") != string::npos) 
		{
			if(isdigit(nodeName.at(4)))
				continue;
		}

		if( !pActorNode->IsAlpha())
		{
			if(!pActorNode->CheckRenderPerMaterialToShadowMap())
			{
				RShadowActorNodeKey key;
				key.m_pMeshNode = pActorNode->m_pMeshNode;
				key.m_nSubMaterial = -1;
				key.m_pMaterial = pActorNode->GetSubMaterial(0);

				if(GetUseDoubleShadow())
					pRenderingList->AddDoubleShadowActorNode(pActorNode, key);
				else
					pRenderingList->AddShadowActorNode(pActorNode, key);
			}

			for(size_t j=0; j< pActorNode->m_vecMaterialTable.size(); ++j)
			{
				//key만들기
				RActorNodeKey key;
				key.m_pMaterial = pActorNode->GetSubMaterial(j);
				key.m_nSubMaterial = j;
				key.m_nNodeType = pActorNode->GetActorNodeType();
				if( pActorNode->m_pMeshNode && pActorNode->m_pMeshNode->m_pVb && pActorNode->m_pMeshNode->m_pVb->m_hVF)
					key.m_hVF = pActorNode->m_pMeshNode->m_pVb->m_hVF;
				key.m_pMeshNode = pActorNode->m_pMeshNode;

				if( key.m_pMaterial->CheckFlag(RM_FLAG_ADDITIVE) )
				{
					pRenderingList->AddAlphaAdditiveActorNode(pActorNode,key);
				}
				else if( key.m_pMaterial->CheckFlag(RM_FLAG_USEOPACITY) )
				{
					pRenderingList->AddAlphaActorNode( pActorNode,key);
				}
				else
				{
					pRenderingList->AddActorNode(pActorNode,key);

					if(pActorNode->CheckRenderPerMaterialToShadowMap())
					{
						RShadowActorNodeKey key;
						key.m_pMeshNode = pActorNode->m_pMeshNode;
						key.m_nSubMaterial = j;
						key.m_pMaterial = pActorNode->GetSubMaterial(j);

						if(GetUseDoubleShadow())
							pRenderingList->AddDoubleShadowActorNode(pActorNode, key);
						else
							pRenderingList->AddShadowActorNode(pActorNode, key);
					}
				}
			}
		}
		else
		{
			RActorNodeKey fakekey;
			fakekey.m_pMaterial = (RMaterial*)-1;
			fakekey.m_pMeshNode = (RMeshNode*)-1;
			fakekey.m_nNodeType = -1;
			fakekey.m_hVF = -1;
			fakekey.m_nSubMaterial = -1;
			pRenderingList->AddAlphaActorNode( pActorNode,fakekey);
		}		
	}			
}

void RActor::RemoveFromRenderingList( RRenderingList* pRenderingList )
{
	if( pRenderingList->IsInNullActor(this) )
	{
		pRenderingList->RemoveNullActor(this);
	}
	else
	{
		if( !m_bUsingActorPassRenderer )
		{
			pRenderingList->Remove(this,GetUsingPass());
		}
		else
		{
			if( m_bAlpha2PassRendering )
			{
				pRenderingList->RemoveAlpha2PassActor(this);
			}
			else
			{
				for(size_t i=0;i< m_sortedRenderableActorNode.size() ;++i)
				{
					RActorNode* pActorNode = m_sortedRenderableActorNode[i];
					pRenderingList->RemoveActorNode(pActorNode);				
				}

				if(IsDistortion())
					pRenderingList->Remove( this, RRP_DISTORTION);
			}

			if( m_bVertexMotionBlur )
				pRenderingList->Remove( this, RRP_VBLUR);
			if( m_nPixelMotionBlur )
				pRenderingList->Remove( this, RRP_PBLUR);
			if( m_bAccumMotionBlur )
				pRenderingList->Remove( this, RRP_ABLUR);
			if( m_bEdge )
				pRenderingList->Remove( this, RRP_EDGE);
			if( m_bPEEdge )
				pRenderingList->Remove( this, RRP_POSTEFFECT_EDGE);
		}

	}
}

void RActor::SetAlpha2PassRendering( bool bAlpha2PassRender )
{
	if( m_bAlpha2PassRendering == bAlpha2PassRender )	return;

	RemoveFromBVH();

	m_bAlpha2PassRendering = bAlpha2PassRender;

	// 알파2패스 렌더링은 Visbility가 1이면 안된다.
	if( m_bAlpha2PassRendering )
	{
		if( GetVisibility() >= 1.f )
			SetVisibility( 0.9999f );
	}

	AddToBVH();
}

void RActor::SetVertexMotionBlur( bool b )
{
	if( m_bVertexMotionBlur == b ) return;

	RemoveFromBVH();

	m_bVertexMotionBlur = b;

	AddToBVH();
}


void RActor::SetEdgeColor( const RVector3& _vEdgeColor)
{ 
	m_vEdgeColor = _vEdgeColor;
	if( 0.f >=  m_vEdgeColor.x )
		m_vEdgeColor.x = 0.05f;
}


void RActor::EnablePEEdge( const RVector3& _vEdgeColor )
{
	RemoveFromBVH();

	m_bPEEdge			= true;

	SetEdgeColor( _vEdgeColor );

	RSceneManager* pSceneManager = REngine::GetSceneManagerPtr();
	RRenderingStrategy* pRenderingStrategy = pSceneManager->GetRenderingStrategy();
	RPostEffectManager* pPostEffectManager = pRenderingStrategy->GetPostEffectManager();
	pPostEffectManager->AddPEEdgeCount();


	AddToBVH();
}


void RActor::DisablePEEdge()
{
	RemoveFromBVH();

	m_bPEEdge			= false;

	RSceneManager* pSceneManager = REngine::GetSceneManagerPtr();
	RRenderingStrategy* pRenderingStrategy = pSceneManager->GetRenderingStrategy();
	RPostEffectManager* pPostEffectManager = pRenderingStrategy->GetPostEffectManager();
	pPostEffectManager->MinusPEEdgeCount();

	AddToBVH();
}



void RActor::EnableEdge( const RVector3& _color, float _fTickNess )
{
	RemoveFromBVH();

	m_bEdge			= true;
	m_vEdgeColor	= _color;
	m_fEdgeTickNess	= _fTickNess;

	RShaderCompositeManager::m_cEdgeColor.SetVector3( _color.x, _color.y, _color.z );

	AddToBVH();
}


void RActor::DisableEdge()
{
	RemoveFromBVH();

	m_bEdge			= false;

	AddToBVH();
}


void RActor::SetAccumMotionBlur( bool b )
{
	if( m_bAccumMotionBlur == b )	return;

	RemoveFromBVH();

	m_bAccumMotionBlur = b;

	AddToBVH();
}

void RActor::SetPixelMotionBlur( int b )
{
	if( m_nPixelMotionBlur == b) return;

	RemoveFromBVH();

	m_nPixelMotionBlur = b;

	AddToBVH();
}

void RActor::RenderAlpha( RCameraSceneNode *pCamera,int iSubMaterial, RRENDERINGMETHOD eMothod)
{
	RPFC_THISFUNC;
	RenderSingleAlpha2Pass( pCamera, eMothod);
}


void RActor::EnableAddColorEffect( const char* _szNodeName, const RVector3& _vColor )
{
	RActorNode* pActorNode = GetActorNode( _szNodeName );
	if( pActorNode )
		pActorNode->EnableAddColorEffect( _vColor );
}


void RActor::DisableAddColorEffect( const char* _szNodeName )
{
	RActorNode* pActorNode = GetActorNode( _szNodeName );
	if( pActorNode )
		pActorNode->DisableAddColorEffect();
}


} // end namespace rs3