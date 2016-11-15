#include "StdAfx.h"
#include "GNPCShopSeller.h"
#include "GGlobal.h"
#include "GNPCShopInfoMgr.h"
#include "GItemHolder.h"
#include "GEntityPlayer.h"
#include "CCommandResultTable.h"
#include "GPlayerFactions.h"
#include "GNPCShopInfo.h"
#include "CSFactionCalculator.h"
#include "CSNPCShopCalculator.h"
#include "GItemActableChecker.h"
#include "GDBManager.h"
#include "CCommandTable.h"
#include "GCommand.h"
#include "GDBTaskNPCShopData.h"
#include "GItemAdder.h"
#include "GInventory.h"

bool GNPCShopSeller::Sell( GEntityPlayer* pPlayer, int nNPCShopID, int nInvenSlotID, int nSellAmount )
{
	VALID_RET(pPlayer, false);
	GNPCShopInfo* pNPCShopInfo = gmgr.pNPCShopInfoMgr->Get(nNPCShopID);
	VALID_RET(pNPCShopInfo, false);
	GItemHolder* pItemHolder = pPlayer->GetItemHolder();
	VALID_RET(pItemHolder, false);
	GItem* pItem = pItemHolder->GetItem(SLOTTYPE_INVENTORY, nInvenSlotID);
	if (NULL == pItem)
		return false;
	
	int nSellPrice = 0;
	if (false == Sell_Check(pPlayer, pNPCShopInfo, pItem, nSellAmount, nSellPrice))
		return false;
	if (false == Sell_DB(pPlayer, pItem, nSellAmount, nSellPrice))
		return false;

	return true;
}

bool GNPCShopSeller::Sell_Check(GEntityPlayer* pPlayer, GNPCShopInfo* pNPCShopInfo, GItem* pItem, int nSellAmount, int& outnSumOfSellPrice)
{
	VALID_RET(pPlayer, false);
	VALID_RET(pNPCShopInfo, false);	
	VALID_RET(pItem, false);

	if (false == CheckSellabeItem(pItem, pPlayer)) return pPlayer->FailAndRouteSystemMsg(CR_FAIL_NPCSHOP_NOT_SELLABLE_ITEM);
	if (false == Sell_CheckItemAmount(pPlayer, pItem, nSellAmount)) return false;

	outnSumOfSellPrice = CalcSumOfSellPrice(pPlayer, pNPCShopInfo, pItem, nSellAmount);
	if (false == CheckMoney(pPlayer, outnSumOfSellPrice)) return pPlayer->FailAndRouteSystemMsg(CR_FAIL_SYSTEM_OVER_MONEY);

	return true;
}

bool GNPCShopSeller::Sell_CheckItemAmount( GEntityPlayer* pPlayer, GItem* pItem, int nSellAmount )
{
	VALID_RET(pPlayer, false);
	VALID_RET(pItem, false);

	if (0 >= nSellAmount) return false;
	if (pItem->GetAmount() < nSellAmount) return false;

	return true;
}

bool GNPCShopSeller::Sell_DB( GEntityPlayer* pPlayer, GItem* pItem, int nSellAmount, int nSumOfSellPrice )
{
	VALID_RET(pPlayer, false);
	VALID_RET(pItem, false);

	if (pItem->GetAmount() == nSellAmount)
	{
		return Sell_DBDelete(pPlayer, pItem, nSellAmount, nSumOfSellPrice);
	}
	else
	{
		return Sell_DBDecrease(pPlayer, pItem, nSellAmount, nSumOfSellPrice);
	}

	return false;
}

