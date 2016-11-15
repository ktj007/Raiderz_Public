#include "StdAfx.h"
#include "GTradeCanceler.h"
#include "GEntityPlayer.h"
#include "GPlayerDoing.h"
#include "CCommandResultTable.h"
#include "CCommandTable.h"
#include "GCommand.h"
#include "GGlobal.h"
#include "GTradeSystem.h"
#include "GPlayerTrade.h"

bool GTradeCanceler::Cancel(GEntityPlayer* pReqPlayer)
{
	if (NULL == pReqPlayer) return false;

	if (false == pReqPlayer->GetDoing().IsNowTrading()) return pReqPlayer->FailAndRouteSystemMsg(CR_FAIL_TRADE_NOT_TRADING);
	GEntityPlayer* pTarPlayer = pReqPlayer->FindPlayer(pReqPlayer->GetPlayerTrade().GetTrader());
	if (NULL == pTarPlayer) return pReqPlayer->FailAndRouteSystemMsg(CR_FAIL_SYSTEM_INVALID_PC_UID);

	if (false == Apply(pReqPlayer, pTarPlayer)) return false;

	Route(pReqPlayer, pTarPlayer);

	return true;
}

bool GTradeCanceler::Apply(GEntityPlayer* pReqPlayer, GEntityPlayer* pTarPlayer)
{
	if (NULL == pReqPlayer) return false;
	if (NULL == pTarPlayer) return false;

	pReqPlayer->GetPlayerTrade().Reset();
	pTarPlayer->GetPlayerTrade().Reset();

	pReqPlayer->GetDoing().ChangeDoing(CD_NORMAL);
	pTarPlayer->GetDoing().ChangeDoing(CD_NORMAL);

	return true;
}

void GTradeCanceler::Route(GEntityPlayer* pReqPlayer, GEntityPlayer* pTarPlayer)
{
	if (NULL == pReqPlayer) return;
	if (NULL == pTarPlayer) return;

	MCommand* pNewCmd1 = MakeNewCommand(MC_TRADE_CANCEL, 0, NULL);
	pTarPlayer->RouteToMe(pNewCmd1);

	MCommand* pNewCmd2 = MakeNewCommand(MC_TRADE_CANCEL, 0, NULL);

	pReqPlayer->RouteToMe(pNewCmd2);
}
