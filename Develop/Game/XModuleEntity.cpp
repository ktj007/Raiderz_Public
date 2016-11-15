#include "stdafx.h"
#include "XModuleEntity.h"
#include "XEventID.h"
#include "XItem.h"
#include "XModuleMotion.h"
#include "XConst.h"
#include "XItemManager.h"
#include "XEffectManager.h"
#include "XMyPlayer.h"
#include "XNonPlayer.h"
#include "CSMeshInfo.h"

#include "XNaming.h"
#include "XAnimationEventHandler.h"

#include "RDebugPassRenderer.h"
#include "RCameraSceneNode.h"
#include "RBackgroundWorker.h"

#include "XCharacterSerializer.h"
#include "XSystem.h"
#include "XGameTransformControllerManager.h"
#include "MockAnimationController.h"
#include "MockCharacter.h"
#include "XEntityFader.h"
#include "XEntityDirectionInterpolator.h"

#include "MPhysX4R3/MPxAttacherTC.h"
#include "MPhysX4R3/MPxRagDollTC.h"
#include "MPhysX4R3/MPxDynamicActorNodeTC.h"
#include "MPhysX4R3/MPxAniLookUpTC.h"

#include "XSceneManager.h"
#include "XResourceManager.h"

#include "RMesh.h"

#include "XGameTransformControllerManager.h"
#include "MPhysX4R3/RPhysXActorController.h"
#include "XModuleCollision.h"


void XVictimFrameFreezer::FreezeFrame( XObject* pOwner )
{
	m_bReserve = false;

	const float HIT_FREEZE_TIME = 0.3f;

	if (HIT_FREEZE_TIME > 0.001f)
	{
		pOwner->GetModuleEntity()->GetAnimationController()->FreezeFrame(HIT_FREEZE_TIME);
	}
}

void XVictimFrameFreezer::OnHit( XObject* pOwner, const wchar_t* szCurrAni )
{
	const float HIT_FREEZE_DELAY = 0.0f;

	if (HIT_FREEZE_DELAY <= 0.001f)
	{
		FreezeFrame(pOwner);
		return;
	}

	m_bReserve = true;
	m_fReserveElapsedTime = 0.0f;
	m_strCurrAni = szCurrAni;
}

void XVictimFrameFreezer::Update( float fDelta, XObject* pOwner, const wchar_t* szCurrAni )
{
	if (m_bReserve == false) return;
	if (wcscmp(m_strCurrAni.c_str(), szCurrAni))
	{
		m_bReserve = false;
		return;
	}

	m_fReserveElapsedTime += fDelta;


	const float HIT_FREEZE_DELAY = 0.0f;

	if (m_fReserveElapsedTime >= HIT_FREEZE_DELAY)
	{
		m_bReserve = false;
		FreezeFrame(pOwner);
	}
}