bool GNPCShopSeller::Sell_DBDelete(GEntityPlayer* pPlayer, GItem* pItem, int nSellAmount, int nSumOfSellPrice)
{
	VALID_RET(pPlayer, false);
	VALID_RET(pItem, false);

	_ASSERT(pItem->GetAmount() == nSellAmount);

	GDBT_NPC_SHOP_TRADE_DATA data(pPlayer->GetAID()
		, pPlayer->GetUID()
		, pPlayer->GetPlayerInfo()->GetDeltaPlayTime()
		, pPlayer->GetLevel()
		, pPlayer->GetPlayerInfo()->nXP
		, pPlayer->GetMoney() + nSumOfSellPrice
		, nSumOfSellPrice
		, nSellAmount
		, -1);

	data.m_Item.Build(pPlayer->GetCID(), pItem);
	data.m_Item.m_nStackAmt = 0;
	data.m_Item.m_nCharPtm = pPlayer->GetPlayerInfo()->nPlayTimeSec;

	return gsys.pDBManager->NPCShop_SellPartDelete(data);
}

bool GNPCShopSeller::Sell_DBDecrease(GEntityPlayer* pPlayer, GItem* pItem, int nSellAmount, int nSumOfSellPrice)
{
	VALID_RET(pPlayer, false);
	VALID_RET(pItem, false);

	GDBT_NPC_SHOP_TRADE_DATA data(pPlayer->GetAID()
		, pPlayer->GetUID()
		, pPlayer->GetPlayerInfo()->GetDeltaPlayTime()
		, pPlayer->GetLevel()
		, pPlayer->GetPlayerInfo()->nXP
		, pPlayer->GetMoney() + nSumOfSellPrice
		, nSumOfSellPrice
		, nSellAmount
		, -1);

	data.m_Item.Build(pPlayer->GetCID(), pItem);
	data.m_Item.m_nStackAmt -= nSellAmount;
	data.m_Item.m_nCharPtm = pPlayer->GetPlayerInfo()->nPlayTimeSec;

	return gsys.pDBManager->NPCShop_SellPartUpdate(data);
}

bool GNPCShopSeller::SellVeryCommon(GEntityPlayer* pPlayer, int nNPCShopID)
{
	VALID_RET(pPlayer, false);
	GNPCShopInfo* pNPCShopInfo = gmgr.pNPCShopInfoMgr->Get(nNPCShopID);
	VALID_RET(pNPCShopInfo, false);

	vector<GItem*> vecSellableVeryCommonItem = CollectSellableVeryCommonItem(pPlayer);
	if (true  == vecSellableVeryCommonItem.empty())
		return true;

	int nSumOfSellPrice = 0;
	if (false == SellVeryCommon_Check(pPlayer, pNPCShopInfo, vecSellableVeryCommonItem, nSumOfSellPrice))
		return false;

	if (false == SellVeryCommon_DB(pPlayer, vecSellableVeryCommonItem, nSumOfSellPrice))
		return false;

	return true;
}

bool GNPCShopSeller::SellVeryCommon_Check(GEntityPlayer* pPlayer, GNPCShopInfo* pNPCShopInfo, vector<GItem*> vecVeryCommonItem, int& outnSumOfSellPrice)
{
	VALID_RET(pPlayer, false);
	VALID_RET(pNPCShopInfo, false);

	outnSumOfSellPrice = CalcSumOfSellPrice(pPlayer, pNPCShopInfo, vecVeryCommonItem);

	if (false == CheckMoney(pPlayer, outnSumOfSellPrice)) return false;

	return true;
}

bool GNPCShopSeller::SellVeryCommon_DB( GEntityPlayer* pPlayer, vector<GItem*>& vecItem, int nSumOfSellPrice )
{
	GITEM_STACK_AMT_VEC v;
	v.reserve(vecItem.size());
	GITEM_STACK_AMT amt;
	for (size_t i = 0; i < vecItem.size(); ++i)
	{
		GItem* pItem = vecItem[i];

		amt.nIUID			= pItem->m_nIUID;
		amt.nSlotType		= pItem->m_nSlotType;
		amt.nSlotID			= pItem->m_nSlotID;
		amt.nItemID			= pItem->m_pItemData->m_nID;
		amt.nModStackAmt	= pItem->GetAmount();
		amt.nToStackAmt		= 0;

		v.push_back(amt);		
	}

	GDBT_SELL_ITEM_VERY_COMMON data(pPlayer->GetAID()
		, pPlayer->GetUID()
		, pPlayer->GetCID()
		, pPlayer->GetPlayerInfo()->GetDeltaPlayTime()
		, pPlayer->GetLevel()
		, pPlayer->GetPlayerInfo()->nXP
		, pPlayer->GetMoney() + nSumOfSellPrice
		, nSumOfSellPrice
		, pPlayer->GetPlayerInfo()->nPlayTimeSec
		, v);

	return gsys.pDBManager->NPCShop_SellVeryCommon(data);
}

