#include "stdafx.h"
#include "PFieldMsgHandler.h"
#include "PGlobal.h"
#include "PServer.h"
#include "PCommandCenter.h"
#include "SProxyPlayerManager.h"
#include "SProxyPlayer.h"
#include "PChatChannelManager.h"
#include "PChatChannel.h"

PFieldMsgHandler::PFieldMsgHandler()
{
	// do nothing
}

PFieldMsgHandler::~PFieldMsgHandler()
{
	// do nothing
}

bool PFieldMsgHandler::OnRequest(const minet::MCommand* pCmd)
{
	// 요청자 확인
	CID nCID = m_msgHelper.GetSenderID(pCmd);
	SProxyPlayerManager* pManager = gsys.pServer->GetPlayerManager();
	SProxyPlayer* pPlayer = pManager->FindPlayer(nCID);

	if (pPlayer == NULL)	return true;


	// 채널 확인
	int nFieldID = m_msgHelper.GetReceiverID(pCmd);
	PChatChannel* pChannel = gmgr.pChatChannelManager->FindChannel_Field(nFieldID);
	if (pChannel == NULL)					return true;
	if (!pChannel->IsExistPlayer(nCID))		return true;


	// 메시지 통보
	wstring strSenderName = pPlayer->GetName();
	MCommand* pNewCmd = m_msgHelper.MakeNewServerCommandFieldRes(m_strMsg, strSenderName, nFieldID);
	gsys.pCommandCenter->RouteToAll(pNewCmd);

	return true;
}