//////////////////////////////////////////////////////////////////////////
XModuleEntity::XModuleEntity(XObject* pOwner) 
: XModule(pOwner)
, m_fScale(1.f)
, m_pNPCInfo(NULL)
, m_uidExtraTarget(MUID::ZERO)
, m_fMaxVisibility(1.0f)
, m_pFader(NULL)
, m_bAlpha2PassRendering(false)
, m_pRideActorMgr( NULL )
{
	m_bLoadingCompleted = false;
	m_bInternalWarp = false;

	// 특별한 Transform을 하기위한 Transform Controller
	class XPxTcEntity : public MPxTcEntity
	{
	public:
		XPxTcEntity(XModuleEntity* pOwnerEntity) : m_pOwnerEntity(pOwnerEntity) {}
		virtual ~XPxTcEntity(){}

		virtual rs3::RActor* GetActor()
		{
			return m_pOwnerEntity->GetActor();
		}

		virtual MPxTransformController* GetTransformController(int nId)
		{
			return m_pOwnerEntity->GetTransformController(nId);
		}

		virtual bool IsAvailableActor(rs3::RActor* pActor)
		{
			return false;
		}

		virtual void OnDynamicObjectCollided(NxActor* _pNxActor, MPxDynamicObjectEntity* _pDynamicObjEntity)
		{
			XObject* pOwnerObject = m_pOwnerEntity->GetOwner();
			if (NULL == pOwnerObject)
				return;

			XModuleCollision* pModuleCollision = pOwnerObject->GetModuleCollision();
			if (NULL == pModuleCollision)
				return;

			CSMeshInfo* pMeshInfo = pModuleCollision->GetMeshInfo();
			if (NULL == pMeshInfo)
				return;

			RPhysXActorController* pActorController = _pDynamicObjEntity->GetPhysXActorController();
			if(NULL == pActorController)
				return;

			RActorNode* pRelatedActorNode = pActorController->GetActorNodeFromUpdateMap(_pNxActor);
			if (NULL == pRelatedActorNode)
				return;

			const std::string& rActorNodeName = pRelatedActorNode->GetNodeName();

			//b_parts_0.length = 9
			int nBPartIdx = atoi(rActorNodeName.substr(9, 1).c_str());
			if (nBPartIdx < 1 || nBPartIdx > 4)
				return;

			//재생음이 없으면 사운드이팩트 연주가 필요없다.
			const std::tstring& strPlaySoundName = pMeshInfo->m_strBPartsCollisionSound[nBPartIdx-1];
			if (strPlaySoundName.empty())
				return;

			// Mass Rank 1~8 까지만 사운드이팩트
			int nMassRank = _pDynamicObjEntity->GetMassRank(rActorNodeName);
			if (nMassRank < 1 || nMassRank > 8)
				return;

			bool bDo = false;
			MAP_DYN_OBJ::iterator itr = m_mapDynamicObject.find(pRelatedActorNode);
			if (itr == m_mapDynamicObject.end())
			{
				bDo = true;
				m_mapDynamicObject.insert(MAP_DYN_OBJ::value_type(pRelatedActorNode, pOwnerObject->GetUID()) );
			}
			else if (itr->second != pOwnerObject->GetUID())
			{
				bDo = true;
				itr->second = pOwnerObject->GetUID();
			}

			if (bDo)
			{
				if ( global.sound)
					global.sound->PlayDynamic3D( MLocale::ConvUTF16ToAnsi(strPlaySoundName.c_str()).c_str(), NULL, 0, pOwnerObject->GetUID(), rActorNodeName.c_str());
			}
		}

	protected:
		XModuleEntity* m_pOwnerEntity;

		//typedef std::map< MPxDynamicObjectEntity*, MUID > MAP_DYN_OBJ;
		//std::map< MPxDynamicObjectEntity*, MUID > m_mapDynamicObject;

		typedef std::map< RActorNode*, MUID > MAP_DYN_OBJ;
		std::map< RActorNode*, MUID > m_mapDynamicObject;
	};

	m_pTcEntity = new XPxTcEntity(this);
	m_bPauseTransformControl = false;

	int nTcID = 0;
	MPxTransformController* pTc = NULL;

	pTc = new MPxAttacherTC(m_pTcEntity);
	MPxAttacherTC::SetID(nTcID++);
	m_vecTransformControllers.push_back(pTc);

	pTc = new MPxRagDollTC(m_pTcEntity);
	MPxRagDollTC::SetID(nTcID++);
	m_vecTransformControllers.push_back(pTc);

	pTc = new MPxDynamicActorNodeTC(m_pTcEntity);
	MPxDynamicActorNodeTC::SetID(nTcID++);
	m_vecTransformControllers.push_back(pTc);

	pTc = new MPxAniLookUpTC(m_pTcEntity);
	MPxAniLookUpTC::SetID(nTcID++);
	m_vecTransformControllers.push_back(pTc);

	#ifdef _DEBUG
	for ( int i = 0; i < nTcID; ++i)
	{
		_ASSERT(m_vecTransformControllers[i] != NULL);
	}
	#endif

	// Actor와 AnimationController 만들기
	if (global.system->IsResourceLoading() == true)
	{
		m_pActor						= global.actor_loader->CreateActor();// new XCharacter();	와 같은데 생성/삭제의 interface를 통일 시키기 위해 이렇게 생성한다. 
		m_pAnimationController			= new XGameTransformControllerManager(m_pActor, GetTransformController<MPxAniLookUpTC>());
	}
	else
	{
		// 만약 리소스 로딩을 안하면 Mock으로 생성함. - birdkr
		m_pActor						= new MockCharacter();
		m_pAnimationController			= new MockAnimationController(m_pActor, GetTransformController<MPxAniLookUpTC>());
	}

	m_pActor->SetUsable(false);
	m_pActor->SetVisible(false);

	bEntitySoundPlaying				= false;

	// 리스너 셋팅
	m_pActor->SetActorListener(this);

	// Entity 자원관련
	m_pSerializer					= new XCharacterSerializer(this);
	m_pCharExtFeature				= NULL;

	m_pDirectionInterpolator = new XEntityDirectionInterpolator(this);

	m_RegMultiplyColor.Stop();

	// 이중 그림자 사용(모두 적용)
	m_pActor->SetUseDoubleShadow(true);

	InitRenderPosDirUp();

	// Ride Mgr
	m_pRideActorMgr			= new XRideActorMgr( this );
}

XModuleEntity::~XModuleEntity(void)
{
	SAFE_DELETE(m_pDirectionInterpolator);

	if (m_pFader)
	{
		SAFE_DELETE(m_pFader)
	}

	SAFE_DELETE(m_pSerializer);

	// transform controller 지우기
	for (VEC_TC::iterator itr = m_vecTransformControllers.begin(); itr != m_vecTransformControllers.end(); ++itr)
	{
		delete *itr;
	}
	SAFE_DELETE(m_pTcEntity);

	SAFE_DELETE(m_pAnimationController);

	// 액터 지우기
	if (global.actor_loader)
		global.actor_loader->DeleteActor(m_pActor);
	else
	{
		// 유닛 테스트인 경우
		SAFE_DELETE( m_pActor );
	}

	// 남아있는 이펙트 삭제
	DelAllLoopAniEffect();

	// 탈것 삭제
	SAFE_DELETE( m_pRideActorMgr );
	/*
	if (global.actor_loader)
		global.actor_loader->DeleteActor( m_pRideActor );
	else
		SAFE_DELETE( m_pRideActor );

	SAFE_DELETE( m_pRideAnimationController );

	SAFE_DELETE( m_pRideSerializer );
	*/
}

void XModuleEntity::OnAdd()
{

}

void XModuleEntity::OnRemove()
{

}

void XModuleEntity::OnActivate()
{
	AddToSceneManager();
}

void XModuleEntity::OnDeactivate()
{
	// by pok. 씬매니저 변경중
	RemoveFromSceneManager();
}

