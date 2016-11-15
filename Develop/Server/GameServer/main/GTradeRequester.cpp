#include "StdAfx.h"
#include "GTradeRequester.h"
#include "GEntityPlayer.h"
#include "GPlayerDoing.h"
#include "CCommandResultTable.h"
#include "CCommandTable.h"
#include "GCommand.h"
#include "GGlobal.h"
#include "GPlayerTrade.h"
#include "GTradeSystem.h"
#include "GTradeChecker.h"

bool GTradeRequester::Request(GEntityPlayer* pReqPlayer, GEntityPlayer* pTarPlayer)
{
	VALID_RET(pReqPlayer, false);
	VALID_RET(pTarPlayer, false);

	if (false == Check(pReqPlayer, pTarPlayer))
	{
		return pReqPlayer->FailAndRouteSystemMsg(CR_FAIL);		
	}

	Apply(pReqPlayer, pTarPlayer);	
	Route(pReqPlayer, pTarPlayer);

	return true;
}

bool GTradeRequester::Check(GEntityPlayer* pReqPlayer, GEntityPlayer* pTarPlayer)
{
	VALID_RET(pReqPlayer, false);
	VALID_RET(pTarPlayer, false);

	if (false == pReqPlayer->GetDoing().IsEndableBusy()) return false;

	if (false == gsys.pTradeSystem->GetChecker().CheckDistance(pReqPlayer, pTarPlayer)) return false;
	if (false == gsys.pTradeSystem->GetChecker().CheckEnemy(pReqPlayer, pTarPlayer)) return false;		
	if (false == CheckSelfRequest(pReqPlayer, pTarPlayer)) return false;
	if (false == CheckTargetBusy(pTarPlayer, pReqPlayer)) return false;	

	return true;
}

bool GTradeRequester::CheckTargetBusy( GEntityPlayer* pTarPlayer, GEntityPlayer* pReqPlayer )
{
	if (true == pTarPlayer->GetDoing().IsBusy())
	{
		CHAR_DOING nDoing = pTarPlayer->GetDoing().GetCurDoing();
		if (CD_INTERACTING == nDoing)
		{
			return pReqPlayer->FailAndRouteSystemMsg(CR_FAIL_SYSTEM_TARGET_IS_INTERACTING);
		}
		else if (CD_TRADING == nDoing)
		{
			return pReqPlayer->FailAndRouteSystemMsg(CR_FAIL_SYSTEM_TARGET_IS_TRADING);
		}
		else if (CD_LOOTING == nDoing)
		{
			return pReqPlayer->FailAndRouteSystemMsg(CR_FAIL_SYSTEM_TARGET_IS_LOOTING);
		}
		else if (CD_CUTSCENING == nDoing)
		{
			return pReqPlayer->FailAndRouteSystemMsg(CR_FAIL_SYSTEM_TARGET_IS_CUTSCENING);
		}		
	}	

	if (true == pTarPlayer->IsNowCombat())
	{
		return pReqPlayer->FailAndRouteSystemMsg(CR_FAIL_SYSTEM_TARGET_IS_NOWCOMBAT);
	}

	if (true == pTarPlayer->HasDuel())
	{
		return pReqPlayer->FailAndRouteSystemMsg(CR_FAIL_SYSTEM_TARGET_IS_DUELING);
	}

	return true;
}

bool GTradeRequester::CheckSelfRequest( GEntityPlayer* pReqPlayer, GEntityPlayer* pTarPlayer )
{
	return pReqPlayer->GetUID() != pTarPlayer->GetUID();
}

void GTradeRequester::Apply(GEntityPlayer* pReqPlayer, GEntityPlayer* pTarPlayer)
{
	VALID(pReqPlayer);
	VALID(pTarPlayer);

	pReqPlayer->GetDoing().EndDoing();
	pTarPlayer->GetPlayerTrade().TradeRequested(pReqPlayer->GetUID());
}

void GTradeRequester::Route(GEntityPlayer* pReqPlayer, GEntityPlayer* pTarPlayer)
{
	VALID(pReqPlayer);
	VALID(pTarPlayer);

	MCommand* pNewCmd = MakeNewCommand(MC_TRADE_START_REQ_RELAY,
		1, 
		NEW_UID(pReqPlayer->GetUID()));

	pTarPlayer->RouteToMe(pNewCmd);
}