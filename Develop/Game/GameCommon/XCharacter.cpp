#include "StdAfx.h"
#include "XDef.h"
#include "XCharacter.h"
#include "RMeshNode.h"
#include "RShaderEffectController.h"
#include "RSceneManager.h"
#include "RMeshMgr.h"
#include "RCameraSceneNode.h"
#include "RBackgroundWorker.h"
#include "RMaterialResource.h"
#include "XModelParts.h"
#include "RAnimationResource.h"
#include "RDirectionalLightSceneNode.h"
#include "XBaseAnimationTC.h"
#include "XRAnimationController.h"
#include "MProfiler.h"
#include "MLocale.h"

MImplementRTTI(XCharacter, RActor);

bool XCharacterSpineController::Create()
{
	m_pSpine1 = m_pActor->GetActorNode("Bip01 Spine1");
	m_pSpine2 = m_pActor->GetActorNode("Bip01 Spine2");
//	_ASSERT(m_pSpine1 && m_pSpine2);

	if(!m_pSpine1 || !m_pSpine2) return false;
	return true;
}

void XCharacterSpineController::SetAngle(float fSpineAngleVertical, float fSpineAngleHorizonal)
{
	m_fSpineVertical = fSpineAngleVertical;
	m_fSpineHorizonal = fSpineAngleHorizonal;
}

void XCharacterSpineController::Update()
{
	if(m_pActor->GetAnimationController().GetState()==RPS_STOP)
	{
		// 애니메이션이 되지않으면 리셋
		if(m_pSpine1)
			m_pSpine1->ResetTransform();
		if(m_pSpine2)
			m_pSpine2->ResetTransform();
	}

	// 모델의 앞방향을 기준으로 회전 ( 위 아래를 바라보도록한다 )

	RVector worldAxisVertical = m_pActor->GetRight();	// 월드의 right 를 기준으로 회전한다
	RVector worldAxisHorizonal = m_pActor->GetUp();		// 좌우로 돌리기

	RVector trAxisVertical; // 각 뼈대노드의 좌표계로 변환된 회전축
	RVector trAxisHorizonal;
	RMatrix matInv;

	// spine 1
	if(m_pSpine1)
	{
		(m_pSpine1->GetWorldTransform()).GetInverse(&matInv);
		RQuaternion spine1Rotation = m_pSpine1->GetLocalTransform();
		matInv.TransformNormal(worldAxisVertical,trAxisVertical);
		matInv.TransformNormal(worldAxisHorizonal,trAxisHorizonal);
		RQuaternion quatRotationVertical = RQuaternion(trAxisVertical,.5f*m_fSpineVertical);
		RQuaternion quatRotationHorizonal = RQuaternion(trAxisHorizonal,.5f*m_fSpineHorizonal);
		m_pSpine1->SetRotation( quatRotationHorizonal * quatRotationVertical * spine1Rotation);
		m_pSpine1->UpdateTransform();
	}

	// spine 2
	if(m_pSpine2)
	{
		(m_pSpine2->GetWorldTransform()).GetInverse(&matInv);
		RQuaternion spine2Rotation = m_pSpine2->GetLocalTransform();
		matInv.TransformNormal(worldAxisVertical,trAxisVertical);
		matInv.TransformNormal(worldAxisHorizonal,trAxisHorizonal);
		RQuaternion quatRotationVertical = RQuaternion(trAxisVertical,.5f*m_fSpineVertical);
		RQuaternion quatRotationHorizonal = RQuaternion(trAxisHorizonal,.5f*m_fSpineHorizonal);
		m_pSpine2->SetRotation( quatRotationHorizonal * quatRotationVertical * spine2Rotation);
		m_pSpine1->UpdateTransform();
	}

}

//////////////////////////////////////////////////////////////////////////
//------------------------------------------------------------------------
// XCharacter
XCharacter::XCharacter(void) 
: m_pSpineController(new XCharacterSpineController)
, m_dummy_loc_keyIteratorPosition(0)
, m_dummy_loc_keyIteratorRotation(0)
, m_bFakeResourceLoading(false)
, m_pAnimationController(NULL)
, m_bAnimationUpdated(false)
, m_bLoadingCompleted(false)
, m_pAnimationInfoMgr(NULL)
, m_bDoNotAnimationUpdate(false)
{
	AddController(m_pSpineController);

	m_fRadius = 0.0f;

	m_pParts = new XCharacterParts(this);

	// XCharacter는 AutoUpdate 안한다.
	m_AutoUpdateContext.m_bMustNotAutoUpdate = true;

	m_InteractionAABB.Initialize();
}