void XModuleEntity::OnUpdate(float fDelta)
{
	PFC_THISFUNC;

	//wstring name(GetOwner()->GetName());
	//if (  name  == L"상급기사 세네스")
	//{
	//	mlog("Here");
	//}

	//----------------------------------------------------------------------------
	// 로딩 체크 구간
	CheckLoadingCompleted();

	RideCheckLoadingCompleted();		// 탈것 _by tripleJ

	PFI_BLOCK_THISFUNC(303);

	// module update
	XModule::OnUpdate(fDelta);

	// serialize update
	if( m_pRideActorMgr )
		m_pRideActorMgr->UpdateSerializer();
	m_pSerializer->Update();

	//----------------------------------------------------------------------------
	// 애니메이션 업데이트 이전에 처리 해야 하는 구간
	if (m_pFader)
	{
		m_pFader->Update(fDelta);
	}

	if (m_pDirectionInterpolator)
	{
		m_pDirectionInterpolator->Update(fDelta);
	}

	//----------------------------------------------------------------------------
	// 애니메이션 컨트롤 업데이트 구간
	if( m_pRideActorMgr )
		m_pRideActorMgr->UpdateAnimationController( fDelta );
	m_pAnimationController->Update(fDelta);

	//----------------------------------------------------------------------------
	// 애니메이션 업데이트 이후에 처리 해야 하는 구간
	// Physx는 애니메이션 컨트롤 업데이트 이후에 해야 한다.
	// special transform control update
	if ( m_bPauseTransformControl == false )
	{
		for (VEC_TC::iterator itr = m_vecTransformControllers.begin(); itr != m_vecTransformControllers.end(); ++itr)
		{
			MPxTransformController* pTc = *itr;
			_ASSERT(pTc);
			if (pTc->IsEnabled())
				pTc->Update(fDelta);
		}
	}

	UpdateRide();

	// 탈것처리	_by tripleJ
	if( m_pRideActorMgr )
		m_pRideActorMgr->UpdateAllSceneNodes();
	m_pActor->UpdateAllSceneNodes();

	#ifdef _DEBUG
	m_debugInfos.clear();
	#endif

	if (m_pActor)
	{
		if(m_RegMultiplyColor.IsActive() && m_RegMultiplyColor.IsReady(fDelta))
		{
			m_pActor->DisableAddColorEffect();
			m_RegMultiplyColor.Stop();
		}

		for(int iBParts = 0; iBParts < BREAKABLEPARTS_MAX_COUNT; ++iBParts)
		{
			if(m_RegMultiplyColorForBParts[iBParts].regMultiplyColor.IsActive() && m_RegMultiplyColorForBParts[iBParts].regMultiplyColor.IsReady(fDelta))
			{
				m_pActor->DisableAddColorEffect(m_RegMultiplyColorForBParts[iBParts].strActorNodeName.c_str());
				m_RegMultiplyColorForBParts[iBParts].regMultiplyColor.Stop();
				m_RegMultiplyColorForBParts[iBParts].strActorNodeName.clear();
			}
		}
	}

	m_FrameFreezer.Update(fDelta, m_pOwner, m_pAnimationController->GetAnimationName().c_str());
}

void XModuleEntity::OnInitialized()
{
	m_vPos	= vec3::ZERO;
	m_vDir	= -vec3::AXISY;
	m_vUp	= UP_VECTOR;

	_SetPosition(m_vPos);
	_SetDirection(m_vDir);
}

void XModuleEntity::OnFinalized()
{

}



void XModuleEntity::_SetPosition(const vec3& pos)
{
	m_vPos.Set(pos);
	_UpdateTransform();
}

void XModuleEntity::_SetDirection(const vec3& dir)
{
	if (dir == vec3::ZERO)
		return;

	m_vDir.Set(dir);
	_UpdateTransform();
}


void XModuleEntity::SetVisible(bool bVisible)
{
	if (m_bLoadingCompleted == false) return;

	m_pActor->SetVisible(bVisible);
	if( m_pRideActorMgr )
		m_pRideActorMgr->SetVisible( bVisible );
}

bool XModuleEntity::IsVisible()
{
	return m_pActor->GetVisible();
}

bool XModuleEntity::IsHaveDummy()
{
	return m_pActor->IsExistActorNode(L"dummy_loc");
}

bool XModuleEntity::GetMovingDummyPos(vec3& vAnimPos)
{
	if (m_pActor->GetMovingDummyPosition(vAnimPos))
		return true;

	return false;
}

bool XModuleEntity::GetMovingDummyDir(vec3& vAnimDir)
{
	if (m_pActor->GetMovingDummyDirection(vAnimDir))
		return true;

	return false;
}

vec3 XModuleEntity::GetAnimationPos()
{
	vec3 vAnimPos = vec3::ZERO;

	if (GetMovingDummyPos(vAnimPos))
	{
		MMatrix tm;
		tm.SetLocalMatrix(vec3::ZERO, -GetDirection(), vec3::AXISZ);
		vec3 ret = vAnimPos * tm;

		return ret;
	}

	return vec3::ZERO;
}

vec3 XModuleEntity::GetAnimationDir()
{
	vec3 vAnimDir = GetDirection();

	if (GetMovingDummyDir(vAnimDir))
	{
		mat4 tm;
		tm.SetLocalMatrix(vec3::ZERO, -GetDirection(), vec3::AXISZ);

		vec3 ret = vAnimDir * tm;
		ret.Normalize();

		return ret;
	}

	return (-vec3::AXISY);
}

void XModuleEntity::OnAnimationEvent(RActor* pActor, const RAnimationEvent* pEvent)
{
	XAnimationEventHandler::GetInstance().OnEvent(pActor, pEvent, *this);

}

vec3 XModuleEntity::GetPosition()
{
	return m_vPos; 
}


void XModuleEntity::SerializeMyPlayer(PLAYER_INFO& playerInfo, XMyEquipmentSlot* pEquipmentSlot)
{
	m_pSerializer->SerializeMyPlayer(playerInfo, pEquipmentSlot);

	XCharExtFeature _feature;
	_feature.nMakeUp = playerInfo.nMakeUp;
	_feature.nTattoo = playerInfo.nTattooType;
	_feature.fTattooScale = (float)playerInfo.nTattooScale * 0.02f;
	_feature.vTattooPos.x = (float)playerInfo.nTattooPosX * 0.01f;
	_feature.vTattooPos.y = (float)playerInfo.nTattooPosY * 0.01f;
	SetCharExtFeature( &_feature);
}


