#include "stdafx.h"
#include "GNoticeMsgHandler.h"
#include "GGlobal.h"
#include "GPlayerObjectManager.h"
#include "GEntityPlayer.h"
#include "GAppServerFacade.h"
#include "GCommandCenter.h"

GNoticeMsgHandler::GNoticeMsgHandler()
{
	// do nothing
}

GNoticeMsgHandler::~GNoticeMsgHandler()
{
	// do nothing
}

bool GNoticeMsgHandler::OnRequest(const MCommand* pCmd)
{
	if (m_strMsg.empty())	return false;

	// 요청자 확인
	GEntityPlayer* pPlayer = gmgr.pPlayerObjectManager->GetEntity(m_uidCommandSender);
	VALID_RET(pPlayer != NULL, false);
	

	// 권한처리
	if (!pPlayer->IsGM())	return false;


	// 앱서버 전달
	MCommand* pNewCmd = m_msgHelper.MakeNewServerCommandNoticeReq(m_strMsg);
	gsys.pAppServerFacade->Route(pNewCmd);

	return true;
}

bool GNoticeMsgHandler::OnResponse(const MCommand* pCmd)
{
	MCommand* pNewCmd = m_msgHelper.MakeNewClientCommandNoticeRes(m_strMsg);	
	gsys.pCommandCenter->RouteToAll(pNewCmd);

	return true;	
}