XCharacter::~XCharacter(void)
{
	Destroy();

	RemoveController(m_pSpineController);
	delete m_pSpineController;

	SAFE_DELETE(m_pParts);
}

void XCharacter::DestroyPartsNode(int nSlotID)
{
	m_pParts->DeleteParts(nSlotID);
}

void XCharacter::Destroy()
{
	m_pParts->Destroy();

//	RActor::Destroy();
}

void XCharacter::OnActorLoadComplete()
{
	RActor::OnActorLoadComplete();

	// 쓰레드에 안전한 부분
	m_pSpineController->Create();

	// 반지름 구하기
	if(m_pMesh)
	{
		RBox b;
		m_pMesh->GetStaticSkeletonBoundingBox(b);
		RVector c	= b.GetCenter();
		m_fRadius	= b.maxx - c.x;
	}

	// 인터렉션에 쓰일 aabb 구하기
	RefreshInteractionAABB();
}

void XCharacter::SetActorSpineTarget(float fAngle1,float fAngle2)
{
	float degree = DegreeToRadian(20);

	if( degree < fAngle1)
		fAngle1 = degree;
	if( fAngle1 < -degree)
		fAngle1 = -degree;

	float MAX_DEGREE_HORIZONAL = DegreeToRadian(80);
	if( MAX_DEGREE_HORIZONAL < fAngle2)
		fAngle2 = MAX_DEGREE_HORIZONAL;
	if( fAngle2 < -MAX_DEGREE_HORIZONAL )
		fAngle2 = -MAX_DEGREE_HORIZONAL;

	((XCharacterSpineController*)m_pSpineController)->SetAngle(fAngle1,fAngle2);
	m_pSpineController->Enable(true);
}


// bool XCharacter::GetMovingDummyPosition(RVector& pos)
// {
// 	RActorNode* pActorNode = GetUpdatedActorNode("dummy_loc");
// 	if(!pActorNode) return false;
// 
// 	pos.x = pActorNode->GetLocalTransform()._41;
// 	pos.y = pActorNode->GetLocalTransform()._42;
// 	pos.z = pActorNode->GetLocalTransform()._43;
// 	return true;
// }
// 
// bool XCharacter::GetMovingDummyDirection(RVector& dir)
// {
// 	RActorNode* pLocActorNode = GetUpdatedActorNode("dummy_loc");
// 	if(!pLocActorNode) return false;
// 
// 	dir = -pLocActorNode->GetDirection();
// 	return true;
// }

bool XCharacter::IsEnableGlowEffect()
{
	if (m_pShaderEffectController)
	{
		return m_pShaderEffectController->IsEnableGlowEffect();
	}

	return false;
}

void XCharacter::EnableGlowEffect(const RVector3& color)
{
	if( m_pShaderEffectController )
	{
		m_pShaderEffectController->EnableGlowEffect( color );
	}
}

void XCharacter::DisableGlowEffect()
{
	if( m_pShaderEffectController )
	{
		m_pShaderEffectController->DisableGlowEffect();
	}
}

void XCharacter::EnableAddColorEffect(const RVector3& color)
{
	if( m_pShaderEffectController )
	{
		m_pShaderEffectController->EnableAddColorEffect( color );
	}
}

void XCharacter::EnableAddColorEffect( const TCHAR* _szNodeName, const RVector3& color )
{
	RActor::EnableAddColorEffect(MLocale::ConvTCHARToAnsi(_szNodeName).c_str(), color);
}
void XCharacter::DisableAddColorEffect()
{
	if( m_pShaderEffectController )
	{
		m_pShaderEffectController->DisableAddColorEffect();
	}
}

void XCharacter::DisableAddColorEffect( const TCHAR* _szNodeName )
{
	RActor::DisableAddColorEffect(MLocale::ConvTCHARToAnsi(_szNodeName).c_str());
}

void XCharacter::OnUpdate()
{
	// animation lod.
// 	RMatrix matView = RDevice::GetDevice()->GetTransform(RST_VIEW);
// 	
// 	RVector trPos = GetWorldPosition() * matView;
// 
// 	float fDistance = -trPos.z;	// matrix 가 rh라 가정

	PFC_THISFUNC;

	RActor::OnUpdate();

	UpdatePartsNode();
	//m_pParts->Update();

	CheckLoadingCompleted();
}

