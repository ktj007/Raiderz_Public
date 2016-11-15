#include "stdafx.h"
#include "GCmdHandler_App_Mail.h"
#include "CCommandTable_App.h"
#include "CTransData.h"
#include "GPlayerObject.h"
#include "GGlobal.h"
#include "GPlayerObjectManager.h"
#include "GMailSystem.h"

GCmdHandler_App_Mail::GCmdHandler_App_Mail(MCommandCommunicator* pCC) : MCommandHandler(pCC)
{
	SetCmdHandler(MPC_MAIL_TRANS_NEW_MAIL,	OnTranNewMailReq);
}

MCommandResult GCmdHandler_App_Mail::OnTranNewMailReq(MCommand* pCommand, MCommandHandler* pHandler)
{
	// 인자 확인
	CID nCID;
	TD_MAIL_MAILBOX_MAIL_INFO* pInfo;

	if (!pCommand->GetParameter(&nCID, 0, MPT_INT))	return CR_ERROR;
	if (!pCommand->GetSingleBlob(pInfo, 1))			return CR_ERROR;

	
	// 수신자 확인
	GPlayerObject* pPlayerObject = gmgr.pPlayerObjectManager->GetPlayer(nCID);

	if (pPlayerObject == NULL)
	{
		// AppServer에 재전송을 요청한다.
		gsys.pMailSystem->RetryTransNewMail(nCID, pInfo);
		return CR_TRUE;
	}

	
	// 게임 상태가 아니라면(서버이동 중), AppServer에 재전송을 요구한다.
	if (!pPlayerObject->IsInWorld())
	{	
		gsys.pMailSystem->RetryTransNewMail(nCID, pInfo);
		return CR_TRUE;
	}		


	// 게임 중이면, 통보한다.
	GEntityPlayer* pPlayer = pPlayerObject->GetEntity();
	gsys.pMailSystem->UpdateMailBox(pPlayer, pInfo);


	return CR_TRUE;
}
