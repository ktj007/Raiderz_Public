#include "stdafx.h"
#include "GGlobalMsgHandler.h"
#include "GGlobal.h"
#include "GPlayerObjectManager.h"
#include "GEntityPlayer.h"
#include "GCommandCenter.h"
#include "GAppServerFacade.h"

GGlobalMsgHandler::GGlobalMsgHandler()
{
	// do nothing
}

GGlobalMsgHandler::~GGlobalMsgHandler()
{
	// do nothing
}

bool GGlobalMsgHandler::OnRequest(const MCommand* pCmd)
{
	if (m_strMsg.empty())	return false;

	GEntityPlayer* pPlayer = gmgr.pPlayerObjectManager->GetEntity(m_uidCommandSender);
	VALID_RET(pPlayer != NULL, false);

	// 권한처리	
	// TODO: 2차 CBT 요청으로 권한처리 막습니다. [1/17/2011 praptor]
	// if (!pPlayer->IsGM())	return false;

	CID nCID = pPlayer->GetCID();
	MCommand* pNewCmd = m_msgHelper.MakeNewServerCommandGlobalReq(m_strMsg, nCID);
	gsys.pAppServerFacade->Route(pNewCmd);

	return true;
}

bool GGlobalMsgHandler::OnResponse(const MCommand* pCmd)
{
	wstring strSenderName = m_msgHelper.GetSenderName(pCmd);
	MCommand* pNewCmd = m_msgHelper.MakeNewClientCommandGlobalRes(m_strMsg, strSenderName);
	gsys.pCommandCenter->RouteToAll(pNewCmd);

	return true;
}
