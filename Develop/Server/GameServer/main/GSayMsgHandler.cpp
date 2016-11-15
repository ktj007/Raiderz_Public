#include "stdafx.h"
#include "GSayMsgHandler.h"
#include "GGlobal.h"
#include "GPlayerObjectManager.h"
#include "GEntityPlayer.h"
#include "GConst.h"
#include "GAppServerFacade.h"

GSayMsgHandler::GSayMsgHandler()
{
	// do nothing
}

GSayMsgHandler::~GSayMsgHandler()
{
	// do nothing
}

bool GSayMsgHandler::OnRequest(const minet::MCommand* pCmd)
{
	GEntityPlayer* pPlayer = gmgr.pPlayerObjectManager->GetEntity(m_uidCommandSender);
	VALID_RET(pPlayer != NULL, false);

	wstring strName = pPlayer->GetName();

	if (GConst::DEFAULT_GLOBAL_SAY)
	{
		int nCID = pPlayer->GetCID();
		MCommand* pNewCmd = m_msgHelper.MakeNewServerCommandGlobalReq(m_strMsg, nCID);
		gsys.pAppServerFacade->Route(pNewCmd);
	}
	else
	{
		GPlayerGrade nGrade = pPlayer->GetPlayerGrade();

		MCommand* pNewCmd = m_msgHelper.MakeNewClientCommandSayRes(m_strMsg, strName, nGrade);
		pPlayer->RouteToThisCell(pNewCmd);
	}

	return true;
}

bool GSayMsgHandler::OnResponse(const minet::MCommand* pCmd)
{
	// do nothing
	return true;
}
