#include "stdafx.h"
#include "ZPlayer.h"
#include "ZCommandTable.h"
#include "ZFixtureHelper.h"
#include "ZDuplicationLoginLogic.h"
#include "ZDuplicationLoginManager.h"
#include "ZPlayer.h"
#include "ZPlayerManager.h"
#include "ZPlayerLogic.h"
#include "ZGameServerObject.h"
#include "ZTestHelper_CmdHandler.h"
#include "ZPlayerStateChanger.h"


SUITE(DuplicationLoginLogic)
{
	struct Fixture : public FBasePlayer, FBaseMockLink2, FBaseLoginServer
	{
		Fixture()
		{
			const int TEST_GAMESERVER_ID = 1;
			const int TEST_FIELD_ID = 110;

			InitDefaultGameServer(TEST_GAMESERVER_ID, TEST_FIELD_ID);
			InitLoginServer();
			InitGameServer();
			InitLinks();

			InitInstanceVariable();
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
			m_nGameServerID = m_nServerID;
			m_uidGameServer = m_uidClient;

			m_nGameServerID2 = m_nServerID + 1;
			ZGameServerInfo* pGameServerInfo2 = AddGameServerInfo(m_nGameServerID2, m_nFieldID, 1, 2, 3);
			m_uidGameServer2 = AddGameServerObject(pGameServerInfo2)->GetUID();
		}
		void InitLinks()
		{
			m_pLinkLoginServer = NewLink(m_uidLoginServer);
			m_pLinkGameServer = NewLink(m_uidGameServer);
			m_pLinkGameServer2 = NewLink(m_uidGameServer2);
		}
		void InitInstanceVariable()
		{
			m_nAID = ZTestHelper::NewAID();
			m_strUserID = ZTestHelper::NewName();
			m_uidPlayer = ZTestHelper::NewUID();
			m_strPlayerName = ZTestHelper::NewName();
		}

		ZPlayer* AddPlayer(MUID uidPlayer=ZTestHelper::NewUID(), AID nAID=ZTestHelper::NewAID(), wstring strUserID=ZTestHelper::NewName())
		{
			m_playerLogic.AddPlayer(uidPlayer, nAID, strUserID);
			return gmgr.pPlayerManager->Find(nAID);
		}
		
		int m_nGameServerID;
		int m_nGameServerID2;

		MUID m_uidLoginServer;
		MUID m_uidGameServer;
		MUID m_uidGameServer2;

		MockLink* m_pLinkLoginServer;
		MockLink* m_pLinkGameServer;
		MockLink* m_pLinkGameServer2;

		int m_nAID;
		wstring m_strUserID;
		MUID m_uidPlayer;
		wstring m_strPlayerName;

		ZPlayerLogic			m_playerLogic;
		ZPlayerStateChanger		m_stateChanger;
		ZDuplicationLoginLogic	m_duplicationLoginLogic;
	};

	TEST_FIXTURE(Fixture, TestDuplicationLoginLogic_StartKickInWorldPlayer_Success)
	{
		/// Fixture 설치
		ZPlayer* pPlayer = AddPlayer();
		CHECK_EQUAL(false, gmgr.pDuplicationLoginManager->IsExist(pPlayer->GetAID()));

		/// SUT 실행
		MUID uidNewPlayer = ZTestHelper::NewUID();
		m_duplicationLoginLogic.StartKickInWorldPlayer(uidNewPlayer, pPlayer->GetAID(), pPlayer->GetUserID(), NULL);

		/// 검증
		CHECK_EQUAL(true, gmgr.pDuplicationLoginManager->IsExist(pPlayer->GetAID()));
	}

	TEST_FIXTURE(Fixture, TestDuplicationLoginLogic_StartKickInWorldPlayer_Success_State_LOGIN_GAMESERVER)
	{
		ZPlayer* pPlayer = AddNewPlayer();
		pPlayer->SetState(PS_READY_LOGIN_GAMESERVER);
		pPlayer->SetReservedGameServerID(m_nGameServerID);
		pPlayer->SetReservedConnectionKey(ZTestHelper::NewUID());

		MUID uidNewPlayer = ZTestHelper::NewUID();
		m_duplicationLoginLogic.StartKickInWorldPlayer(uidNewPlayer, pPlayer->GetAID(), pPlayer->GetUserID(), NULL);

		CHECK_EQUAL(true, gmgr.pDuplicationLoginManager->IsExist(pPlayer->GetAID()));
		CHECK_EQUAL(PS_KICK_BY_DUPLOGIN, pPlayer->GetState());
		ASSERT_EQUAL(1, m_pLinkLoginServer->GetCommandCount());
		CHECK_EQUAL(MLC_PLAYER_DUPLICATED_PLAYER_LOGIN,	m_pLinkLoginServer->GetCommandID(0));
		CHECK_EQUAL(pPlayer->GetUID(),					m_pLinkLoginServer->GetParam<MUID>(0, 0));
		CHECK_EQUAL(pPlayer->GetReservedConnectionKey(),m_pLinkLoginServer->GetParam<MUID>(0, 1));

		ASSERT_EQUAL(1, m_pLinkGameServer->GetCommandCount());
		CHECK_EQUAL(MMC_PLAYER_DUPLICATED_PLAYER_LOGIN,	m_pLinkGameServer->GetCommandID(0));
		CHECK_EQUAL(pPlayer->GetUID(),					m_pLinkGameServer->GetParam<MUID>(0, 0));
		CHECK_EQUAL(pPlayer->GetReservedConnectionKey(),m_pLinkGameServer->GetParam<MUID>(0, 1));
	}

	TEST_FIXTURE(Fixture, TestDuplicationLoginLogic_StartKickInWorldPlayer_Success_State_MOVE_GAMESERVER)
	{
		ZPlayer* pPlayer = AddNewPlayer();
		pPlayer->SetState(PS_MOVE_GAMESERVER);
		pPlayer->SetReservedGameServerID(m_nGameServerID);
		pPlayer->SetReservedConnectionKey(ZTestHelper::NewUID());

		MUID uidNewPlayer = ZTestHelper::NewUID();
		m_duplicationLoginLogic.StartKickInWorldPlayer(uidNewPlayer, pPlayer->GetAID(), pPlayer->GetUserID(), NULL);

		CHECK_EQUAL(true, gmgr.pDuplicationLoginManager->IsExist(pPlayer->GetAID()));
		CHECK_EQUAL(PS_KICK_BY_DUPLOGIN, pPlayer->GetState());

		ASSERT_EQUAL(0, m_pLinkLoginServer->GetCommandCount());

		ASSERT_EQUAL(1, m_pLinkGameServer->GetCommandCount());
		CHECK_EQUAL(MMC_PLAYER_DUPLICATED_PLAYER_LOGIN,	m_pLinkGameServer->GetCommandID(0));
		CHECK_EQUAL(pPlayer->GetUID(),					m_pLinkGameServer->GetParam<MUID>(0, 0));
		CHECK_EQUAL(pPlayer->GetReservedConnectionKey(),m_pLinkGameServer->GetParam<MUID>(0, 1));
	}

	TEST_FIXTURE(Fixture, TestDuplicationLoginLogic_StartKickInWorldPlayer_Success_State_READY_MOVE_LOGINSERVER)
	{
		ZPlayer* pPlayer = AddNewPlayerInWorld(m_nGameServerID);
		pPlayer->SetState(PS_READY_MOVE_LOGINSERVER);
		pPlayer->SetReservedConnectionKey(ZTestHelper::NewUID());

		MUID uidNewPlayer = ZTestHelper::NewUID();
		m_duplicationLoginLogic.StartKickInWorldPlayer(uidNewPlayer, pPlayer->GetAID(), pPlayer->GetUserID(), NULL);

		CHECK_EQUAL(true, gmgr.pDuplicationLoginManager->IsExist(pPlayer->GetAID()));
		CHECK_EQUAL(PS_KICK_BY_DUPLOGIN, pPlayer->GetState());
		ASSERT_EQUAL(1, m_pLinkLoginServer->GetCommandCount());
		CHECK_EQUAL(MLC_PLAYER_DUPLICATED_PLAYER_LOGIN,	m_pLinkLoginServer->GetCommandID(0));
		CHECK_EQUAL(pPlayer->GetUID(),					m_pLinkLoginServer->GetParam<MUID>(0, 0));
		CHECK_EQUAL(pPlayer->GetReservedConnectionKey(),m_pLinkLoginServer->GetParam<MUID>(0, 1));

		ASSERT_EQUAL(1, m_pLinkGameServer->GetCommandCount());
		CHECK_EQUAL(MMC_PLAYER_DUPLICATED_PLAYER_LOGIN,	m_pLinkGameServer->GetCommandID(0));
		CHECK_EQUAL(pPlayer->GetUID(),					m_pLinkGameServer->GetParam<MUID>(0, 0));
		CHECK_EQUAL(pPlayer->GetReservedConnectionKey(),m_pLinkGameServer->GetParam<MUID>(0, 1));
	}

	TEST_FIXTURE(Fixture, TestDuplicationLoginLogic_StartKickInWorldPlayer_Success_State_MOVE_LOGINSERVER)
	{
		ZPlayer* pPlayer = AddNewPlayerInWorld(m_nGameServerID);
		pPlayer->SetState(PS_MOVE_LOGINSERVER);
		pPlayer->SetReservedConnectionKey(ZTestHelper::NewUID());

		MUID uidNewPlayer = ZTestHelper::NewUID();
		m_duplicationLoginLogic.StartKickInWorldPlayer(uidNewPlayer, pPlayer->GetAID(), pPlayer->GetUserID(), NULL);

		CHECK_EQUAL(true, gmgr.pDuplicationLoginManager->IsExist(pPlayer->GetAID()));
		CHECK_EQUAL(PS_KICK_BY_DUPLOGIN, pPlayer->GetState());
		ASSERT_EQUAL(1, m_pLinkLoginServer->GetCommandCount());
		CHECK_EQUAL(MLC_PLAYER_DUPLICATED_PLAYER_LOGIN,	m_pLinkLoginServer->GetCommandID(0));
		CHECK_EQUAL(pPlayer->GetUID(),					m_pLinkLoginServer->GetParam<MUID>(0, 0));
		CHECK_EQUAL(pPlayer->GetReservedConnectionKey(),m_pLinkLoginServer->GetParam<MUID>(0, 1));

		ASSERT_EQUAL(1, m_pLinkGameServer->GetCommandCount());
		CHECK_EQUAL(MMC_PLAYER_DUPLICATED_PLAYER_LOGIN,	m_pLinkGameServer->GetCommandID(0));
		CHECK_EQUAL(pPlayer->GetUID(),					m_pLinkGameServer->GetParam<MUID>(0, 0));
		CHECK_EQUAL(pPlayer->GetReservedConnectionKey(),m_pLinkGameServer->GetParam<MUID>(0, 1));
	}

	TEST_FIXTURE(Fixture, TestDuplicationLoginLogic_StartKickInWorldPlayer_Failure_NoPlayer)
	{
		CHECK_EQUAL(0, gmgr.pPlayerManager->GetObjectCount());

		AID nAID = ZTestHelper::NewAID();
		wstring strUserID = ZTestHelper::NewName();

		/// SUT 실행
		MUID uidNewPlayer = ZTestHelper::NewUID();
		m_duplicationLoginLogic.StartKickInWorldPlayer(uidNewPlayer, nAID, strUserID, NULL);

		/// 검증
		CHECK_EQUAL(false, gmgr.pDuplicationLoginManager->IsExist(nAID));
	}

	TEST_FIXTURE(Fixture, TestDuplicationLoginLogic_FlushDuplicatedLogin_Success)
	{
		/// Fixture 설치
		ZPlayer* pPlayer = AddPlayer();
		MUID uidNewPlayer = ZTestHelper::NewUID();
		m_duplicationLoginLogic.StartKickInWorldPlayer(uidNewPlayer, pPlayer->GetAID(), pPlayer->GetUserID(), NULL);

		CHECK_EQUAL(1, gmgr.pPlayerManager->GetObjectCount());
		CHECK_EQUAL(true, gmgr.pDuplicationLoginManager->IsExist(pPlayer->GetAID()));

		/// SUT 실행
		m_duplicationLoginLogic.FlushDuplicatedLogin(pPlayer->GetAID(), pPlayer->GetUID());

		/// 검증
		CHECK_EQUAL(false, gmgr.pDuplicationLoginManager->IsExist(pPlayer->GetAID()));
		CHECK_EQUAL(1, gmgr.pPlayerManager->GetObjectCount());
	}

	TEST_FIXTURE(Fixture, TestDuplicationLoginLogic_FlushDuplicatedLogin_Failure_InvalidAID)
	{
		/// Fixture 설치
		ZPlayer* pPlayer = AddPlayer();
		MUID uidNewPlayer = ZTestHelper::NewUID();
		m_duplicationLoginLogic.StartKickInWorldPlayer(uidNewPlayer, pPlayer->GetAID(), pPlayer->GetUserID(), NULL);
		CHECK_EQUAL(true, gmgr.pDuplicationLoginManager->IsExist(pPlayer->GetAID()));

		AID nInvalidAID = ZTestHelper::NewAID();

		/// SUT 실행
		m_duplicationLoginLogic.FlushDuplicatedLogin(nInvalidAID, pPlayer->GetUID());

		/// 검증
		CHECK_EQUAL(true, gmgr.pDuplicationLoginManager->IsExist(pPlayer->GetAID()));
	}


	namespace Command
	{
		TEST_FIXTURE(Fixture, TestPlayer_OnCommand_MLC_PLAYER_ADD_PLAYER_REQ_Duplicated_CheckResponseCommand)
		{
			/// Fixture 설치
			ZTestHelper_CmdHandler::OnRecv_MLC_PLAYER_ADD_PLAYER_REQ(m_pLinkLoginServer, m_uidPlayer, m_nAID, m_strUserID);
			m_pLinkLoginServer->ResetCommands();

			/// SUT 중복 실행
			MUID uidPlayerNew = ZTestHelper::NewUID();
			ZTestHelper_CmdHandler::OnRecv_MLC_PLAYER_ADD_PLAYER_REQ(m_pLinkLoginServer, uidPlayerNew, m_nAID, m_strUserID);

			/// 검증
			ASSERT_EQUAL(2,									m_pLinkLoginServer->GetCommandCount());
			CHECK_EQUAL(MLC_PLAYER_DUPLICATED_PLAYER_LOGIN,	m_pLinkLoginServer->GetCommand(0).GetID());
			CHECK_EQUAL(m_uidPlayer,						m_pLinkLoginServer->GetParam<MUID>(0, 0));
			CHECK_EQUAL(MUID::ZERO,							m_pLinkLoginServer->GetParam<MUID>(0, 1));

			CHECK_EQUAL(MLC_PLAYER_ADD_PLAYER_RES,			m_pLinkLoginServer->GetCommand(1).GetID());
		}

		TEST_FIXTURE(Fixture, TestPlayer_OnCommand_MLC_PLAYER_ADD_PLAYER_REQ_Duplicated_OnMoveServerState)
		{
			/// Fixture 설치
			ZTestHelper_CmdHandler::OnRecv_MLC_PLAYER_ADD_PLAYER_REQ(m_pLinkLoginServer, m_uidPlayer, m_nAID, m_strUserID);
			m_pLinkLoginServer->ResetCommands();

			ZPlayer* pTestPlayer = gmgr.pPlayerManager->Find(m_nAID);
			pTestPlayer->SetReservedGameServerID(m_nServerID);
			pTestPlayer->SetState(PS_MOVE_GAMESERVER);

			/// SUT 중복 실행
			MUID uidPlayerNew = ZTestHelper::NewUID();
			ZTestHelper_CmdHandler::OnRecv_MLC_PLAYER_ADD_PLAYER_REQ(m_pLinkLoginServer, uidPlayerNew, m_nAID, m_strUserID);

			/// 검증
			ASSERT_EQUAL(1,											m_pLinkGameServer->GetCommandCount());
			CHECK_EQUAL(MMC_PLAYER_DUPLICATED_PLAYER_LOGIN,			m_pLinkGameServer->GetCommand(0).GetID());
			CHECK_EQUAL(m_uidPlayer,								m_pLinkGameServer->GetParam<MUID>(0, 0));

			CHECK_EQUAL(MLC_PLAYER_ADD_PLAYER_RES,					m_pLinkLoginServer->GetCommand(0).GetID());
			CHECK_EQUAL(CR_INFO_LOGIN_KICKING_DUPLICATED_PLAYER,	m_pLinkLoginServer->GetParam<int>(0, 1));
		}

		TEST_FIXTURE(Fixture, TestPlayer_OnCommand_MLC_PLAYER_ADD_PLAYER_REQ_Duplicated2_CheckResponseCommand)
		{
			/// Fixture 설치
			ZTestHelper_CmdHandler::OnRecv_MLC_PLAYER_ADD_PLAYER_REQ(m_pLinkLoginServer, m_uidPlayer, m_nAID, m_strUserID);

			MUID uidPlayerNew = ZTestHelper::NewUID();
			ZTestHelper_CmdHandler::OnRecv_MLC_PLAYER_ADD_PLAYER_REQ(m_pLinkLoginServer, uidPlayerNew, m_nAID, m_strUserID);
			m_pLinkLoginServer->ResetCommands();

			/// SUT 중복 실행
			MUID uidPlayerNew2 = ZTestHelper::NewUID();
			ZTestHelper_CmdHandler::OnRecv_MLC_PLAYER_ADD_PLAYER_REQ(m_pLinkLoginServer, uidPlayerNew2, m_nAID, m_strUserID);

			/// 검증
			ASSERT_EQUAL(2,									m_pLinkLoginServer->GetCommandCount());

			CHECK_EQUAL(MLC_PLAYER_DUPLICATED_PLAYER_LOGIN,	m_pLinkLoginServer->GetCommand(0).GetID());
			CHECK_EQUAL(uidPlayerNew,						m_pLinkLoginServer->GetParam<MUID>(0, 0));
			CHECK_EQUAL(MUID::ZERO,							m_pLinkLoginServer->GetParam<MUID>(0, 1));

			CHECK_EQUAL(MLC_PLAYER_ADD_PLAYER_RES,			m_pLinkLoginServer->GetCommand(1).GetID());
		}

		TEST_FIXTURE(Fixture, TestPlayer_OnCommand_MLC_PLAYER_ADD_PLAYER_REQ_Duplicated_WhenInWorld_CheckResponseCommand)
		{
			/// Fixture 설치
			ZTestHelper_CmdHandler::OnRecv_MLC_PLAYER_ADD_PLAYER_REQ(m_pLinkLoginServer, m_uidPlayer, m_nAID, m_strUserID);
			ZPlayer* pPlayer = gmgr.pPlayerManager->Find(m_nAID);
			pPlayer->SetState(PS_MOVE_GAMESERVER);

			CID nCID = ZTestHelper::NewCID();
			MUID uidPlayerInGame = ZTestHelper::NewUID();
			wstring strName = L"aibeast";
			m_playerLogic.OnInWorld(m_uidGameServer, m_nAID, nCID, uidPlayerInGame, strName.c_str(), 0);
			m_pLinkGameServer->ResetCommands();
			m_pLinkLoginServer->ResetCommands();

			/// SUT 중복 실행
			MUID uidPlayerNew = ZTestHelper::NewUID();
			ZTestHelper_CmdHandler::OnRecv_MLC_PLAYER_ADD_PLAYER_REQ(m_pLinkLoginServer, uidPlayerNew, m_nAID, m_strUserID);

			/// 검증
			ASSERT_EQUAL(1,									m_pLinkGameServer->GetCommandCount());
			CHECK_EQUAL(MMC_PLAYER_DUPLICATED_PLAYER_LOGIN,	m_pLinkGameServer->GetCommand(0).GetID());
			CHECK_EQUAL(uidPlayerInGame,					m_pLinkGameServer->GetParam<MUID>(0, 0));

			ASSERT_EQUAL(1,									m_pLinkLoginServer->GetCommandCount());
			CHECK_EQUAL(MLC_PLAYER_ADD_PLAYER_RES,			m_pLinkLoginServer->GetCommand(0).GetID());
		}

		TEST_FIXTURE(Fixture, TestPlayer_OnCommand_MLC_PLAYER_ADD_PLAYER_REQ_Duplicated_WhenReadyMoveServer_CheckResponseCommand)
		{
			/// Fixture 설치
			ZTestHelper_CmdHandler::OnRecv_MLC_PLAYER_ADD_PLAYER_REQ(m_pLinkLoginServer, m_uidPlayer, m_nAID, m_strUserID);

			ZPlayer* pPlayer = gmgr.pPlayerManager->FindByUID(m_uidPlayer);
			ZGameServerObject* pGameServer = gmgr.pGameServerObjectManager->FindGameServer(m_uidGameServer);
			MUID uidConnectionKey = ZTestHelper::NewUID();
			pPlayer->SetReservedConnectionKey(uidConnectionKey);

			m_stateChanger.ChangeToREADY_LOGIN_GAMESERVER(pPlayer->GetAID());
			m_pLinkGameServer->ResetCommands();
			m_pLinkLoginServer->ResetCommands();

			/// SUT 중복 실행
			MUID uidPlayerNew = ZTestHelper::NewUID();
			ZTestHelper_CmdHandler::OnRecv_MLC_PLAYER_ADD_PLAYER_REQ(m_pLinkLoginServer, uidPlayerNew, m_nAID, m_strUserID);

			/// 검증
			ASSERT_EQUAL(2,									m_pLinkLoginServer->GetCommandCount());
			CHECK_EQUAL(MLC_PLAYER_DUPLICATED_PLAYER_LOGIN,	m_pLinkLoginServer->GetCommand(0).GetID());
			CHECK_EQUAL(m_uidPlayer,						m_pLinkLoginServer->GetParam<MUID>(0, 0));
			CHECK_EQUAL(uidConnectionKey,					m_pLinkLoginServer->GetParam<MUID>(0, 1));

			CHECK_EQUAL(MLC_PLAYER_ADD_PLAYER_RES,			m_pLinkLoginServer->GetCommand(1).GetID());
		}

		TEST_FIXTURE(Fixture, TestPlayer_OnCommand_MLC_PLAYER_ADD_PLAYER_REQ_Duplicated_WhenLoginGameServer_CheckResponseCommand)
		{
			/// Fixture 설치
			ZTestHelper_CmdHandler::OnRecv_MLC_PLAYER_ADD_PLAYER_REQ(m_pLinkLoginServer, m_uidPlayer, m_nAID, m_strUserID);

			ZPlayer* pPlayer = gmgr.pPlayerManager->FindByUID(m_uidPlayer);
			ZGameServerObject* pGameServer = gmgr.pGameServerObjectManager->FindGameServer(m_uidGameServer);
			MUID uidConnectionKey = ZTestHelper::NewUID();

			pPlayer->SetState(PS_MOVE_GAMESERVER);
			pPlayer->SetReservedGameServerID(pGameServer->GetID());
			pPlayer->SetReservedConnectionKey(uidConnectionKey);

			m_pLinkGameServer->ResetCommands();
			m_pLinkLoginServer->ResetCommands();

			/// SUT 중복 실행
			MUID uidPlayerNew = ZTestHelper::NewUID();
			ZTestHelper_CmdHandler::OnRecv_MLC_PLAYER_ADD_PLAYER_REQ(m_pLinkLoginServer, uidPlayerNew, m_nAID, m_strUserID);

			/// 검증
			CHECK_EQUAL(1,									m_pLinkGameServer->GetCommandCount());
			CHECK_EQUAL(MMC_PLAYER_DUPLICATED_PLAYER_LOGIN,	m_pLinkGameServer->GetCommand(0).GetID());
			CHECK_EQUAL(m_uidPlayer,						m_pLinkGameServer->GetParam<MUID>(0, 0));
			CHECK_EQUAL(uidConnectionKey,					m_pLinkGameServer->GetParam<MUID>(0, 1));

			CHECK_EQUAL(1,											m_pLinkLoginServer->GetCommandCount());
			CHECK_EQUAL(uidPlayerNew,								m_pLinkLoginServer->GetParam<MUID>(0, 0));
			CHECK_EQUAL(CR_INFO_LOGIN_KICKING_DUPLICATED_PLAYER,	m_pLinkLoginServer->GetParam<int>(0, 1));
		}

	}

}
