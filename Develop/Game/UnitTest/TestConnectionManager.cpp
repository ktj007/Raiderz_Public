#include "stdafx.h"
#include "XConnectionManager.h"
#include "FBaseNetClient.h"
#include "XUnitTestUtil.h"
#include "CSNet.h"

SUITE(ConnectionManager)
{
	class FConnectionManager : public FBaseNetClient
	{
	public:
		FConnectionManager() : FBaseNetClient()
		{
			/// true 일 경우 StandAlone 게임서버로 접속하도록 동작하므로 false로 설정.
			gvar.Network.bLoginToGameServer = false;

			m_pConnectionManager = new XConnectionManager(m_pNetClient);

			m_pNetAgent->IgnoreCmd(MC_COMM_ENTER_WORLD_LOCAL_MSG);
		}
		virtual ~FConnectionManager()
		{
			m_pNetAgent->ResetIgnoreCmd();

			SAFE_DELETE(m_pConnectionManager);

			 /// 원래 값으로 복원
			gvar.Network.bLoginToGameServer = m_bLoginToGameServer_Origin;
		}
		XConnectionManager*	m_pConnectionManager;
		bool m_bLoginToGameServer_Origin;
	};

	TEST_FIXTURE(FConnectionManager, TestConnectionManager_InitialStatus)
	{
		CHECK_EQUAL(CONN_STATUS_DISCONNECT, m_pConnectionManager->GetStatus());
	}

	TEST_FIXTURE(FConnectionManager, TestConnectionManager_ConnectToLoginServer)
	{
		m_pConnectionManager->OnConnectedToLoginServer();

		CHECK_EQUAL(CONN_STATUS_CONNECTED_TO_LOGIN_SERVER, m_pConnectionManager->GetStatus());

		m_pConnectionManager->OnDisconnect();

		CHECK_EQUAL(CONN_STATUS_DISCONNECT, m_pConnectionManager->GetStatus());

	}

	TEST_FIXTURE(FConnectionManager, TestConnectionManager_MoveToGameServer_FromLoginServer)
	{
		m_pConnectionManager->OnConnectedToLoginServer();

		MUID uidConnectionKey = XUnitTestUtil::NewMUID();

		m_pConnectionManager->MoveFromLoginServerToGameServer(L"localhost", DEFAULT_GAME_SERVER_PORT, uidConnectionKey);

		CHECK_EQUAL(CONN_STATUS_MOVING_FROM_LOGINSERVER_TO_GAMESERVER, m_pConnectionManager->GetStatus());

		m_pConnectionManager->OnDisconnect();

		m_pConnectionManager->OnReplyConnect();

		CHECK_EQUAL(MC_COMM_REQUEST_LOGIN_GAME_SERVER, m_pNetAgent->GetSendCommandID(1));
		CHECK_EQUAL(uidConnectionKey, m_pNetAgent->GetSendParam<MUID>(1, 0));
		CHECK_EQUAL(SH_COMMAND_VERSION, m_pNetAgent->GetSendParam<int>(1, 1));
	}

	TEST_FIXTURE(FConnectionManager, TestConnectionManager_ConnectToGameServer)
	{
		m_pConnectionManager->OnConnectedToGameServer();

		CHECK_EQUAL(CONN_STATUS_CONNECTED_TO_GAME_SERVER, m_pConnectionManager->GetStatus());
	}


	TEST_FIXTURE(FConnectionManager, TestConnectionManager_MoveToGameServer_FromGameServer)
	{
		m_pConnectionManager->OnConnectedToLoginServer();

		MUID uidConnectionKey = XUnitTestUtil::NewMUID();

		m_pConnectionManager->MoveFromLoginServerToGameServer(L"localhost", DEFAULT_GAME_SERVER_PORT, uidConnectionKey);

		m_pConnectionManager->OnDisconnect();
		m_pConnectionManager->OnReplyConnect();
		m_pConnectionManager->OnConnectedToGameServer();

		m_pNetAgent->ResetSendCommands();

		// 새로운 게임 서버로 접속
		MUID uidConnectionKey2 = XUnitTestUtil::NewMUID();
		m_pConnectionManager->MoveFromGameServerToGameServer(L"localhost2", DEFAULT_GAME_SERVER_PORT, uidConnectionKey2);

		CHECK_EQUAL(CONN_STATUS_MOVING_FROM_GAMESERVER_TO_GAMESERVER, m_pConnectionManager->GetStatus());

		m_pConnectionManager->OnDisconnect();
		m_pConnectionManager->OnReplyConnect();

		CHECK_EQUAL(2, m_pNetAgent->GetSendCommandCount());
		CHECK_EQUAL(MC_COMM_REQUEST_MOVE_GAME_SERVER, m_pNetAgent->GetSendCommandID(1));
		CHECK_EQUAL(uidConnectionKey2, m_pNetAgent->GetSendParam<MUID>(1, 0));
		CHECK_EQUAL(SH_COMMAND_VERSION, m_pNetAgent->GetSendParam<int>(1, 1));
	}
}