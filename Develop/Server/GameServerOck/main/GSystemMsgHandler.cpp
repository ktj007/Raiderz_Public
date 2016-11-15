#include "stdafx.h"
#include "GSystemMsgHandler.h"
#include "GGlobal.h"
#include "GPlayerObjectManager.h"
#include "GEntityPlayer.h"

GSystemMsgHandler::GSystemMsgHandler()
{
	// do nothing
}

GSystemMsgHandler::~GSystemMsgHandler()
{
	// do nothing
}

bool GSystemMsgHandler::OnRequest(const MCommand* pCmd)
{
	// do nothing
	return true;
}

bool GSystemMsgHandler::OnResponse(const MCommand* pCmd)
{
	CID nCID = m_msgHelper.GetReceiverID(pCmd);
	GEntityPlayer* pPlayer = gmgr.pPlayerObjectManager->GetEntity(nCID);
	if (pPlayer == NULL)	return false;
	
	MCommand* pNewCmd = m_msgHelper.MakeNewClientCommandSystemRes((CCommandResultTable)m_nAdditional);
	pPlayer->RouteToMe(pNewCmd);
	return true;
}