void XCharacter::RemoveActorSpineController()
{
	m_pSpineController->Enable(false);
}


float XCharacter::GetMaxFrameToSec()
{
	if(m_pAnimationController)
		return m_pAnimationController->GetMaxFrameToSecond();

	return 0.0f;
}

float XCharacter::GetCurFrameToSec()
{
	if(m_pAnimationController)
		return m_pAnimationController->GetCurFrameToSecond();

	return 0.0f;
}

int XCharacter::GetSecToFrame( float fTime )
{
	float fSpeed = 0.0f;

	if(m_pAnimationController)
		fSpeed = m_pAnimationController->GetAnimationSpeed();

	return (int)(4.8f * fSpeed * fTime * 1000.0f);
}

float XCharacter::GetFrameToSec( int nFrame )
{
	float fSpeed = 0.0f;

	if(m_pAnimationController)
		fSpeed = m_pAnimationController->GetAnimationSpeed();

	return nFrame / 4.8f / (fSpeed * 1000.f);
}

void XCharacter::SetColor( unsigned int nColor )
{
	size_t nSize = m_actorNodes.size();
	for (size_t i = 0; i < nSize; i++)
	{
		m_actorNodes[i]->SetPartsColor(nColor);
	}
}

void XCharacter::SetColor( unsigned int nColor, const TCHAR* _szNodeName )
{
	RActorNode* pActorNode = GetActorNode(MLocale::ConvTCHARToAnsi(_szNodeName).c_str());
	if(pActorNode == NULL)
		return;

	pActorNode->SetPartsColor(nColor);
}

void XCharacter::CreatePartsNodeAsync( XPartsParam* pPartsParam )
{
	m_pParts->CreatePartsNodeAsync(pPartsParam);
}

void XCharacter::SetAnimationForMock()
{
	SetForceAnimationControlPlayDoneForTest(true);
}

bool XCharacter::CheckLoadingCompleted()
{
	m_bLoadingCompleted = IsLoadingCompleted();
	return m_bLoadingCompleted;
}

bool XCharacter::IsLoadingCompleted()
{
	// 시스템에서 리소스 로딩을 안한다면 그냥 true로 리턴한다(unit test 같은 경우)
	if (m_bFakeResourceLoading)
	{
		return true;
	}
	

	if (RActor::IsLoadingCompleted() == false) 
	{
		return false;
	}

	if (m_pParts->IsLoadingCompleted() == false) 
	{
		return false;
	}

	if (GetUsable() == false)
	{
		return false;
	}

	return true;
}

tstring XCharacter::GetReservedSlotMeshName( int nSlotID ) /* for test */
{
	return m_pParts->GetReservedSlotMeshName(nSlotID);
}

bool XCharacter::IsExistActorNode( tstring strDummyName )
{
	RActorNode* pActorNode = GetActorNode(MLocale::ConvTCHARToAnsi(strDummyName.c_str()).c_str());
	if(!pActorNode ||
		m_pAnimationController == NULL) return false;

	return m_pAnimationController->IsNodeController(strDummyName);
}

void XCharacter::GetAnimationEventData( vector<ANIMATION_EVENT>& vecAnimaitonEvent )
{
	for(vector<ANIMATION_EVENT>::iterator it = m_animationController.GetAnimationEvent().begin(); it != m_animationController.GetAnimationEvent().end(); it++)
	{
		vecAnimaitonEvent.push_back((*it));
	}	
}

RActorNode* XCharacter::GetAnimatedActorNodeDir( const TCHAR* szNodeName, RVector& dir, E_TRANSFORMSPACE eTransformSpace)
{
	RActorNode* pActorNode = m_pAnimationController->GetAnimatedActorNodeTransform(tstring(szNodeName), m_matAnimatedTemp, eTransformSpace);
	if (NULL != pActorNode)
	{
		RVector3 v;
		RQuaternion qOrient;
		m_matAnimatedTemp.Decompose(v, v, qOrient);
		m_matAnimatedTemp = qOrient.Conv2Matrix();

		dir.Set(m_matAnimatedTemp._21, m_matAnimatedTemp._22, m_matAnimatedTemp._23);
	}
	return pActorNode;
}

