#include "stdafx.h"
#include "XGame.h"
#include "XWorld.h"
#include "MockWorld.h"
#include "RCameraSceneNode.h"
#include "XNonPlayer.h"
#include "XModuleEntity.h"
#include "XModuleMyControl.h"
#include "XUtil.h"
#include "XNPCInfo.h"
#include "XEventID.h"
#include "XMyPlayer.h"
#include "XItemContainer.h"
#include "XController.h"
#include "XModuleNetControl.h"
#include "XProfiler.h"
#include "RDirectionalLightSceneNode.h"
#include "REnvironmentSet.h"
#include "XPost_Loot.h"
#include "XPost_Interaction.h"
#include "XStrings.h"
#include "XItemManager.h"
#include "XBaseApplication.h"
#include "XEffectManager.h"
#include "XResourceManager.h"
#include "XCameraManager.h"
#include "XSoulObject.h"
#include "XModuleNonControl.h"
#include "XFeedbackObject.h"
#include "XFeedbackSystem.h"

#include "XSystem.h"
#include "XMagicArea.h"
#include "XTalentInfoMgr.h"
#include "XBattleArena.h"
#include "XModuleBuff.h"
#include "XPvPAreaLogic.h"


XGame::XGame(bool bRealGameNotTest) : m_nUpdateObjectCacheTime(0), m_bEnableUpdateObjectCache(true)
{
	gg.game = this;
	gg.omgr = &m_ObjectManager;

	m_pCurrentGameState = m_GameStateMgr.CreateGameState(bRealGameNotTest);
	gg.currentgamestate = m_pCurrentGameState;
	gg.controller		= m_pCurrentGameState->GetController();
}

XGame::~XGame()
{
	Destroy();

	gvar.Game.pMyPlayer = NULL;
	gg.game = NULL;
	gg.omgr = NULL;
	gg.currentgamestate = NULL;
	gg.controller = NULL;
}

bool XGame::Create()
{
	// 게임에 쓰이는 클래스들의 Create함수들이다. 
	// 게임 이외는 Create가 필요가 없다.

	// 월드 만들기
	if (m_pCurrentGameState->Create() == false)
		return false;

	if (gg.controller)
	{
		if (gg.controller->Create() == false)
			return false;

		gg.controller->SetEnabled(true);
	}

	// by pok, 전투 피격후 lack 원인을 알기위해 주석처리 되어있던 MProfile2를 다시 엽니다.
	// 기본적으로 MInitStatisticsProfile(true, false, false); 으로 해놓고 씁니다.
	// 자세히 프로파일링을 하려고 할때 맡으신분께서 옵션을 열어 사용합니다.
	// 레코드 옵션을 열면 메모리 사용량이 급격히 높아질 수 있습니다.
#ifndef _PUBLISH
	MInitStatisticsProfile(true, false);

	if (global.script)		global.script->GetGlueGameEvent().OnDebugOnGameStarted();
	if (global.ui)			global.ui->SetLogCallbackToScript();
#endif

	m_bCreated = true;

	return true;
}

void XGame::Destroy()
{
	if (!m_bCreated) return;
	m_bCreated = false;

	// 씬 매니저와 오부젝트 매니저를 리셋해주기 전에 AsyncLoading을 멈춤
	REngine::GetBackgroundWorker().ClearQueues();

	m_ObjectManager.Destroy();
	m_GameStateMgr.DestroyGameState();

	// 이펙트 리셋
	if(global.emgr)
		global.emgr->Destroy();

	// 씬 매니저 리셋
	if (global.smgr)
		global.smgr->Clear();

	if (global.res)
		global.res->FiniCaching();
}

void XGame::Update(float fDelta)
{
	PFC_THISFUNC;
	PFI_BLOCK_THISFUNC(200);

	// game controller update : input / camera / target
	gg.controller->OnUpdate(fDelta);

	m_pCurrentGameState->Update(fDelta);

	PFI_B(202, "XGame::Update : m_ObjectManager.Update");
	PFC_B("XGame::Update : m_ObjectManager.Update");
	m_ObjectManager.Update(fDelta);
	PFC_E;
	PFI_E(202);

	PFI_B(203, "XGame::Update : global.emgr->OnUpdate");
	PFC_B("XGame::Update : global.emgr->OnUpdate");
	global.emgr->OnUpdate(fDelta);		//이펙트 삭제루틴을 돌린다.
	PFC_E;
	PFI_E(203);

	PFI_B(204, "XGame::Update : UpdateObjectCache");
	PFC_B("XGame::Update : UpdateObjectCache");
	UpdateObjectCache();
	PFC_E;
	PFI_E(204);

	PFI_B(205, "XGame::Update : 기타");
	PFC_B("XGame::Update : 기타");

	UpdateMyInfo(fDelta);

	m_pCurrentGameState->UpdateEnvSound();

	UpdateFeedbackObjects(fDelta);
	//if (gvar.World.nDynamicFieldExpiredTime > 0)
	//{
	//	XRemainTime	time;
	//	time.Set(gvar.World.nDynamicFieldExpiredTime-global.system->GetTimer().GetNowServerTime());

	//	if (!time.bTimeover)
	//	{
	//		mlog("XGame::Update - [%s] 남은시간: %d시간 %d분 %d초 %s\n",
	//			"다이나믹 필드 삭제까지",
	//			time.nHour, time.nMin, time.nSec, time.bTimeover?"[시간초과]":"");
	//	}
	//}

	if (global.res)
		global.res->UpdateCaching(fDelta);

	// 캐릭터가 이동한 만큼 카메라도 이동시켜준다
	gg.controller->UpdateCameraAfterGameUpdate(fDelta);

	PFC_E;
	PFI_E(205);
}

