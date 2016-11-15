#include "StdAfx.h"
#include "GTradeSilverPutUper.h"
#include "GEntityPlayer.h"
#include "CCommandResultTable.h"
#include "GCommand.h"
#include "CCommandTable.h"
#include "GGlobal.h"
#include "GTradeSystem.h"
#include "GPlayerTrade.h"
#include "GTradeChecker.h"

bool GTradeMoneyPutUper::PutUp(GEntityPlayer* pReqPlayer, GEntityPlayer* pTarPlayer, int nMoney)
{
	VALID_RET(pReqPlayer, false);
	VALID_RET(pTarPlayer, false);

	if (false == Check(pReqPlayer, pTarPlayer, nMoney)) return false;

	Apply(pReqPlayer, pTarPlayer, nMoney);	
	Route(pReqPlayer, pTarPlayer, nMoney);

	return true;
}

bool GTradeMoneyPutUper::Check(GEntityPlayer* pReqPlayer, GEntityPlayer* pTarPlayer, int nMoney)
{
	VALID_RET(pReqPlayer, false);
	VALID_RET(pTarPlayer, false);

	if (false == gsys.pTradeSystem->GetChecker().CancelInvalidTrade(pReqPlayer, pTarPlayer)) return false;
	if (false == CheckMoney(pReqPlayer, pTarPlayer, nMoney)) return false;

	return true;
}

bool GTradeMoneyPutUper::CheckMoney( GEntityPlayer* pReqPlayer, GEntityPlayer* pTarPlayer, int nMoney )
{
	VALID_RET(pReqPlayer, false);

	if (0 > nMoney) return pReqPlayer->FailAndRouteSystemMsg(CR_FAIL_SYSTEM_NOT_PLUS_QUANTITY);
	if (nMoney > pReqPlayer->GetPlayerInfo()->GetMoney()) return pReqPlayer->FailAndRouteSystemMsg(CR_FAIL_TRADE_PUTUP_OVER_MONEY);
	if (MAX_MONEY - nMoney < pTarPlayer->GetMoney()) return pReqPlayer->FailAndRouteSystemMsg(CR_FAIL_TRADE_TARGET_CANT_TAKE_THISMONEY);

	return true;
}

void GTradeMoneyPutUper::Apply(GEntityPlayer* pReqPlayer, GEntityPlayer* pTarPlayer, int nMoney)
{
	VALID(pReqPlayer);
	VALID(pTarPlayer);

	pReqPlayer->GetPlayerTrade().SetMoney(nMoney);
	pReqPlayer->GetPlayerTrade().Unconfirm();
	pTarPlayer->GetPlayerTrade().Unconfirm();
}

void GTradeMoneyPutUper::Route(GEntityPlayer* pReqPlayer, GEntityPlayer* pTarPlayer, int nMoney)
{
	VALID(pReqPlayer);
	VALID(pTarPlayer);

	MCommand* pNewCmd1 = MakeNewCommand(MC_TRADE_PUTUP_MONEY, 
		2, 
		NEW_UID(pReqPlayer->GetUID()), 
		NEW_INT(nMoney));

	pTarPlayer->RouteToMe(pNewCmd1);

	MCommand* pNewCmd2 = MakeNewCommand(MC_TRADE_PUTUP_MONEY,
		2, 
		NEW_UID(pReqPlayer->GetUID()),
		NEW_INT(nMoney));

	pReqPlayer->RouteToMe(pNewCmd2);
}