#include "StdAfx.h"
#include "GNPCShopRepairer.h"
#include "GItemHolder.h"
#include "GEntityPlayer.h"
#include "GGlobal.h"
#include "GNPCShopInfoMgr.h"
#include "GNPCShopInfo.h"
#include "GItemActableChecker.h"
#include "CCommandResultTable.h"
#include "GDBManager.h"
#include "GMoneySystem.h"
#include "GItemDurabilityUpdater.h"
#include "CCommandTable.h"
#include "GCommand.h"
#include "CSNPCShopCalculator.h"
#include "GDBTaskNPCShopData.h"


bool GNPCShopRepairer::RepairOne(GEntityPlayer* pPlayer, int nNPCShopID, int8 slotType, int nSlotID)
{
	VALID_RET(pPlayer, false);
	GItemHolder* pItemHolder = pPlayer->GetItemHolder();
	VALID_RET(pItemHolder, false);
	GItem* pItem = pItemHolder->GetItem((SH_ITEM_SLOT_TYPE)slotType, nSlotID);
	VALID_RET(pItem, false);
	GNPCShopInfo* pNPCShopInfo = gmgr.pNPCShopInfoMgr->Get(nNPCShopID);
	VALID_RET(pNPCShopInfo, false);

	int nRepairPrice = 0;
	if (false == RepairOne_Check(pPlayer, pNPCShopInfo, pItem, nRepairPrice)) return false;

	GDBT_REPAIR_ITEM data(pPlayer->GetAID()
		, pPlayer->GetUID()
		, pPlayer->GetCID()
		, pItem->m_nSlotType
		, pItem->m_nSlotID
		, pItem->m_nIUID
		, pPlayer->GetPlayerInfo()->nPlayTimeSec
		, pPlayer->GetPlayerInfo()->GetDeltaPlayTime()
		, pPlayer->GetLevel()
		, pPlayer->GetPlayerInfo()->nXP
		, pPlayer->GetMoney() - nRepairPrice
		, nRepairPrice		
		);

	return gsys.pDBManager->NPCShop_Repair(data);
}

bool GNPCShopRepairer::RepairOne_Check(GEntityPlayer* pPlayer, GNPCShopInfo* pNPCShopInfo, GItem* pItem, int& outnRepairPrice)
{
	VALID_RET(pPlayer, false);
	VALID_RET(pNPCShopInfo, false);
	VALID_RET(pItem, false);
	
	if (false == CheckRepairableItem(pItem, pPlayer)) return false;
	if (false == CheckRepairableNPC(pNPCShopInfo)) return false;

	outnRepairPrice = CalcRepairPrice(pItem);
	if (false == CheckMoney(pPlayer, outnRepairPrice)) return false;

	return true;	
}

bool GNPCShopRepairer::RepairAll(GEntityPlayer* pPlayer, int nNPCShopID)
{
	VALID_RET(pPlayer, false);
	GNPCShopInfo* pNPCShopInfo = gmgr.pNPCShopInfoMgr->Get(nNPCShopID);
	VALID_RET(pNPCShopInfo, false);

	vector<GItem*> vecRepairableItem = CollectRepairableItem(pPlayer);
	if (true == vecRepairableItem.empty()) return true;

	int nRepairPrice = 0;
	if (false == RepairAll_Check(pPlayer, pNPCShopInfo, vecRepairableItem, nRepairPrice)) return false;

	if (1 == vecRepairableItem.size())
	{
		GDBT_REPAIR_ITEM data(pPlayer->GetAID()
			, pPlayer->GetUID()
			, pPlayer->GetCID()
			, vecRepairableItem[0]->m_nSlotType
			, vecRepairableItem[0]->m_nSlotID
			, vecRepairableItem[0]->m_nIUID
			, pPlayer->GetPlayerInfo()->nPlayTimeSec
			, pPlayer->GetPlayerInfo()->GetDeltaPlayTime()
			, pPlayer->GetLevel()
			, pPlayer->GetPlayerInfo()->nXP
			, pPlayer->GetMoney() - nRepairPrice
			, nRepairPrice		
			);

		return gsys.pDBManager->NPCShop_Repair(data);
	}
	else if (1 < vecRepairableItem.size())
	{
		GDBT_ITEM_REPAIR_VEC vRepair;
		vRepair.reserve(vecRepairableItem.size());
		for (size_t i = 0; i < vecRepairableItem.size(); ++i)
		{
			GDBT_ITEM_REPAIR_ALL rep;
			rep.m_nSlotType = vecRepairableItem[i]->m_nSlotType;
			rep.m_nSlotID	= vecRepairableItem[i]->m_nSlotID;
			rep.m_nIUID		= vecRepairableItem[i]->m_nIUID;

			vRepair.push_back(rep);
		}

		GDBT_REPAIR_ALL_ITEM data(pPlayer->GetAID()
			, pPlayer->GetUID()
			, pPlayer->GetCID()
			, pPlayer->GetMoney() - nRepairPrice
			, nRepairPrice
			, pPlayer->GetPlayerInfo()->nXP
			, pPlayer->GetPlayerInfo()->nPlayTimeSec
			, pPlayer->GetPlayerInfo()->GetDeltaPlayTime()
			, pPlayer->GetLevel()
			, vRepair);

		return gsys.pDBManager->NPCShop_RepairAll(data);
	}
	
	return true;
}