void XModuleEntity::SerializeNetPlayer( bool bLoadingAsync /*= true*/ )
{
	if (MIsExactlyClass(XNetPlayer, m_pOwner) == false) return;

	XNetPlayer* pNetPlayer = AsNetPlayer(m_pOwner);
	XPlayerInfoFeature& feature = pNetPlayer->GetFeature();

	// mesh serialize
	XResourceLoadingPolicy policy = LOADING_ASYNC;
	if(bLoadingAsync == false)
		policy = LOADING_SYNC;

	m_pSerializer->SerializeNetPlayer(pNetPlayer, feature, policy);

	XCharExtFeature _feature;
	_feature.nMakeUp = feature.nMakeUp;
	_feature.nTattoo = feature.nTattoo;
	_feature.fTattooScale = feature.fTattooScale;
	_feature.vTattooPos = feature.vTattooPos;
	SetCharExtFeature( &_feature);
}

void XModuleEntity::SerializeNPC( XNPCInfo* pNPCInfo, bool bLoadingAsync /*= true*/ )
{
	if (global.smgr == NULL) return;

	m_pNPCInfo = pNPCInfo;

	XResourceLoadingPolicy policy = LOADING_ASYNC;
	if(bLoadingAsync == false)
		policy = LOADING_SYNC;

	m_pSerializer->SerializeNPC(pNPCInfo, m_fScale, policy);
}

void XModuleEntity::OnSubscribeEvent()
{
	Subscribe(XEVTL_ON_CHANGE_MOTION);
	XModule::OnSubscribeEvent();	
}

XEventResult XModuleEntity::OnEvent(XEvent& msg)
{
	switch (msg.m_nID)
	{
	case XEVTL_ON_CHANGE_MOTION:
		{
			// TODO: 
			// [10/16/2007 madduck] 흠 남아 있는 이펙트를 지워줘야 하느 시점이 애매하다..... 
			//m_pOwner->GetModuleEntity()->DelAllLoopAniEffect();			
			DelAllLoopAniEffect();
		}
		break;
	}

	return MR_FALSE;
}

void XModuleEntity::DelAllLoopAniEffect()
{
	for( size_t i = 0; i < m_vLoopAniEffect.size(); i++)
	{
		if (global.emgr)
			global.emgr->DeleteXEffect(m_vLoopAniEffect[i].m_uid);
	}

	m_vLoopAniEffect.clear();
}
void XModuleEntity::_SetUpVector( const vec3& up )
{
	if (up == vec3::ZERO) 
		m_vUp.Set(UP_VECTOR);

	m_vUp.Set(up);
	m_vDir.z = 0.0f;
	_UpdateTransform();
}

void XModuleEntity::_SetRenderPosDir( const vec3& rpos, const vec3& rdir)
{
	m_vRenderPos.Set(rpos);
	m_vRenderDir.Set(rdir);

	_UpdateTransform();
}

void XModuleEntity::_SetRenderPosition( const vec3& rpos)
{
	m_vRenderPos.Set(rpos);

	_UpdateTransform();
}

void XModuleEntity::_SetRenderDirection( const vec3& rdir)
{
	m_vRenderDir.Set(rdir);

	_UpdateTransform();
}

void XModuleEntity::_SetRenderUpDir( const vec3& rup, const vec3& rdir )
{
	m_vRenderDir.Set(rdir);
	m_vRenderUp.Set(rup);

	_UpdateTransform();
}

void XModuleEntity::_UpdateTransform()
{
	// 지울때는 update transform을 하지 않는다.
	_ASSERT(m_pOwner);
	if (m_pOwner->GetReferenceCount() == 0)
		return;

	if ( GetTransformController<MPxAttacherTC>()->IsEnabled())
		return;


	MMatrix mat;
	mat.SetTranslationMatrix( m_vRenderPos);
	mat.SetScale(m_fScale);

	MMatrix tm;
	if ( m_vRenderDir != vec3::ZERO)	tm.SetLocalMatrix( m_vPos, -m_vRenderDir, m_vRenderUp);
	else								tm.SetLocalMatrix( m_vPos, -m_vDir, m_vUp);
	mat *= tm;


	// 이 어서션이 걸리면, UpdateForRender 가 불린 이후에 다시 위치 세팅을 하는것이며, 칼등등이 한프레임 늦게 따라오게됩니다.
	_ASSERT( !m_pActor->CheckAlreadyUpdatedForRender() );
	if (m_pActor->CheckAlreadyUpdatedForRender())
	{
//		mlog("From XModuleEntity::_UpdateTransform() : Already called UpdateForRender!\n");
	}

	// SetTransform에서 스케일값을 새로 뽑으므로 Matrix값에 스케일이 있어야 한다. 

	m_pActor->SetTransform( mat );

	// 아직 로딩중일때 실행되면 뻗는다.
//	if(m_pActor->GetUsable())
	{
		m_pActor->UpdateTransform();
		m_pActor->UpdateVisibility();
	}

	if( m_pRideActorMgr )
		m_pRideActorMgr->SetTransform( mat );
}

void XModuleEntity::AddToSceneManager()
{
	if (global.smgr)
	{
		// 더해질때는 Warp를 한것과 동일한 효과를 나타낸다.
		OnWarp();

		// Actor가 Char, Ride 두개이므로 확인하고 넣어준다
		if( NULL == m_pActor->GetParent() )
			global.smgr->AddSceneNode( m_pActor );

		if( m_pRideActorMgr )
			m_pRideActorMgr->AddToSceneManager();
	}
}

bool XModuleEntity::RemoveFromSceneManager()
{
	bool bResult		= false;
	if( m_pActor )
		bResult			= m_pActor->RemoveFromParent();

	if( m_pRideActorMgr )
		m_pRideActorMgr->RemoveFromSceneManager();

	return bResult;
}

