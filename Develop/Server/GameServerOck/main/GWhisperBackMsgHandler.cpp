#include "stdafx.h"
#include "GWhisperBackMsgHandler.h"
#include "GGlobal.h"
#include "GPlayerObjectManager.h"
#include "GEntityPlayer.h"

GWhisperBackMsgHandler::GWhisperBackMsgHandler()
{
	// do nothing
}

GWhisperBackMsgHandler::~GWhisperBackMsgHandler()
{
	// do nothing
}

bool GWhisperBackMsgHandler::OnRequest(const minet::MCommand* pCmd)
{
	// do nothing
	return true;
}

bool GWhisperBackMsgHandler::OnResponse(const minet::MCommand* pCmd)
{
	int nCID = m_msgHelper.GetSenderID(pCmd);
	GEntityPlayer* pPlayer = gmgr.pPlayerObjectManager->GetEntity(nCID);
	VALID_RET(pPlayer != NULL, false);

	wstring strName = m_msgHelper.GetReceiverName(pCmd);
	MCommand* pNewCmd = m_msgHelper.MakeNewClientCommandWhisperBackRes(m_strMsg, strName);
	pPlayer->RouteToMe(pNewCmd);

	return true;
}
