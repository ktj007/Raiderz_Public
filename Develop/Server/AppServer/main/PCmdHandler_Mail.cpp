#include "stdafx.h"
#include "PCmdHandler_Mail.h"
#include "CCommandTable_App.h"
#include "STypes.h"
#include "CTransData.h"
#include "PGlobal.h"
#include "SProxyPlayerManager.h"
#include "PServer.h"
#include "PPostOfficeSystem.h"

PCmdHandler_Mail::PCmdHandler_Mail(MCommandCommunicator* pCC) : MCommandHandler(pCC)
{
	SetCmdHandler(MPC_MAIL_ROUTE_NEW_MAIL_TO_RECEIVER, RouteNewMailToReceiver);
	SetCmdHandler(MPC_MAIL_RETRY_TRANS_NEW_MAIL, TransNewMailRes);
}

MCommandResult PCmdHandler_Mail::RouteNewMailToReceiver(MCommand* pCommand, MCommandHandler* pHandler)
{
	// 인자 확인
	CID nCID;
	TD_MAIL_MAILBOX_MAIL_INFO* pInfo;

	if (!pCommand->GetParameter(&nCID, 0, MPT_INT))		return CR_ERROR;
	if (!pCommand->GetSingleBlob(pInfo, 1))				return CR_ERROR;

	
	// 통보 플레이어를 확인한다.
	SProxyPlayerManager* pPlayerManager = gsys.pServer->GetPlayerManager();
	SProxyPlayer* pPlayer = pPlayerManager->FindPlayer(nCID);

	if (pPlayer == NULL)
	{
		// 해당 플레이어는 이미 접속을 종료했다. 통보 않는다.
		return CR_TRUE;
	}
		

	// 통보한다.
	gsys.pPostOfficeSystem->RouteNewMailToReceiver(pPlayer, pInfo);
	

	return CR_TRUE;
}

MCommandResult PCmdHandler_Mail::TransNewMailRes(MCommand* pCommand, MCommandHandler* pHandler)
{
	// 인자 확인
	CID nCID;
	TD_MAIL_MAILBOX_MAIL_INFO* pInfo;

	if (!pCommand->GetParameter(&nCID, 0, MPT_INT))		return CR_ERROR;
	if (!pCommand->GetSingleBlob(pInfo, 1))				return CR_ERROR;
	
	
	// 통보 플레이어를 확인한다.
	SProxyPlayerManager* pPlayerManager = gsys.pServer->GetPlayerManager();
	SProxyPlayer* pPlayer = pPlayerManager->FindPlayer(nCID);

	if (pPlayer == NULL)
	{
		// 해당 플레이어는 이미 접속을 종료했다. 통보 않는다.
		return CR_TRUE;
	}


	// 다시 통보한다.
	gsys.pPostOfficeSystem->RetryNewMailToReceiver(pPlayer, pInfo);


	return CR_TRUE;
}
