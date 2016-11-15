#include "StdAfx.h"
#include "GTradeItemPutDowner.h"
#include "GEntityPlayer.h"
#include "CCommandResultTable.h"
#include "GCommand.h"
#include "CCommandTable.h"
#include "GGlobal.h"
#include "GTradeSystem.h"
#include "GPlayerTrade.h"
#include "GTradeChecker.h"

bool GTradeItemPutDowner::PutDown(GEntityPlayer* pReqPlayer, GEntityPlayer* pTarPlayer, int nSlotID, int nAmount)
{
	VALID_RET(pReqPlayer, false);
	VALID_RET(pTarPlayer, false);

	if (false == Check(pReqPlayer, pTarPlayer, nSlotID, nAmount)) return false;
	
	Apply(pReqPlayer, pTarPlayer, nSlotID, nAmount);
	Route(pReqPlayer, pTarPlayer, nSlotID, nAmount);

	return true;
}

bool GTradeItemPutDowner::Check(GEntityPlayer* pReqPlayer, GEntityPlayer* pTarPlayer, int nSlotID, int nAmount)
{
	VALID_RET(pReqPlayer, false);
	VALID_RET(pTarPlayer, false);	

	if (false == gsys.pTradeSystem->GetChecker().CancelInvalidTrade(pReqPlayer, pTarPlayer)) return false;
	if (false == CheckItemAmount(pReqPlayer, nAmount, nSlotID)) return false;

	return true;
}

bool GTradeItemPutDowner::CheckItemAmount( GEntityPlayer* pReqPlayer, int nAmount, int nSlotID )
{
	VALID_RET(pReqPlayer, false);

	if (0 >= nAmount) return pReqPlayer->FailAndRouteSystemMsg(CR_FAIL_SYSTEM_NOT_PLUS_QUANTITY);
	int nTradeItemAmount = pReqPlayer->GetPlayerTrade().GetItemAmount(nSlotID);
	if (nAmount > nTradeItemAmount) return pReqPlayer->FailAndRouteSystemMsg(CR_FAIL_TRADE_PUDOWN_OVER_ITEM);

	return true;
}

void GTradeItemPutDowner::Apply(GEntityPlayer* pReqPlayer, GEntityPlayer* pTarPlayer, int nSlotID, int nAmount)
{
	VALID(pReqPlayer);
	VALID(pTarPlayer);

	pReqPlayer->GetPlayerTrade().RemoveItem(nSlotID, nAmount);

	pReqPlayer->GetPlayerTrade().Unconfirm();
	pTarPlayer->GetPlayerTrade().Unconfirm();
}

void GTradeItemPutDowner::Route(GEntityPlayer* pReqPlayer, GEntityPlayer* pTarPlayer, int nSlotID, int nAmount)
{
	VALID(pReqPlayer);
	VALID(pTarPlayer);

	TD_TRADE_ITEM_PUTDOWN tdItemPutDown = TD_TRADE_ITEM_PUTDOWN(nSlotID, nAmount);

	MCommand* pNewCmd1 = MakeNewCommand(MC_TRADE_PUTDOWN_ITEM, 
		2,
		NEW_UID(pReqPlayer->GetUID()),
		NEW_SINGLE_BLOB(tdItemPutDown));

	pTarPlayer->RouteToMe(pNewCmd1);


	MCommand* pNewCmd2 = MakeNewCommand(MC_TRADE_PUTDOWN_ITEM,
		2,
		NEW_UID(pReqPlayer->GetUID()),
		NEW_SINGLE_BLOB(tdItemPutDown));

	pReqPlayer->RouteToMe(pNewCmd2);
}