void XModuleEntity::OnLoadingComplete()
{
	// 확장 외형값 적용
	SetCharExtFeature( m_pCharExtFeature);

	// 애니메이션 컨트롤러
	m_pAnimationController->OnLoadingComplete();

	// 랙돌 트랜스폼 컨트롤러
	//GetTransformController<MPxRagDollTC>()->InitRagDollInfoFromModelName( m_pSerializer->GetMeshName(), SH_PX_COL_RAGDOLL );
	//GetTransformController<MPxRagDollTC>()->InitRagDollInfoFromModelName( MLocale::ConvUTF16ToAnsi(m_pSerializer->GetMeshName().c_str()), NULL);

	// 다이나믹 오브젝트 트랜스폼 컨트롤러
	//GetTransformController<MPxDynamicActorNodeTC>()->GrabDynamicActorShape();


	if (m_pActor && m_pActor->m_pMesh && global.res)
	{
		global.res->OnCreateResource(m_pActor->m_pMesh);

		//mlog("메쉬로딩 컴플리트? %d %d\n", m_pActor->m_pMesh->IsLoadingCompleted(), m_pActor->m_pMesh->IsLoadingSucceeded());
	}

	// 로딩 완료 이벤트 발생
	XEvent evt;
	evt.m_nID = XEVTD_MESH_LOADING_COMPLETE;
	m_pOwner->Event(evt);

	// 알파 2 패스 체크
	if(m_pActor->m_pMesh)
		m_bAlpha2PassRendering = m_pActor->m_pMesh->IsAlpha2PassRendering();
}

void XModuleEntity::SetTransform( const MMatrix& mat, const MPxTransformController* pCaller )
{
	// 이 어서션이 걸리면, UpdateForRender 가 불린 이후에 다시 위치 세팅을 하는것이며, 칼등등이 한프레임 늦게 따라오게됩니다.
	_ASSERT( !m_pActor->CheckAlreadyUpdatedForRender() );
	if (m_pActor->CheckAlreadyUpdatedForRender())
	{
//		mlog("From XModuleEntity::SetTransform(mat, MPxTransformController) : Already called UpdateForRender!\n");
	}

	// Actor 설정
	m_pActor->SetTransform(mat);
	m_pActor->UpdateTransform();

	// ModuleEntity와 동기화
	m_vPos	= m_pActor->GetWorldPosition();
	m_vDir	= m_pActor->GetWorldDirection() * -1;
	m_vUp	= m_pActor->GetWorldUp();

	//REngine::GetSceneManager().GetPassRenderer< RDebugPassRenderer >()->AddDebugBox(m_pActor->GetWorldTransform(), 30, 10.0f, D3DCOLOR_XRGB(255, 255, 255));
}

void XModuleEntity::CheckLoadingCompleted()
{
	if (m_pActor == NULL) return;
	if (m_bLoadingCompleted == true) return;
	if (m_pActor->GetVisible() == true) return;

	if (m_pActor->IsLoadingCompleted())
	{
		m_bLoadingCompleted = true;
		m_pActor->SetVisible(true);
	}
}

void XModuleEntity::SetCharExtFeature( const XCharExtFeature* pExtFeature)
{
	if ( pExtFeature == NULL)
		return;

	if ( IsLoadingCompleted() == true)
	{
		RActorNode* pActorNode = m_pActor->GetActorNode( "face");
		if ( pActorNode != NULL  &&  pActorNode->m_pMeshNode != NULL  &&  pActorNode->m_pMeshNode->m_pParentMesh != NULL)
		{
			RMesh* pMesh = pActorNode->m_pMeshNode->m_pParentMesh;
			map< string/*material name*/, pair<int/*index*/, int/*count*/>> vMaterials;
			int _size = pMesh->GetMaterialResourceCount();
			for ( int i = 0;  i < _size;  i++)
			{
				string name = pMesh->GetMaterialResource( i)->m_pMaterial->GetName();

				map<string,pair<int,int>>::iterator _find = vMaterials.find( name);
				if ( _find == vMaterials.end())
					vMaterials.insert( map<string,pair<int,int>>::value_type( name, pair<int,int>( i, 1)));
				else
					(*_find).second.second++;
			}

			// Set make up
			int nMakeUp = min( max( 0, (int)vMaterials.size() - 1),  max( 0, pExtFeature->nMakeUp));
			if ( true)
			{
				map<string,pair<int,int>>::iterator itr = vMaterials.begin();
				for ( int i = 0;  i < nMakeUp;  i++, itr++);

  				for ( int i = 0;  i < min( 3, (*itr).second.second);  i++)
					m_pActor->ChangeMatetial( "face", i, (*itr).second.first + i, 0.0f);
			}

			// Set tattoo
			if ( true)
			{
				for ( map<string,pair<int,int>>::iterator itr = vMaterials.begin();  itr != vMaterials.end();  itr++)
					m_pActor->GetMaterialDecal()->RemoveDecalLayer( (*itr).first);

				if ( pExtFeature->nTattoo > 0)
				{
					RTextureDesc DiffuseMap;
					DiffuseMap.nMapChannel = 1;
					DiffuseMap.TextureName = XCONST::DEFAULT_TATTOO[ pExtFeature->nTattoo - 1];
					DiffuseMap.UVTransform.SetScale( RVector2( pExtFeature->fTattooScale, pExtFeature->fTattooScale));
					DiffuseMap.UVTransform.SetOffset( pExtFeature->vTattooPos);
					DiffuseMap.textureAddressU = RTADDRESS_CLAMP;
					DiffuseMap.textureAddressV = RTADDRESS_CLAMP;

					RTextureDesc OpacityMap;
					OpacityMap.TextureName = "tattoo_mask.dds";
					OpacityMap.nMapChannel = 0;

					for ( map<string,pair<int,int>>::iterator itr = vMaterials.begin();  itr != vMaterials.end();  itr++)
						m_pActor->GetMaterialDecal()->AddDecalLayer( (*itr).first, &DiffuseMap, NULL);
				}
			}
		}

		if ( m_pCharExtFeature != NULL)
		{
			delete m_pCharExtFeature;
			m_pCharExtFeature = NULL;
		}
	}
	else
	{
		if ( m_pCharExtFeature == NULL)
			m_pCharExtFeature = new XCharExtFeature();

		m_pCharExtFeature->nMakeUp = pExtFeature->nMakeUp;
		m_pCharExtFeature->nTattoo = pExtFeature->nTattoo;
		m_pCharExtFeature->fTattooScale = pExtFeature->fTattooScale;
		m_pCharExtFeature->vTattooPos = pExtFeature->vTattooPos;
	}
}

