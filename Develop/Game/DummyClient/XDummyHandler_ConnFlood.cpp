#include "stdafx.h"
#include "XDummyHandler_ConnFlood.h"
#include "XBirdDummyAgent.h"
#include "XDummyPattern.h"
#include "CSNet.h"

minet::MCommandResult XDummyHandler_ConnFlood::OnCommand( XBirdDummyAgent* pAgent, MCommand* pCmd )
{
	XBirdClient* pClient = pAgent->GetClient();
	switch(pCmd->GetID())
	{
	case MC_LOCAL_NET_ONCONNECT:
		{
			pClient->Disconnect();
		}
		break;
	case MC_LOCAL_NET_ONDISCONNECT:
		{
			USES_CONVERSION_EX;

			pAgent->PostConnectToLoginServer();
		}
		break;
	}
	return CR_FALSE;
}

