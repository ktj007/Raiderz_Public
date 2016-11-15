#include "StdAfx.h"
#include "LUnitTestUtil.h"
#include "LTestHelper.h"
#include "LPlayerObjectManager.h"
#include "LTestWrapper.h"
#include "MockPlayerObject.h"
#include "MockServer.h"
#include "LTimeoutWatcher.h"
#include "LMoveServerTimeoutHandler.h"
#include "LSelMyCharTimeoutHandler.h"
#include "MockLink.h"
#include "CCommandTable.h"
#include "CTransData.h"
#include "LSystem.h"
#include "CCommandResultTable.h"
#include "FBaseMasterClient.h"
#include "LMasterClient.h"
#include "MockMasterNetClient.h"
#include "CCommandTable_Master.h"


SUITE(DisconnectionWatcher)
{
	struct MockDisconnectServer: public MockServer
	{
		MockDisconnectServer() : MockServer() , m_uidLastDisconnectedPlayer(MUID::ZERO) {}
		virtual ~MockDisconnectServer() {}

		virtual void DisconnectPlayer(MUID uidPlayer) override
		{
			m_uidLastDisconnectedPlayer = uidPlayer;
		}
		void CreateMasterServerNetwork()
		{
			minet::MNetClientDesc master_net_client_desc;
			master_net_client_desc.bKeepAlive = false;
			MockMasterNetClient* pNetClient = new MockMasterNetClient(master_net_client_desc);

			m_pMasterClient = new LMasterClient();
			if (m_pMasterClient->Create(pNetClient) == false)
			{
				mlog("Failed To MasterClient Creation\n");
				return;
			}
		}
		void DestroyMasterServerNetwork()
		{
			m_pMasterClient->Destroy();
			SAFE_DELETE(m_pMasterClient);
		}

		MUID m_uidLastDisconnectedPlayer;
	};
	
	struct Fixture
	{
		Fixture()
		{
			m_pFakeSystem = (LUnitTestUtil::FakeSystem*)gsys.pSystem;
			m_pServer = (MockDisconnectServer*)gsys.pServer;
			m_pServer->CreateMasterServerNetwork();

			watcher.SetTimeoutPeriod(60000);		// 타임아웃 10분

			m_pNetAgent = new minet::MTestNetAgent(m_pServer->GetMasterClient()->GetNetClient());
		}
		~Fixture()
		{
			SAFE_DELETE(m_pNetAgent);
			m_pServer->DestroyMasterServerNetwork();
		}

		MUID AddNewPlayerObject()
		{
			LPlayerObject* pPlayerObject = LTestHelper::NewMockPlayerObject();			
			gmgr.pPlayerObjectManager->AddObject(pPlayerObject);

			return pPlayerObject->GetUID();
		}

		minet::MTestNetAgent*	m_pNetAgent;

		LTimeoutWatcher watcher;
		LMoveServerTimeoutHandler timeoutHandler;
		LTestMgrWrapper<LPlayerObjectManager> PlayerObjectManager;
		LTestSysWrapper2<LServer, MockDisconnectServer> Server;
		LTestSysWrapper2<LSystem, LUnitTestUtil::FakeSystem> System;
		LUnitTestUtil::FakeSystem* m_pFakeSystem;
		MockDisconnectServer* m_pServer;		
	};

	TEST_FIXTURE(Fixture, TestWatcher)
	{
		MUID uidPlayer = AddNewPlayerObject();

		CHECK(m_pServer->m_uidLastDisconnectedPlayer != uidPlayer);

		watcher.AddPlayer(uidPlayer, &timeoutHandler);
		
		gsys.pSystem->Sleep(30);
		watcher.UpdateTimeout();		
		CHECK(m_pServer->m_uidLastDisconnectedPlayer != uidPlayer);

		gsys.pSystem->Sleep(30);
		watcher.UpdateTimeout();		
		CHECK_EQUAL(m_pServer->m_uidLastDisconnectedPlayer, uidPlayer);
		CHECK_EQUAL(1, m_pNetAgent->GetSendCommandCount());
		CHECK_EQUAL(MLC_PLAYER_DELETE_PLAYER, m_pNetAgent->GetSendCommandID(0));
		CHECK_EQUAL(uidPlayer, m_pNetAgent->GetSendParam<MUID>(0, 1));
	}

	TEST_FIXTURE(Fixture, TestWatcherForMultiplePlayer)
	{
		MUID uidPlayer1 = AddNewPlayerObject();
		MUID uidPlayer2 = AddNewPlayerObject();
		MUID uidPlayer3 = AddNewPlayerObject();

		watcher.AddPlayer(uidPlayer1, &timeoutHandler);
		watcher.AddPlayer(uidPlayer2, &timeoutHandler);
		watcher.AddPlayer(uidPlayer3, &timeoutHandler);

		gsys.pSystem->Sleep(60);
		watcher.UpdateTimeout();

		CHECK_EQUAL(m_pServer->m_uidLastDisconnectedPlayer, uidPlayer1);
		CHECK(m_pServer->m_uidLastDisconnectedPlayer != uidPlayer2);
		CHECK(m_pServer->m_uidLastDisconnectedPlayer != uidPlayer3);

		watcher.UpdateTimeout();
		CHECK_EQUAL(m_pServer->m_uidLastDisconnectedPlayer, uidPlayer2);
		CHECK(m_pServer->m_uidLastDisconnectedPlayer != uidPlayer3);
		
		watcher.UpdateTimeout();
		CHECK_EQUAL(m_pServer->m_uidLastDisconnectedPlayer, uidPlayer3);
	}

	TEST_FIXTURE(Fixture, TestWatchOverflowTime)
	{
		m_pFakeSystem->SetNowTime(UINT_MAX);
		MUID uidPlayer1 = AddNewPlayerObject();

		watcher.AddPlayer(uidPlayer1, &timeoutHandler);
		watcher.UpdateTimeout();
		CHECK(m_pServer->m_uidLastDisconnectedPlayer != uidPlayer1);


		m_pFakeSystem->SetNowTime(60000 - 2);		// UNIT_MAX + MOVE_SERVER_TIME_OUT_PERIOD - 1
		watcher.UpdateTimeout();
		CHECK(m_pServer->m_uidLastDisconnectedPlayer != uidPlayer1);


		m_pFakeSystem->SetNowTime(60000 - 1);		// UNIT_MAX + MOVE_SERVER_TIME_OUT_PERIOD
		watcher.UpdateTimeout();
		CHECK_EQUAL(m_pServer->m_uidLastDisconnectedPlayer, uidPlayer1);
	}

	TEST_FIXTURE(Fixture, TestRemoveWatchPlayer)
	{
		MUID uidPlayer1 = AddNewPlayerObject();
		MUID uidPlayer2 = AddNewPlayerObject();
		
		watcher.AddPlayer(uidPlayer1, &timeoutHandler);
		watcher.AddPlayer(uidPlayer2, &timeoutHandler);

		watcher.RemovePlayer(uidPlayer1);
		gsys.pSystem->Sleep(60);

		watcher.UpdateTimeout();
		CHECK_EQUAL(m_pServer->m_uidLastDisconnectedPlayer, uidPlayer2);

		// 처리할 플레이어가 없음으로, LastDisconnectedPlayer는 변경되지 않는다.
		watcher.UpdateTimeout();
		CHECK_EQUAL(m_pServer->m_uidLastDisconnectedPlayer, uidPlayer2);	
	}

	TEST_FIXTURE(Fixture, TestRemoveCurrentIteratorPlayer)
	{
		MUID uidPlayer1 = AddNewPlayerObject();
		MUID uidPlayer2 = AddNewPlayerObject();
		MUID uidPlayer3 = AddNewPlayerObject();

		watcher.AddPlayer(uidPlayer1, &timeoutHandler);
		watcher.AddPlayer(uidPlayer2, &timeoutHandler);
		watcher.AddPlayer(uidPlayer3, &timeoutHandler);

		gsys.pSystem->Sleep(30);
		watcher.UpdateTimeout();	// CurrentIterator is Player2

		gsys.pSystem->Sleep(30);
		watcher.RemovePlayer(uidPlayer2);		
		watcher.UpdateTimeout();

		CHECK_EQUAL(m_pServer->m_uidLastDisconnectedPlayer, uidPlayer3);
	}

	TEST_FIXTURE(Fixture, TestSelMyCharTimeout)
	{
		LSelMyCharTimeoutHandler CommandHandler;
		MUID uidPlayer = AddNewPlayerObject();		
		MockLink* pLink = LTestHelper::NewLink(uidPlayer);
		
		watcher.AddPlayer(uidPlayer, &CommandHandler);
		
		gsys.pSystem->Sleep(60);
		watcher.UpdateTimeout();


		// 타임아웃 패킷이 보내졌여야한다.
		MCommand& SendCommand = pLink->GetCommand(0);

		if (SendCommand.GetID() != MC_CHAR_RESPONSE_SEL_MYCHAR)		{ CHECK(!"wrong command type"); return;}

		int nRet;
		if (SendCommand.GetParameter(&nRet,				0, MPT_INT)==false)		{ CHECK(!"wrong param type"); return; }		
				
		CHECK_EQUAL(nRet, CR_FAIL_CHAR_TIMEOUT);		
	}
}
