#include "StdAfx.h"
#include "GNPCShopBuyer.h"
#include "GGlobal.h"
#include "GNPCShopInfoMgr.h"
#include "GEntityPlayer.h"
#include "CCommandResultTable.h"
#include "GItemManager.h"
#include "GConditionsInfo.h"
#include "GNPCShopInfo.h"
#include "GConditionsSystem.h"
#include "GItemAdder.h"
#include "GPlayerFactions.h"
#include "CSFactionCalculator.h"
#include "CSNPCShopCalculator.h"
#include "GDBManager.h"
#include "GItemHolder.h"
#include "CCommandTable.h"
#include "GCommand.h"
#include "GDBTaskNPCShopData.h"



bool GNPCShopBuyer::Buy(GEntityPlayer* pPlayer, int nNPCShopID, int nItemID, int nItemAmount)
{
	VALID_RET(pPlayer, false);	
	GNPCShopInfo* pNPCShopInfo = gmgr.pNPCShopInfoMgr->Get(nNPCShopID);
	VALID_RET(pNPCShopInfo, false);
	GItemData* pItemData = gmgr.pItemManager->GetItemData(nItemID);
	VALID_RET(pItemData, false);

	int nBuyPrice = 0;
	if (false == Check(pPlayer, pNPCShopInfo, pItemData, nItemAmount, nBuyPrice))
		return false;
	if (false == DB(pPlayer, pItemData, nItemAmount, nBuyPrice))
		return false;

	return true;
}

bool GNPCShopBuyer::Check(GEntityPlayer* pPlayer, GNPCShopInfo* pNPCShopInfo, GItemData* pItemData, int nBuyAmount, int& outnSumOfBuyPrice)
{
	VALID_RET(pPlayer, false);
	VALID_RET(pNPCShopInfo, false);
	VALID_RET(pItemData, false);

	if (false == CheckItemAmount(pPlayer, nBuyAmount, pItemData->m_nStackAmount)) return false;
	if (false == CheckShopCondition(pPlayer, pNPCShopInfo, pItemData)) return pPlayer->FailAndRouteSystemMsg(CR_FAIL_NPCSHOP_CONDITION_CHECK_FAIL);
	if (false == CheckItemAddable(pPlayer, pItemData, nBuyAmount)) return pPlayer->FailAndRouteSystemMsg(CR_FAIL_SYSTEM_CANT_HAVE_TWO_UNIQUE_ITEM);

	outnSumOfBuyPrice = CalcSumOfBuyPrice(pPlayer, pNPCShopInfo, pItemData, nBuyAmount);
	if (false == CheckMoney(pPlayer, outnSumOfBuyPrice)) return false;

	if (true == pPlayer->GetItemHolder()->IsFull(SLOTTYPE_INVENTORY)) return pPlayer->FailAndRouteSystemMsg(CR_FAIL_SYSTEM_FEW_INVENTORY);

	return true;
}

bool GNPCShopBuyer::CheckItemAmount(GEntityPlayer* pPlayer, int nBuyAmount, int nMaxStackAmount)
{
	VALID_RET(pPlayer, false);

	if (0 >= nBuyAmount) return false;
	if (nBuyAmount > nMaxStackAmount)	return false;

	return true;
}

bool GNPCShopBuyer::CheckShopCondition(GEntityPlayer* pPlayer, GNPCShopInfo* pNPCShopInfo, GItemData* pItemData)
{
	VALID_RET(pPlayer, false);
	VALID_RET(pNPCShopInfo, false);
	VALID_RET(pItemData, false);

	GConditionsInfo* pItemConditionsInfo = pNPCShopInfo->GetConditionsInfo(pItemData);
	if (NULL != pItemConditionsInfo)
	{
		if (false == gsys.pConditionsSystem->Check(pPlayer, pItemConditionsInfo)) return pPlayer->FailAndRouteSystemMsg(CR_FAIL_NPCSHOP_CONDITION_CHECK_FAIL);
	}

	return true;
}

bool GNPCShopBuyer::CheckItemAddable(GEntityPlayer* pPlayer, GItemData* pItemData, int nBuyAmount)
{
	VALID_RET(pPlayer, false);	
	VALID_RET(pItemData, false);

	return gsys.pItemSystem->GetAdder().CheckItemAddable(pPlayer, CHECK_ADDABLE_ITEM(pItemData->m_nID, nBuyAmount));
}

bool GNPCShopBuyer::CheckMoney(GEntityPlayer* pPlayer, int nSumOfBuyPrice)
{	
	VALID_RET(pPlayer, false);
	
	if (nSumOfBuyPrice > pPlayer->GetPlayerInfo()->GetMoney()) return false;

	return true;
}

