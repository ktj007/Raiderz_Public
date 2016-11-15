#include "stdafx.h"
#include "GAdviceMsgHandler.h"
#include "GGlobal.h"
#include "GPlayerObjectManager.h"
#include "GEntityPlayer.h"

GAdviceMsgHandler::GAdviceMsgHandler()
{
	// do nothing
}

GAdviceMsgHandler::~GAdviceMsgHandler()
{
	// do nothing
}

bool GAdviceMsgHandler::OnRequest(const MCommand* pCmd)
{
	// do nothing
	return true;
}

bool GAdviceMsgHandler::OnResponse(const MCommand* pCmd)
{
	CID nCID = m_msgHelper.GetReceiverID(pCmd);
	GEntityPlayer* pPlayer = gmgr.pPlayerObjectManager->GetEntity(nCID);
	if (pPlayer == NULL)	return false;

	MCommand* pNewCmd = m_msgHelper.MakeNewClientCommandSystemRes((CCommandResultTable)m_nAdditional);
	pPlayer->RouteToMe(pNewCmd);
	return true;
}
