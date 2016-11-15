#include "StdAfx.h"
#include "GAsyncTask_Logout.h"
#include "GGlobal.h"
#include "GPlayerObjectManager.h"
#include "GPlayerObject.h"
#include "GEntityPlayerDBAsyncMonitor.h"
#include "GDBManager.h"
#include "GActorCooltimeChecker.h"
#include "GServerPlayerRouter.h"
#include "GMasterServerFacade.h"
#include "PmRegionStat.h"
#include "GPMSSystem.h"
#include "GDBCacheFlush.h"
#include "GDBCacheSystem.h"
#include "GConst.h"
#include "GScriptManager.h"
#include "GPlayerDBCache.h"
#include "GConfig.h"
#include "GPartySystem.h"
#include "GPlayerLogoutDBExecuter.h"
#include "GDBTaskDataConnection.h"
#include "GAsyncTask_MoveGameServer.h"
#include "GAsyncTask_MoveLoginServer.h"
#include "GServer.h"



GAsyncTask_Logout::GAsyncTask_Logout(MUID uid, uint32 nNowTime, MUID uidPlayer)
: GAsyncTask(uid, nNowTime)
, m_uidPlayer(uidPlayer)
, m_state(StartLogoutState)
, m_isLeaveGame(true)
, m_isBackToLogin(false)
, m_pPlayerLogoutDBExecuter(NULL)
{
	m_pPlayerObject = gmgr.pPlayerObjectManager->GetPlayer(m_uidPlayer);
	if (m_pPlayerObject == NULL)
	{
		Done();
		return;
	}

	m_pEntityPlayer = m_pPlayerObject->GetEntity();
	if (m_pEntityPlayer == NULL)
	{
		mlog3("Error! GAsyncTask_Logout::GAsyncTask_Logout(), Not Exist EntityPlayer(%d, %d)\n", uidPlayer.High, uidPlayer.Low);
		Done();
		return;
	}
	
	m_pPlayerLogoutDBExecuter = new GPlayerLogoutDBExecuter(m_pPlayerObject);
}

GAsyncTask_Logout::~GAsyncTask_Logout()
{
	SAFE_DELETE(m_pPlayerLogoutDBExecuter);
}

void GAsyncTask_Logout::OnUpdate()
{
	switch(m_state)
	{
	case StartLogoutState:				if (!OnStartLogout())					break;
	case LogoutDBExecuteState:			if (!OnLogoutDBExecuteState())			break;
	case CheckMoveServerAsyncTaskState:	if (!OnCheckMoveServerAsyncTaskState())	break;
	case PublisherLogoutTaskState:		if (!OnPublisherLogoutTask())			break;
	case DestroyPlayerState:			if (!OnDestroyPlayer())					break;
	case FinalState:					Done();									break;
	}
}

bool GAsyncTask_Logout::OnStartLogout()
{
	// 먼저 시작한 로그아웃 처리가 있다면, 중복처리하지 않는다.
	if (POS_LOGOUT == m_pPlayerObject->GetState())
	{
		SetState(FinalState);
		return true;
	}

	// StandAlone 모드의 캐릭터 선택 중 접속 종료는 DB 작업을 하지 않는다.
	if (GConfig::IsStandAloneMode()
		&& m_pPlayerObject->GetState() == POS_SELECT_CHAR)
	{
		SetState(DestroyPlayerState);
		return true;
	}


	if (m_pPlayerObject->IsMoveGameServer() ||
		m_pPlayerObject->IsMoveLoginServer())
	{
		// 서버 이동 중이었을 경우 DB처리가 진행 중이거나 끝났으므로 처리하지 않고 대기만 합니다.
		m_isLeaveGame = false;
		if (m_pPlayerObject->IsMoveLoginServer())
			m_isBackToLogin = true;

		SetState(CheckMoveServerAsyncTaskState);
	}
	else
	{
		// DB 처리 시작
		SetState(LogoutDBExecuteState);
	}

	// 로그아웃 상태로 변경
	m_pPlayerObject->OnLogout();
	return true;
}

bool GAsyncTask_Logout::OnLogoutDBExecuteState()
{
	if (GetState() != LogoutDBExecuteState)
		return true;

	m_pPlayerLogoutDBExecuter->Update();
	if (!m_pPlayerLogoutDBExecuter->IsDone())
		return false;

	SetState(PublisherLogoutTaskState);
	return true;
}

