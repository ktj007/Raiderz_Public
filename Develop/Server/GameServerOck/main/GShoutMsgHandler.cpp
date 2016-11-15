#include "stdafx.h"
#include "GShoutMsgHandler.h"
#include "GGlobal.h"
#include "GPlayerObjectManager.h"
#include "GEntityPlayer.h"

GShoutMsgHandler::GShoutMsgHandler()
{
	// do nothing
}

GShoutMsgHandler::~GShoutMsgHandler()
{
	// do nothing
}

bool GShoutMsgHandler::OnRequest(const minet::MCommand* pCmd)
{
	if (m_strMsg.empty())	return false;

	GEntityPlayer* pPlayer = gmgr.pPlayerObjectManager->GetEntity(m_uidCommandSender);
	VALID_RET(pPlayer != NULL, false);

	wstring strSenderName = pPlayer->GetName();
	MCommand* pNewCmd = m_msgHelper.MakeNewClientCommandShoutRes(m_strMsg, strSenderName);
	pPlayer->RouteToField(pNewCmd);

	return true;
}

bool GShoutMsgHandler::OnResponse(const minet::MCommand* pCmd)
{
	// do nothing
	return true;
}
