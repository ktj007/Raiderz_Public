#include "stdafx.h"
#include "GAuctionMsgHandler.h"
#include "GGlobal.h"
#include "GPlayerObjectManager.h"
#include "GEntityPlayer.h"
#include "GCommandCenter.h"
#include "GAppServerFacade.h"
#include "GSystem.h"
#include "GMsgCommandFacade.h"
#include "CCommandResultTable.h"

GAuctionMsgHandler::GAuctionMsgHandler()
{
	// do nothing
}

GAuctionMsgHandler::~GAuctionMsgHandler()
{
	// do nothing
}

bool GAuctionMsgHandler::OnRequest(const MCommand* pCmd)
{
	if (m_strMsg.empty())	return false;

	GEntityPlayer* pPlayer = gmgr.pPlayerObjectManager->GetEntity(m_uidCommandSender);
	VALID_RET(pPlayer != NULL, false);


	// 임시 시간제한 1분		
	uint32 nNowTime = gsys.pSystem->GetNowTime();
	
	if (!pPlayer->CheckAllowAuctionMsgTime(nNowTime))
	{
		gsys.pMsgCommandFacade->RouteSystemMsg(pPlayer, CR_FAIL_MSG_LIMIT_AUCTION_MSG_TIME);
		return false;
	}


	CID nCID = pPlayer->GetCID();
	MCommand* pNewCmd = m_msgHelper.MakeNewServerCommandAuctionReq(m_strMsg, nCID);
	gsys.pAppServerFacade->Route(pNewCmd);

	return true;
}

bool GAuctionMsgHandler::OnResponse(const MCommand* pCmd)
{
	wstring strSenderName = m_msgHelper.GetSenderName(pCmd);
	MCommand* pNewCmd = m_msgHelper.MakeNewClientCommandAuctionRes(m_strMsg, strSenderName);
	gsys.pCommandCenter->RouteToAll(pNewCmd);

	return true;
}
