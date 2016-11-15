#include "stdafx.h"
#include "GPartyMsgHandler.h"
#include "GGlobal.h"
#include "GPartySystem.h"
#include "GPlayerObjectManager.h"
#include "GEntityPlayer.h"
#include "GAppServerFacade.h"

GPartyMsgHandler::GPartyMsgHandler()
{
	// do nothing
}

GPartyMsgHandler::~GPartyMsgHandler()
{
	// do nothing
}

bool GPartyMsgHandler::OnRequest(const minet::MCommand* pCmd)
{	
	if (m_strMsg.empty())	return false;

	// 요청자 확인
	GEntityPlayer* pPlayer = gmgr.pPlayerObjectManager->GetEntity(m_uidCommandSender);
	VALID_RET(pPlayer != NULL, false);
	if (!pPlayer->HasParty())	return false;

	// 앱서버 전달
	int nPlayerCID = pPlayer->GetCID();
	MUID uidParty = pPlayer->GetPartyUID();
	MCommand* pNewCmd = m_msgHelper.MakeNewServerCommandPartyReq(m_strMsg, nPlayerCID, uidParty);
	gsys.pAppServerFacade->Route(pNewCmd);

	return true;
}

bool GPartyMsgHandler::OnResponse(const minet::MCommand* pCmd)
{
	wstring strSenderName = m_msgHelper.GetSenderName(pCmd);
	MCommand* pNewCmd = m_msgHelper.MakeNewClientCommandPartyRes(m_strMsg, strSenderName);	

	MUID uidParty = m_msgHelper.GetReceiverUID(pCmd);
	gsys.pPartySystem->RouteToAllMembers(uidParty, pNewCmd);

	return true;
}
