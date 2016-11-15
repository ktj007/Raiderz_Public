#include "stdafx.h"
#include "ZFixtureHelper.h"
#include "ZPlayerLogic.h"
#include "ZGameServerObject.h"
#include "ZGameServerPlayerMap.h"


SUITE(PlayerLogic_OnInWorld)
{
	struct Fixture : public FBasePlayer, FBaseServerField, FBaseLoginServer
	{
		Fixture()
		{
			InitLoginServer();
			InitGameServer();
		}
		~Fixture()
		{
			
		}

		void InitLoginServer()
		{
			int nLoginServerID = 10;
			m_uidLoginServer = Connected(nLoginServerID);
		}
		void InitGameServer()
		{
			const int TEST_GAMESERVER_ID = 1;
			const int TEST_FIELD_ID = 110;

			InitDefaultGameServer(TEST_GAMESERVER_ID, TEST_FIELD_ID);
			m_uidGameServer = gmgr.pGameServerObjectManager->IDToUID(TEST_GAMESERVER_ID);
		}

		ZPlayer* AddPlayer(MUID uidPlayer=ZTestHelper::NewUID(), AID nAID=ZTestHelper::NewAID(), wstring strUserID=ZTestHelper::NewName())
		{
			m_playerLogic.AddPlayer(uidPlayer, nAID, strUserID);
			return gmgr.pPlayerManager->Find(nAID);
		}

		MUID			m_uidLoginServer;
		MUID			m_uidGameServer;

		ZPlayerLogic	m_playerLogic;
	};

	TEST_FIXTURE(Fixture, TestPlayerLogic_OnInWorld_Success)
	{
		ZPlayer* pPlayer = AddPlayer();
		pPlayer->SetState(PS_MOVE_GAMESERVER);

		// Act
		m_playerLogic.OnInWorld(m_uidGameServer, pPlayer->GetAID(), ZTestHelper::NewCID(), ZTestHelper::NewUID(), ZTestHelper::NewName(), 0);

		// Assert
		ZGameServerObject* pGameServer = gmgr.pGameServerObjectManager->FindGameServer(m_uidGameServer);
		CHECK_EQUAL(1, pGameServer->GetPlayerMap().GetObjectCount());
	}

	TEST_FIXTURE(Fixture, TestPlayerLogic_OnInWorld_Failure_Duplicated)
	{
		ZPlayer* pPlayer = AddPlayer();
		pPlayer->SetState(PS_MOVE_GAMESERVER);

		m_playerLogic.OnInWorld(m_uidGameServer, pPlayer->GetAID(), ZTestHelper::NewCID(), ZTestHelper::NewUID(), ZTestHelper::NewName(), 0);

		// Act
		bool bRet = m_playerLogic.OnInWorld(m_uidGameServer, pPlayer->GetAID(), ZTestHelper::NewCID(), ZTestHelper::NewUID(), ZTestHelper::NewName(), 0);

		// Assert
		CHECK_EQUAL(false, bRet);
	}

	TEST_FIXTURE(Fixture, TestPlayerLogic_OnInWorld_Failure_StateIsNot_MOVE_GAMESERVER)
	{
		ZPlayer* pPlayer = AddPlayer();
		pPlayer->SetState(PS_SELECT_CHAR);

		MUID uidPlayerInWorld = ZTestHelper::NewUID();

		// Act
		m_playerLogic.OnInWorld(m_uidGameServer, pPlayer->GetAID(), ZTestHelper::NewCID(), uidPlayerInWorld, ZTestHelper::NewName(), 0);

		// Assert
		ZGameServerObject* pGameServer = gmgr.pGameServerObjectManager->FindGameServer(m_uidGameServer);
		CHECK_EQUAL(0, pGameServer->GetPlayerMap().GetObjectCount());

		CHECK_EQUAL(PS_SELECT_CHAR, pPlayer->GetState());

		CHECK_EQUAL(INVALID_ID, pPlayer->GetCID());
		CHECK(uidPlayerInWorld != pPlayer->GetUID());
		CHECK_EQUAL(0, pPlayer->GetPlayerName().size());
	}

	TEST_FIXTURE(Fixture, TestPlayerLogic_OnInWorld_Failure_Already_PlayerState_IN_WORLD)
	{
		ZPlayer* pPlayer = AddPlayer();
		pPlayer->SetState(PS_IN_WORLD);

		/// Act
		m_playerLogic.OnInWorld(m_uidGameServer, pPlayer->GetAID(), ZTestHelper::NewCID(), ZTestHelper::NewUID(), ZTestHelper::NewName(), 0);

		/// Assert
		ZGameServerObject* pGameServer = gmgr.pGameServerObjectManager->FindGameServer(m_uidGameServer);
		CHECK_EQUAL(0, pGameServer->GetPlayerMap().GetObjectCount());
	}

}
