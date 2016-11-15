#include "stdafx.h"
#include "XDummyHandler_LoginFlood.h"
#include "XBirdDummyAgent.h"
#include "XDummyPattern.h"
#include "CCommandResultTable.h"

minet::MCommandResult XDummyHandler_LoginFlood::OnCommand( XBirdDummyAgent* pAgent, MCommand* pCmd )
{
	XBirdClient* pClient = pAgent->GetClient();
	switch(pCmd->GetID())
	{
	case MC_LOCAL_NET_ONDISCONNECT:
		{
			if (pAgent->GetConnectionManager()->GetStatus() != CONN_STATUS_MOVING_FROM_LOGINSERVER_TO_GAMESERVER &&
				pAgent->GetConnectionManager()->GetStatus() != CONN_STATUS_MOVING_FROM_GAMESERVER_TO_GAMESERVER )
			{
				pAgent->PostConnectToLoginServer();
			}
		}
		break;
	case MC_COMM_RESPONSE_ACCOUNT_CHAR_LIST:
		{

		}
		break;
	}
	return CR_FALSE;
}

///////////////////////////////////////////////////////////

minet::MCommandResult XDummyHandler_LoginDisconnect::OnCommand( XBirdDummyAgent* pAgent, MCommand* pCmd )
{
	XBirdClient* pClient = pAgent->GetClient();
	switch(pCmd->GetID())
	{
	case MC_COMM_RESPONSE_LOGIN:
		{
			int nResult;
			pCmd->GetParameter(&nResult, 0, MPT_INT);

			if (nResult == CR_SUCCESS)
			{
				pAgent->SetDisconnectForTest();
				pClient->Disconnect();
			}

			return CR_TRUE;
		}
	}
	return CR_FALSE;
}
