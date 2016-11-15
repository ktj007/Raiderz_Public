#include "stdafx.h"
#include "LCmdHandler_Master.h"
#include "MCommand.h"
#include "LCommandTable.h"
#include "LCommObject.h"
#include "LCommandCenter.h"
#include "LCommObject.h"
#include "LCommObjectManager.h"
#include "LServer.h"
#include "MCommandCommunicator.h"
#include "LNetServer.h"


LCmdHandler_Master::LCmdHandler_Master(MCommandCommunicator* pCC) : MCommandHandler(pCC)
{
	SetCmdHandler(MC_LOCAL_NET_CLEAR,			OnNetClear);

	// change command name from MMC_COMM_REQUEST_LOGIN to MMC_COMM_REQUEST_LOGIN_G2M, in revision 14573.
	SetCmdHandler(MMC_COMM_REQUEST_LOGIN_G2M,		OnRequestLogin);
}

MCommandResult LCmdHandler_Master::OnNetClear(MCommand* pCommand, MCommandHandler* pHandler)
{
	MUID uidPlayer;

	if (pCommand->GetParameter(&uidPlayer,	0, MPT_UID)==false) return CR_FALSE;

	SCommObject* pClientObject = gmgr.pCommObjectManager->GetClient(uidPlayer);
	if (pClientObject == NULL) return CR_TRUE;

	gmgr.pCommObjectManager->DeleteObject(uidPlayer);

	return CR_TRUE;
}


MCommandResult LCmdHandler_Master::OnRequestLogin(MCommand* pCommand, MCommandHandler* pHandler)
{

	
	return CR_TRUE;
}
