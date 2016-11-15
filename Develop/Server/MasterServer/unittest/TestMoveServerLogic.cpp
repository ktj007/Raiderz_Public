#include "stdafx.h"
#include "ZMoveServerLogic.h"
#include "ZFixtureHelper.h"
#include "ZGameServerObject.h"
#include "CTransData.h"
#include "CCommandTable_Master.h"
#include "ZCommandCenter.h"


SUITE(MoveServerLogic)
{
	struct Fixture : FBasePlayer, FBaseMockLink, FBaseGameServer, FBaseField, FBaseFieldInfo, FBaseLoginServer
	{
		Fixture()
		{
			// Init LoginServer
			m_uidLoginServer = FBaseLoginServer::Connected();
			m_pLinkLoginServer = NewLink(m_uidLoginServer);
			m_pLinkLoginServer->ResetCommands();

			// Init GameServer1
			m_nTestFIeldID = ZTestHelper::NewID();
			ZFieldInfo* pFieldInfo = AddFieldInfo(m_nTestFIeldID);

			ZGameServerInfo* pGameServerInfo = AddGameServerInfo(ZTestHelper::NewID(), m_nTestFIeldID, 1, 1);
			m_pGameServer = AddGameServerObject(pGameServerInfo);
			m_pLinkGameServer = NewLink(m_pGameServer->GetUID());
			m_pLinkGameServer->ResetCommands();

			m_pLinkGameServer->AddIgnoreID(MMC_FIELD_UPDATE_PROXY_FIELD_INFO);

			// Init GameServer2
			m_pGameServer2 = AddGameServerObject();
			m_pLinkGameServer2 = NewLink(m_pGameServer2->GetUID());
			m_pLinkGameServer2->ResetCommands();
		}
		~Fixture()
		{

		}

		MUID				m_uidLoginServer;
		MockLink*			m_pLinkLoginServer;

		ZGameServerObject*	m_pGameServer;
		int					m_nTestFIeldID;
		MockLink*			m_pLinkGameServer;

		ZGameServerObject*	m_pGameServer2;
		MockLink*			m_pLinkGameServer2;

		ZMoveServerLogic	m_logic;
	};

	namespace _RequestLoginGameServer
	{
		TEST_FIXTURE(Fixture, TestMoveServerLogic_RequestLoginGameServer_Success)
		{
			ZPlayer* pPlayer = AddNewPlayer();
			CHECK_EQUAL(PS_SELECT_CHAR, pPlayer->GetState());

			TD_LOGIN_ACCOUNT_INFO tdLoginAccountInfo;
			tdLoginAccountInfo.nAID = pPlayer->GetAID();
			tdLoginAccountInfo.nCID = pPlayer->GetCID();
			wcscpy_s(tdLoginAccountInfo.szUserID, PMANG_USER_ID_LEN, pPlayer->GetUserID().c_str());

			TD_LOGIN_FIELD_INFO tdLoginFieldInfo;
			tdLoginFieldInfo.nSharedFieldID = m_nTestFIeldID;

			m_logic.RequestLoginGameServer(pPlayer->GetUID(), &tdLoginAccountInfo, &tdLoginFieldInfo);

			CHECK_EQUAL(PS_READY_LOGIN_GAMESERVER, pPlayer->GetState());
			CHECK_EQUAL(m_pGameServer->GetID(), pPlayer->GetReservedGameServerID());
			CHECK(MUID::ZERO != pPlayer->GetReservedConnectionKey());

			ASSERT_EQUAL(1, m_pLinkGameServer->GetCommandCount());
			CHECK_EQUAL(MMC_COMM_REQUEST_LOGIN_GAME_SERVER,	m_pLinkGameServer->GetCommandID(0));
			CHECK_EQUAL(m_uidLoginServer,					m_pLinkGameServer->GetParam<MUID>(0, 0));
			CHECK_EQUAL(pPlayer->GetUID(),					m_pLinkGameServer->GetParam<MUID>(0, 1));

			MCommandParameterSingleBlob* pParamBlob = (MCommandParameterSingleBlob*)m_pLinkGameServer->GetCommand(0).GetParameter(3);
			TD_LOGIN_ACCOUNT_INFO* pLoginAccountInfo = (TD_LOGIN_ACCOUNT_INFO*)pParamBlob->GetPointer();
			CHECK_EQUAL(pLoginAccountInfo->nAID,	tdLoginAccountInfo.nAID);
			CHECK_EQUAL(pLoginAccountInfo->nCID,	tdLoginAccountInfo.nCID);
			CHECK(wstring(pLoginAccountInfo->szUserID) == wstring(tdLoginAccountInfo.szUserID));
		}
	}
	
	namespace _ResponseLoginGameServer
	{
		TEST_FIXTURE(Fixture, TestMoveServerLogic_ResponseLoginGameServer_Success)
		{
			ZPlayer* pPlayer = AddNewPlayer();
			pPlayer->SetState(PS_READY_LOGIN_GAMESERVER);

			TD_LOGIN_GAME_SERVER_INFO tdToGameServerInfo;
			tdToGameServerInfo.uidConnectionKey = ZTestHelper::NewUID();
			m_logic.ResponseLoginGameServer(pPlayer->GetUID(), CR_SUCCESS, m_pGameServer->GetUID(), &tdToGameServerInfo);

			CHECK_EQUAL(PS_MOVE_GAMESERVER, pPlayer->GetState());

			ASSERT_EQUAL(1, m_pLinkLoginServer->GetCommandCount());
			CHECK_EQUAL(MLC_COMM_RESPONSE_LOGIN_GAME_SERVER,	m_pLinkLoginServer->GetCommandID(0));
			CHECK_EQUAL(pPlayer->GetUID(),						m_pLinkLoginServer->GetParam<MUID>(0, 0));
			CHECK_EQUAL(CR_SUCCESS,								m_pLinkLoginServer->GetParam<int>(0, 1));
		}

		TEST_FIXTURE(Fixture, TestMoveServerLogic_ResponseLoginGameServer_NotSuccess)
		{
			ZPlayer* pPlayer = AddNewPlayer();
			pPlayer->SetState(PS_READY_LOGIN_GAMESERVER);

			TD_LOGIN_GAME_SERVER_INFO tdToGameServerInfo;
			tdToGameServerInfo.uidConnectionKey = ZTestHelper::NewUID();
			m_logic.ResponseLoginGameServer(pPlayer->GetUID(), CR_FAIL, m_pGameServer->GetUID(), &tdToGameServerInfo);

			CHECK_EQUAL(PS_SELECT_CHAR, pPlayer->GetState());

			ASSERT_EQUAL(1, m_pLinkLoginServer->GetCommandCount());
			CHECK_EQUAL(MLC_COMM_RESPONSE_LOGIN_GAME_SERVER,	m_pLinkLoginServer->GetCommandID(0));
			CHECK_EQUAL(pPlayer->GetUID(),						m_pLinkLoginServer->GetParam<MUID>(0, 0));
			CHECK_EQUAL(CR_FAIL,								m_pLinkLoginServer->GetParam<int>(0, 1));
		}

		TEST_FIXTURE(Fixture, TestMoveServerLogic_ResponseLoginGameServer_Filure_State_Not_PS_READY_LOGIN_GAMESERVER)
		{
			ZPlayer* pPlayer = AddNewPlayer();
			pPlayer->SetState(PS_SELECT_CHAR);

			TD_LOGIN_GAME_SERVER_INFO tdToGameServerInfo;
			tdToGameServerInfo.uidConnectionKey = ZTestHelper::NewUID();
			bool bRet = m_logic.ResponseLoginGameServer(pPlayer->GetUID(), CR_SUCCESS, m_pGameServer->GetUID(), &tdToGameServerInfo);

			CHECK_EQUAL(false, bRet);
			CHECK_EQUAL(PS_SELECT_CHAR, pPlayer->GetState());
			CHECK_EQUAL(0, m_pLinkLoginServer->GetCommandCount());
		}

		TEST_FIXTURE(Fixture, TestMoveServerLogic_ResponseLoginGameServer_Filure_InvalidPlayerUID)
		{
			MUID uidInvalidPlayer = ZTestHelper::NewUID();

			TD_LOGIN_GAME_SERVER_INFO tdToGameServerInfo;
			tdToGameServerInfo.uidConnectionKey = ZTestHelper::NewUID();
			bool bRet = m_logic.ResponseLoginGameServer(uidInvalidPlayer, CR_SUCCESS, m_pGameServer->GetUID(), &tdToGameServerInfo);

			CHECK_EQUAL(false, bRet);
			CHECK_EQUAL(0, m_pLinkLoginServer->GetCommandCount());
		}

		TEST_FIXTURE(Fixture, TestMoveServerLogic_ResponseLoginGameServer_Filure_LoginServerDisconnected)
		{
			ZPlayer* pPlayer = AddNewPlayer();
			pPlayer->SetState(PS_READY_MOVE_GAMESERVER);

			gmgr.pLoginServerFacade->Disconnect();

			TD_LOGIN_GAME_SERVER_INFO tdToGameServerInfo;
			tdToGameServerInfo.uidConnectionKey = ZTestHelper::NewUID();
			bool bRet = m_logic.ResponseLoginGameServer(pPlayer->GetUID(), CR_SUCCESS, m_pGameServer->GetUID(), &tdToGameServerInfo);

			CHECK_EQUAL(false, bRet);
			CHECK_EQUAL(0, m_pLinkLoginServer->GetCommandCount());
		}

		TEST_FIXTURE(Fixture, TestMoveServerLogic_ResponseLoginGameServer_Filure_InvalidGameServerUID)
		{
			ZPlayer* pPlayer = AddNewPlayer();
			pPlayer->SetState(PS_READY_MOVE_GAMESERVER);

			MUID uidInvalidDestGameServer = ZTestHelper::NewUID();

			TD_LOGIN_GAME_SERVER_INFO tdToGameServerInfo;
			tdToGameServerInfo.uidConnectionKey = ZTestHelper::NewUID();
			bool bRet = m_logic.ResponseLoginGameServer(pPlayer->GetUID(), CR_SUCCESS, uidInvalidDestGameServer, &tdToGameServerInfo);

			CHECK_EQUAL(false, bRet);
			CHECK_EQUAL(0, m_pLinkLoginServer->GetCommandCount());
		}

		TEST_FIXTURE(Fixture, TestMoveServerLogic_ResponseLoginGameServer_Filure_InvalidLoginGameServerInfo)
		{
			ZPlayer* pPlayer = AddNewPlayer();
			pPlayer->SetState(PS_READY_MOVE_GAMESERVER);
			
			bool bRet = m_logic.ResponseLoginGameServer(pPlayer->GetUID(), CR_SUCCESS, m_pGameServer->GetUID(), NULL);

			CHECK_EQUAL(false, bRet);
			CHECK_EQUAL(0, m_pLinkLoginServer->GetCommandCount());
		}
	} // _ResponseLoginGameServer
	
	namespace _RequestReserveMoveGameServer
	{
		TEST_FIXTURE(Fixture, TestMoveServerLogic_RequestReserveMoveGameServer_Success)
		{
			/// Arrange
			ZPlayer* pPlayer = AddNewPlayerInWorld(m_pGameServer->GetID());
			CHECK_EQUAL(PS_IN_WORLD, pPlayer->GetState());
			
			MCommand* pCommand = gsys.pCommandCenter->NewCommand(MMC_COMM_REQUEST_RESERVE_MOVE_GAME_SERVER);
			pCommand->AddParameter(NEW_INT(ZTestHelper::NewID()));
			pCommand->AddParameter(NEW_UID(ZTestHelper::NewUID()));
			
			MCommandDesc* m_pCommandDesc = MGetCommandDescMap()->GetCommandDescByID(MMC_COMM_REQUEST_RESERVE_MOVE_GAME_SERVER);
			for (size_t i = 0; i < m_pCommandDesc->GetParameterDescCount()-2; ++i)
			{
				int nTestBlobData = i;
				pCommand->AddParameter(NEW_BLOB(&nTestBlobData, sizeof(int), 1));
			}
			
			/// Act
			m_logic.RequestReserveMoveGameServer(m_pGameServer->GetUID(), pPlayer->GetUID(), m_pGameServer2->GetID(), pCommand);
			
			/// Assert
			CHECK_EQUAL(PS_READY_MOVE_GAMESERVER, pPlayer->GetState());

			ASSERT_EQUAL(1, m_pLinkGameServer2->GetCommandCount());
			ASSERT_EQUAL(MMC_COMM_REQUEST_READY_ENTER_MOVE_PLAYER, m_pLinkGameServer2->GetCommandID(0));
			CHECK_EQUAL(m_pGameServer->GetUID(),	m_pLinkGameServer2->GetParam<MUID>(0, 1));
			CHECK_EQUAL(pPlayer->GetUID(),			m_pLinkGameServer2->GetParam<MUID>(0, 2));

			MCommand& refSendCmd = m_pLinkGameServer2->GetCommand(0);
			for (size_t i = 3; i < m_pCommandDesc->GetParameterDescCount(); ++i)
			{
				MCommandParameterBlob* pParamBlob = (MCommandParameterBlob*)refSendCmd.GetParameter(i);
				int* pData = (int*)pParamBlob->GetPointer();

				CHECK_EQUAL(i-3, *pData);
			}

			SAFE_DELETE(pCommand);
		}

		TEST_FIXTURE(Fixture, TestMoveServerLogic_RequestReserveMoveGameServer_Filure_State_Not_PS_IN_WORLD)
		{
			/// Arrange
			ZPlayer* pPlayer = AddNewPlayerInWorld(m_pGameServer->GetID());
			pPlayer->SetState(PS_SELECT_CHAR);

			MCommand* pCommand = gsys.pCommandCenter->NewCommand(MMC_COMM_REQUEST_RESERVE_MOVE_GAME_SERVER);
			pCommand->AddParameter(NEW_INT(ZTestHelper::NewID()));
			pCommand->AddParameter(NEW_UID(ZTestHelper::NewUID()));

			MCommandDesc* m_pCommandDesc = MGetCommandDescMap()->GetCommandDescByID(MMC_COMM_REQUEST_RESERVE_MOVE_GAME_SERVER);
			for (size_t i = 0; i < m_pCommandDesc->GetParameterDescCount()-2; ++i)
			{
				int nTestBlobData = i;
				pCommand->AddParameter(NEW_BLOB(&nTestBlobData, sizeof(int), 1));
			}

			/// Act
			bool bRet =m_logic.RequestReserveMoveGameServer(m_pGameServer->GetUID(), pPlayer->GetUID(), m_pGameServer2->GetID(), pCommand);

			/// Assert
			CHECK_EQUAL(false, bRet);
			CHECK_EQUAL(PS_SELECT_CHAR, pPlayer->GetState());
			CHECK_EQUAL(0, m_pLinkGameServer2->GetCommandCount());
			
			SAFE_DELETE(pCommand);

			ASSERT_EQUAL(1, m_pLinkGameServer->GetCommandCount());
			CHECK_EQUAL(MMC_COMM_RESPONSE_RESERVE_MOVE_GAME_SERVER, m_pLinkGameServer->GetCommandID(0));
			CHECK_EQUAL(pPlayer->GetUID(), m_pLinkGameServer->GetParam<MUID>(0, 0));
			CHECK_EQUAL(CR_FAIL, m_pLinkGameServer->GetParam<int>(0, 1));
		}

		TEST_FIXTURE(Fixture, TestMoveServerLogic_RequestReserveMoveGameServer_Filure_InvalidToGameServer)
		{
			/// Arrange
			ZPlayer* pPlayer = AddNewPlayerInWorld(m_pGameServer->GetID());
			CHECK_EQUAL(PS_IN_WORLD, pPlayer->GetState());

			MCommand* pCommand = gsys.pCommandCenter->NewCommand(MMC_COMM_REQUEST_RESERVE_MOVE_GAME_SERVER);
			pCommand->AddParameter(NEW_INT(ZTestHelper::NewID()));
			pCommand->AddParameter(NEW_UID(ZTestHelper::NewUID()));

			MCommandDesc* m_pCommandDesc = MGetCommandDescMap()->GetCommandDescByID(MMC_COMM_REQUEST_RESERVE_MOVE_GAME_SERVER);
			for (size_t i = 0; i < m_pCommandDesc->GetParameterDescCount()-2; ++i)
			{
				int nTestBlobData = i;
				pCommand->AddParameter(NEW_BLOB(&nTestBlobData, sizeof(int), 1));
			}

			int nInvalidGameServer2 = ZTestHelper::NewID();

			/// Act
			bool bRet =m_logic.RequestReserveMoveGameServer(m_pGameServer->GetUID(), pPlayer->GetUID(), nInvalidGameServer2, pCommand);

			/// Assert
			CHECK_EQUAL(false, bRet);
			CHECK_EQUAL(PS_IN_WORLD, pPlayer->GetState());
			CHECK_EQUAL(0, m_pLinkGameServer2->GetCommandCount());

			SAFE_DELETE(pCommand);

			ASSERT_EQUAL(1, m_pLinkGameServer->GetCommandCount());
			CHECK_EQUAL(MMC_COMM_RESPONSE_RESERVE_MOVE_GAME_SERVER, m_pLinkGameServer->GetCommandID(0));
			CHECK_EQUAL(pPlayer->GetUID(), m_pLinkGameServer->GetParam<MUID>(0, 0));
			CHECK_EQUAL(CR_FAIL, m_pLinkGameServer->GetParam<int>(0, 1));
		}
	}

	namespace _ResponseReadyEnterMovePlayer
	{
		TEST_FIXTURE(Fixture, TestMoveServerLogic_ResponseReadyEnterMovePlayer_Success)
		{
			ZPlayer* pPlayer = AddNewPlayerInWorld(m_pGameServer->GetID());
			pPlayer->SetState(PS_READY_MOVE_GAMESERVER);

			CHECK_EQUAL(1, m_pGameServer->GetPlayerMap().GetObjectCount());
			CHECK_EQUAL(0, m_pGameServer2->GetPlayerMap().GetObjectCount());

			TD_LOGIN_GAME_SERVER_INFO tdToGameServerInfo;
			tdToGameServerInfo.uidConnectionKey = ZTestHelper::NewUID();
			m_logic.ResponseReadyEnterMovePlayer(m_pGameServer->GetUID(), pPlayer->GetUID(), CR_SUCCESS, m_pGameServer2->GetUID(), &tdToGameServerInfo);

			CHECK_EQUAL(PS_MOVE_GAMESERVER, pPlayer->GetState());

			CHECK_EQUAL(0, m_pGameServer->GetPlayerMap().GetObjectCount());
			CHECK_EQUAL(0, m_pGameServer2->GetPlayerMap().GetObjectCount());

			ASSERT_EQUAL(1, m_pLinkGameServer->GetCommandCount());
			CHECK_EQUAL(MMC_COMM_RESPONSE_RESERVE_MOVE_GAME_SERVER,	m_pLinkGameServer->GetCommandID(0));
			CHECK_EQUAL(pPlayer->GetUID(),							m_pLinkGameServer->GetParam<MUID>(0, 0));
			CHECK_EQUAL(CR_SUCCESS,									m_pLinkGameServer->GetParam<int>(0, 1));
		}

		TEST_FIXTURE(Fixture, TestMoveServerLogic_ResponseReadyEnterMovePlayer_Failure_State_Not_READY_MOVE_GAMESERVER)
		{
			ZPlayer* pPlayer = AddNewPlayerInWorld(m_pGameServer->GetID());
			CHECK_EQUAL(PS_IN_WORLD, pPlayer->GetState());

			CHECK_EQUAL(1, m_pGameServer->GetPlayerMap().GetObjectCount());
			CHECK_EQUAL(0, m_pGameServer2->GetPlayerMap().GetObjectCount());

			TD_LOGIN_GAME_SERVER_INFO tdToGameServerInfo;
			tdToGameServerInfo.uidConnectionKey = ZTestHelper::NewUID();
			bool bRet = m_logic.ResponseReadyEnterMovePlayer(m_pGameServer->GetUID(), pPlayer->GetUID(), CR_SUCCESS, m_pGameServer2->GetUID(), &tdToGameServerInfo);

			CHECK_EQUAL(PS_IN_WORLD, pPlayer->GetState());
			CHECK_EQUAL(false, bRet);
			CHECK_EQUAL(1, m_pGameServer->GetPlayerMap().GetObjectCount());
			CHECK_EQUAL(0, m_pGameServer2->GetPlayerMap().GetObjectCount());

			ASSERT_EQUAL(1, m_pLinkGameServer->GetCommandCount());
			CHECK_EQUAL(MMC_COMM_RESPONSE_RESERVE_MOVE_GAME_SERVER,	m_pLinkGameServer->GetCommandID(0));
			CHECK_EQUAL(pPlayer->GetUID(),							m_pLinkGameServer->GetParam<MUID>(0, 0));
			CHECK_EQUAL(CR_FAIL,									m_pLinkGameServer->GetParam<int>(0, 1));
		}

		TEST_FIXTURE(Fixture, TestMoveServerLogic_ResponseReadyEnterMovePlayer_Failure_InvalidFromServerUID)
		{
			ZPlayer* pPlayer = AddNewPlayerInWorld(m_pGameServer->GetID());
			pPlayer->SetState(PS_READY_MOVE_GAMESERVER);

			MUID uidIvalidFromServer = ZTestHelper::NewUID();

			TD_LOGIN_GAME_SERVER_INFO tdToGameServerInfo;
			tdToGameServerInfo.uidConnectionKey = ZTestHelper::NewUID();
			bool bRet = m_logic.ResponseReadyEnterMovePlayer(uidIvalidFromServer, pPlayer->GetUID(), CR_SUCCESS, m_pGameServer2->GetUID(), &tdToGameServerInfo);

			CHECK_EQUAL(PS_READY_MOVE_GAMESERVER, pPlayer->GetState());
			CHECK_EQUAL(false, bRet);
			CHECK_EQUAL(1, m_pGameServer->GetPlayerMap().GetObjectCount());
			CHECK_EQUAL(0, m_pGameServer2->GetPlayerMap().GetObjectCount());
			CHECK_EQUAL(0, m_pLinkGameServer->GetCommandCount());
		}

		TEST_FIXTURE(Fixture, TestMoveServerLogic_ResponseReadyEnterMovePlayer_Failure_InvalidPlayerUID)
		{
			MUID uidIvalidPlayer = ZTestHelper::NewUID();

			TD_LOGIN_GAME_SERVER_INFO tdToGameServerInfo;
			tdToGameServerInfo.uidConnectionKey = ZTestHelper::NewUID();
			bool bRet = m_logic.ResponseReadyEnterMovePlayer(m_pGameServer->GetUID(), uidIvalidPlayer, CR_SUCCESS, m_pGameServer2->GetUID(), &tdToGameServerInfo);

			CHECK_EQUAL(false, bRet);
			CHECK_EQUAL(0, m_pLinkGameServer->GetCommandCount());
		}

		TEST_FIXTURE(Fixture, TestMoveServerLogic_ResponseReadyEnterMovePlayer_Failure_InvalidToServerUID)
		{
			ZPlayer* pPlayer = AddNewPlayerInWorld(m_pGameServer->GetID());
			pPlayer->SetState(PS_READY_MOVE_GAMESERVER);

			MUID uidIvalidToServer = ZTestHelper::NewUID();

			TD_LOGIN_GAME_SERVER_INFO tdToGameServerInfo;
			tdToGameServerInfo.uidConnectionKey = ZTestHelper::NewUID();
			bool bRet = m_logic.ResponseReadyEnterMovePlayer(m_pGameServer->GetUID(), pPlayer->GetUID(), CR_SUCCESS, uidIvalidToServer, &tdToGameServerInfo);

			CHECK_EQUAL(PS_READY_MOVE_GAMESERVER, pPlayer->GetState());
			CHECK_EQUAL(false, bRet);
			CHECK_EQUAL(1, m_pGameServer->GetPlayerMap().GetObjectCount());
			CHECK_EQUAL(0, m_pGameServer2->GetPlayerMap().GetObjectCount());
			CHECK_EQUAL(0, m_pLinkGameServer->GetCommandCount());
		}

		TEST_FIXTURE(Fixture, TestMoveServerLogic_ResponseReadyEnterMovePlayer_Failure_InvalidLoginGameServerInfo)
		{
			ZPlayer* pPlayer = AddNewPlayerInWorld(m_pGameServer->GetID());
			pPlayer->SetState(PS_READY_MOVE_GAMESERVER);

			bool bRet = m_logic.ResponseReadyEnterMovePlayer(m_pGameServer->GetUID(), pPlayer->GetUID(), CR_SUCCESS, m_pGameServer2->GetUID(), NULL);

			CHECK_EQUAL(PS_READY_MOVE_GAMESERVER, pPlayer->GetState());
			CHECK_EQUAL(false, bRet);
			CHECK_EQUAL(1, m_pGameServer->GetPlayerMap().GetObjectCount());
			CHECK_EQUAL(0, m_pGameServer2->GetPlayerMap().GetObjectCount());
			CHECK_EQUAL(0, m_pLinkGameServer->GetCommandCount());
		}
	} // _ResponseReadyEnterMovePlayer

}