bool XModuleEntity::IsLoadingCompleted()
{
	// 시스템에서 리소스 로딩을 안한다면 그냥 true로 리턴한다(unit test 같은 경우)
	if (global.system->IsResourceLoading() == false)
	{
		return true;
	}

	return m_bLoadingCompleted;
}

bool XModuleEntity::Pick( RPICKINFO& pickInfo )
{
	if (m_pActor == NULL) return false;
	return m_pActor->Pick(pickInfo);
}

void XModuleEntity::OnWarp()
{
	if (m_bInternalWarp)
		return;

	// Animation LookUp Controller에게 Warp 되었음을 알림
	// GetTransformController<MPxAniLookUpTC>()->OnLookUpWarp();
}

void XModuleEntity::InitRenderPosDirUp()
{
	m_vRenderPos = vec3::ZERO;
	m_vRenderDir = vec3::ZERO;
	m_vRenderUp = vec3::AXISZ;
}

void XModuleEntity::SetVisibility( float fVisibility )
{
	if (m_pActor)
	{
		if (fVisibility >= 1.0f &&
			m_bAlpha2PassRendering == false &&
			(m_pActor->m_pMesh && m_pActor->m_pMesh->IsAlpha2PassRendering() == false)) // 메시가 알파2패스 렌더 속성이면 무조건 알파2패스로
		{
			m_pActor->SetAlpha2PassRendering(false);
		}
		else
		{
			// 알파2패스는 불투명하게 그려지면 안된다.
			if (fVisibility >= 1.f)
				fVisibility = 0.9999f;

			m_pActor->SetAlpha2PassRendering(true);
		}

		if (fVisibility > m_fMaxVisibility) fVisibility = m_fMaxVisibility;
		m_pActor->SetVisibility(fVisibility);
	}
}

void XModuleEntity::SetMaxVisibility( float fMaxVisibility )
{
	m_fMaxVisibility = fMaxVisibility;
}

void XModuleEntity::StartFade( float fAlpha, float fTime )
{
	if (fAlpha > m_fMaxVisibility) fAlpha = m_fMaxVisibility;

	if (m_pFader == NULL)
	{
		m_pFader = new XEntityFader(m_pActor);
	}

	m_pFader->Start(fAlpha, fTime);
}

bool XModuleEntity::IsFading()
{
	if (m_pFader)
	{
		return m_pFader->IsDoing();
	}
	return false;
}

void XModuleEntity::StopFade()
{
	if (m_pFader)
	{
		m_pFader->Stop();
	}
}

float XModuleEntity::GetVisibility()
{
	if (m_pActor) return m_pActor->GetVisibility();
	return 0.0f;
}


void XModuleEntity::CreateFaceAnimation( bool bNPC /*= false*/ )
{
	m_pAnimationController->SetFaceAnimationNPC(bNPC);
	SetFaceAnimationActive(true);
}

void XModuleEntity::SetFaceAnimationActive( bool bActive )
{
	m_pAnimationController->SetFaceAnimationActive(bActive);
}

void XModuleEntity::SetDirectionSmoothly( const vec3& dir, float fRotateSpeed )
{
	vec3 vCurrDir = GetRenderDirection();
	if (vCurrDir == vec3::ZERO)
	{
		vCurrDir = GetDirection();
	}

	m_pDirectionInterpolator->Start(vCurrDir, dir, fRotateSpeed);
}

void XModuleEntity::StopDirectionInterpolation()
{
	m_pDirectionInterpolator->Stop();
}

bool XModuleEntity::IsDirectionInterpolatingNow()
{
	return m_pDirectionInterpolator->IsDoing();
}

bool XModuleEntity::IsAnimationUpdateCompleted()
{
	return m_pAnimationController->IsUpdated();
}

void XModuleEntity::ReSetAnimationUpdateCompleted()
{
	m_pAnimationController->ReSetUpdated();
}

void XModuleEntity::StartMultiplyColor()
{
	if (m_pActor)
	{
		m_RegMultiplyColor.SetTickTime(0.1f);
		m_RegMultiplyColor.Start();

		m_pActor->EnableAddColorEffect(vec3(XCONST::HIT_EFFECT_MODEL_ADD_COLOR.x, XCONST::HIT_EFFECT_MODEL_ADD_COLOR.y, XCONST::HIT_EFFECT_MODEL_ADD_COLOR.z));
	}
}

void XModuleEntity::StartFrameFreeze()
{
	m_FrameFreezer.OnHit(m_pOwner, m_pAnimationController->GetAnimationName().c_str() )	;
}

void XModuleEntity::CreateFakeBeatenAnimationController(tstring& strAniName, float fWeight, float fSpeed)
{
	m_pAnimationController->InitFakeBeatenAnimation(strAniName, fWeight, fSpeed);
}

