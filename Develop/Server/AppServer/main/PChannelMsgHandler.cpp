#include "stdafx.h"
#include "PChannelMsgHandler.h"
#include "PGlobal.h"
#include "PServer.h"
#include "PCommandCenter.h"
#include "SProxyPlayerManager.h"
#include "SProxyPlayer.h"
#include "PChatChannelManager.h"
#include "PChatChannel.h"

PChannelMsgHandler::PChannelMsgHandler()
{
	// do nothing
}

PChannelMsgHandler::~PChannelMsgHandler()
{
	// do nothing
}

bool PChannelMsgHandler::OnRequest(const minet::MCommand* pCmd)
{
	// 요청자 확인
	CID nCID = m_msgHelper.GetSenderID(pCmd);
	SProxyPlayerManager* pManager = gsys.pServer->GetPlayerManager();
	SProxyPlayer* pPlayer = pManager->FindPlayer(nCID);

	if (pPlayer == NULL)	return false;


	// 채널 확인	
	MUID uidChannel = m_msgHelper.GetReceiverUID(pCmd);
	PChatChannel* pChannel = gmgr.pChatChannelManager->FindChannel(uidChannel);

	if (pChannel == NULL)					return false;
	if (!pChannel->IsExistPlayer(nCID))		return false;
	if (CCT_PRIVATE != pChannel->GetType())	return false;


	// 메시지 전달
	wstring strSenderName = pPlayer->GetName();
	MCommand* pNewCmd = m_msgHelper.MakeNewServerCommandChannelRes(m_strMsg, strSenderName, uidChannel);
	gsys.pCommandCenter->RouteToAll(pNewCmd);

	return true;
}