bool GAsyncTask_Logout::OnCheckMoveServerAsyncTaskState()
{
	if (GetState() != CheckMoveServerAsyncTaskState)
		return true;

	if (!CheckMoveServerAsyncTask_GameServer())
		return false;

	if (!CheckMoveServerAsyncTask_LoginServer())
		return false;

	SetState(PublisherLogoutTaskState);
	return true;
}
bool GAsyncTask_Logout::CheckMoveServerAsyncTask_GameServer()
{
	MUID uidTask = m_pPlayerObject->GetMoveGameServerAsyncTaskUID();
	GAsyncTask_MoveGameServer* pTask = static_cast<GAsyncTask_MoveGameServer*>(gsys.pServer->GetAsyncTaskManager()->FindTask(uidTask));
	if (pTask == NULL) return true;
	
	if (pTask->IsDone())
		return true;

	if (pTask->GetState() == GAsyncTask_MoveGameServer::MTS_INITIAL ||
		pTask->GetState() == GAsyncTask_MoveGameServer::MTS_LOGOUT_DB_EXECUTE)
		return false;	///< DB처리 끝날 때까지 기다립시다.

	gsys.pServer->GetAsyncTaskManager()->Delete(uidTask);
	return true;
}
bool GAsyncTask_Logout::CheckMoveServerAsyncTask_LoginServer()
{
	MUID uidTask = m_pPlayerObject->GetMoveLoginServerAsyncTaskUID();
	GAsyncTask_MoveLoginServer* pTask = static_cast<GAsyncTask_MoveLoginServer*>(gsys.pServer->GetAsyncTaskManager()->FindTask(uidTask));
	if (pTask == NULL) return true;

	if (pTask->IsDone())
		return true;
	
	if (pTask->GetState() == GAsyncTask_MoveLoginServer::MTS_INITIAL ||
		pTask->GetState() == GAsyncTask_MoveLoginServer::MTS_LOGOUT_DB_EXECUTE)
		return false;	///< DB처리 끝날 때까지 기다립시다.

	gsys.pServer->GetAsyncTaskManager()->Delete(uidTask);
	return true;
}

bool GAsyncTask_Logout::OnPublisherLogoutTask()
{
	if (GetState() != PublisherLogoutTaskState)
		return true;

	// 피망 통계정보 정리
	if (m_pPlayerObject->IsExistStatIndex())
	{
		PmRegionStat* pRegionStat = gsys.pPMSSystem->GetRegionStat();
		PmStatIndex* pStatIndex = m_pPlayerObject->GetStatIndex();

		// 지역 코드 예외처리
		PmRegionCode regionCode = pStatIndex->nRegionCode;
		PmAgeCode ageCode = pStatIndex->nAgeCode;
		PmGenderCode genderCode = pStatIndex->nGenderCode;
		int nPCCafeID = m_pPlayerObject->GetPCCafeID();

		pRegionStat->RemoveUser(regionCode, ageCode, genderCode, nPCCafeID);
	}
		
	SetState(DestroyPlayerState);
	return true;
}

bool GAsyncTask_Logout::OnDestroyPlayer()
{
	if (GetState() != DestroyPlayerState)
		return true;
	
	// 종료/이동 처리
	if (IsLeaveGame())
	{
		// 마스터 서버에 접속 종료 알림	
		GServerPlayerRouter* pServerRouter = gsys.pMasterServerFacade->GetPlayerRouter();			
		pServerRouter->RouteDeletePlayer((int)m_pPlayerObject->GetAccountInfo().nAID, m_pPlayerObject->GetUID());

		// 접속종료시간 기록
		gsys.pDBManager->DisconnLogInsert(
			GDBT_DISCONN_LOG(m_pPlayerObject->GetAccountInfo().nAID
				, m_pPlayerObject->GetEntity()->GetCID()
				, m_pPlayerObject->GetEntity()->GetPlayerInfo()->nPlayTimeSec
				, m_pPlayerObject->GetEntity()->GetPlayerInfo()->GetDeltaPlayTime()
				, m_pPlayerObject->GetEntity()->GetMoney()
				, m_pPlayerObject->GetEntity()->GetPlayerInfo()->nLootGainMoney
				, m_pPlayerObject->GetEntity()->GetFieldID()));
	}
	else
	{
		// 서버 이동 상태 설정 - 플레이어 엔티티 제거 시, 파티 오프라인 처리여부 결정
		if (!m_isBackToLogin)
			m_pEntityPlayer->SetMoveServer(true);
	}

	gmgr.pPlayerObjectManager->DeletePlayer(m_uidPlayer);
	gsys.pDBCacheSystem->DeletePlayerCache(m_uidPlayer);
	
	SetState(FinalState);
	return true;
}

GAsyncTask_Logout::State GAsyncTask_Logout::GetState()	
{
	return m_state;
}

void GAsyncTask_Logout::SetState(GAsyncTask_Logout::State state)
{
	m_state = state;
}

bool GAsyncTask_Logout::IsLeaveGame()
{
	return m_isLeaveGame;
}