bool GNPCShopRepairer::RepairAllForGM(GEntityPlayer* pPlayer)
{
	VALID_RET(pPlayer, false);

	vector<GItem*> vecRepairableItem = CollectRepairableItem(pPlayer);
	if (true == vecRepairableItem.empty()) return true;

	GDBT_ITEM_REPAIR_VEC vRepair;
	vRepair.reserve(vecRepairableItem.size());
	for (size_t i = 0; i < vecRepairableItem.size(); ++i)
	{
		GDBT_ITEM_REPAIR_ALL rep;
		rep.m_nSlotType = vecRepairableItem[i]->m_nSlotType;
		rep.m_nSlotID	= vecRepairableItem[i]->m_nSlotID;
		rep.m_nIUID		= vecRepairableItem[i]->m_nIUID;
		
		vRepair.push_back(rep);
	}

	GDBT_REPAIR_ALL_ITEM data(pPlayer->GetAID()
		, pPlayer->GetUID()
		, pPlayer->GetCID()
		, pPlayer->GetMoney()
		, 0
		, pPlayer->GetPlayerInfo()->nXP
		, pPlayer->GetPlayerInfo()->nPlayTimeSec
		, pPlayer->GetPlayerInfo()->GetDeltaPlayTime()
		, pPlayer->GetLevel()
		, vRepair);

	return gsys.pDBManager->NPCShop_RepairAll(data);
}

bool GNPCShopRepairer::RepairAll_Check(GEntityPlayer* pPlayer, GNPCShopInfo* pNPCShopInfo, const vector<GItem*>& vecItem, int& outnRepairPrice)
{
	VALID_RET(pPlayer, false);
	VALID_RET(pNPCShopInfo, false);

	if (false == CheckRepairableNPC(pNPCShopInfo)) return false;

	outnRepairPrice = CalcRepairPrice(vecItem);
	if (false == CheckMoney(pPlayer, outnRepairPrice)) return false;

	return true;	
}

bool GNPCShopRepairer::CheckRepairableNPC( GNPCShopInfo* pNPCShopInfo )
{
	if (false == pNPCShopInfo->m_bRepairable) return false;

	return true;
}

bool GNPCShopRepairer::CheckRepairableItem(GItem* pItem, GEntityPlayer* pPlayer)
{
	if (0 == pItem->m_pItemData->m_nMaxDurability) return false;
	if (pItem->m_nDurability == pItem->m_pItemData->m_nMaxDurability) return false;
	if (false == pItem->m_pItemData->m_bRepairable) return false;
	if (false == gsys.pItemSystem->GetActableChecker().IsRepairable(pPlayer, pItem)) return false;

	return true;
}

bool GNPCShopRepairer::CheckMoney( GEntityPlayer* pPlayer, int nRepairPrice )
{
	if (nRepairPrice > pPlayer->GetPlayerInfo()->GetMoney()) return pPlayer->FailAndRouteSystemMsg(CR_FAIL_NPCSHOP_NOT_ENOUGH_MONEY);

	return true;
}

void GNPCShopRepairer::Route(GEntityPlayer* pPlayer, const vector<pair<int, int>>& vecSlotInfo)
{
	VALID(pPlayer);

	vector<TD_ITEM_SLOT_INFO> vecTDItemSlotInfo;	
	for each (const vector<pair<int, int>>::value_type& data in vecSlotInfo)
	{
		TD_ITEM_SLOT_INFO info;		
		info.nSlotType = data.first;
		info.nSlotID = data.second;

		vecTDItemSlotInfo.push_back(info);
	}

	MCommand* pNewCmd = MakeNewCommand(MC_NPCSHOP_REPAIR,
		1,
		NEW_BLOB(vecTDItemSlotInfo));

	pPlayer->RouteToMe(pNewCmd);	
}

int GNPCShopRepairer::CalcRepairPrice(GItem* pItem)
{
	VALID_RET(pItem, 0);

	return CSNPCShopCalculator::CalcRepairPrice(pItem->GetLostedDurability(), pItem->m_pItemData->m_nRepairGrade, pItem->GetEnchantCount());
}

int GNPCShopRepairer::CalcRepairPrice(vector<GItem*> vecItem)
{
	int nSumOfRepairPrice = 0; 

	for each (GItem* pItem in vecItem)
	{
		int nRepairPrice = CalcRepairPrice(pItem);
		if (INT_MAX - nRepairPrice < nSumOfRepairPrice) continue;

		nSumOfRepairPrice += nRepairPrice;
	}

	return nSumOfRepairPrice;
}

vector<GItem*> GNPCShopRepairer::CollectRepairableItem(GEntityPlayer* pPlayer)
{
	VALID_RET(pPlayer, vector<GItem*>());
	GItemHolder* pItemHolder = pPlayer->GetItemHolder();
	VALID_RET(pItemHolder, vector<GItem*>());

	vector<GItem*> vecItem;
	pItemHolder->CollectAllItem(SLOTTYPE_EQUIP, vecItem);
	pItemHolder->CollectAllItem(SLOTTYPE_INVENTORY, vecItem);

	vector<GItem*> vecRepairableItem;
	for each (GItem* pItem in vecItem)
	{
		if (false == CheckRepairableItem(pItem, pPlayer)) continue;

		vecRepairableItem.push_back(pItem);
	}

	return vecRepairableItem;
}