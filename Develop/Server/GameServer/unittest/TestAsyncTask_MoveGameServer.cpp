#include "stdafx.h"
#include "GUTHelper.h"
#include "GUTHelper_Field.h"
#include "GUnitTestUtil.h"

#include "GSystem.h"
#include "GAsyncTask.h"
#include "GAsyncTaskFactory.h"
#include "GServer.h"
#include "GAsyncTask_MoveGameServer.h"
#include "GEntityPlayerDBAsyncMonitor.h"
#include "CCommandTable_Login.h"
#include "CCommandTable_Master.h"

#include "FBaseMockLink.h"
#include "FBaseNetClient.h"
#include "FBasePlayer.h"
#include "FBaseFieldMgr.h"

#include "MockField.h"
#include "CCommandResultTable.h"



SUITE(AsyncTask_MoveGameServer)
{
	struct Fixture : public FBasePlayer, FBaseFieldMgr, FBaseMockLink, FBaseNetClient
	{
		Fixture()
		{
			InitAsyncTaskManager();

			MockField* pField = GUTHelper_Field::DefaultMockField();
			m_pEntityPlayer = NewEntityPlayer(pField);

			m_pNetAgent->ResetSendCommands();

			m_pPlayerLink = NewLink(m_pEntityPlayer);
		}
		virtual ~Fixture()
		{
			InitAsyncTaskManager();
		}

		TD_LOGIN_GAME_SERVER_INFO MakeLoginGameServerInfo(wstring strHostName, int nPort, MUID uidConnectionKey)
		{
			TD_LOGIN_GAME_SERVER_INFO LoginGameServerInfo;
			wcsncpy_s(LoginGameServerInfo.strHostName, strHostName.c_str(), _TRUNCATE);
			LoginGameServerInfo.nPort = nPort;
			LoginGameServerInfo.uidConnectionKey = uidConnectionKey;

			return LoginGameServerInfo;
		}

		void InitAsyncTaskManager()
		{
			m_pAsyncTaskManager = gsys.pServer->GetAsyncTaskManager();
			m_pAsyncTaskManager->Clear();
		}

		MCommandResult RecvCommand_ResponseReserveMoveGameServer(MUID uidRequestPlayer, int nResult, const TD_LOGIN_GAME_SERVER_INFO* pLoginGameServerInfo)
		{
			int nCount = 1;
			if (nResult != CR_SUCCESS || pLoginGameServerInfo == NULL)
			{
				nCount = 0;
			}

			return m_pNetAgent->OnRecv(MMC_COMM_RESPONSE_RESERVE_MOVE_GAME_SERVER,
				3,
				NEW_UID(uidRequestPlayer),
				NEW_INT(nResult),
				NEW_BLOB(pLoginGameServerInfo, sizeof(TD_LOGIN_GAME_SERVER_INFO), nCount));
		}

		GEntityPlayer* m_pEntityPlayer;
		MockLink* m_pPlayerLink;

		GAsyncTaskManager* m_pAsyncTaskManager;
	};

	TEST_FIXTURE(Fixture, TestAsyncTask_MoveGameServer_StateChanging)
	{
		int nTargetGameServerID = SUnitTestUtil::NewID();
		GAsyncTask_MoveGameServer* pNewAsyncTask = GAsyncTaskFactory::NewMoveGameServerTask(m_pAsyncTaskManager);

		CHECK_EQUAL(GAsyncTask_MoveGameServer::MTS_INITIAL, pNewAsyncTask->GetState());

		pNewAsyncTask->Start(m_pEntityPlayer->GetUID(), nTargetGameServerID);
		CHECK_EQUAL(GAsyncTask_MoveGameServer::MTS_PACK_AND_ROUTE_PLAYDATA, pNewAsyncTask->GetState());

		pNewAsyncTask->Update();
		CHECK_EQUAL(GAsyncTask_MoveGameServer::MTS_WAIT_RESPONSE_COMMAND, pNewAsyncTask->GetState());
	}

	TEST_FIXTURE(Fixture, TestAsyncTask_MoveGameServer_Update_WAIT_RESPONSE_COMMAND_ShouldPost_REQUEST_RESERVE_MOVE_GAME_SERVER)
	{
		int nTargetGameServerID = SUnitTestUtil::NewID();
		GAsyncTask_MoveGameServer* pNewAsyncTask = GAsyncTaskFactory::NewMoveGameServerTask(m_pAsyncTaskManager);
		pNewAsyncTask->Start(m_pEntityPlayer->GetUID(), nTargetGameServerID);
		CHECK_EQUAL(GAsyncTask_MoveGameServer::MTS_PACK_AND_ROUTE_PLAYDATA, pNewAsyncTask->GetState());

		pNewAsyncTask->Update();

		CHECK_EQUAL(1, m_pNetAgent->GetSendCommandCount());
		CHECK_EQUAL(MMC_COMM_REQUEST_RESERVE_MOVE_GAME_SERVER, m_pNetAgent->GetSendCommand(0).GetID());
	}

	TEST_FIXTURE(Fixture, TestAsyncTask_MoveGameServer_Recv_RESPONSE_RESERVE_MOVE_GAME_SERVER)
	{
		int nTargetGameServerID = SUnitTestUtil::NewID();
		GAsyncTask_MoveGameServer* pNewAsyncTask = GAsyncTaskFactory::NewMoveGameServerTask(m_pAsyncTaskManager);
		pNewAsyncTask->Start(m_pEntityPlayer->GetUID(), nTargetGameServerID);
		pNewAsyncTask->Update();
		CHECK_EQUAL(GAsyncTask_MoveGameServer::MTS_WAIT_RESPONSE_COMMAND, pNewAsyncTask->GetState());

		/// Recv RESPONSE_RESERVE_MOVE_GAME_SERVER
			GPlayerObject* pPlayerObject = gmgr.pPlayerObjectManager->GetPlayer(m_pEntityPlayer->GetUID());

			GGateInfo gateInfo;
			pPlayerObject->OnStartGame();
			pPlayerObject->ReadyMoveGameServer(2, gateInfo);

			wstring strHostName = GUnitTestUtil::NewString();
			int nPort = 200;
			MUID uidConnectionKey = SUnitTestUtil::NewUID();		
			TD_LOGIN_GAME_SERVER_INFO LoginGameServerInfo = MakeLoginGameServerInfo(strHostName, nPort, uidConnectionKey);
			RecvCommand_ResponseReserveMoveGameServer(m_pEntityPlayer->GetUID(), CR_SUCCESS, &LoginGameServerInfo);
		//////////////////////////////////////////////////////////////////////////

		CHECK_EQUAL(true, pPlayerObject->IsGetSetMoveGameServer());

		// 다시 업데이트
		pNewAsyncTask->Update();
		CHECK_EQUAL(true, pNewAsyncTask->IsDone());

		CHECK_EQUAL(1, m_pPlayerLink->GetCommandCount());
		CHECK_EQUAL(MC_COMM_MOVE_GAME_SERVER, m_pPlayerLink->GetCommand(0).GetID());
	}

	TEST_FIXTURE(Fixture, TestAsyncTask_MoveGameServer_Timeout)
	{
		GTestSysWrapper2<GSystem, GUnitTestUtil::FakeSystem>	FakeSystemWrapper;

		GAsyncTask_MoveGameServer* pNewAsyncTask = GAsyncTaskFactory::NewMoveGameServerTask(m_pAsyncTaskManager);
		int nTargetGameServerID = SUnitTestUtil::NewID();
		pNewAsyncTask->Start(m_pEntityPlayer->GetUID(), nTargetGameServerID);
		pNewAsyncTask->Update();
		m_pNetAgent->ResetSendCommands();


		gsys.pSystem->Sleep((TIMEOUT_TICK_MOVE_SERVER/1000) + 1);

		pNewAsyncTask->Update();

		CHECK(pNewAsyncTask->IsDone());
	}
}
