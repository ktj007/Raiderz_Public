#include "stdafx.h"
#include "GNarrationMsgHandler.h"
#include "GGlobal.h"
#include "GPlayerObjectManager.h"
#include "GEntityPlayer.h"

GNarrationMsgHandler::GNarrationMsgHandler()
{
	// do nothing
}

GNarrationMsgHandler::~GNarrationMsgHandler()
{
	// do nothing
}

bool GNarrationMsgHandler::OnRequest(const MCommand* pCmd)
{
	if (m_strMsg.empty())	return false;

	// 요청자 확인
	GEntityPlayer* pPlayer = gmgr.pPlayerObjectManager->GetEntity(m_uidCommandSender);
	VALID_RET(pPlayer != NULL, false);
	
	
	// 권한처리	
	if (!pPlayer->IsGM())	return false;


	// 메시지 전달
	MCommand* pNewCmd = m_msgHelper.MakeNewClientCommandNarrationRes(m_strMsg);
	pPlayer->RouteToThisCell(pNewCmd);

	return true;
}

bool GNarrationMsgHandler::OnResponse(const MCommand* pCmd)
{
	// do nothing
	return true;
}
