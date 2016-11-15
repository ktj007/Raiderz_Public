#include "stdafx.h"
#include "GGuildMsgHandler.h"
#include "GGlobal.h"
#include "GPlayerObjectManager.h"
#include "GEntityPlayer.h"
#include "GGuild.h"
#include "GGuildMgr.h"
#include "GGuildSystem.h"

GGuildMsgHandler::GGuildMsgHandler()
{
	// do nothing
}

GGuildMsgHandler::~GGuildMsgHandler()
{
	// do nothing
}

bool GGuildMsgHandler::OnRequest(const MCommand* pCmd)
{	
	// TODO: 앱서버 길드 연동 후 수정할 것 - praptor, 2010.01.11

	if (m_strMsg.empty())	return false;

	GEntityPlayer* pPlayer = gmgr.pPlayerObjectManager->GetEntity(m_uidCommandSender);
	VALID_RET(pPlayer != NULL, false);

	int nGuildID = pPlayer->GetGID();
	GGuild* pGuild = gmgr.pGuildMgr->Get(nGuildID);
	VALID_RET(pGuild != NULL, false);

	wstring strSenderName = pPlayer->GetName();
	MCommand* pNewCmd = m_msgHelper.MakeNewClientCommandGuildRes(m_strMsg, strSenderName);
	gsys.pGuildSystem->RouteToMember(pGuild, pNewCmd);

	return true;
}

bool GGuildMsgHandler::OnResponse(const MCommand* pCmd)
{
	// TODO: 앱서버 길드 연동 후 수정할 것 - praptor, 2010.01.11
	return true;
}
