#include "StdAfx.h"
#include "XDummyBot_MultiLogin.h"
#include "XBirdClient.h"
#include "XBirdDummyAgent.h"

XDummyBot_MultiLogin::XDummyBot_MultiLogin(XBirdDummyAgent* pAgent, XDummyAgentInfo* pAgentInfo)
: XDummyBot(pAgent, pAgentInfo)
{
}

XDummyBot_MultiLogin::~XDummyBot_MultiLogin(void)
{
}

minet::MCommandResult XDummyBot_MultiLogin::OnCommand( XBirdDummyAgent* pAgent, MCommand* pCommand )
{
	__super::OnCommand(pAgent, pCommand);

	XBirdClient* pClient = pAgent->GetClient();
	switch(pCommand->GetID())
	{
	case MC_FIELD_START_GAME:
		{
			XBIRDPOSTCMD2(pClient, MC_GM_SET_ME_REQ, MCommandParameterWideString(L"grade"), MCommandParameterWideString(L"3"));
			XBIRDPOSTCMD0(pClient, MC_GM_QUERY_MULTILOGIN_REQ);
		}
		break;
	case MC_GM_QUERY_MULTILOGIN:
		{
			bool bMultiLogin;
			if ( pCommand->GetParameter(&bMultiLogin, 0, MPT_BOOL) == false)		return CR_ERROR;

			if (bMultiLogin)
			{
				mlog("중복 접속 발생 : %s\n", pAgent->GetLoginID());
			}

			pClient->Disconnect();
			return CR_TRUE;
		}
		break;
	case MC_LOCAL_NET_ONDISCONNECT:
		{
			if (pAgent->GetConnectionManager()->GetStatus() != CONN_STATUS_MOVING_FROM_LOGINSERVER_TO_GAMESERVER)
			{
				pAgent->PostConnectToLoginServer();
				pAgent->SetDisconnectForTest();
			}
		}
		break;
	}

	return CR_FALSE;
}
