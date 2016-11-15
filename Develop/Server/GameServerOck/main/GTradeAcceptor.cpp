#include "StdAfx.h"
#include "GTradeAcceptor.h"
#include "CCommandResultTable.h"
#include "CCommandTable.h"
#include "GCommand.h"
#include "GEntityPlayer.h"
#include "GPlayerDoing.h"
#include "GGlobal.h"
#include "GPlayerTrade.h"
#include "GTradeSystem.h"
#include "GTradeChecker.h"


bool GTradeAcceptor::Accept(GEntityPlayer* pAcceptPlayer, GEntityPlayer* pTarPlayer, bool bAccept)
{
	VALID_RET(pAcceptPlayer, false);
	VALID_RET(pTarPlayer, false);

	if (false == Check(pAcceptPlayer, pTarPlayer, bAccept))
	{
		return pAcceptPlayer->FailAndRouteSystemMsg(CR_FAIL);
	}
	
	Apply(pAcceptPlayer, pTarPlayer, bAccept);
	Route(pAcceptPlayer, pTarPlayer, bAccept);

	return true;
}

bool GTradeAcceptor::Check(GEntityPlayer* pAcceptPlayer, GEntityPlayer* pTarPlayer, bool bAccept)
{
	VALID_RET(pAcceptPlayer, false);
	VALID_RET(pTarPlayer, false);

	if (true == bAccept)
	{
		if (true == pTarPlayer->GetDoing().IsBusy()) return false;
		if (false == pAcceptPlayer->GetDoing().IsEndableBusy()) return false;

		if (false == gsys.pTradeSystem->GetChecker().CheckDistance(pAcceptPlayer, pTarPlayer)) return false;
		if (false == gsys.pTradeSystem->GetChecker().CheckEnemy(pAcceptPlayer, pTarPlayer)) return false;
	}	

	return true;
}

void GTradeAcceptor::Apply(GEntityPlayer* pAcceptPlayer, GEntityPlayer* pTarPlayer, bool bAccept)
{
	VALID(pAcceptPlayer);
	VALID(pTarPlayer);

	if (true == bAccept)
	{
		pTarPlayer->GetPlayerTrade().TradeStart(pAcceptPlayer->GetUID());
		pAcceptPlayer->GetPlayerTrade().TradeStart(pTarPlayer->GetUID());		

		pTarPlayer->GetDoing().ChangeDoing(CD_TRADING);
		pAcceptPlayer->GetDoing().ChangeDoing(CD_TRADING);
	}
	else
	{
		pAcceptPlayer->GetPlayerTrade().TradeReject();
	}
}

void GTradeAcceptor::Route(GEntityPlayer* pAcceptPlayer, GEntityPlayer* pTarPlayer, bool bAccept)
{
	VALID(pAcceptPlayer);
	VALID(pTarPlayer);

	if (true == bAccept)
	{
		MCommand* pNewCmd1 = MakeNewCommand(MC_TRADE_START,
			2, 
			NEW_UID(pAcceptPlayer->GetUID()), 
			NEW_INT(pAcceptPlayer->GetPlayerInfo()->nLevel));

		pTarPlayer->RouteToMe(pNewCmd1);

		MCommand* pNewCmd2 = MakeNewCommand(MC_TRADE_START,
			2, 
			NEW_UID(pTarPlayer->GetUID()), 
			NEW_INT(pTarPlayer->GetPlayerInfo()->nLevel));

		pAcceptPlayer->RouteToMe(pNewCmd2);		
	}
	else
	{
		MCommand* pNewCmd = MakeNewCommand(MC_TRADE_START_REJECT, 0, NULL);

		pTarPlayer->RouteToMe(pNewCmd);
	}
}
