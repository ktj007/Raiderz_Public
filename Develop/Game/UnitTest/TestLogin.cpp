#include "stdafx.h"
#include "XNetwork.h"
#include "XConnectionManager.h"
#include "XNetClient.h"
#include "FBaseNetClient.h"
#include "XUnitTestUtil.h"
#include "CCommandResultTable.h"

SUITE(Login)
{
	class FLogin : public FBaseNetClient
	{
	public:
		FLogin() : FBaseNetClient()
		{
			m_uidHost = XUnitTestUtil::NewMUID();
			m_uidAlloc = XUnitTestUtil::NewMUID();
		}
		virtual ~FLogin()
		{

		}
		MUID m_uidHost;
		MUID m_uidAlloc;
	};

	TEST_FIXTURE(FLogin, TestLoginServer_Login)
	{
		m_pNetAgent->OnRecv(MC_NET_REPLY_CONNECT, 3, NEW_UID(m_uidHost), NEW_UID(m_uidAlloc), NEW_UINT(0));
		m_pNetAgent->OnRecv(MC_COMM_RESPONSE_LOGIN, 3, NEW_INT(CR_SUCCESS), NEW_UID(m_uidAlloc), NEW_UCHAR(SERVER_MODE_NORMAL));

		CHECK_EQUAL(CONN_STATUS_CONNECTED_TO_LOGIN_SERVER, global.net->GetConnectionManager()->GetStatus());

		// 2010.3.29일 rev. 20194 버젼 이후로 해당 커맨드는 UI에서 요청하도록 변경됨 - 로그인 후 카메라 이동이 완료된 이후 보내도록 수정됨. UI 변경 후 다시 검토 예정.
//		CHECK_EQUAL(MC_COMM_REQUEST_ACCOUNT_CHAR_LIST, m_pNetAgent->GetSendCommandID(0));

		TD_LOGIN_GAME_SERVER_INFO tdLoginGameServerInfo;
		wcsncpy_s(tdLoginGameServerInfo.strHostName, L"TestHostName", _TRUNCATE);
		tdLoginGameServerInfo.nPort = 200;
		tdLoginGameServerInfo.uidConnectionKey = XUnitTestUtil::NewMUID();

		
		m_pNetAgent->OnRecv(MC_COMM_MOVE_TO_GAME_SERVER, 1, NEW_BLOB(&tdLoginGameServerInfo, sizeof(TD_LOGIN_GAME_SERVER_INFO), 1));

		CHECK_EQUAL(CONN_STATUS_MOVING_FROM_LOGINSERVER_TO_GAMESERVER, global.net->GetConnectionManager()->GetStatus());

		// 접속이 끊기면 게임 서버로 다시 접속한다.
		m_pNetAgent->OnRecv(MC_LOCAL_NET_ONDISCONNECT, 0, NULL);

		// 2010.3.29일 rev. 20194 버젼 이후로 접속이 끊기면 자동으로 재접속하지 않도록 수정됨. 재접속 여부는 UI 수정 후 결정.
//		CHECK_EQUAL(MC_LOCAL_NET_CONNECT, m_pNetAgent->GetSendCommandID(1));


		// 게임 서버에 접속됨
		m_pNetAgent->OnRecv(MC_COMM_RESPONSE_LOGIN_GAME_SERVER, 3, NEW_INT(CR_SUCCESS), NEW_UID(m_uidAlloc), NEW_USHORT(0));
		CHECK_EQUAL(CONN_STATUS_CONNECTED_TO_GAME_SERVER, global.net->GetConnectionManager()->GetStatus());
	}
}