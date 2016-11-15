#include "StdAfx.h"
#include "GAsyncTask_MoveLoginServer.h"
#include "GPlayerLogoutDBExecuter.h"
#include "GGlobal.h"
#include "GPlayerObjectManager.h"
#include "GPlayerObject.h"
#include "GSystem.h"
#include "GMoveServerRouter.h"
#include "GMasterServerFacade.h"
#include "CCommandResultTable.h"


GAsyncTask_MoveLoginServer::GAsyncTask_MoveLoginServer(MUID uid, uint32 nNowTime)
: GAsyncTask(uid, nNowTime)
, m_stateMoveTask(MTS_INITIAL)
, m_pPlayerLogoutDBExecuter(NULL)
{
}

GAsyncTask_MoveLoginServer::~GAsyncTask_MoveLoginServer()
{
	SAFE_DELETE(m_pPlayerLogoutDBExecuter);
}

void GAsyncTask_MoveLoginServer::Start(MUID uidPlayer)
{
	if (m_stateMoveTask != MTS_INITIAL)
	{
		return;
	}

	m_uidPlayer = uidPlayer;

	GPlayerObject* pPlayerObject = gmgr.pPlayerObjectManager->GetPlayer(m_uidPlayer);
	if (pPlayerObject == NULL)
	{
		Done();
		return;
	}

	if (pPlayerObject->IsReadyMoveLoginServer() == false)
	{
		mlog("Failed! GAsyncTask_MoveLoginServer::Start(), PlayerObject::IsReadyMoveLoginServer() is false! (PlayerUID: (%u:%u) , POS: %d)\n"
			, m_uidPlayer.High, m_uidPlayer.Low, pPlayerObject->GetState());
		Done();
		return;
	}

	m_pPlayerLogoutDBExecuter = new GPlayerLogoutDBExecuter(pPlayerObject);

	m_stateMoveTask = MTS_LOGOUT_DB_EXECUTE;
	OnUpdate();
}

void GAsyncTask_MoveLoginServer::OnUpdate()
{
	// 객체가 사라졌다면, 테스크를 끝낸다.
	GPlayerObject* pPlayerObject = gmgr.pPlayerObjectManager->GetPlayer(m_uidPlayer);
	if (pPlayerObject == NULL)
	{
		Done();
		return;
	}

	// 대기
	switch(GetState())
	{
	case MTS_INITIAL:
		// do nothing
		break;
	case MTS_LOGOUT_DB_EXECUTE:
		OnTaskState_LOGOUT_DB_EXECUTE();
		break;
	case MTS_ROUTE_REQUEST_MOVE_SERVER:
		OnTaskState_ROUTE_REQUEST_MOVE_SERVER(pPlayerObject);
		break;
	case MTS_WAIT_RESPONSE_COMMAND:
		OnTaskState_WAIT_RESPONSE_COMMAND(pPlayerObject);
		break;
	default:
		Done();
		break;
	}
}

void GAsyncTask_MoveLoginServer::OnTaskState_LOGOUT_DB_EXECUTE()
{
	if (GetState() != MTS_LOGOUT_DB_EXECUTE) return;

	m_pPlayerLogoutDBExecuter->Update();
	if (!m_pPlayerLogoutDBExecuter->IsDone())
		return;

	SetState(MTS_ROUTE_REQUEST_MOVE_SERVER);
}

void GAsyncTask_MoveLoginServer::OnTaskState_ROUTE_REQUEST_MOVE_SERVER(GPlayerObject* pPlayerObject)
{
	_ASSERT(NULL != pPlayerObject);
	if (GetState() != MTS_ROUTE_REQUEST_MOVE_SERVER) return;

	// 마스터서버에 전달
	GMoveServerRouter* pRouter = gsys.pMasterServerFacade->GetMoveServerRouter();
	RVALID(pRouter != NULL);
	pRouter->RouteReserveMoveLoginServerReq(pPlayerObject->GetUID());

	// 응답대기 상태로 변경
	SetState(MTS_WAIT_RESPONSE_COMMAND);

	// 응답대기 타임아웃 시작 시간 설정
	m_nTimeoutStartTime = gsys.pSystem->GetNowTime();
}

void GAsyncTask_MoveLoginServer::OnTaskState_WAIT_RESPONSE_COMMAND(GPlayerObject* pPlayerObject)
{
	_ASSERT(NULL != pPlayerObject);
	if (GetState() != MTS_WAIT_RESPONSE_COMMAND) return;

	// 서버이동 응답 커맨드가 도착해서 상태가 변경되었으면, 서버이동 시작.
	if (pPlayerObject->IsMoveLoginServer())
	{
		pPlayerObject->GoMoveLoginServer(CR_SUCCESS);
		Done();
		return;
	}

	// 타임아웃 처리
	if (CheckTimeout())
	{
		mlog2("Timeout! GAsyncTask_MoveLoginServer::OnTaskState_WAIT_RESPONSE_COMMAND()\n");

		pPlayerObject->CancelMoveLoginServer(CR_MOVE_TO_SELECT_CHAR_TIMEOUT);	// 서버이동 취소
		Done();
		return;
	}
}


bool GAsyncTask_MoveLoginServer::CheckTimeout()
{
	uint32 nNowTime = gsys.pSystem->GetNowTime();
	if ((nNowTime - m_nTimeoutStartTime) > TIMEOUT_TICK_MOVE_SERVER)
		return true;

	return false;
}
