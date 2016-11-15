#include "stdafx.h"
#include "PGuildMsgHandler.h"
#include "PGlobal.h"
#include "PServer.h"
#include "PCommandCenter.h"
#include "SProxyPlayerManager.h"
#include "SProxyPlayer.h"

PGuildMsgHandler::PGuildMsgHandler()
{
	// do nothing
}

PGuildMsgHandler::~PGuildMsgHandler()
{
	// do nothing
}

bool PGuildMsgHandler::OnRequest(const minet::MCommand* pCmd)
{
	// 요청자 확인
	CID nCID = m_msgHelper.GetSenderID(pCmd);
	SProxyPlayerManager* pManager = gsys.pServer->GetPlayerManager();
	SProxyPlayer* pPlayer = pManager->FindPlayer(nCID);
	if (pPlayer == NULL)	return true;

	// 채널 확인
	int nGuildID = m_msgHelper.GetReceiverID(pCmd);
	// TODO: 길드채널 추가할 것 - praptor, 2010.01.11

	// 메시지 통보
	wstring strSenderName = pPlayer->GetName();
	MCommand* pNewCmd = m_msgHelper.MakeNewServerCommandGuildRes(m_strMsg, strSenderName, nGuildID);
	gsys.pCommandCenter->RouteToAll(pNewCmd);

	return true;
}
