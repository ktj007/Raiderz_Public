#include "stdafx.h"
#include "PPartyMsgHandler.h"
#include "PGlobal.h"
#include "PServer.h"
#include "PCommandCenter.h"
#include "SProxyPlayerManager.h"
#include "SProxyPlayer.h"
#include "PChatChannelManager.h"
#include "PChatChannel.h"

PPartyMsgHandler::PPartyMsgHandler()
{
	// do nothing
}

PPartyMsgHandler::~PPartyMsgHandler()
{
	// do nothing
}

bool PPartyMsgHandler::OnRequest(const minet::MCommand* pCmd)
{
	// 요청자 확인
	CID nCID = m_msgHelper.GetSenderID(pCmd);
	SProxyPlayerManager* pManager = gsys.pServer->GetPlayerManager();
	SProxyPlayer* pPlayer = pManager->FindPlayer(nCID);
	
	if (pPlayer == NULL)	return false;

	// 채널 확인	
	MUID uidParty = m_msgHelper.GetReceiverUID(pCmd);
	PChatChannel* pChannel = gmgr.pChatChannelManager->FindChannel_Party(uidParty);
	
	if (pChannel == NULL)				return false;
	if (!pChannel->IsExistPlayer(nCID))	return false;

	// 메시지 전달
	wstring strSenderName = pPlayer->GetName();
	MCommand* pNewCmd = m_msgHelper.MakeNewServerCommandPartyRes(m_strMsg, strSenderName, uidParty);
	gsys.pCommandCenter->RouteToAll(pNewCmd);

	return true;
}