RActorNode* XCharacter::GetAnimatedActorNodeTransform( const TCHAR* szNodeName, RMatrix& _rOut, E_TRANSFORMSPACE eTransformSpace)
{
	return m_pAnimationController->GetAnimatedActorNodeTransform(tstring(szNodeName), _rOut, eTransformSpace);
}

bool XCharacter::GetMovingDummyPosition(RVector& pos)
{
	RMatrix matAnimatedTemp;
	if (m_pAnimationController->GetAnimatedActorNodeTransform(tstring(_T("dummy_loc")), matAnimatedTemp, E_TS_LOCAL))
	{
		pos.Set(matAnimatedTemp._41, matAnimatedTemp._42, matAnimatedTemp._43);
		return true;
	}

	return false;
}

bool XCharacter::GetMovingDummyDirection(RVector& dir)
{
	if ( GetAnimatedActorNodeDir(_T("dummy_loc"), dir, E_TS_LOCAL) )
	{
		dir = -dir;
		return true;
	}

	return false;
}

void XCharacter::SetAngleVertical( float fSpineAngleVertical )
{
	m_pSpineController->SetAngle(fSpineAngleVertical, 0.0f);
}

void XCharacter::DumpSceneNode(const TCHAR* szFileName)
{
	MXml xml;
	MXmlDocument* root = xml.Doc();

	MXmlDeclaration* pDec = xml.CreateDeclaration("1.0", "UTF-8", "");
	root->LinkEndChild(pDec);

	MXmlElement* pMaietElement = new MXmlElement("MAIET");


	class XActorNodeXmlDumpVisitor : public RSceneNodeVisitor
	{
	public:
		XActorNodeXmlDumpVisitor(RActor* _pCaller, MXmlElement* pElement) 
			: m_pCaller(_pCaller), m_pRootElement(pElement)
		{
			Push(m_pRootElement);
		}
		virtual ~XActorNodeXmlDumpVisitor(){}

		// post order 로 traversal 할꺼임
		virtual bool OnPreOrderVisit( RSceneNode* pSceneNode )
		{ 
			RActorNode* pActorNode = dynamic_cast<RActorNode*>(pSceneNode);
			if (pActorNode && pActorNode->m_pActor == m_pCaller)
			{
				MXmlElement* pParent = GetTop();
				MXmlElement* pElement = new MXmlElement("ActorNode");
				_SetAttribute(pElement, "name", pActorNode->GetNodeName());

				if (pActorNode->m_pMeshNode && pActorNode->m_pMeshNode->m_pBaseMesh)
				{
					_SetAttribute(pElement, "mesh", pActorNode->m_pMeshNode->m_pBaseMesh->GetName());
				}

				pParent->LinkEndChild(pElement);

				Push(pElement);
			}

			return true; 
		}
		virtual void OnPostOrderVisit( RSceneNode* pSceneNode )
		{
			RActorNode* pActorNode = dynamic_cast<RActorNode*>(pSceneNode);
			if (pActorNode && pActorNode->m_pActor == m_pCaller)
			{
				Pop();
			}
		}
	private:
		RActor* m_pCaller;
		MXmlElement* m_pRootElement;
		vector<MXmlElement*> m_Stack;

		MXmlElement* GetTop()
		{
			size_t n = m_Stack.size();
			return m_Stack[n-1];
		}
		void Pop()
		{
			size_t n = m_Stack.size();
			m_Stack.resize(n-1);
		}
		void Push(MXmlElement* pElement)
		{
			m_Stack.push_back(pElement);
		}
	};

	// 딸린 액터 노드 분류
	XActorNodeXmlDumpVisitor visitor(this, pMaietElement);
	Traverse(&visitor);

	SaveToXML(*pMaietElement);

	root->LinkEndChild(pMaietElement);

	xml.SaveFile(MLocale::ConvTCHARToAnsi(szFileName).c_str());

}

void XCharacter::UpdateAllSceneNodes()
{
	_ASSERT(!CheckAlreadyUpdatedForRender());
	if (CheckAlreadyUpdatedForRender())
	{
//		mlog("From XCharacter::UpdateAllSceneNodes() : Already called UpdateForRender!\n");
	}

	if (m_SceneUpdator.OnPreOrderVisit(this))
	{
		m_SceneUpdator.OnPostOrderVisit(this);

		return;
	}

	// 프리오더 업데이트에서 실패했다면,
	// Usable이 false여야지 업데이트 중복실행 방지가 아니어야한다.
	_ASSERT( GetUsable() == false);
}

