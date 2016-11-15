#include "stdafx.h"
#include "ZFixtureHelper.h"
#include "CCommandTable_Master.h"
#include "SCommandTable.h"
#include "ZFixtureHelper.h"
#include "ZGameServerObject.h"
#include "ZGameServerObjectManager.h"


SUITE(CmdHandler_MoveServer)
{
	struct FCmdHandler_MoveServer : public FBaseMockLink2, FBaseLoginServer, FBasePlayer
	{
		FCmdHandler_MoveServer()
		{
			InitLoginServer();
			m_pLSLink = NewLink(m_uidLoginServer);

			InitDefaultGameServer(TEST_GAMESERVER_ID, TEST_FIELD_ID);
			m_uidGameServer = m_uidClient;
			m_pGSLink = NewLink(m_uidGameServer);

			m_pPlayer = AddNewPlayerInWorld(TEST_GAMESERVER_ID);
		}
		~FCmdHandler_MoveServer()
		{

		}

		void InitLoginServer()
		{
			m_uidLoginServer = Connected(TEST_LOGINSERVER_ID);
		}

		static const int TEST_LOGINSERVER_ID = 10;
		static const int TEST_GAMESERVER_ID = 1;
		static const int TEST_FIELD_ID = 110;

		MUID		m_uidLoginServer;
		MUID		m_uidGameServer;

		MockLink*	m_pLSLink;
		MockLink*	m_pGSLink;

		ZPlayer*	m_pPlayer;
	};
	
	TEST_FIXTURE(FCmdHandler_MoveServer, TestCmdHandler_MoveServer_OnCommReserveMoveLoginServerReq_Success)
	{
		MCommandResult result = m_pGSLink->OnRecv(MMC_COMM_RESERVE_MOVE_LOGINSERVER_REQ, 1, NEW_UID(m_pPlayer->GetUID()));
		CHECK_EQUAL(result, CR_TRUE);

		CHECK_EQUAL(PS_READY_MOVE_LOGINSERVER, m_pPlayer->GetState());

		CHECK_EQUAL(1, m_pLSLink->GetCommandCount());
		CHECK_EQUAL(MLC_COMM_READY_ENTER_LOGINSERVER_REQ, m_pLSLink->GetCommandID(0));
		CHECK_EQUAL(m_uidGameServer, m_pLSLink->GetParam<MUID>(0, 0));
		CHECK_EQUAL(m_pPlayer->GetUID(), m_pLSLink->GetParam<MUID>(0, 1));
		CHECK_EQUAL(m_pPlayer->GetAID(), m_pLSLink->GetParam<int>(0, 3));
		CHECK(m_pPlayer->GetUserID() == wstring(m_pLSLink->GetParam<const wchar_t*>(0, 4)));
	}

	TEST_FIXTURE(FCmdHandler_MoveServer, TestCmdHandler_MoveServer_OnCommCancelMoveLoginServer_Success_PS_READY_MOVE_LOGINSERVER)
	{
		MUID uidConnectionKey = ZTestHelper::NewUID();
		m_pPlayer->SetReservedConnectionKey(uidConnectionKey);
		m_pPlayer->SetState(PS_READY_MOVE_LOGINSERVER);
		
		/// Act
		MCommandResult result = m_pGSLink->OnRecv(MMC_COMM_CANCEL_MOVE_LOGINSERVER, 1, NEW_UID(m_pPlayer->GetUID()));
		CHECK_EQUAL(result, CR_TRUE);

		/// Assert
		CHECK_EQUAL(PS_IN_WORLD, m_pPlayer->GetState());

		CHECK_EQUAL(0, m_pGSLink->GetCommandCount());

		CHECK_EQUAL(1, m_pLSLink->GetCommandCount());
		CHECK_EQUAL(MLC_COMM_CANCEL_MOVE_LOGINSERVER, m_pLSLink->GetCommandID(0));
		CHECK_EQUAL(uidConnectionKey, m_pLSLink->GetParam<MUID>(0, 0));
	}

	TEST_FIXTURE(FCmdHandler_MoveServer, TestCmdHandler_MoveServer_OnCommCancelMoveLoginServer_Success_PS_MOVE_LOGINSERVER)
	{
		/// Arrange
		MUID uidConnectionKey = ZTestHelper::NewUID();

		m_pPlayer->SetGameServerID(INVALID_ID);
		m_pPlayer->SetReservedConnectionKey(uidConnectionKey);

		ZGameServerObject* pGameServer = gmgr.pGameServerObjectManager->FindGameServer(m_uidGameServer);
		pGameServer->GetPlayerMap().Delete(m_pPlayer->GetAID());

		CHECK_EQUAL(0, pGameServer->GetPlayerMap().GetCIDCount());
		CHECK_EQUAL(0, pGameServer->GetPlayerMap().GetUIDCount());

		m_pPlayer->SetState(PS_MOVE_LOGINSERVER);

		/// Act
		MCommandResult result = m_pGSLink->OnRecv(MMC_COMM_CANCEL_MOVE_LOGINSERVER, 1, NEW_UID(m_pPlayer->GetUID()));
		CHECK_EQUAL(result, CR_TRUE);

		/// Assert
		CHECK_EQUAL(PS_IN_WORLD, m_pPlayer->GetState());

		CHECK_EQUAL(1, m_pGSLink->GetCommandCount());
		CHECK_EQUAL(MSC_PROXY_PLAYER_UPDATE, m_pGSLink->GetCommandID(0));

		CHECK_EQUAL(1, m_pLSLink->GetCommandCount());
		CHECK_EQUAL(MLC_COMM_CANCEL_MOVE_LOGINSERVER, m_pLSLink->GetCommandID(0));
		CHECK_EQUAL(uidConnectionKey, m_pLSLink->GetParam<MUID>(0, 0));

		CHECK_EQUAL(1, pGameServer->GetPlayerMap().GetCIDCount());
		CHECK_EQUAL(1, pGameServer->GetPlayerMap().GetUIDCount());
	}

	TEST_FIXTURE(FCmdHandler_MoveServer, TestCmdHandler_MoveServer_OnCommReadyEnterLoginServerRes_Success)
	{
		ZGameServerObject* pGameServer = gmgr.pGameServerObjectManager->FindGameServer(m_uidGameServer);
		CHECK_EQUAL(1, pGameServer->GetPlayerMap().GetCIDCount());
		CHECK_EQUAL(1, pGameServer->GetPlayerMap().GetUIDCount());

		m_pPlayer->SetState(PS_READY_MOVE_LOGINSERVER);

		int nResult = CR_SUCCESS;
		MUID uidConnectionKey = ZTestHelper::NewUID();

		MCommandResult result = m_pLSLink->OnRecv(MLC_COMM_READY_ENTER_LOGINSERVER_RES
									, 4
									, NEW_UID(m_uidGameServer)
									, NEW_UID(m_pPlayer->GetUID())
									, NEW_INT(nResult)
									, NEW_UID(uidConnectionKey)
									);
		CHECK_EQUAL(result, CR_TRUE);

		/// Assert
		CHECK_EQUAL(PS_MOVE_LOGINSERVER, m_pPlayer->GetState());

		CHECK_EQUAL(INVALID_ID, m_pPlayer->GetGameServerID());
		CHECK_EQUAL(uidConnectionKey, m_pPlayer->GetReservedConnectionKey());

		CHECK_EQUAL(0, pGameServer->GetPlayerMap().GetCIDCount());
		CHECK_EQUAL(0, pGameServer->GetPlayerMap().GetUIDCount());

		CHECK_EQUAL(2, m_pGSLink->GetCommandCount());
		CHECK_EQUAL(MSC_PROXY_PLAYER_UPDATE, m_pGSLink->GetCommandID(0));

		CHECK_EQUAL(MMC_COMM_RESERVE_MOVE_LOGINSERVER_RES, m_pGSLink->GetCommandID(1));
		CHECK_EQUAL(m_pPlayer->GetUID(), m_pGSLink->GetParam<MUID>(1, 0));
		CHECK_EQUAL(nResult, m_pGSLink->GetParam<int>(1, 1));
		CHECK_EQUAL(uidConnectionKey, m_pGSLink->GetParam<MUID>(1, 2));
	}

}