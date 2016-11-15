#include "stdafx.h"
#include "LCmdHandler_Master_Net.h"
#include "LGlobal.h"
#include "LServer.h"
#include "LCommandTable.h"
#include "LMasterClient.h"

LCmdHandler_Master_Net::LCmdHandler_Master_Net(MCommandCommunicator* pCC) : MCommandHandler(pCC)
{
	SetCmdHandler(MC_LOCAL_NET_ONCONNECT,		OnConnect);
	SetCmdHandler(MC_LOCAL_NET_ONDISCONNECT,	OnDisconnect);
	SetCmdHandler(MC_LOCAL_NET_ONERROR,			OnError);
}

MCommandResult LCmdHandler_Master_Net::OnConnect(MCommand* pCommand, MCommandHandler* pHandler)
{
	gsys.pServer->GetMasterClient()->OnConnect();

	return CR_TRUE;
}


MCommandResult LCmdHandler_Master_Net::OnDisconnect(MCommand* pCommand, MCommandHandler* pHandler)
{
	gsys.pServer->GetMasterClient()->OnDisconnect();
	return CR_TRUE;
}

MCommandResult LCmdHandler_Master_Net::OnError(MCommand* pCommand, MCommandHandler* pHandler)
{
	int nResult = 0;
	if (!pCommand->GetParameter(&nResult, 0, MPT_INT))		return CR_ERROR;

	gsys.pServer->GetMasterClient()->OnError(nResult);

	return CR_TRUE;
}
