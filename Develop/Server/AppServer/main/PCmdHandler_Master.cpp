#include "stdafx.h"
#include "PCmdHandler_Master.h"
#include "MCommand.h"
#include "PCommandTable.h"
#include "PGameServerObject.h"
#include "PCommandCenter.h"
#include "PGameServerObject.h"
#include "PGameServerObjectManager.h"
#include "PServer.h"
#include "MCommandCommunicator.h"
#include "PNetServer.h"
#include "PLoginLogic.h"

PCmdHandler_Master::PCmdHandler_Master(MCommandCommunicator* pCC) : MCommandHandler(pCC)
{
	SetCmdHandler(MC_LOCAL_NET_CLEAR,			OnNetClear);
	SetCmdHandler(MPC_COMM_REQUEST_LOGIN,		OnRequestLogin);	
}

MCommandResult PCmdHandler_Master::OnNetClear(MCommand* pCommand, MCommandHandler* pHandler)
{
	MUID uidClient;

	if (pCommand->GetParameter(&uidClient,	0, MPT_UID)==false) return CR_FALSE;

	SCommObject* pClientObject = gmgr.pGameServerObjectManager->GetClient(uidClient);
	if (pClientObject == NULL) return CR_TRUE;

	gmgr.pGameServerObjectManager->DeleteObject(uidClient);

	return CR_TRUE;
}


MCommandResult PCmdHandler_Master::OnRequestLogin(MCommand* pCommand, MCommandHandler* pHandler)
{
	// 커멘드 확인
	SERVER_LOGIN_DESC desc;

	if (!pCommand->GetParameter(desc.strServerName, 0, MPT_WSTR))		return CR_FALSE;
	if (!pCommand->GetParameter(&desc.nServerType, 1, MPT_INT))			return CR_FALSE;
	if (!pCommand->GetParameter(&desc.nWorldID, 2, MPT_INT))			return CR_FALSE;
	if (!pCommand->GetParameter(&desc.nServerID, 3, MPT_INT))			return CR_FALSE;
	if (!pCommand->GetParameter(&desc.nCommandVersion, 4, MPT_INT))		return CR_FALSE;

		
	// 로그인 요청처리
	MUID uidClient = pCommand->GetSenderUID();
	gsys.pLoginLogic->RequestLogin(uidClient, desc);
	
	return CR_TRUE;
}