//b_parts 제외하고 루팅용 aabb 만들기
// 두번만 만듭니다. 캐릭터 로드할때와 다이 애니메이션후에
void XCharacter::RefreshInteractionAABB()
{
	m_InteractionAABB.Initialize();

	RMatrix			matWorldTemp;
	GetWorldTransform().GetInverse(&matWorldTemp);

	for(int i=0;i<GetActorNodeCount();i++)
	{
		RActorNode* pNode = m_actorNodes[i];

		//b_parts 제외
		if(pNode->m_strName.find("b_parts_") != std::string::npos)
			continue;

		pNode->NeedUpdateTransform();
		pNode->UpdateTransform();
		RMeshNode *pMeshNode = pNode->m_pMeshNode;

		// 참조되는 뼈라면 노드위치 기반으로 바운딩박스를 구한다
		if(	//pMeshNode->GetFlag(RM_FLAG_BONE_MESH) &&	// 참조되는 뼈가 bone 이 아닌경우도 있네요.
			pNode->GetBoneRefCount()
			)
		{
			const RBoundingBox &obb = pMeshNode->m_boundingBox;
			// 좋은 코드는 아니지만, 일단 이렇게 해서 아무 데이터도 없는 node를 판별한다.
			if ( obb.vmin.x == FLT_MAX )
				continue;

			RBoundingBox bbox;
			TransformBox(&bbox, obb, pNode->GetWorldTransform());
			bbox.vmax = bbox.vmax * matWorldTemp;
			bbox.vmin = bbox.vmin * matWorldTemp;
			m_InteractionAABB.Add(bbox);
		}
		else
		{
			if(!pMeshNode || !pMeshNode->isRenderAble()) continue;
			//			if(pMeshNode->isPhysique()) 
			{
				//				bSkinningUsed = true;
				//				continue;	// 스키닝된 메시들은 뺀다( 뼈기준으로 더해지기 때문에 )
			}

			// 뼈이외의 노드, obb -> aabb 기반으로 대충 빠르게 구한다.
			RBox obb = pNode->m_pMeshNode->m_boundingBox;
			TransformBox(&pNode->GetAABB(), obb, pNode->GetWorldTransform());
			//m_InteractionAABB.Add(pNode->GetAABB());

			RBoundingBox bbox;
			TransformBox(&bbox, obb, pNode->GetWorldTransform());
			bbox.vmax = pNode->GetAABB().vmax * matWorldTemp;
			bbox.vmin = pNode->GetAABB().vmin * matWorldTemp;
			m_InteractionAABB.Add(bbox);
		}

		// 라이트맵 노드이면 버텍스버퍼 갱신
		pNode->UpdateBoundingVolume();
	}
}

void XCharacter::GetPartsActorNode( int nSlotID, vector<RActorNode*>& vecPartsActorNode, vector<tstring>& vecPartsActorNodeName )
{
	vector<XPartsNodeInfo>* pPartsNodeInfo = m_pParts->GetPartsActorNode(nSlotID);
	if(pPartsNodeInfo)
	{
		for(vector<XPartsNodeInfo>::iterator itParts = pPartsNodeInfo->begin(); itParts != pPartsNodeInfo->end(); ++itParts)
		{
			vecPartsActorNode.push_back(itParts->pActorNode);
			vecPartsActorNodeName.push_back(itParts->strActorNodeName);
		}
	}
}

void XCharacter::SetActorListener( RListener* pListener )
{
	SetListener(pListener);
}

RActorNode* XCharacter::GetActorNodePos(const TCHAR* szNodeName, RVector& pos, E_TRANSFORMSPACE eTransformSpace, bool bForce /*= false*/)
{
	if(_tcsicmp(szNodeName, _T("")) == 0 ||
		GetVisible() == false)
		return NULL;

	// 업데이트가 안되어 있다면...작동을 하지 않는다.
	// 한 프레임 늦어두 상관없다면 bForce를 true로 주셔야 합니다.
	if(m_bDoNotAnimationUpdate == false &&					// 업데이트를 하지 않는거면... Force 처리한다.
		m_bAnimationUpdated == false && bForce == false)
	{
		_ASSERT(0);
		return NULL;
	}

	RMatrix _rOut;
	_rOut.MakeIdentity();
	RActorNode * pActorNode = GetActorNodeTransform(szNodeName, _rOut, eTransformSpace, bForce);
	if(pActorNode)
		pos = _rOut.GetTranslation();

	return pActorNode;
}