void XModuleEntity::StartFakeBeaten(WEAPON_TYPE nAttackWeaponType, float fCriWeightRate, float fCriSpeedRate)
{
	// 무기별 증가율
	float fWeightRate = 1.0f;
	float fSpeedRate = 1.0f;

	if(nAttackWeaponType != WEAPON_NONE)
	{
		fWeightRate = XCONST::HIT_FAKE_BEATEN_WEAPON_TYPE_BLEND_WEIGHT[nAttackWeaponType - 1];
		fSpeedRate = XCONST::HIT_FAKE_BEATEN_WEAPON_TYPE_BLEND_SPEED[nAttackWeaponType - 1];
	}

	fWeightRate *= fCriWeightRate;
	fSpeedRate *= fCriSpeedRate;

	if (m_pOwner->IsNPC())
	{
		m_pAnimationController->StartFakeBeaten(GetAnimationController()->GetAnimationName().c_str(), fWeightRate, fSpeedRate, m_pOwner->AsNPC()->GetModeAnimationNameHeader().c_str());
	}
	else if (m_pOwner->IsPlayer())
	{
		WEAPON_TYPE nWeaponType = m_pOwner->AsPlayer()->GetCurrWeaponType();
		CHAR_STANCE nStance = m_pOwner->AsPlayer()->GetStance();

		m_pAnimationController->StartFakeBeaten(GetAnimationController()->GetAnimationName().c_str(), fWeightRate, fSpeedRate, XNaming::PlayerAnimationNameHeader(nWeaponType, nStance).c_str());
	}
}

void XModuleEntity::StartMultiplyColorForBParts( wstring strActorNodeName, int nBPartsIndex )
{
	if (m_pActor && nBPartsIndex < BREAKABLEPARTS_MAX_COUNT)
	{
		m_RegMultiplyColorForBParts[nBPartsIndex].regMultiplyColor.SetTickTime(0.2f);
		m_RegMultiplyColorForBParts[nBPartsIndex].regMultiplyColor.Start();
		m_RegMultiplyColorForBParts[nBPartsIndex].strActorNodeName = strActorNodeName;

		m_pActor->EnableAddColorEffect(strActorNodeName.c_str(), vec3(XCONST::HIT_EFFECT_BPARTS_ADD_COLOR.x, XCONST::HIT_EFFECT_BPARTS_ADD_COLOR.y, XCONST::HIT_EFFECT_BPARTS_ADD_COLOR.z));
	}
	
}

void XModuleEntity::SetFaceBlinkAnimationActive( bool bActive )
{
	m_pAnimationController->SetFaceBlinkAnimationActive(bActive);
}

void XModuleEntity::SerializeBPartNPC( XNPCInfo* pNPCInfo, std::wstring strMeshNodeName, std::wstring strActorNodeName, bool bLoadingAsync /*= true*/ )
{
	if (global.smgr == NULL ||
		pNPCInfo == NULL) 
		return;

	XResourceLoadingPolicy policy = LOADING_ASYNC;
	if(bLoadingAsync == false)
		policy = LOADING_SYNC;

	m_pSerializer->SerializeBPartNPC(pNPCInfo, strMeshNodeName, strActorNodeName, policy);
}

bool XModuleEntity::CreateRide()
{
	if( ETID_PLAYER != m_pOwner->GetEntityType() )
		return false;

	DeleteRide();
	
	XPlayer* pPlayer		= m_pOwner->AsPlayer();
	if( NULL == pPlayer )
		return false;

	int nRideID		= pPlayer->GetRideID();
	if( 0 == nRideID )
		return false;

	if( NULL == m_pRideActorMgr )
		return false;

	if( false == m_pRideActorMgr->CreateRideActor( nRideID ) )
	{
		m_pRideActorMgr->DeleteRideActor();
		return false;
	}

	// 위치 한번 변경
	_UpdateTransform();

	return true;
}

void XModuleEntity::DeleteRide()
{
	if( ETID_PLAYER != m_pOwner->GetEntityType() )
		return ;
	
	XPlayer* pPlayer		= m_pOwner->AsPlayer();
	if( NULL == pPlayer )
		return ;

	if( NULL == m_pRideActorMgr )
		return ;

	pPlayer->GetRideInfo()->SetRide( false );

	m_pRideActorMgr->DeleteRideActor();
	
	// 위치 한번 변경
	_UpdateTransform();
	m_pAnimationController->SetAnimation( wstring(L"none_idle") );
}

void XModuleEntity::UpdateRide()
{
	if( ETID_PLAYER != m_pOwner->GetEntityType() )
		return ;

	XPlayer* pPlayer		= m_pOwner->AsPlayer();
	if( NULL == pPlayer )
		return ;

	if( false == pPlayer->IsRide() )
		return ;

	if( NULL == m_pRideActorMgr )
		return ;
	
	MMatrix mRide;
	if( true == m_pRideActorMgr->GetRideDummyTransform( mRide ) )
	{
		// 탈것의 더미 위치에 캐릭터를 위치시킨다
		m_pActor->SetTransform( mRide );
		m_pActor->UpdateTransform();
		m_pActor->UpdateVisibility();
	}
}

void XModuleEntity::RideCheckLoadingCompleted()
{
	if( ETID_PLAYER != m_pOwner->GetEntityType() )
		return ;

	if( NULL == m_pRideActorMgr )
		return ;

	if( true == m_pRideActorMgr->IsRideLoadingCompleted() )
		return ;

	if( false == m_pRideActorMgr->CheckLoadingCompleted() )
		return ;

	XPlayer* pPlayer		= m_pOwner->AsPlayer();
	if( pPlayer )
		pPlayer->GetRideInfo()->SetRide( true );

	m_pAnimationController->SetAnimation( wstring(L"none_idle_ride") );

	// 위치 한번 변경
	_UpdateTransform();
}
XCharacter* XModuleEntity::GetCollisionActor()
{
	if( ETID_PLAYER != m_pOwner->GetEntityType() )
		return GetActor();

	XPlayer* pPlayer		= m_pOwner->AsPlayer();
	if( ( pPlayer ) && ( true == pPlayer->IsRide() ) )
	{
		if( NULL == m_pRideActorMgr )
			return GetActor();
		else
			return m_pRideActorMgr->GetRideCharacter();
	}

	return GetActor();
}

