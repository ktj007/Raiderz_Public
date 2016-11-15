#include "stdafx.h"
#include "PWhisperMsgHandler.h"
#include "STypes.h"
#include "PCommandCenter.h"
#include "PGlobal.h"
#include "SProxyPlayerManager.h"
#include "SProxyPlayer.h"
#include "PServer.h"
#include "PGameServerObject.h"
#include "PGameServerObjectManager.h"

PWhisperMsgHandler::PWhisperMsgHandler()
{
	// do nothing
}

PWhisperMsgHandler::~PWhisperMsgHandler()
{
	// do nothing
}

bool PWhisperMsgHandler::OnRequest(const minet::MCommand* pCmd)
{	
	SProxyPlayerManager* pManager = gsys.pServer->GetPlayerManager();

	// 요청자 찾기
	CID nSenderCID = m_msgHelper.GetSenderID(pCmd);	
	SProxyPlayer* pSenderPlayer = pManager->FindPlayer(nSenderCID);	
	if (pSenderPlayer == NULL)		return true;
	
	// 수신자 찾기
	wstring strReceiverName = m_msgHelper.GetReceiverName(pCmd);
	SProxyPlayer* pReceiverPlayer = pManager->FindPlayer(strReceiverName);
	if (pReceiverPlayer == NULL)
	{
		// 수신자가 없을 경우
		MCommand* pErrCmd = m_msgHelper.MakeNewServerCommandSystemRes(CR_FAIL_MSG_NO_WHISPER_RECEIVER, nSenderCID);
		pErrCmd->SetReceiverUID(pCmd->GetSenderUID());
		gsys.pCommandCenter->PostCommand(pErrCmd);
		return true;
	}

	int nReceiverServerID = pReceiverPlayer->GetGameServerID();
	PGameServerObject* pReceiverServer =  gmgr.pGameServerObjectManager->Find(nReceiverServerID);
	if (pReceiverServer == NULL)	return true;


	// 발신/수신 정보 입력
	wstring strSenderName = pSenderPlayer->GetName();
	CID nReceiverCID = pReceiverPlayer->GetCID();

	
	// 수신자 서버에 전달 - 자신에게도 메시지 반송
	MCommand* pBackCmd = m_msgHelper.MakeNewServerCommandWhisperBackRes(m_strMsg, nSenderCID, strReceiverName);

	MUID uidSenderServer = pCmd->GetSenderUID();
	pBackCmd->AddReceiver(uidSenderServer);	
	gsys.pCommandCenter->PostCommand(pBackCmd);

		
	MCommand* pNewCmd = m_msgHelper.MakeNewServerCommandWhisperRes(m_strMsg, strSenderName, nReceiverCID);
	MUID uidReceiverServer = pReceiverServer->GetUID();
	pNewCmd->AddReceiver(uidReceiverServer);
	gsys.pCommandCenter->PostCommand(pNewCmd);


	return true;
}