bool GNPCShopSeller::CheckSellabeItem( GItem* pItem, GEntityPlayer* pPlayer )
{
	if (false == pItem->m_pItemData->m_bSellable) return false;
	if (false == gsys.pItemSystem->GetActableChecker().IsSellable(pPlayer, pItem)) return false;

	return true;
}

bool GNPCShopSeller::CheckMoney( GEntityPlayer* pPlayer, int nSumOfSellPrice )
{
	if (MAX_MONEY - nSumOfSellPrice < pPlayer->GetPlayerInfo()->GetMoney()) return false;

	return true;
}

vector<GItem*> GNPCShopSeller::CollectSellableVeryCommonItem( GEntityPlayer* pPlayer )
{
	VALID_RET(pPlayer, vector<GItem*>());

	GItemHolder* pItemHolder = pPlayer->GetItemHolder();	

	vector<GItem*> vecVeryCommonItem;
	pItemHolder->GetInventory().CollectVeryCommonItem(vecVeryCommonItem);

	vector<GItem*> vecSellableVeryCommonItem;
	for each (GItem* pItem in vecVeryCommonItem)
	{
		if (false ==CheckSellabeItem(pItem, pPlayer)) continue;

		vecSellableVeryCommonItem.push_back(pItem);
	}

	return vecSellableVeryCommonItem;
}

int GNPCShopSeller::CalcSumOfSellPrice(GEntityPlayer* pPlayer, GNPCShopInfo* pNPCShopInfo, GItem* pItem, int nSellAmount)
{
	VALID_RET(pPlayer, 0);
	VALID_RET(pNPCShopInfo, 0);
	VALID_RET(pItem, 0);

	FACTION_RELATION_TYPE nFRT = pPlayer->GetPlayerFactions().GetRelation(pNPCShopInfo->m_nFactionID);
	int nSellPrice = CSNPCShopCalculator::CalcSellPrice(pItem->m_pItemData->m_nBaseSellingPrice, pNPCShopInfo->m_fBaseSellMod, nFRT);
	if (INT_MAX / nSellAmount < nSellPrice)
	{
		return INT_MAX;
	}

	int nSumOfSellPrice = nSellPrice * nSellAmount;

	return nSumOfSellPrice;
}

int GNPCShopSeller::CalcSumOfSellPrice(GEntityPlayer* pPlayer, GNPCShopInfo* pNPCShopInfo, vector<GItem*> vecItem)
{
	VALID_RET(pPlayer, 0);
	VALID_RET(pNPCShopInfo, 0);

	int nSumOfSumOfSellPrice = 0;

	for each (GItem* pItem in vecItem)
	{
		int nSumOfSellPrice = CalcSumOfSellPrice(pPlayer, pNPCShopInfo, pItem, pItem->GetAmount());
		if (INT_MAX - nSumOfSellPrice < nSumOfSumOfSellPrice) return INT_MAX;

		nSumOfSumOfSellPrice += nSumOfSellPrice;
	}

	return nSumOfSumOfSellPrice;
}

void GNPCShopSeller::Sell_Route(GEntityPlayer* pPlayer)
{
	VALID(pPlayer);

	MCommand* pNewCmd = MakeNewCommand(MC_NPCSHOP_SELL, 0, NULL);

	pPlayer->RouteToMe(pNewCmd);
}

void GNPCShopSeller::SellVeryCommon_Route( GEntityPlayer* pPlayer )
{
	VALID(pPlayer);

	MCommand* pNewCmd = MakeNewCommand(MC_NPCSHOP_SELL_VERYCOMMON, 0, NULL);

	pPlayer->RouteToMe(pNewCmd);
}