XGameTransformControllerManager* XModuleEntity::GetRideAnimationController()
{
	if( ETID_PLAYER != m_pOwner->GetEntityType() )
		return GetAnimationController();

	XPlayer* pPlayer		= m_pOwner->AsPlayer();
	if( ( pPlayer ) && ( true == pPlayer->IsRide() ) )
	{
		if( NULL == m_pRideActorMgr )
			return GetAnimationController();
		else
			return m_pRideActorMgr->GetRideAnimationController();
	}

	return GetAnimationController();
}

XRideActorMgr::XRideActorMgr( XModuleEntity* pOwner )
:m_pOwnerEntity( pOwner ), m_pRideActor( NULL ), m_pRideAnimationController( NULL ),
m_pRideSerializer( NULL ), m_bRideLoadingCompleted( false )
{
	
}

XRideActorMgr::~XRideActorMgr()
{
	DeleteRideActor();
}

bool XRideActorMgr::CreateRideActor( int nRideID )
{
	if( 0 == nRideID )
		return false;

	XNPCInfo* pRideInfo = info.npc->Get( nRideID );
	if( NULL == pRideInfo )
		return false;

	if( NULL == m_pOwnerEntity )
		return false;

	DeleteRideActor();

	// Actor & AnmationController
	if( true == global.system->IsResourceLoading() )
	{
		m_pRideActor					= global.actor_loader->CreateActor();
		m_pRideAnimationController		= new XGameTransformControllerManager( m_pRideActor, m_pOwnerEntity->GetTransformController<MPxAniLookUpTC>() );
	}
	else
	{
		m_pRideActor					= new MockCharacter();
		m_pRideAnimationController		= new MockAnimationController( m_pRideActor, m_pOwnerEntity->GetTransformController<MPxAniLookUpTC>() );
	}

	m_pRideActor->SetUsable( false );
	m_pRideActor->SetVisible( false );
	m_pRideActor->SetActorListener( m_pOwnerEntity );
	m_pRideActor->SetUseDoubleShadow( true );

	m_bRideLoadingCompleted		= false;

	// Serializer
	m_pRideSerializer					= new XCharacterSerializer( m_pOwnerEntity, m_pRideActor );

	// Model Load
	SerializeRide( pRideInfo );

	// AddSceneNode
	AddToSceneManager();

	return true;
}

void XRideActorMgr::DeleteRideActor()
{
	m_bRideLoadingCompleted		= false;

	if( m_pRideActor )
		m_pRideActor->RemoveFromParent();

	if (global.actor_loader)
		global.actor_loader->DeleteActor( m_pRideActor );
	else
		SAFE_DELETE( m_pRideActor );

	SAFE_DELETE( m_pRideAnimationController );

	SAFE_DELETE( m_pRideSerializer );
}

XCharacter* XRideActorMgr::GetRideCharacter()
{
	return m_pRideActor;
}

XGameTransformControllerManager* XRideActorMgr::GetRideAnimationController()
{
	return m_pRideAnimationController;
}

XCharacterSerializer* XRideActorMgr::GetRideSerializer()
{
	return m_pRideSerializer;
}

bool XRideActorMgr::IsRideLoadingCompleted()
{
	return m_bRideLoadingCompleted;
}

void XRideActorMgr::SerializeRide( XNPCInfo* pNPCInfo, bool bLoadingAsync )
{
	if( NULL == global.smgr )
		return;

	if( NULL == m_pRideSerializer )
		return ;

	XResourceLoadingPolicy policy = LOADING_ASYNC;
	if( false == bLoadingAsync )
		policy = LOADING_SYNC;

	float fScale		= m_pOwnerEntity->GetScale();

	m_pRideSerializer->SerializeNPC( pNPCInfo, fScale, policy );
}

void XRideActorMgr::UpdateSerializer()
{
	if( m_pRideSerializer )
		m_pRideSerializer->Update();
}

void XRideActorMgr::UpdateAnimationController( float fDelta )
{
	if( m_pRideAnimationController )
		m_pRideAnimationController->Update( fDelta );
}

void XRideActorMgr::UpdateAllSceneNodes()
{
	if( m_pRideActor )
		m_pRideActor->UpdateAllSceneNodes();
}

void XRideActorMgr::SetVisible( bool bVisible )
{
	if( m_pRideActor )
		m_pRideActor->SetVisible( bVisible );
}

void XRideActorMgr::SetTransform( MMatrix& mat )
{
	if( m_pRideActor )
	{
		m_pRideActor->SetTransform( mat );
		m_pRideActor->UpdateTransform();
		m_pRideActor->UpdateVisibility();
	}
}

void XRideActorMgr::AddToSceneManager()
{
	if( ( m_pRideActor ) && ( NULL == m_pRideActor->GetParent() ) )
		global.smgr->AddSceneNode( m_pRideActor );
}

void XRideActorMgr::RemoveFromSceneManager()
{
	if( m_pRideActor )
		m_pRideActor->RemoveFromParent();
}

bool XRideActorMgr::GetRideDummyTransform( MMatrix& mat )
{
	RActorNode* pNode = m_pRideActor->GetActorNodeTransform( _T("ride_Dummy"), mat, E_TS_WORLD );
	if( pNode )
		return true;
	else 
		return false;
}

bool XRideActorMgr::CheckLoadingCompleted()
{
	if( true == m_bRideLoadingCompleted )
		return true;

	if( NULL == m_pRideActor )
		return false;

	if( false == m_pRideActor->IsLoadingCompleted() )
		return false;

	if( false == m_pRideActor->GetUsable() )
		return false;

	m_bRideLoadingCompleted		= true;

	m_pRideActor->SetVisible( true );
	m_pRideAnimationController->OnLoadingComplete();

	// DefaultAnmation 위치 변경해야함 임시
	m_pRideAnimationController->SetAnimation( wstring(L"ride_idle") );

	return m_bRideLoadingCompleted;
}