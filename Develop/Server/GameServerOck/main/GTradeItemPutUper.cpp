#include "StdAfx.h"
#include "GTradeItemPutUper.h"
#include "CCommandResultTable.h"
#include "CCommandTable.h"
#include "GCommand.h"
#include "GEntityPlayer.h"
#include "GGlobal.h"
#include "GTradeSystem.h"
#include "GItemActableChecker.h"
#include "GItemHolder.h"
#include "GPlayerTrade.h"
#include "GTradeChecker.h"
#include "GItemAdder.h"

bool GTradeItemPutUper::PutUp(GEntityPlayer* pReqPlayer, GEntityPlayer* pTarPlayer, int nSlotID, int nAmount)
{
	VALID_RET(pReqPlayer, false);
	VALID_RET(pTarPlayer, false);

	GItemHolder* pReqItemHolder = pReqPlayer->GetItemHolder();
	GItem* pItem = pReqItemHolder->GetItem(SLOTTYPE_INVENTORY, nSlotID);
	if (NULL == pItem) return false;

	if (false == Check(pReqPlayer, pTarPlayer, pItem, nAmount)) return false;
	
	Apply(pReqPlayer, pTarPlayer, pItem, nAmount);
	Route(pReqPlayer, pTarPlayer, pItem, nAmount);

	return true;
}

bool GTradeItemPutUper::Check(GEntityPlayer* pReqPlayer, GEntityPlayer* pTarPlayer, GItem* pItem, int nAmount)
{	
	VALID_RET(pReqPlayer, false);
	VALID_RET(pTarPlayer, false);
	VALID_RET(pItem, false);

	if (false == gsys.pTradeSystem->GetChecker().CancelInvalidTrade(pReqPlayer, pTarPlayer)) return false;
	if (false == CheckItemAmount(pReqPlayer, pItem, nAmount)) return false;
	if (false == CheckTradeSpace(pReqPlayer)) return pReqPlayer->FailAndRouteSystemMsg(CR_FAIL_TRADE_OVER_MAX_TRADE_SAPCE);
	if (false == CheckTradableItem(pReqPlayer, pItem)) return pReqPlayer->FailAndRouteSystemMsg(CR_FAIL_TRADE_PUTUP_NOT_TRADABLE_ITEM);
	/// 인벤토리가 꽉찼는지 검사. - 거래 아이템 취득시 스택되지 않음.
	if (true == pTarPlayer->GetItemHolder()->IsFull(SLOTTYPE_INVENTORY)) return pReqPlayer->FailAndRouteSystemMsg(CR_FAIL_TRADE_TARGET_CANT_TAKE_THISITEM);
	if (false == CheckItemAddable(pReqPlayer, pTarPlayer, pItem->m_pItemData->m_nID, nAmount)) return pReqPlayer->FailAndRouteSystemMsg(CR_FAIL_TRADE_TARGET_CANT_TAKE_THISITEM);
	
	return true;
}

bool GTradeItemPutUper::CheckItemAmount(GEntityPlayer* pReqPlayer, GItem* pItem, int nAmount)
{
	if (0 >= nAmount) return pReqPlayer->FailAndRouteSystemMsg(CR_FAIL_SYSTEM_NOT_PLUS_QUANTITY);
	int nTradeItemAmount = pReqPlayer->GetPlayerTrade().GetItemAmount(pItem->m_nSlotID);	
	if (nAmount > pItem->GetAmount() - nTradeItemAmount)	return pReqPlayer->FailAndRouteSystemMsg(CR_FAIL_TRADE_PUTUP_OVER_ITEM);

	return true;
}

bool GTradeItemPutUper::CheckTradeSpace(GEntityPlayer* pReqPlayer)
{
	VALID_RET(pReqPlayer, false);

	if (true == pReqPlayer->GetPlayerTrade().IsItemFull()) return false;

	return true;
}

bool GTradeItemPutUper::CheckTradableItem(GEntityPlayer* pReqPlayer, GItem* pItem)
{
	VALID_RET(pReqPlayer, false);
	VALID_RET(pItem, false);

	if (true == pItem->m_bBind) return false;
	if (false == gsys.pItemSystem->GetActableChecker().IsTradable(pReqPlayer, pItem)) return false;

	return true;
}

bool GTradeItemPutUper::CheckItemAddable(GEntityPlayer* pReqPlayer, GEntityPlayer* pTarPlayer, int nItemID, int nAmount)
{
	VALID_RET(pReqPlayer, false);
	VALID_RET(pTarPlayer, false);

	vector<GTradeItem*> vecTradeItem = pReqPlayer->GetPlayerTrade().GetAllItem();

	vector<CHECK_ADDABLE_ITEM> vecCheckAddableItem;
	for each (GTradeItem* pTradeItem in vecTradeItem)
	{
		vecCheckAddableItem.push_back(CHECK_ADDABLE_ITEM(pTradeItem->GetID(), pTradeItem->GetAmount()));
	}
	vecCheckAddableItem.push_back(CHECK_ADDABLE_ITEM(nItemID, nAmount));

	if (false == gsys.pItemSystem->GetAdder().CheckItemAddable(pTarPlayer, vecCheckAddableItem)) return false;

	return true;
}

void GTradeItemPutUper::Apply(GEntityPlayer* pReqPlayer, GEntityPlayer* pTarPlayer, GItem* pItem, int nAmount)
{
	VALID(pReqPlayer);
	VALID(pTarPlayer);
	VALID(pItem);
	
	pReqPlayer->GetPlayerTrade().AddItem(pItem->m_nSlotID, nAmount, pItem->m_pItemData->m_nID);

	pReqPlayer->GetPlayerTrade().Unconfirm();
	pTarPlayer->GetPlayerTrade().Unconfirm();
}

void GTradeItemPutUper::Route(GEntityPlayer* pReqPlayer, GEntityPlayer* pTarPlayer, GItem* pItem, int nAmount)
{
	VALID(pReqPlayer);
	VALID(pTarPlayer);
	VALID(pItem);

	TD_TRADE_ITEM_PUTUP tdItemPutUp = TD_TRADE_ITEM_PUTUP(pItem->m_nSlotID, pItem->m_pItemData->m_nID, nAmount, pItem->m_nDurability, pItem->m_nDyedColor);

	MCommand* pNewCmd1 = MakeNewCommand(MC_TRADE_PUTUP_ITEM,
		2,
		NEW_UID(pReqPlayer->GetUID()),
		NEW_SINGLE_BLOB(tdItemPutUp));

	pTarPlayer->RouteToMe(pNewCmd1);

	MCommand* pNewCmd2 = MakeNewCommand(MC_TRADE_PUTUP_ITEM,
		2,
		NEW_UID(pReqPlayer->GetUID()),
		NEW_SINGLE_BLOB(tdItemPutUp));

	pReqPlayer->RouteToMe(pNewCmd2);
}