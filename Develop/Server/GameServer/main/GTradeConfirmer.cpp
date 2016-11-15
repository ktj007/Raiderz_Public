#include "StdAfx.h"
#include "GTradeConfirmer.h"
#include "GEntityPlayer.h"
#include "CCommandTable.h"
#include "GCommand.h"
#include "GGlobal.h"
#include "GTradeSystem.h"
#include "CCommandResultTable.h"
#include "GPlayerTrade.h"
#include "GTradeChecker.h"
#include "GItemHolder.h"

bool GTradeConfirmer::Confirm(GEntityPlayer* pReqPlayer, GEntityPlayer* pTarPlayer)
{
	VALID_RET(pReqPlayer, false);
	VALID_RET(pTarPlayer, false);

	if (false == Check(pReqPlayer, pTarPlayer)) return false;

	Apply(pReqPlayer);
	Route(pReqPlayer, pTarPlayer);

	return true;
}

bool GTradeConfirmer::Check(GEntityPlayer* pReqPlayer, GEntityPlayer* pTarPlayer)
{
	VALID_RET(pReqPlayer, false);
	VALID_RET(pTarPlayer, false);

	if (true == pReqPlayer->GetPlayerTrade().IsConfirm()) return false;
	if (false == gsys.pTradeSystem->GetChecker().CancelInvalidTrade(pReqPlayer, pTarPlayer)) return false;	

	return true;
}

void GTradeConfirmer::Apply(GEntityPlayer* pReqPlayer)
{
	pReqPlayer->GetPlayerTrade().Confirm();
}

void GTradeConfirmer::Route(GEntityPlayer* pReqPlayer, GEntityPlayer* pTarPlayer)
{
	MCommand* pNewCmd1 = MakeNewCommand(MC_TRADE_CONFIRM,
		1,
		NEW_UID(pReqPlayer->GetUID()));

	pTarPlayer->RouteToMe(pNewCmd1);

	MCommand* pNewCmd2 = MakeNewCommand(MC_TRADE_CONFIRM, 
		1,
		NEW_UID(pReqPlayer->GetUID()));

	pReqPlayer->RouteToMe(pNewCmd2);
}
