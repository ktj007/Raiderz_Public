#include "StdAfx.h"
#include "SUnitTestUtil.h"
#include "FBasePlayer.h"
#include "FBaseMasterClient.h"
#include "LGlobal.h"
#include "LServer.h"
#include "LPlayerLogic.h"
#include "LPlayerLoginGameServerProcManager.h"
#include "LPlayerObject.h"
#include "CCommandTable_Master.h"


SUITE(PlayerLoginGameServerProcManager)
{
	struct Fixture : public FBasePlayer, FBaseMasterClient
	{
		Fixture()
		{
			m_pPlayerLoginGameServerProcManager = gsys.pServer->GetPlayerLoginGameServerProcManager();
			m_pPlayerLoginGameServerProcManager->Clear();
			
			m_pNetAgent->ResetSendCommands();
			m_pNetAgent->ResetIgnoreCmd();
			m_pNetAgent->IgnoreCmd(MLC_PLAYER_STATE_SELECT_CHAR);
		}
		virtual ~Fixture()
		{
			m_pPlayerLoginGameServerProcManager->Clear();
		}

		LPlayerLogic m_PlayerLogic;
		LPlayerLoginGameServerProcManager* m_pPlayerLoginGameServerProcManager;
	};
	
	TEST_FIXTURE(Fixture, TestPlayerLoginGameServerProcManager_Should_AddQueue)
	{
		/// Fixture 설치
		m_pPlayerLoginGameServerProcManager->Clear();
		
		LPlayerObject* pPlayerObject = NewLoginedTestPlayerObject();
		int nCharIndex = 0;

		CHECK_EQUAL(0, m_pPlayerLoginGameServerProcManager->GetQueueSize());

		/// SUT 실행
		m_PlayerLogic.SelMyChar(pPlayerObject->GetUID(), nCharIndex);

		/// 검증
		CHECK_EQUAL(1, m_pPlayerLoginGameServerProcManager->GetQueueSize());
	}

	TEST_FIXTURE(Fixture, TestPlayerLoginGameServerProcManager_Update_Should_QueueSizeZero)
	{
		/// Fixture 설치
		LPlayerObject* pPlayerObject = NewLoginedTestPlayerObject();
		m_PlayerLogic.SelMyChar(pPlayerObject->GetUID(), 0);

		/// SUT 실행
		m_pPlayerLoginGameServerProcManager->Update(LPlayerLoginGameServerProcManager::TIME_UPDATE_SEC);

		/// 검증
		CHECK_EQUAL(0, m_pPlayerLoginGameServerProcManager->GetQueueSize());
	}

	TEST_FIXTURE(Fixture, TestPlayerLoginGameServerProcManager_Update_Should_SendCommand_To_MasterServer)
	{
		/// Fixture 설치
		LPlayerObject* pPlayerObject = NewLoginedTestPlayerObject();
		m_PlayerLogic.SelMyChar(pPlayerObject->GetUID(), 0);

		/// SUT 실행
		m_pPlayerLoginGameServerProcManager->Update(LPlayerLoginGameServerProcManager::TIME_UPDATE_SEC);

		/// 검증
		CHECK_EQUAL(1, m_pNetAgent->GetSendCommandCount());
		CHECK_EQUAL(MLC_COMM_REQUEST_LOGIN_GAME_SERVER, m_pNetAgent->GetSendCommandID(0));
	}

	TEST_FIXTURE(Fixture, TestPlayerLoginGameServerProcManager_Update_PlayerLimitCount)
	{
		/// Fixture 설치
		int nLimitCount = LPlayerLoginGameServerProcManager::LIMIT_COUNT_PLAYER_LOGIN_GAMESERVER_A_UPDATE;

		LPlayerObject* pPlayerObject = NULL;
		for (int i = 0; i < nLimitCount; ++i)
		{
			pPlayerObject = NewLoginedTestPlayerObject();
			m_PlayerLogic.SelMyChar(pPlayerObject->GetUID(), 0);
		}

		CHECK_EQUAL(nLimitCount, m_pPlayerLoginGameServerProcManager->GetQueueSize());

		/// SUT 실행
		m_pPlayerLoginGameServerProcManager->Update(LPlayerLoginGameServerProcManager::TIME_UPDATE_SEC);

		/// 검증
		CHECK_EQUAL(0, m_pPlayerLoginGameServerProcManager->GetQueueSize());
	}

	TEST_FIXTURE(Fixture, TestPlayerLoginGameServerProcManager_Update_PlayerLimitCount_Over)
	{
		/// Fixture 설치
		int nOverCount = 10;
		int nLimitCount = LPlayerLoginGameServerProcManager::LIMIT_COUNT_PLAYER_LOGIN_GAMESERVER_A_UPDATE + nOverCount;

		LPlayerObject* pPlayerObject = NULL;
		for (int i = 0; i < nLimitCount; ++i)
		{
			pPlayerObject = NewLoginedTestPlayerObject();
			m_PlayerLogic.SelMyChar(pPlayerObject->GetUID(), 0);
		}

		CHECK_EQUAL(nLimitCount, m_pPlayerLoginGameServerProcManager->GetQueueSize());

		/// SUT 실행
		m_pPlayerLoginGameServerProcManager->Update(LPlayerLoginGameServerProcManager::TIME_UPDATE_SEC);

		/// 검증
		CHECK_EQUAL(nOverCount, m_pPlayerLoginGameServerProcManager->GetQueueSize());
		CHECK(0 != m_pPlayerLoginGameServerProcManager->GetQueueSize());
	}

}