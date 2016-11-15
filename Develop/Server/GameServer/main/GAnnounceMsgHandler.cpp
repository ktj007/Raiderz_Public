#include "stdafx.h"
#include "GAnnounceMsgHandler.h"
#include "GGlobal.h"
#include "GPlayerObjectManager.h"
#include "GEntityPlayer.h"
#include "GAppServerFacade.h"
#include "GCommandCenter.h"

GAnnounceMsgHandler::GAnnounceMsgHandler()
{
	// do nothing
}

GAnnounceMsgHandler::~GAnnounceMsgHandler()
{
	// do nothing
}

bool GAnnounceMsgHandler::OnRequest(const MCommand* pCmd)
{
	if (m_strMsg.empty())	return false;

	GEntityPlayer* pPlayer = gmgr.pPlayerObjectManager->GetEntity(m_uidCommandSender);
	VALID_RET(pPlayer != NULL, false);

	
	// 권한처리
	if (!pPlayer->IsGM())	return false;


	int nCID = pPlayer->GetCID();
	MCommand* pNewCmd = m_msgHelper.MakeNewServerCommandAnnounceReq(m_strMsg, nCID);	
	gsys.pAppServerFacade->Route(pNewCmd);	
	
	return true;
}

bool GAnnounceMsgHandler::OnResponse(const MCommand* pCmd)
{
	wstring strSenderName = m_msgHelper.GetSenderName(pCmd);
	MCommand* pNewCmd = m_msgHelper.MakeNewClientCommandAnnounceRes(m_strMsg, strSenderName);
	gsys.pCommandCenter->RouteToAll(pNewCmd);
	return true;
}
