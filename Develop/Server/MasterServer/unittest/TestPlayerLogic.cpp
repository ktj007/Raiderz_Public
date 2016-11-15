#include "stdafx.h"
#include "ZPlayer.h"
#include "ZCommandTable.h"
#include "ZFixtureHelper.h"
#include "ZPlayerManager.h"
#include "ZPlayerLogic.h"
#include "ZGameServerObject.h"
#include "ZSharedField.h"
#include "SCommandTable.h"
#include "ZTestHelper_CmdHandler.h"
#include "ZDuplicationLoginManager.h"
#include "ZPlayerStateChanger.h"


SUITE(PlayerLogic)
{
	struct FPlayerLogic : public FBasePlayer, FBaseMockLink2, FBaseLoginServer
	{
		FPlayerLogic()
		{
			InitDefaultGameServer(TEST_GAMESERVER_ID, TEST_FIELD_ID);
			InitLoginServer();
			InitGameServer();

			InitInstanceVariable();
		}
		~FPlayerLogic()
		{

		}

		void InitLoginServer()
		{
			m_uidLoginServer = Connected(TEST_LOGINSERVER_ID);
		}
		void InitGameServer()
		{
			m_uidGameServer = m_uidClient;
			m_nGameServerID = m_nServerID;

			m_nGameServerID2 = m_nGameServerID + 1;
			ZGameServerInfo* pGameServerInfo = AddGameServerInfo(m_nGameServerID2, m_nFieldID, 2, 2, 3);
			ZGameServerObject* pGameServerObject2 = AddGameServerObject(pGameServerInfo);
			m_uidGameServer2 = pGameServerObject2->GetUID();
		}
		void InitInstanceVariable()
		{
			m_nAID = ZTestHelper::NewAID();
			m_strUserID = ZTestHelper::NewName();
			m_nCID = ZTestHelper::NewCID();
			m_uidPlayer = ZTestHelper::NewUID();
			m_strPlayerName = L"birdkr";
			m_nGID = ZTestHelper::NewID();
		}

		void AddPlayer(MUID uidPlayer, AID nAID)
		{
			m_playerLogic.AddPlayer(uidPlayer, nAID, m_strUserID);
		}

		static const int TEST_LOGINSERVER_ID = 10;
		static const int TEST_GAMESERVER_ID = 1;
		static const int TEST_FIELD_ID = 110;
		static const int TEST_CHANNEL_ID = 1;

		ZPlayerLogic m_playerLogic;
		ZPlayerStateChanger m_stateChanger;
		
		MUID m_uidLoginServer;
		MUID m_uidGameServer;
		int m_nGameServerID;
		int m_nGameServerID2;
		MUID m_uidGameServer2;

		///	테스트용 인스턴스 변수들
		int m_nAID;
		wstring m_strUserID;
		int m_nCID;
		MUID m_uidPlayer;
		wstring m_strPlayerName;
		int m_nGID;

		ZTestMgrWrapper<ZDuplicationLoginManager > m_testDuplecationLoginManager;
	};

	TEST_FIXTURE(FPlayerLogic, TestPlayerLogic_AddPlayer)
	{
		CHECK_EQUAL(0, gmgr.pPlayerManager->GetObjectCount());

		m_playerLogic.AddPlayer(ZTestHelper::NewUID(), ZTestHelper::NewAID(), ZTestHelper::NewName());

		CHECK_EQUAL(1, gmgr.pPlayerManager->GetObjectCount());
	}

	TEST_FIXTURE(FPlayerLogic, TestPlayerLogic_DeletePlayer)
	{
		ZPlayer* pPlayer = AddNewPlayerInWorld();
		CHECK_EQUAL(1, gmgr.pPlayerManager->GetObjectCount());

		m_playerLogic.DeletePlayer(pPlayer->GetAID());

		CHECK_EQUAL(0, gmgr.pPlayerManager->GetObjectCount());
	}

	TEST_FIXTURE(FPlayerLogic, TestPlayerLogic_OnSelectChar_Success_From_State_MOVE_LOGINSERVER)
	{
		ZPlayer* pPlayer = AddNewPlayerInWorld(TEST_GAMESERVER_ID);
		ZGameServerObject* pGameServer = gmgr.pGameServerObjectManager->FindGameServer(TEST_GAMESERVER_ID);

		CHECK_EQUAL(TEST_GAMESERVER_ID, pPlayer->GetGameServerID());
		CHECK_EQUAL(1, pGameServer->GetPlayerMap().GetObjectCount());
		CHECK_EQUAL(1, gmgr.pPlayerManager->GetCIDCount());

		pPlayer->SetState(PS_MOVE_LOGINSERVER);
		MUID newPlayerUID = ZTestHelper::NewUID();

		/// Act
		m_playerLogic.OnSelectChar(pPlayer->GetAID(), newPlayerUID);

		/// Assert
		CHECK_EQUAL(PS_SELECT_CHAR, pPlayer->GetState());
		CHECK_EQUAL(newPlayerUID, pPlayer->GetUID());

		CHECK_EQUAL(INVALID_ID, pPlayer->GetGameServerID());
		CHECK_EQUAL(0, pGameServer->GetPlayerMap().GetObjectCount());
		CHECK_EQUAL(0, gmgr.pPlayerManager->GetCIDCount());
	}

	TEST_FIXTURE(FPlayerLogic, TestPlayerLogic_OnSelectChar_Failure_State_IN_WORLD)
	{
		ZPlayer* pPlayer = AddNewPlayer();
		pPlayer->SetState(PS_IN_WORLD);

		m_playerLogic.OnSelectChar(pPlayer->GetAID(), pPlayer->GetUID());

		CHECK_EQUAL(PS_IN_WORLD, pPlayer->GetState());
	}

	TEST_FIXTURE(FPlayerLogic, TestPlayerLogic_PrepareGateField)
	{
		/// Fixture 설치
		ZPlayer* pPlayer = AddNewPlayerInWorld(TEST_GAMESERVER_ID);
		ZField* pField = gmgr.pFieldManager->FindSharedField(TEST_FIELD_ID, TEST_CHANNEL_ID);

		CHECK_EQUAL(MUID::ZERO, pPlayer->GetReservedFieldUID());

		/// SUT 실행
		m_playerLogic.PrepareGateField(pPlayer, pField);

		/// 검증
		CHECK_EQUAL(pField->GetUID(), pPlayer->GetReservedFieldUID());
	}

	TEST_FIXTURE(FPlayerLogic, TestPlayerLogic_InField)
	{
		/// Fixture 설치
		ZPlayer* pPlayer = AddNewPlayerInWorld(TEST_GAMESERVER_ID);
		ZField* pField = gmgr.pFieldManager->FindSharedField(TEST_FIELD_ID, TEST_CHANNEL_ID);

		m_playerLogic.PrepareGateField(pPlayer, pField);

		CHECK_EQUAL(INVALID_ID, pPlayer->GetFieldID());
		CHECK_EQUAL(MUID::ZERO, pPlayer->GetFieldUID());
		
		/// SUT 실행
		m_playerLogic.InField(m_uidGameServer, pPlayer->GetCID(), pField->GetUID(), pField->GetID());

		/// 검증
		CHECK_EQUAL(MUID::ZERO,			pPlayer->GetReservedFieldUID());
		CHECK_EQUAL(pField->GetUID(),	pPlayer->GetFieldUID());
		CHECK_EQUAL(pField->GetID(),	pPlayer->GetFieldID());
	}

	TEST_FIXTURE(FPlayerLogic, TestPlayerLogic_InField_Failure_InvalidID)
	{
		/// Fixture 설치
		ZPlayer* pPlayer = AddNewPlayerInWorld(TEST_GAMESERVER_ID);
		ZField* pField = gmgr.pFieldManager->FindSharedField(TEST_FIELD_ID, TEST_CHANNEL_ID);

		m_playerLogic.PrepareGateField(pPlayer, pField);

		CHECK_EQUAL(INVALID_ID, pPlayer->GetFieldID());
		CHECK_EQUAL(MUID::ZERO, pPlayer->GetFieldUID());

		/// SUT 실행
		int nInvalidFieldID = 999;
		m_playerLogic.InField(m_uidGameServer, pPlayer->GetCID(), pField->GetUID(), nInvalidFieldID);

		/// 검증
		CHECK_EQUAL(pField->GetUID(),	pPlayer->GetReservedFieldUID());
		CHECK_EQUAL(MUID::ZERO,			pPlayer->GetFieldUID());
		CHECK_EQUAL(INVALID_ID,			pPlayer->GetFieldID());
	}

	TEST_FIXTURE(FPlayerLogic, TestPlayerLogic_AllMemberDeleteInGameServer)
	{
		/// Fixture 설치
		for (int i = 0; i < 50; ++i)
		{
			AddNewPlayerInWorld(m_nGameServerID);
		}

		for (int i = 0; i < 50; ++i)
		{
			AddNewPlayerInWorld(m_nGameServerID2);
		}

		ZGameServerObject* pGameServerObject = gmgr.pGameServerObjectManager->FindGameServer(m_nGameServerID);
		ZGameServerObject* pGameServerObject2 = gmgr.pGameServerObjectManager->FindGameServer(m_nGameServerID2);

		CHECK_EQUAL(100, gmgr.pPlayerManager->GetObjectCount());
		CHECK_EQUAL(50, pGameServerObject->GetPlayerMap().GetObjectCount());
		CHECK_EQUAL(50, pGameServerObject2->GetPlayerMap().GetObjectCount());

		/// SUT 실행
		m_playerLogic.AllMemberDeleteInGameServer(m_nGameServerID2);

		/// 검증
		CHECK_EQUAL(50, gmgr.pPlayerManager->GetObjectCount());
		CHECK_EQUAL(50, pGameServerObject->GetPlayerMap().GetObjectCount());
		CHECK_EQUAL(0, pGameServerObject2->GetPlayerMap().GetObjectCount());

		/// SUT 실행
		m_playerLogic.AllMemberDeleteInGameServer(m_nGameServerID);

		/// 검증
		CHECK_EQUAL(0, gmgr.pPlayerManager->GetObjectCount());
		CHECK_EQUAL(0, pGameServerObject->GetPlayerMap().GetObjectCount());
		CHECK_EQUAL(0, pGameServerObject2->GetPlayerMap().GetObjectCount());
	}

	TEST_FIXTURE(FPlayerLogic, TestPlayerLogic_AllMemberDeleteInGameServer_State_MOVE_GAMESERVER)
	{
		/// Fixture 설치
		ZGameServerObject* pGameServerObject = gmgr.pGameServerObjectManager->FindGameServer(m_nGameServerID);
		ZGameServerObject* pGameServerObject2 = gmgr.pGameServerObjectManager->FindGameServer(m_nGameServerID2);

		for (int i = 0; i < 50; ++i)
		{
			ZPlayer* pTestPlayer = AddNewPlayerInWorld(m_nGameServerID);
			pGameServerObject->GetPlayerMap().Delete(pTestPlayer->GetAID());
			pTestPlayer->SetState(PS_MOVE_GAMESERVER);
			pTestPlayer->SetGameServerID(INVALID_ID);
			pTestPlayer->SetReservedGameServerID(m_nGameServerID2);
		}

		CHECK_EQUAL(50, gmgr.pPlayerManager->GetObjectCount());
		CHECK_EQUAL(0, pGameServerObject->GetPlayerMap().GetObjectCount());
		CHECK_EQUAL(0, pGameServerObject2->GetPlayerMap().GetObjectCount());

		/// SUT 실행
		m_playerLogic.AllMemberDeleteInGameServer(m_nServerID);

		/// 검증
		CHECK_EQUAL(50, gmgr.pPlayerManager->GetObjectCount());
		CHECK_EQUAL(0, pGameServerObject->GetPlayerMap().GetObjectCount());
		CHECK_EQUAL(0, pGameServerObject2->GetPlayerMap().GetObjectCount());

		/// SUT 실행
		m_playerLogic.AllMemberDeleteInGameServer(m_nGameServerID2);

		/// 검증
		CHECK_EQUAL(0, gmgr.pPlayerManager->GetObjectCount());
		CHECK_EQUAL(0, pGameServerObject->GetPlayerMap().GetObjectCount());
		CHECK_EQUAL(0, pGameServerObject2->GetPlayerMap().GetObjectCount());
	}

	TEST_FIXTURE(FPlayerLogic, TestPlayerLogic_AllMemberDeleteInLoginServer_Success_State_SELECT_CHAR)
	{
		/// Fixture 설치
		for (int i = 0; i < 50; ++i)
		{
			MUID uidNew = ZTestHelper::NewUID();
			int nAIDNew = ZTestHelper::NewID();

			AddPlayer(uidNew, nAIDNew);
		}
		CHECK_EQUAL(50, gmgr.pPlayerManager->GetObjectCount());

		/// SUT 실행
		m_playerLogic.AllMemberDeleteInLoginServer();

		/// 검증
		CHECK_EQUAL(0, gmgr.pPlayerManager->GetObjectCount());
	}

	TEST_FIXTURE(FPlayerLogic, TestPlayerLogic_AllMemberDeleteInLoginServer_Success_State_READY_LOGIN_GAMESERVER)
	{
		/// Fixture 설치
		for (int i = 0; i < 50; ++i)
		{
			MUID uidNew = ZTestHelper::NewUID();
			int nAIDNew = ZTestHelper::NewID();

			AddPlayer(uidNew, nAIDNew);
			ZPlayer* pPlayer = gmgr.pPlayerManager->Find(nAIDNew);
			pPlayer->SetState(PS_READY_LOGIN_GAMESERVER);
		}
		CHECK_EQUAL(50, gmgr.pPlayerManager->GetObjectCount());

		/// SUT 실행
		m_playerLogic.AllMemberDeleteInLoginServer();

		/// 검증
		CHECK_EQUAL(0, gmgr.pPlayerManager->GetObjectCount());
	}

	TEST_FIXTURE(FPlayerLogic, TestPlayerLogic_AllMemberDeleteInLoginServer_Success_State_MOVE_LOGINSERVER)
	{
		/// Fixture 설치
		for (int i = 0; i < 50; ++i)
		{
			MUID uidNew = ZTestHelper::NewUID();
			int nAIDNew = ZTestHelper::NewID();

			AddPlayer(uidNew, nAIDNew);
			ZPlayer* pPlayer = gmgr.pPlayerManager->Find(nAIDNew);
			pPlayer->SetState(PS_MOVE_LOGINSERVER);
		}
		CHECK_EQUAL(50, gmgr.pPlayerManager->GetObjectCount());

		/// SUT 실행
		m_playerLogic.AllMemberDeleteInLoginServer();

		/// 검증
		CHECK_EQUAL(0, gmgr.pPlayerManager->GetObjectCount());
	}

	TEST_FIXTURE(FPlayerLogic, TestPlayerLogic_AllMemberDeleteInLoginServer_Failure_PlayerStateIs_IN_WORLD)
	{
		/// Fixture 설치
		int nFakeLoginServerID = 999;
		MUID uidFakeLoginServer = Connected(nFakeLoginServerID);

		for (int i = 0; i < 50; ++i)
		{
			MUID uidNew = ZTestHelper::NewUID();
			int nAIDNew = ZTestHelper::NewID();

			AddPlayer(uidNew, nAIDNew);

			ZPlayer* pPlayer = gmgr.pPlayerManager->Find(nAIDNew);
			pPlayer->SetState(PS_IN_WORLD);
		}
		CHECK_EQUAL(50, gmgr.pPlayerManager->GetObjectCount());

		/// SUT 실행
		m_playerLogic.AllMemberDeleteInLoginServer();

		/// 검증
		CHECK_EQUAL(50, gmgr.pPlayerManager->GetObjectCount());
	}

	//////////////////////////////////////////////////////////////////////////
	struct FPlayerLogic_Command : public FPlayerLogic
	{
		FPlayerLogic_Command()
		{
			m_pLinkLoginServer = NewLink(m_uidLoginServer);
			m_pLinkGameServer = NewLink(m_uidGameServer);
			m_pLinkGameServer2 = NewLink(m_uidGameServer2);
		}
		~FPlayerLogic_Command()
		{

		}

		ZPlayer* InitFixture_Recv_PlayerStateInWorldReq(MockLink* pGameServerLink)
		{
			ZTestHelper_CmdHandler::OnRecv_MLC_PLAYER_ADD_PLAYER_REQ(m_pLinkLoginServer, m_uidPlayer, m_nAID, m_strUserID);
			ZPlayer* pPlayer = gmgr.pPlayerManager->Find(m_nAID);
			pPlayer->SetState(PS_MOVE_GAMESERVER);

			ZTestHelper_CmdHandler::OnRecv_PlayerStateInWorldReq(pGameServerLink, m_nAID, m_nCID, m_uidPlayer, m_strPlayerName, m_nGID);

			m_pLinkGameServer->ResetCommands();
			m_pLinkGameServer2->ResetCommands();

			return pPlayer;
		}

		MockLink* m_pLinkLoginServer;

		MockLink* m_pLinkGameServer;
		MockLink* m_pLinkGameServer2;

	};

	namespace Add_Player
	{
		TEST_FIXTURE(FPlayerLogic_Command, TestPlayer_OnCommand_MLC_PLAYER_ADD_PLAYER_REQ_CheckInstanceCount)
		{
			CHECK_EQUAL(0, gmgr.pPlayerManager->GetObjectCount());

			/// SUT 실행
			ZTestHelper_CmdHandler::OnRecv_MLC_PLAYER_ADD_PLAYER_REQ(m_pLinkLoginServer, m_uidPlayer, m_nAID, m_strUserID);

			/// 검증
			CHECK_EQUAL(1, gmgr.pPlayerManager->GetObjectCount());
		}

		TEST_FIXTURE(FPlayerLogic_Command, TestPlayer_OnCommand_MLC_PLAYER_ADD_PLAYER_REQ_CheckResponseCommand)
		{
			/// SUT 실행
			ZTestHelper_CmdHandler::OnRecv_MLC_PLAYER_ADD_PLAYER_REQ(m_pLinkLoginServer, m_uidPlayer, m_nAID, m_strUserID);

			/// 검증
			CHECK_EQUAL(1,							m_pLinkLoginServer->GetCommandCount());
			CHECK_EQUAL(MLC_PLAYER_ADD_PLAYER_RES,	m_pLinkLoginServer->GetCommand(0).GetID());
			CHECK_EQUAL(m_uidPlayer,				m_pLinkLoginServer->GetParam<MUID>(0, 0));
			CHECK_EQUAL(CR_SUCCESS,					m_pLinkLoginServer->GetParam<int>(0, 1));
		}

		TEST_FIXTURE(FPlayerLogic_Command, TestPlayer_OnCommand_MLC_PLAYER_ADD_PLAYER_REQ_OverWorldPlayerLimit)
		{
			/// Fixture 설치
			for (int i = 0; i < WORLD_PLAYER_LIMIT; ++i)
			{
				ZPlayer* pPlayer = gmgr.pPlayerManager->NewPlayer(ZTestHelper::NewUID(), ZTestHelper::NewAID(), ZTestHelper::NewName());
				gmgr.pPlayerManager->Add(pPlayer);
			}
			CHECK_EQUAL(WORLD_PLAYER_LIMIT, gmgr.pPlayerManager->GetObjectCount());

			/// SUT 실행
			ZTestHelper_CmdHandler::OnRecv_MLC_PLAYER_ADD_PLAYER_REQ(m_pLinkLoginServer, m_uidPlayer, m_nAID, m_strUserID);

			/// 검증
			CHECK_EQUAL(1,							m_pLinkLoginServer->GetCommandCount());
			CHECK_EQUAL(MLC_PLAYER_ADD_PLAYER_RES,	m_pLinkLoginServer->GetCommandID(0));
			CHECK_EQUAL(m_uidPlayer,				m_pLinkLoginServer->GetParam<MUID>(0, 0));
			CHECK_EQUAL(CR_FAIL_LOGIN_WORLD_IS_FULL,m_pLinkLoginServer->GetParam<int>(0, 1));
		}
	}

	namespace Delete_Player
	{
		TEST_FIXTURE(FPlayerLogic_Command, TestPlayer_OnCommand_MLC_PLAYER_DELETE_PLAYER)
		{
			/// Fixture 설치
			ZTestHelper_CmdHandler::OnRecv_MLC_PLAYER_ADD_PLAYER_REQ(m_pLinkLoginServer, m_uidPlayer, m_nAID, m_strUserID);
			m_pLinkLoginServer->ResetCommands();
			CHECK_EQUAL(1, gmgr.pPlayerManager->GetObjectCount());

			/// SUT 중복 실행
			ZTestHelper_CmdHandler::OnRecv_MLC_PLAYER_DELETE_PLAYER(m_pLinkLoginServer, m_nAID, m_uidPlayer);

			/// 검증
			CHECK_EQUAL(0, gmgr.pPlayerManager->GetObjectCount());
			CHECK_EQUAL(0, m_pLinkLoginServer->GetCommandCount());
		}

		TEST_FIXTURE(FPlayerLogic_Command, TestPlayer_OnCommand_MLC_PLAYER_DELETE_PLAYER_InvalidAID)
		{
			/// Fixture 설치
			ZTestHelper_CmdHandler::OnRecv_MLC_PLAYER_ADD_PLAYER_REQ(m_pLinkLoginServer, m_uidPlayer, m_nAID, m_strUserID);
			m_pLinkLoginServer->ResetCommands();

			CHECK_EQUAL(1, gmgr.pPlayerManager->GetObjectCount());

			/// SUT 중복 실행
			AID nInvalidAID = 9999;
			ZTestHelper_CmdHandler::OnRecv_MLC_PLAYER_DELETE_PLAYER(m_pLinkLoginServer, nInvalidAID, m_uidPlayer);

			/// 검증
			CHECK_EQUAL(1, gmgr.pPlayerManager->GetObjectCount());
		}
	}


	TEST_FIXTURE(FPlayerLogic_Command, TestPlayer_OnCommand_MMC_PLAYER_IN_FIELD_NOTIFY)
	{
		/// Fixture 설치
		ZPlayer* pPlayer = InitFixture_Recv_PlayerStateInWorldReq(m_pLinkGameServer);
		m_pLinkGameServer->AddIgnoreID(MMC_FIELD_UPDATE_PROXY_FIELD_INFO);
		m_pLinkGameServer2->AddIgnoreID(MMC_FIELD_UPDATE_PROXY_FIELD_INFO);

		ZGameServerObject* pServerObject = gmgr.pGameServerObjectManager->FindGameServer(m_uidGameServer);
		int nChannelID1 = 1;
		ZSharedField* pField = pServerObject->GetFieldMap().FindSharedField(m_nFieldID, nChannelID1);
		m_playerLogic.PrepareGateField(pPlayer, pField);

		/// SUT 실행
		ZTestHelper_CmdHandler::OnRecv_MMC_PLAYER_IN_FIELD_NOTIFY(m_pLinkGameServer, m_nCID, pField->GetUID(), pField->GetID());

		/// 검증
		CHECK_EQUAL(pField->GetUID(),	pPlayer->GetFieldUID());

		CHECK_EQUAL(1, m_pLinkGameServer->GetCommandCount());
		ASSERT_EQUAL(MSC_PROXY_PLAYER_UPDATE, m_pLinkGameServer->GetCommand(0).GetID());
	}

	TEST_FIXTURE(FPlayerLogic_Command, TestPlayer_OnCommand_MMC_PLAYER_IN_FIELD_NOTIFY_ChageField)
	{
		/// Fixture 설치
		ZPlayer* pPlayer = InitFixture_Recv_PlayerStateInWorldReq(m_pLinkGameServer2);
		m_pLinkGameServer->AddIgnoreID(MMC_FIELD_UPDATE_PROXY_FIELD_INFO);
		m_pLinkGameServer2->AddIgnoreID(MMC_FIELD_UPDATE_PROXY_FIELD_INFO);

		ZGameServerObject* pServerObject2 = gmgr.pGameServerObjectManager->FindGameServer(m_uidGameServer2);
		int nChannelID2 = 2;
		int nChannelID3 = 3;
		ZSharedField* pField = pServerObject2->GetFieldMap().FindSharedField(m_nFieldID, nChannelID2);
		ZSharedField* pField2 = pServerObject2->GetFieldMap().FindSharedField(m_nFieldID, nChannelID3);

		m_playerLogic.PrepareGateField(pPlayer, pField);
		ZTestHelper_CmdHandler::OnRecv_MMC_PLAYER_IN_FIELD_NOTIFY(m_pLinkGameServer2, m_nCID, pField->GetUID(), pField->GetID());
		m_playerLogic.PrepareGateField(pPlayer, pField2);
		m_pLinkGameServer->ResetCommands();

		/// SUT 실행
		ZTestHelper_CmdHandler::OnRecv_MMC_PLAYER_IN_FIELD_NOTIFY(m_pLinkGameServer2, m_nCID, pField2->GetUID(), pField2->GetID());

		/// 검증
		CHECK_EQUAL(m_nGameServerID2, pPlayer->GetGameServerID());
		CHECK_EQUAL(pField2->GetUID(), pPlayer->GetFieldUID());

		CHECK_EQUAL(1, m_pLinkGameServer->GetCommandCount());
		CHECK_EQUAL(MSC_PROXY_PLAYER_UPDATE, m_pLinkGameServer->GetCommand(0).GetID());
	}

	TEST_FIXTURE(FPlayerLogic_Command, TestPlayer_OnCommand_MMC_PLAYER_IN_FIELD_NOTIFY_Post_MMC_FIELD_UPDATE_PROXY_FIELD_INFO_ChangeField)
	{
		/// Fixture 설치
		InitFixture_Recv_PlayerStateInWorldReq(m_pLinkGameServer2);
		m_pLinkGameServer->AddIgnoreID(MSC_PROXY_PLAYER_UPDATE);
		m_pLinkGameServer2->AddIgnoreID(MSC_PROXY_PLAYER_UPDATE);

		ZGameServerObject* pServerObject2 = gmgr.pGameServerObjectManager->FindGameServer(m_uidGameServer2);
		ZSharedField* pField = pServerObject2->GetFieldMap().FindSharedField(m_nFieldID, 2);
		ZSharedField* pField2 = pServerObject2->GetFieldMap().FindSharedField(m_nFieldID, 3);

		ZPlayer* pTeestPlayer = gmgr.pPlayerManager->Find(m_nAID);
		m_playerLogic.PrepareGateField(pTeestPlayer, pField);
		ZTestHelper_CmdHandler::OnRecv_MMC_PLAYER_IN_FIELD_NOTIFY(m_pLinkGameServer2, m_nCID, pField->GetUID(), pField->GetID());

		CHECK_EQUAL(1, pField->GetPlayerCount());
		CHECK_EQUAL(0, pField2->GetPlayerCount());

		m_playerLogic.PrepareGateField(pTeestPlayer, pField2);
		CHECK_EQUAL(1, pField->GetPlayerCount());
		CHECK_EQUAL(1, pField2->GetPlayerCount());

		m_pLinkGameServer->ResetCommands();
		m_pLinkGameServer2->ResetCommands();

		/// SUT 실행
		ZTestHelper_CmdHandler::OnRecv_MMC_PLAYER_IN_FIELD_NOTIFY(m_pLinkGameServer2, m_nCID, pField2->GetUID(), pField2->GetID());

		/// 검증
		CHECK_EQUAL(0, pField->GetPlayerCount());
		CHECK_EQUAL(1, pField2->GetPlayerCount());

		ASSERT_EQUAL(1, m_pLinkGameServer->GetCommandCount());
		ASSERT_EQUAL(MMC_FIELD_UPDATE_PROXY_FIELD_INFO, m_pLinkGameServer->GetCommand(0).GetID());	///< 1번 필드 플레이어 수 갱신 커맨드 (M -> G1)
		CHECK_EQUAL(pField->GetUID(),	m_pLinkGameServer->GetParam<MUID>(0, 0));

		ASSERT_EQUAL(1, m_pLinkGameServer2->GetCommandCount());
		ASSERT_EQUAL(MMC_FIELD_UPDATE_PROXY_FIELD_INFO, m_pLinkGameServer2->GetCommand(0).GetID()); ///< 1번 필드 플레이어 수 갱신 커맨드 (M -> G2)
		CHECK_EQUAL(pField->GetUID(),	m_pLinkGameServer2->GetParam<MUID>(0, 0));

	}
}