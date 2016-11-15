#include "stdafx.h"
#include "GUTHelper.h"
#include "GUTHelper_Field.h"
#include "FBaseMockLink.h"
#include "FBaseNetClient.h"
#include "FBasePlayer.h"
#include "GAsyncTask_MoveLoginServer.h"
#include "GSystem.h"
#include "GServer.h"
#include "MockField.h"
#include "CCommandTable_Master.h"
#include "CCommandTable.h"
#include "GAsyncTaskFactory.h"
#include "CCommandResultTable.h"
#include "GUnitTestUtil.h"


SUITE(AsyncTask_MoveLoginServer)
{
	struct Fixture : public FBasePlayer, FBaseMockLink, FBaseNetClient
	{
		Fixture()
		{
			InitAsyncTaskManager();

			MockField* pField = GUTHelper_Field::DefaultMockField();
			m_pEntityPlayer = NewEntityPlayer(pField);
			m_PlayerObject = gmgr.pPlayerObjectManager->GetPlayer(m_pEntityPlayer->GetUID());

			m_pNetAgent->ResetSendCommands();

			m_pPlayerLink = NewLink(m_pEntityPlayer);
		}
		~Fixture()
		{
			InitAsyncTaskManager();
		}

		void InitAsyncTaskManager()
		{
			m_pAsyncTaskManager = gsys.pServer->GetAsyncTaskManager();
			m_pAsyncTaskManager->Clear();
		}

		MCommandResult RecvCommand_ReserveMoveLoginServerRes(const MUID& uidRequestPlayer, int nResult, const MUID& uidConnectionKey)
		{
			return m_pNetAgent->OnRecv(MMC_COMM_RESERVE_MOVE_LOGINSERVER_RES,
				3,
				NEW_UID(uidRequestPlayer),
				NEW_INT(nResult),
				NEW_UID(uidConnectionKey));
		}

		GPlayerObject* m_PlayerObject;
		GEntityPlayer* m_pEntityPlayer;
		MockLink* m_pPlayerLink;

		GAsyncTaskManager* m_pAsyncTaskManager;
	};

	TEST_FIXTURE(Fixture, TestAsyncTask_MoveLoginServer_Failure_False_IsReadyMoveLoginServer)
	{
		GAsyncTask_MoveLoginServer* pNewAsyncTask = GAsyncTaskFactory::NewMoveLoginServerTask(m_pAsyncTaskManager);

		CHECK_EQUAL(GAsyncTask_MoveLoginServer::MTS_INITIAL, pNewAsyncTask->GetState());

		pNewAsyncTask->Start(m_pEntityPlayer->GetUID());
		CHECK_EQUAL(true, pNewAsyncTask->IsDone());
		CHECK_EQUAL(GAsyncTask_MoveLoginServer::MTS_INITIAL, pNewAsyncTask->GetState());
	}

	TEST_FIXTURE(Fixture, TestAsyncTask_MoveLoginServer_StateChanging)
	{
		m_PlayerObject->OnReadyMoveLoginServer();
		GAsyncTask_MoveLoginServer* pNewAsyncTask = GAsyncTaskFactory::NewMoveLoginServerTask(m_pAsyncTaskManager);

		CHECK_EQUAL(GAsyncTask_MoveLoginServer::MTS_INITIAL, pNewAsyncTask->GetState());

		pNewAsyncTask->Start(m_pEntityPlayer->GetUID());
		CHECK_EQUAL(GAsyncTask_MoveLoginServer::MTS_ROUTE_REQUEST_MOVE_SERVER, pNewAsyncTask->GetState());

		pNewAsyncTask->Update();
		CHECK_EQUAL(GAsyncTask_MoveLoginServer::MTS_WAIT_RESPONSE_COMMAND, pNewAsyncTask->GetState());
	}

	TEST_FIXTURE(Fixture, TestAsyncTask_MoveLoginServer_Update_WAIT_RESPONSE_COMMAND_ShouldPost_RESERVE_MOVE_LOGINSERVER_REQ)
	{
		m_PlayerObject->OnReadyMoveLoginServer();
		GAsyncTask_MoveLoginServer* pNewAsyncTask = GAsyncTaskFactory::NewMoveLoginServerTask(m_pAsyncTaskManager);
		pNewAsyncTask->Start(m_pEntityPlayer->GetUID());
		CHECK_EQUAL(GAsyncTask_MoveLoginServer::MTS_ROUTE_REQUEST_MOVE_SERVER, pNewAsyncTask->GetState());

		pNewAsyncTask->Update();

		CHECK_EQUAL(1, m_pNetAgent->GetSendCommandCount());
		CHECK_EQUAL(MMC_COMM_RESERVE_MOVE_LOGINSERVER_REQ, m_pNetAgent->GetSendCommand(0).GetID());
	}

	TEST_FIXTURE(Fixture, TestAsyncTask_MoveLoginServer_Recv_RESERVE_MOVE_LOGINSERVER_RES)
	{
		m_PlayerObject->OnReadyMoveLoginServer();
		GAsyncTask_MoveLoginServer* pNewAsyncTask = GAsyncTaskFactory::NewMoveLoginServerTask(m_pAsyncTaskManager);
		pNewAsyncTask->Start(m_pEntityPlayer->GetUID());
		pNewAsyncTask->Update();
		CHECK_EQUAL(GAsyncTask_MoveLoginServer::MTS_WAIT_RESPONSE_COMMAND, pNewAsyncTask->GetState());

		/// Recv RESERVE_MOVE_LOGINSERVER_RES
		GPlayerObject* pPlayerObject = gmgr.pPlayerObjectManager->GetPlayer(m_pEntityPlayer->GetUID());

		GGateInfo gateInfo;
		pPlayerObject->OnReadyMoveLoginServer();

		MUID uidConnectionKey = SUnitTestUtil::NewUID();
		RecvCommand_ReserveMoveLoginServerRes(m_pEntityPlayer->GetUID(), CR_SUCCESS, uidConnectionKey);
		//////////////////////////////////////////////////////////////////////////

		CHECK_EQUAL(true, pPlayerObject->IsMoveLoginServer());

		// 다시 업데이트
		pNewAsyncTask->Update();
		CHECK_EQUAL(true, pNewAsyncTask->IsDone());

		CHECK_EQUAL(1, m_pPlayerLink->GetCommandCount());
		CHECK_EQUAL(MC_COMM_START_MOVE_TO_LOGIN_SERVER_RES, m_pPlayerLink->GetCommand(0).GetID());
	}

	TEST_FIXTURE(Fixture, TestAsyncTask_MoveLoginServer_Timeout)
	{
		GTestSysWrapper2<GSystem, GUnitTestUtil::FakeSystem>	FakeSystemWrapper;

		m_PlayerObject->OnReadyMoveLoginServer();
		GAsyncTask_MoveLoginServer* pNewAsyncTask = GAsyncTaskFactory::NewMoveLoginServerTask(m_pAsyncTaskManager);
		pNewAsyncTask->Start(m_pEntityPlayer->GetUID());
		pNewAsyncTask->Update();
		m_pNetAgent->ResetSendCommands();


		gsys.pSystem->Sleep((TIMEOUT_TICK_MOVE_SERVER/1000) + 1);

		pNewAsyncTask->Update();

		CHECK_EQUAL(true, pNewAsyncTask->IsDone());
		CHECK_EQUAL(1, m_pPlayerLink->GetCommandCount());
		CHECK_EQUAL(MC_COMM_START_MOVE_TO_LOGIN_SERVER_RES,	m_pPlayerLink->GetCommand(0).GetID());
		CHECK_EQUAL(CR_MOVE_TO_SELECT_CHAR_TIMEOUT,			m_pPlayerLink->GetParam<int>(0, 0));
	}

}
