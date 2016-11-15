#include "stdafx.h"
#include "PAuctionMsgHandler.h"
#include "SProxyPlayerManager.h"
#include "PGlobal.h"
#include "PServer.h"
#include "SProxyPlayer.h"
#include "PCommandCenter.h"
#include "PChatChannelManager.h"
#include "PChatChannel.h"

PAuctionMsgHandler::PAuctionMsgHandler()
{
	// do nothing
}

PAuctionMsgHandler::~PAuctionMsgHandler()
{
	// do nothing
}

bool PAuctionMsgHandler::OnRequest(const minet::MCommand* pCmd)
{
	// 요청자 확인
	CID nCID = m_msgHelper.GetSenderID(pCmd);
	SProxyPlayerManager* pManager = gsys.pServer->GetPlayerManager();
	SProxyPlayer* pPlayer = pManager->FindPlayer(nCID);
	if (pPlayer == NULL)	return true;
	
	// 채널 확인
	PChatChannel* pChannel = gmgr.pChatChannelManager->FindChannel_Auction();
	if (pChannel == NULL)				return true;
	if (!pChannel->IsExistPlayer(nCID))	return true;

	// 메시지 전달
	wstring strSenderName = pPlayer->GetName();
	MCommand* pNewCmd = m_msgHelper.MakeNewServerCommandAuctionRes(m_strMsg, strSenderName);
	gsys.pCommandCenter->RouteToAll(pNewCmd);

	return true;
}