void XGame::OnPreRender()
{
	if ((!m_pCurrentGameState->GetWorld()) || (!m_pCurrentGameState->GetWorld()->GetInfo())) return;
	//REffect *pEffect = m_pWorld->GetWorldEnv()->GetCurrentSky().m_pEffect;
}

void XGame::OnRender()
{
	PFC_THISFUNC;
	PFI_BLOCK_THISFUNC(206);

	m_ObjectManager.Render();

	if(m_pCurrentGameState)
		m_pCurrentGameState->Render();
}

void XGame::UpdateObjectCache()
{
	if (!m_pCurrentGameState->GetWorld()) return;
	if (!m_bEnableUpdateObjectCache) return;

	// 3초마다 업데이트
	const unsigned int OBJECT_CACHE_TICK = 3000;
	unsigned int nNowTime = XGetNowTime();
	if ((nNowTime - m_nUpdateObjectCacheTime) < OBJECT_CACHE_TICK) return;
	m_nUpdateObjectCacheTime = nNowTime;

	XObject* pMyObject = m_ObjectManager.Find(XGetMyUID());
	if (pMyObject == NULL) return;

	vec3 my_pos = pMyObject->GetPosition();
	MPoint my_pt = m_pCurrentGameState->GetWorld()->GetSectorIndex(my_pos.x, my_pos.y);

	for (XObjectMap::iterator itor = m_ObjectManager.BeginItor(); itor != m_ObjectManager.EndItor();)
	{
		XObject* pObj = itor->second;
		if ( pObj == pMyObject || !pObj->IsModuleEntity() )
		{
			++itor;
			continue;
		}

		vec3 pos = pObj->GetPosition();

		MPoint tar_pt = m_pCurrentGameState->GetWorld()->GetSectorIndex(pos.x, pos.y);
		if (XWorld::IsManagedSector(tar_pt.x, tar_pt.y, my_pt.x, my_pt.y, pObj->IsHuge()?HUGE_SECTOR_SCALE:1))
		{
			// 섹터내의 것이라면
			++itor;
		}
		else
		{
			// 섹터내의 것이 아니라면 없애준다
			itor = m_ObjectManager.Erase(pObj);

			if( pMyObject->GetModuleMyControl() )
			{
				XActor* pActor = static_cast<XActor*>(pObj);
				if( pActor )
					pMyObject->GetModuleMyControl()->ClearLockOnTarget( pActor->GetUIID() );
			}

			pObj->Destroy();
			pObj->Drop();
		}
	}
}

void XGame::UpdateMyInfo( float fDelta )
{
	gvar.MyInfo.Talent.CoolTime.Update( fDelta);
}

void XGame::OnWorldLoadingComplete()
{
	m_pCurrentGameState->OnWorldLoadingComplete();
}

void XGame::ChangeField( int nNewFieldID, vec3 vMyPos, vec3 vMyDir )
{
	m_pCurrentGameState->ChangeField(nNewFieldID, vMyPos, vMyDir);
}

/*
	소울 오브젝트 생성

	pOwnerNpc = 소울을 스폰시킬 NPC 포인터
	uidTarget = 소울을 흡수 할 PC의 UID
	nType = 소울 타입(기본 0)
	nWaitTime = 소울이 생성되기 까지 지연 시간(ms단위)
*/

void XGame::UpdateFeedbackObjects( float fDelta )
{
	if (global.feedback)
	{
		global.feedback->Update(fDelta);
	}
}

void XGame::ChangeState( GAME_PLAY_STATE eState, int nParam /*= 0*/ )
{
	m_pCurrentGameState = m_GameStateMgr.ChangeGameStateStart(eState, nParam);
	gg.currentgamestate = m_pCurrentGameState;
	gg.controller		= m_pCurrentGameState->GetController();
}

bool XGame::OnEvent( MEvent& e )
{
	if (gg.controller && gg.controller->OnEvent(e)) 
		return true;

	return false;
}

void XGame::OnActive( bool bActive )
{
	if (gg.controller) gg.controller->OnActive(bActive);
}

void XGame::OnChangeStart( GAME_PLAY_STATE ePlayState, int nParam /*= 0*/ )
{
	ChangeState(ePlayState, nParam);
}

void XGame::OnChangeEnd()
{
	m_GameStateMgr.ChangeGameStateEnd();
}

bool XGame::OnForceEvent( MEvent& e )
{
	if (gg.controller && gg.controller->OnFoceEvent(e)) 
		return true;

	return false;
}

bool XGame::Check18Over()
{
	// 2010.7.19 - 클베1차는 15세로 무조건 진행한다. 사내 테스트로 15세로 한다. 단, const.lua에서 설정하면 18로 인식
	// 이후에는 년도로 체크
	// 2010.7.22 - 피망으로 접속시 18세로 진행한다.
	// 2010.7.23 - 무조건 18세로... 심의를 18세로 받는다고 합니다.
	return true;

	// 피망 조건
	if(gvar.Network.PmangInfo.isLoginOnPmang)
	{
		return gvar.Network.PmangInfo.isAdult;
	}

	// 사내 조건
	if(XCONST::PLAYER_AGE_LIMIT == AL_18_OVER)
		return true;

	return false;
}
