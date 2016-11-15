#include "stdafx.h"
#include "GShoutFakeMsgHandler.h"
#include "GGlobal.h"
#include "GPlayerObjectManager.h"
#include "GEntityPlayer.h"

GShoutFakeMsgHandler::GShoutFakeMsgHandler()
{
	// do nothing
}

GShoutFakeMsgHandler::~GShoutFakeMsgHandler()
{
	// do nothing
}

bool GShoutFakeMsgHandler::OnRequest(const minet::MCommand* pCmd)
{
	if (m_strMsg.empty())	return false;

	GEntityPlayer* pPlayer = gmgr.pPlayerObjectManager->GetEntity(m_uidCommandSender);
	VALID_RET(pPlayer != NULL, false);

	// 권한처리	
	if (!pPlayer->IsGM())	return false;

	wstring strSenderName = pPlayer->GetName();
	MCommand* pNewCmd = m_msgHelper.MakeNewClientCommandShoutFakeRes(m_strMsg, strSenderName);
	pPlayer->RouteToField(pNewCmd);

	return true;
}

bool GShoutFakeMsgHandler::OnResponse(const minet::MCommand* pCmd)
{
	// do nothing
	return true;
}
