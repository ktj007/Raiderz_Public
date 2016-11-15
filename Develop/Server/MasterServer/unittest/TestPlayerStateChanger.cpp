#include "stdafx.h"
#include "ZFixtureHelper.h"
#include "ZPlayerStateChanger.h"
#include "ZPlayer.h"

#include "ZGameServerObject.h"
#include "ZPlayerLogic.h"

#include "ZTestHelper_CmdHandler.h"

#include "SCommandTable.h"


SUITE(PlayerStateChanger)
{
	struct FPlayerStateChanger : public FBasePlayer, FBaseMockLink2, FBaseLoginServer
	{
		FPlayerStateChanger()
		{
			InitLoginServer();
			InitDefaultGameServer(TEST_GAMESERVER_ID, TEST_FIELD_ID);
			InitGameServer();

			InitInstanceVariable();
		}
		~FPlayerStateChanger()
		{

		}

		void InitLoginServer()
		{
			m_uidLoginServer = Connected(TEST_LOGINSERVER_ID);
		}
		void InitGameServer()
		{
			m_uidGameServer = m_uidClient;

			m_nGameServerID2 = m_nServerID + 1;
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

		ZPlayer* InitFixture_PlayerInWorld()
		{
			ZPlayer* pPlayer = AddPlayer(m_uidPlayer, m_nAID);
			pPlayer->SetState(PS_MOVE_GAMESERVER);
			m_playerLogic.OnInWorld(m_uidGameServer, m_nAID, m_nCID, m_uidPlayer, m_strPlayerName.c_str(), m_nGID);

			return pPlayer;
		}
		ZPlayer* AddPlayer(MUID uidPlayer=ZTestHelper::NewUID(), AID nAID=ZTestHelper::NewAID(), wstring strUserID=ZTestHelper::NewName())
		{
			m_playerLogic.AddPlayer(uidPlayer, nAID, strUserID);
			return gmgr.pPlayerManager->Find(nAID);
		}

		static const int TEST_LOGINSERVER_ID = 10;
		static const int TEST_GAMESERVER_ID = 1;
		static const int TEST_FIELD_ID = 110;

		MUID m_uidLoginServer;
		MUID m_uidGameServer;
		int m_nGameServerID2;
		MUID m_uidGameServer2;

		///	테스트용 인스턴스 변수들
		int m_nAID;
		wstring m_strUserID;
		int m_nCID;
		MUID m_uidPlayer;
		wstring m_strPlayerName;
		int m_nGID;
		
		ZPlayerStateChanger m_stateChanger;
		ZPlayerLogic m_playerLogic;
	};

	void CheckPlayerManager_Has_CID_UID()
	{
		CHECK_EQUAL(1, gmgr.pPlayerManager->GetObjectCount());
		CHECK_EQUAL(1, gmgr.pPlayerManager->GetCIDCount());
		CHECK_EQUAL(1, gmgr.pPlayerManager->GetUIDCount());
	}
	void CheckPlayerManager_NoHas_CID()
	{
		CHECK_EQUAL(1, gmgr.pPlayerManager->GetObjectCount());
		CHECK_EQUAL(0, gmgr.pPlayerManager->GetCIDCount());
		CHECK_EQUAL(1, gmgr.pPlayerManager->GetUIDCount());
	}

	TEST_FIXTURE(FPlayerStateChanger, TestPlayerStateChanger_ChangeToREADY_LOGIN_GAMESERVER_Success)
	{
		ZPlayer* pPlayer = AddPlayer();

		CHECK_EQUAL(PS_SELECT_CHAR, pPlayer->GetState());

		m_stateChanger.ChangeToREADY_LOGIN_GAMESERVER(pPlayer->GetAID());

		CHECK_EQUAL(PS_READY_LOGIN_GAMESERVER, pPlayer->GetState());
	}

	TEST_FIXTURE(FPlayerStateChanger, TestPlayerStateChanger_ChangeToREADY_MOVE_SERVER)
	{
		ZPlayer* pPlayer = InitFixture_PlayerInWorld();
		
		CHECK_EQUAL(PS_IN_WORLD, pPlayer->GetState());

		m_stateChanger.ChangeToREADY_MOVE_GAMESERVER(pPlayer->GetUID());

		CHECK_EQUAL(PS_READY_MOVE_GAMESERVER, pPlayer->GetState());
	}

	namespace ChangeState_LoginGameServer
	{
		TEST_FIXTURE(FPlayerStateChanger, TestPlayerStateChanger_ChangeState_LoginGameServer_Success)
		{
			ZPlayer* pPlayer = AddPlayer();

			CHECK_EQUAL(PS_SELECT_CHAR, pPlayer->GetState());

			m_stateChanger.ChangeToREADY_LOGIN_GAMESERVER(pPlayer->GetAID());

			CHECK_EQUAL(PS_READY_LOGIN_GAMESERVER, pPlayer->GetState());
		}
	}

	namespace Cancel_LoginGameServer
	{
		TEST_FIXTURE(FPlayerStateChanger, TestPlayerStateChanger_Cancel_LoginGameServer_Success)
		{
			ZPlayer* pPlayer = AddPlayer();
			pPlayer->SetState(PS_READY_LOGIN_GAMESERVER);

			m_stateChanger.Cancel_READY_LOGIN_GAMESERVER(pPlayer->GetAID());

			CHECK_EQUAL(PS_SELECT_CHAR, pPlayer->GetState());
		}
	}

	namespace ChangeState_InWorld
	{
		TEST_FIXTURE(FPlayerStateChanger, TestStateChanger_ChangeTo_IN_WORLD_Success)
		{
			/// Fixture 설치
			ZPlayer* pPlayer = AddPlayer();
			CHECK_EQUAL(PS_SELECT_CHAR, pPlayer->GetState());

			pPlayer->SetState(PS_MOVE_GAMESERVER);
			pPlayer->SetWorldInfo(ZTestHelper::NewCID(), ZTestHelper::NewID(), ZTestHelper::NewUID(), ZTestHelper::NewName(), ZTestHelper::NewID());
			
			/// SUT 실행
			m_stateChanger.ChangeToIN_WORLD(pPlayer->GetAID());
			
			/// 검증
			CHECK_EQUAL(PS_IN_WORLD, pPlayer->GetState());
		}

		TEST_FIXTURE(FPlayerStateChanger, TestStateChanger_ChangeTo_IN_WORLD_Failure_State_SELECT_CHAR)
		{
			/// Fixture 설치
			ZPlayer* pPlayer = AddPlayer();
			pPlayer->SetWorldInfo(ZTestHelper::NewCID(), ZTestHelper::NewID(), ZTestHelper::NewUID(), ZTestHelper::NewName(), ZTestHelper::NewID());
			pPlayer->SetState(PS_SELECT_CHAR);

			/// SUT 실행
			m_stateChanger.ChangeToIN_WORLD(pPlayer->GetAID());

			/// 검증
			CHECK_EQUAL(PS_SELECT_CHAR, pPlayer->GetState());
		}
	} ///< ChangeState_InWorld

	namespace ChangeState_ReadyMoveGameServer
	{
		TEST_FIXTURE(FPlayerStateChanger, TestStateChanger_ChangeTo_ReadyMoveGameServer_Success)
		{
			ZPlayer* pTestPlayer = AddNewPlayerInWorld();
			CHECK_EQUAL(PS_IN_WORLD,	pTestPlayer->GetState());

			bool bRet = m_stateChanger.ChangeToREADY_MOVE_GAMESERVER(pTestPlayer->GetUID());

			CHECK_EQUAL(true, bRet);
			CHECK_EQUAL(PS_READY_MOVE_GAMESERVER,	pTestPlayer->GetState());
		}

		TEST_FIXTURE(FPlayerStateChanger, TestStateChanger_ChangeTo_ReadyMoveGameServer_Filure)
		{
			ZPlayer* pTestPlayer = AddNewPlayerInWorld();
			pTestPlayer->SetState(PS_SELECT_CHAR);

			CHECK(PS_IN_WORLD != pTestPlayer->GetState());

			bool bRet = m_stateChanger.ChangeToREADY_MOVE_GAMESERVER(pTestPlayer->GetUID());

			CHECK_EQUAL(false, bRet);
			CHECK_EQUAL(PS_SELECT_CHAR,	pTestPlayer->GetState());
		}
	}

	namespace ChangeState_MoveGameServer
	{
		TEST_FIXTURE(FPlayerStateChanger, TestStateChanger_ChangeTo_MoveGameServer_Failure)
		{
			ZPlayer* pPlayer = InitFixture_PlayerInWorld();
			CHECK_EQUAL(PS_IN_WORLD,	pPlayer->GetState());

			bool bRet = m_stateChanger.ChangeToMOVE_GAMESERVER(pPlayer->GetUID());

			CHECK_EQUAL(false, bRet);
			CHECK_EQUAL(PS_IN_WORLD,	pPlayer->GetState());
		}

		TEST_FIXTURE(FPlayerStateChanger, TestStateChanger_ChangeTo_MoveGameServer_Success)
		{
			ZPlayer* pPlayer = AddNewPlayerInWorld();
			pPlayer->SetState(PS_READY_MOVE_GAMESERVER);
			
			m_stateChanger.ChangeToMOVE_GAMESERVER(pPlayer->GetUID());

			CHECK_EQUAL(PS_MOVE_GAMESERVER,	pPlayer->GetState());

		}
	} ///< ChangeState_MoveServer

	namespace ChangeState_SelectChar
	{
		TEST_FIXTURE(FPlayerStateChanger, TestStateChanger_ChangeTo_SelectChar_Failure)
		{
			ZPlayer* pTestPlayer = InitFixture_PlayerInWorld();
			CHECK_EQUAL(PS_IN_WORLD,	pTestPlayer->GetState());

			bool bRet = m_stateChanger.ChangeToSELECT_CHAR(pTestPlayer->GetAID());

			CHECK_EQUAL(false, bRet);
			CHECK_EQUAL(PS_IN_WORLD,	pTestPlayer->GetState());
		}

		TEST_FIXTURE(FPlayerStateChanger, TestStateChanger_ChangeTo_SelectChar_Success)
		{
			ZPlayer* pTestPlayer = InitFixture_PlayerInWorld();
			pTestPlayer->SetState(PS_MOVE_LOGINSERVER);

			CHECK_EQUAL(PS_MOVE_LOGINSERVER,	pTestPlayer->GetState());

			m_stateChanger.ChangeToSELECT_CHAR(pTestPlayer->GetAID());

			CHECK_EQUAL(PS_SELECT_CHAR,	pTestPlayer->GetState());
		}
	}

	namespace ChangeState_ReadyMoveLoginServer
	{
		TEST_FIXTURE(FPlayerStateChanger, TestStateChanger_ChangeTo_ReadyMoveLoginServer_Failure)
		{
			ZPlayer* pTestPlayer = InitFixture_PlayerInWorld();
			pTestPlayer->SetState(PS_SELECT_CHAR);

			bool bRet = m_stateChanger.ChangeToREADY_MOVE_LOGINSERVER(pTestPlayer->GetUID());

			CHECK_EQUAL(false, bRet);
			CHECK_EQUAL(PS_SELECT_CHAR,	pTestPlayer->GetState());
		}

		TEST_FIXTURE(FPlayerStateChanger, TestStateChanger_ChangeTo_ReadyMoveLoginServer_Success)
		{
			ZPlayer* pTestPlayer = InitFixture_PlayerInWorld();
			CHECK_EQUAL(PS_IN_WORLD,	pTestPlayer->GetState());

			m_stateChanger.ChangeToREADY_MOVE_LOGINSERVER(pTestPlayer->GetUID());

			CHECK_EQUAL(PS_READY_MOVE_LOGINSERVER,	pTestPlayer->GetState());
		}
	}

	namespace ChangeState_MoveLoginServer
	{
		TEST_FIXTURE(FPlayerStateChanger, TestStateChanger_ChangeTo_MoveLoginServer_Failure)
		{
			ZPlayer* pTestPlayer = InitFixture_PlayerInWorld();
			CHECK_EQUAL(PS_IN_WORLD,	pTestPlayer->GetState());

			bool bRet = m_stateChanger.ChangeToMOVE_LOGINSERVER(pTestPlayer->GetUID());

			CHECK_EQUAL(false, bRet);
			CHECK_EQUAL(PS_IN_WORLD,	pTestPlayer->GetState());
		}

		TEST_FIXTURE(FPlayerStateChanger, TestStateChanger_ChangeTo_MoveLoginServer_Success)
		{
			ZPlayer* pTestPlayer = InitFixture_PlayerInWorld();
			pTestPlayer->SetState(PS_READY_MOVE_LOGINSERVER);

			m_stateChanger.ChangeToMOVE_LOGINSERVER(pTestPlayer->GetUID());

			CHECK_EQUAL(PS_MOVE_LOGINSERVER,	pTestPlayer->GetState());
		}
	}

	namespace StateCancel_MoveLoginServer
	{
		TEST_FIXTURE(FPlayerStateChanger, TestStateChanger_Cancel_MoveLoginServer_Success_PS_READY_MOVE_LOGINSERVER)
		{
			ZPlayer* pTestPlayer = InitFixture_PlayerInWorld();
			pTestPlayer->SetState(PS_READY_MOVE_LOGINSERVER);

			bool bRet = m_stateChanger.Cancel_READY_MOVE_LOGINSERVER(pTestPlayer->GetUID());

			CHECK_EQUAL(true, bRet);
			CHECK_EQUAL(PS_IN_WORLD,	pTestPlayer->GetState());
		}

		TEST_FIXTURE(FPlayerStateChanger, TestStateChanger_Cancel_MoveLoginServer_Success_PS_MOVE_LOGINSERVER)
		{
			ZPlayer* pTestPlayer = InitFixture_PlayerInWorld();
			pTestPlayer->SetState(PS_MOVE_LOGINSERVER);

			bool bRet = m_stateChanger.Cancel_READY_MOVE_LOGINSERVER(pTestPlayer->GetUID());

			CHECK_EQUAL(true, bRet);
			CHECK_EQUAL(PS_IN_WORLD,	pTestPlayer->GetState());
		}
	}

//////////////////////////////////////////////////////////////////////////
///	Test Command

	struct FPlayerStateChanger_OnCommand : public FPlayerStateChanger
	{
		FPlayerStateChanger_OnCommand()
		{
			m_pLinkLoginServer = NewLink(m_uidLoginServer);
			m_pLinkGameServer = NewLink(m_uidGameServer);
			m_pLinkGameServer2 = NewLink(m_uidGameServer2);
		}
		~FPlayerStateChanger_OnCommand()
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

	namespace PlayerState_InWorld
	{
		TEST_FIXTURE(FPlayerStateChanger_OnCommand, TestPlayer_OnCommand_MMC_PLAYER_STATE_IN_WORLD_REQ)
		{
			/// Fixture 설치
			ZTestHelper_CmdHandler::OnRecv_MLC_PLAYER_ADD_PLAYER_REQ(m_pLinkLoginServer, m_uidPlayer, m_nAID, m_strUserID);
			CHECK_EQUAL(1, gmgr.pPlayerManager->GetObjectCount());

			ZPlayer* pPlayer = gmgr.pPlayerManager->Find(m_nAID);
			ASSERT_CHECK(pPlayer != NULL);
			pPlayer->SetState(PS_MOVE_GAMESERVER);

			/// SUT 실행
			ZTestHelper_CmdHandler::OnRecv_PlayerStateInWorldReq(m_pLinkGameServer, m_nAID, m_nCID, m_uidPlayer, m_strPlayerName, m_nGID);

			// 검증
			CHECK_EQUAL(m_nCID, pPlayer->GetCID());
			CHECK_EQUAL(m_uidPlayer, pPlayer->GetUID());
			CHECK_EQUAL(m_nServerID, pPlayer->GetGameServerID());
			CHECK(m_strPlayerName == pPlayer->GetPlayerName());			
			CHECK_EQUAL(m_nGID, pPlayer->GetGuildID());

			CHECK_EQUAL(2, m_pLinkGameServer->GetCommandCount());
			{
				ASSERT_EQUAL(MMC_PLAYER_STATE_IN_WORLD_RES, m_pLinkGameServer->GetCommand(0).GetID());
				{
					CHECK_EQUAL(m_nCID,		m_pLinkGameServer->GetParam<int>(0, 0));
					CHECK_EQUAL(CR_SUCCESS,	m_pLinkGameServer->GetParam<int>(0, 1));
				}
				ASSERT_EQUAL(MSC_PROXY_PLAYER_ADD,			m_pLinkGameServer->GetCommand(1).GetID());
			}

			CHECK_EQUAL(1, m_pLinkGameServer2->GetCommandCount());
			ASSERT_EQUAL(MSC_PROXY_PLAYER_ADD, m_pLinkGameServer2->GetCommand(0).GetID());
		}

		TEST_FIXTURE(FPlayerStateChanger_OnCommand, TestPlayerLogic_OnCommand_MMC_PLAYER_STATE_IN_WORLD_REQ_Success_PlayerState_MOVE_GAMESERVER)
		{
			/// Fixture 설치
			ZTestHelper_CmdHandler::OnRecv_MLC_PLAYER_ADD_PLAYER_REQ(m_pLinkLoginServer, m_uidPlayer, m_nAID, m_strUserID);
			ZPlayer* pTestPlayer = gmgr.pPlayerManager->Find(m_nAID);
			pTestPlayer->SetState(PS_MOVE_GAMESERVER);

			/// SUT 실행
			ZTestHelper_CmdHandler::OnRecv_PlayerStateInWorldReq(m_pLinkGameServer, m_nAID, m_nCID, m_uidPlayer, m_strPlayerName, m_nGID);

			/// 검증
			CHECK_EQUAL(2, m_pLinkGameServer->GetCommandCount());	///< 응답 커맨드, 동기화 커맨드
			CHECK_EQUAL(1, m_pLinkGameServer2->GetCommandCount());	///< 동기화 커맨드
		}

		TEST_FIXTURE(FPlayerStateChanger_OnCommand, TestPlayer_OnCommand_MMC_PLAYER_STATE_IN_WORLD_REQ_NoPlayerObject)
		{
			/// SUT 실행 - 플레이어가 없는데 요청이 왔을 경우
			CHECK_EQUAL(0, gmgr.pPlayerManager->GetObjectCount());
			ZTestHelper_CmdHandler::OnRecv_PlayerStateInWorldReq(m_pLinkGameServer, m_nAID, m_nCID, m_uidPlayer, m_strPlayerName, m_nGID);

			/// 검증
			CHECK_EQUAL(0, gmgr.pPlayerManager->GetObjectCount());
			CHECK_EQUAL(0, m_pLinkGameServer->GetCommandCount());
			CHECK_EQUAL(0, m_pLinkGameServer2->GetCommandCount());
		}

		TEST_FIXTURE(FPlayerStateChanger_OnCommand, TestPlayer_OnCommand_MMC_PLAYER_STATE_IN_WORLD_REQ_InvalidAID)
		{
			/// Fixture 설치
			ZTestHelper_CmdHandler::OnRecv_MLC_PLAYER_ADD_PLAYER_REQ(m_pLinkLoginServer, m_uidPlayer, m_nAID, m_strUserID);
			ZPlayer* pTestPlayer = gmgr.pPlayerManager->Find(m_nAID);
			pTestPlayer->SetState(PS_MOVE_GAMESERVER);

			/// SUT 실행
			int nInvalidAID = 9999;
			ZTestHelper_CmdHandler::OnRecv_PlayerStateInWorldReq(m_pLinkGameServer, nInvalidAID, m_nCID, m_uidPlayer, m_strPlayerName, m_nGID);

			/// 검증
			CHECK_EQUAL(0, m_pLinkGameServer->GetCommandCount());
			CHECK_EQUAL(0, m_pLinkGameServer2->GetCommandCount());
		}

		TEST_FIXTURE(FPlayerStateChanger_OnCommand, TestPlayerLogic_OnCommand_MMC_PLAYER_STATE_IN_WORLD_REQ_Failure_Already_PlayerState_IN_WORLD)
		{
			/// Fixture 설치
			ZTestHelper_CmdHandler::OnRecv_MLC_PLAYER_ADD_PLAYER_REQ(m_pLinkLoginServer, m_uidPlayer, m_nAID, m_strUserID);

			ZPlayer* pTestPlayer = gmgr.pPlayerManager->Find(m_nAID);
			pTestPlayer->SetState(PS_IN_WORLD);

			/// SUT 실행
			ZTestHelper_CmdHandler::OnRecv_PlayerStateInWorldReq(m_pLinkGameServer, m_nAID, m_nCID, m_uidPlayer, m_strPlayerName, m_nGID);

			/// 검증
			CHECK_EQUAL(1, m_pLinkGameServer->GetCommandCount());
			CHECK_EQUAL(MMC_PLAYER_STATE_IN_WORLD_RES, m_pLinkGameServer->GetCommand(0).GetID());
			CHECK_EQUAL(m_nCID, m_pLinkGameServer->GetParam<int>(0, 0));
			CHECK_EQUAL(CR_FAIL_LOGIN_INVALID_PLAYER_WORLD_INFO, m_pLinkGameServer->GetParam<int>(0, 1));
		}
	}

}