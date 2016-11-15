#include "stdafx.h"
#include "GWhisperMsgHandler.h"
#include "GGlobal.h"
#include "GPlayerObjectManager.h"
#include "GEntityPlayer.h"
#include "GAppServerFacade.h"
#include "GMsgCommandFacade.h"

GWhisperMsgHandler::GWhisperMsgHandler()
{
	// do nothing
}

GWhisperMsgHandler::~GWhisperMsgHandler()
{
	// do nothing
}

bool GWhisperMsgHandler::OnRequest(const MCommand* pCmd)
{
	if (m_strMsg.empty())	return false;

	GEntityPlayer* pPlayer = gmgr.pPlayerObjectManager->GetEntity(m_uidCommandSender);
	VALID_RET(pPlayer != NULL, false);

	int nCID = pPlayer->GetCID();
	wstring strTargetName = m_msgHelper.GetReceiverName(pCmd);
	if (strTargetName.empty() || PLAYER_NAME_LEN < strTargetName.size()) return false;
	if (strTargetName == pPlayer->GetName()) return false;

	MCommand* pNewCmd = m_msgHelper.MakeNewServerCommandWhisperReq(m_strMsg, nCID, strTargetName);
	gsys.pAppServerFacade->Route(pNewCmd);

	return true;
}

bool GWhisperMsgHandler::OnResponse(const MCommand* pCmd)
{
	int nCID = m_msgHelper.GetReceiverID(pCmd);
	GEntityPlayer* pPlayer = gmgr.pPlayerObjectManager->GetEntity(nCID);
	VALID_RET(pPlayer != NULL, false);

	wstring strName = m_msgHelper.GetSenderName(pCmd);
	MCommand* pNewCmd = m_msgHelper.MakeNewClientCommandWhisperRes(m_strMsg, strName);
	pPlayer->RouteToMe(pNewCmd);

	return true;
}