int GNPCShopBuyer::CalcSumOfBuyPrice(GEntityPlayer* pPlayer, GNPCShopInfo* pNPCShopInfo, GItemData* pItemData, int nBuyAmount)
{
	VALID_RET(pPlayer, INT_MAX);
	VALID_RET(pNPCShopInfo, INT_MAX);
	VALID_RET(pItemData, INT_MAX);

	FACTION_RELATION_TYPE nFRT = pPlayer->GetPlayerFactions().GetRelation(pNPCShopInfo->m_nFactionID);
	int nBuyPrice = CSNPCShopCalculator::CalcBuyPrice(pItemData->m_nBaseBuyingPrice, pNPCShopInfo->m_fBaseBuyMod, nFRT);
	if (INT_MAX / nBuyAmount < nBuyPrice)
	{
		return INT_MAX;
	}

	int nSumOfBuyPrice = nBuyPrice * nBuyAmount;

	return nSumOfBuyPrice;
}

bool GNPCShopBuyer::DB(GEntityPlayer* pPlayer, GItemData* pItemData, int nBuyAmount, int nSumOfBuyPrice)
{
	VALID_RET(pPlayer, false);
	VALID_RET(pItemData, false);

	int nRemainMoney = pPlayer->GetMoney() - nSumOfBuyPrice;

	if (nBuyAmount == pItemData->m_nStackAmount)
	{
		return DB_Insert(pPlayer, pItemData, nBuyAmount, nRemainMoney, nSumOfBuyPrice);
	}
	else
	{
		GItemHolder* pItemHolder = pPlayer->GetItemHolder();
		GItem* pMergeableItem = pItemHolder->GetMergeableItem(SLOTTYPE_INVENTORY, pItemData->m_nID, nBuyAmount);
		if (NULL == pMergeableItem)
		{
			return DB_Insert(pPlayer, pItemData, nBuyAmount, nRemainMoney, nSumOfBuyPrice);
		}
		else
		{
			return DB_Increase(pPlayer, pMergeableItem, nBuyAmount,nRemainMoney, nSumOfBuyPrice);
		}
	}

	return false;
}

bool GNPCShopBuyer::DB_Insert(GEntityPlayer* pPlayer, GItemData* pItemData, int nBuyAmount, int nRemainMoney, int nSumOfBuyPrice)
{
	VALID_RET(pPlayer, false);
	VALID_RET(pItemData, false);

	GDBT_NPC_SHOP_TRADE_DATA data(pPlayer->GetAID()
		, pPlayer->GetUID()
		, pPlayer->GetPlayerInfo()->GetDeltaPlayTime()
		, pPlayer->GetLevel()
		, pPlayer->GetPlayerInfo()->nXP
		, nRemainMoney
		, nSumOfBuyPrice
		, nBuyAmount
		, -1);

	data.m_Item.Set(pPlayer->GetCID()
		, SLOTTYPE_INVENTORY
		, pPlayer->GetItemHolder()->ReserveEmptySlot(SLOTTYPE_INVENTORY, 0)
		, pItemData->m_nID
		, nBuyAmount
		, pPlayer->GetPlayerInfo()->nPlayTimeSec
		, 0
		, 0
		, pItemData->m_nMaxDurability
		, pItemData->m_nMaxDurability
		, 0
		, pItemData->m_nTexColor
		, false);

	return gsys.pDBManager->NPCShop_BuyInsert(data);
}

bool GNPCShopBuyer::DB_Increase(GEntityPlayer* pPlayer, GItem* pItem, int nBuyAmount, int nRemainMoney, int nSumOfBuyPrice)
{
	VALID_RET(pPlayer, false);
	VALID_RET(pItem, false);

	GDBT_NPC_SHOP_TRADE_DATA data(pPlayer->GetAID()
		, pPlayer->GetUID()
		, pPlayer->GetPlayerInfo()->GetDeltaPlayTime()
		, pPlayer->GetLevel()
		, pPlayer->GetPlayerInfo()->nXP
		, nRemainMoney
		, nSumOfBuyPrice
		, nBuyAmount
		, -1);

	data.m_Item.Set(pPlayer->GetCID()
		, pItem->m_nSlotType
		, pItem->m_nSlotID
		, pItem->m_pItemData->m_nID
		, pItem->GetAmount() + nBuyAmount
		, pPlayer->GetPlayerInfo()->nPlayTimeSec
		, pItem->m_nIUID
		, 0
		, pItem->m_nDurability
		, pItem->m_pItemData->m_nMaxDurability
		, pItem->GetEnchantCount()
		, pItem->m_nDyedColor
		, pItem->m_bBind
		, pItem->m_UsagePeriod.IsPeriod() || pItem->m_ExpirationPeriod.IsPeriod()
		, 0
		, pItem->m_strExpiDt);

	return gsys.pDBManager->NPCShop_BuyUpdate(data);
}

void GNPCShopBuyer::Route(GEntityPlayer* pPlayer)
{
	VALID(pPlayer);

	MCommand* pNewCmd = MakeNewCommand(MC_NPCSHOP_BUY, 0, NULL);
	pPlayer->RouteToMe(pNewCmd);
}