RActorNode* XCharacter::GetActorNodeTransform(const TCHAR* szNodeName, RMatrix& _rOut, E_TRANSFORMSPACE eTransformSpace, bool bForce /*= false*/)
{
	RActorNode* pActorNode = GetActorNode(MLocale::ConvTCHARToAnsi(szNodeName).c_str());

	if(pActorNode == NULL)
		return NULL;

	// 이 RActorNode가 Active 하고 있는지 체크하자.
	AddActiveActorNode(pActorNode, bForce);

	// 업데이트가 안되어 있다면...작동을 하지 않는다.
	// 한 프레임 늦어두 상관없다면 bForce를 true로 주셔야 합니다.
	if(m_bDoNotAnimationUpdate == false &&					// 업데이트를 하지 않는거면... Force 처리한다.
		m_bAnimationUpdated == false && bForce == false)
	{
		_ASSERT(0);
		return NULL;
	}

	switch(eTransformSpace)
	{
	case E_TS_WORLD:
		{
			_rOut = pActorNode->GetWorldTransform();
		}
		break;
	case E_TS_LOCAL:
		{
			_rOut = pActorNode->GetLocalTransform();
		}
		break;
	case E_TS_LOCAL_RESULT:
		{
			RMatrix matInverse;
			GetWorldTransform().GetInverse(&matInverse);
			//_rOut = matInverse * pActorNode->GetWorldTransform();
			_rOut = pActorNode->GetWorldTransform() * matInverse;

		}
		break;
	}

	return pActorNode;
}

void XCharacter::AddActiveActorNode( tstring strActorNodeName )
{
	RActorNode* pActorNode = GetActorNode(MLocale::ConvTCHARToAnsi(strActorNodeName.c_str()).c_str());
	
	if(pActorNode == NULL)
		return;

	AddActiveActorNode(pActorNode);
}

void XCharacter::AddActiveActorNode( RActorNode* pActorNode, bool bForceUpdate /*= false*/ )
{
	if (pActorNode == NULL) return;

	if(m_pAnimationController)
		m_pAnimationController->CheckActiveActorNode(pActorNode, bForceUpdate);
}

int XCharacter::GetCurFrame()
{
	if(m_pAnimationController)
		return m_pAnimationController->GetAniFrame();

	return 0;
}

int XCharacter::GetMaxFrame()
{
	if(m_pAnimationController)
		return m_pAnimationController->GetAniMaxFrame();

	return 0;
}

bool XCharacter::Create( const TCHAR* szMeshName, bool bBackgroundCreation /*= false*/ )
{
	if(m_pAnimationInfoMgr)
		m_pAnimationInfoMgr->LoadInfo(tstring(szMeshName));

	return  RActor::Create(MLocale::ConvTCHARToAnsi(szMeshName).c_str(), bBackgroundCreation);
}

BLEND_DATA XCharacter::GetAnimationBlendData( tstring strSource, tstring strTarget )
{
	if(m_pAnimationInfoMgr && m_pMesh)
		return m_pAnimationInfoMgr->m_AnimationBlendInfo.GetAnimationTransData(strSource, strTarget, &m_pMesh->m_AniMgr);

	return XAnimationBlendInfo::DEFAULT_ANIMATION_TRANSITION_DATA;
}

XOverlayAnimationInfo* XCharacter::GetOverlayAnimaionInfo()
{
	if(m_pAnimationInfoMgr && m_pMesh)
		return &(m_pAnimationInfoMgr->m_OverlayAnimationInfo);

	return NULL;
}

XAnimationHitSoundInfo* XCharacter::GetAnimationHitSoundInfo()
{
	if(m_pAnimationInfoMgr)
		return &(m_pAnimationInfoMgr->m_AnimationHitSoundInfo);

	return NULL;
}

XReActionAnimationInfo* XCharacter::GetReActionAnimationInfo()
{
	if(m_pAnimationInfoMgr)
		return &(m_pAnimationInfoMgr->m_ReActionAnimationInfo);

	return NULL;
}

bool XCharacter::GetNodePosition( const TCHAR* szNodeName, RVector& vPos )
{
	RActorNode* pActorNode = GetActorNode(MLocale::ConvTCHARToAnsi(szNodeName).c_str());
	if(!pActorNode) return false;

	pActorNode->ForceUpdateAnimation();

	vPos = pActorNode->GetPosition();

	return true;
}
