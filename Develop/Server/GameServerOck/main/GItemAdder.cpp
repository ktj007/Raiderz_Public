#include "StdAfx.h"
#include "GItemAdder.h"
#include "GGlobal.h"
#include "GPlayerObjectManager.h"
#include "GItemData.h"
#include "GItemManager.h"
#include "GEntityPlayer.h"
#include "GDBManager.h"
#include "GCommand.h"
#include "GQuestSystem.h"
#include "GInteractionSystem.h"
#include "GPlayerNPCIconSender.h"
#include "CSConditionInfo.h"
#include "GSystem.h"
#include "GItemHolder.h"
#include "CCommandResultTable.h"
#include "CCommandTable.h"
#include "GDBCacheSystem.h"
#include "GDBTaskItemData.h"
#include "GQObjUpdater.h"


bool GItemAdder::Add(const MUID& uidPlayer, int nItemID, int nAddAmount)
{
	GEntityPlayer* pPlayer = gmgr.pPlayerObjectManager->GetEntity(uidPlayer);
	if (NULL == pPlayer) return false;

	return Add(pPlayer, nItemID, nAddAmount);
}

bool GItemAdder::Add(GEntityPlayer* pPlayer, int nItemID, int nAddAmount)
{
	GItemData* pItemData = gmgr.pItemManager->GetItemData(nItemID);
	if (NULL == pItemData) return false;

	int nRemainAmount = nAddAmount;

	while (0 < nRemainAmount)
	{
		int nSingleAddAmount = nRemainAmount;
		if (pItemData->m_nStackAmount <= nRemainAmount)
		{
			nSingleAddAmount = pItemData->m_nStackAmount;
		}

		SingleAdd(pPlayer, pItemData, nSingleAddAmount);
		nRemainAmount -= nSingleAddAmount;
	}

	return true;
}

bool GItemAdder::SingleAdd(GEntityPlayer* pPlayer, GItemData* pItemData, int nAddAmount)
{
	if (NULL == pPlayer) return false;
	if (NULL == pItemData) return false;
	GItemHolder* pItemHolder = pPlayer->GetItemHolder();
	if (NULL == pItemHolder) return false;

	if (false == CheckItemAddable(pPlayer, CHECK_ADDABLE_ITEM(pItemData->m_nID, nAddAmount))) return false;

	GItem* pItem = pItemHolder->GetMergeableItem(SLOTTYPE_INVENTORY, pItemData->m_nID, nAddAmount);
	if (NULL == pItem)
	{
		return DB_Insert(pPlayer, pItemData->m_nID, nAddAmount, pItemData->m_nMaxDurability, pItemData->m_nTier);
	}
	else
	{
		return DB_Increase(pPlayer, pItem, nAddAmount);
	}

	return false;
}

bool GItemAdder::AddForDBTask(const MUID& nPlayerUID, const ADD_ITEM& addItem, bool bRoute)
{
	vector<ADD_ITEM> vecAddItem;
	vecAddItem.push_back(addItem);

	return AddForDBTask(nPlayerUID, vecAddItem, bRoute);
}

bool GItemAdder::AddForDBTask(const MUID& nPlayerUID, vector<ADD_ITEM>& vecAddItem, bool bRoute)
{
	GEntityPlayer* pPlayer = gmgr.pPlayerObjectManager->GetEntity(nPlayerUID);
	if (NULL == pPlayer) return false;
	GItemHolder* pItemHolder = pPlayer->GetItemHolder();
	if (NULL == pItemHolder) return false;

	if (true == bRoute)
	{
		Route(pPlayer, vecAddItem);
	}	

	for each (const ADD_ITEM& addItem in vecAddItem)
	{
		if (addItem.m_nStackModAmt == addItem.m_nToStackAmt)
		{
			Server_Insert(pPlayer, addItem.m_nSlotType, addItem.m_nSlotID, addItem.m_nStackModAmt,
				addItem.m_nIUID, addItem.m_nItemID, addItem.m_nAddExpiMin, addItem.m_nUsagePeriod, addItem.m_nDurability, addItem.m_nColor);			
		}
		else
		{
			Server_Increase(pPlayer, addItem.m_nSlotType, addItem.m_nSlotID, addItem.m_nStackModAmt);
		}
	}	

	pPlayer->GetNPCIconSender().SendByPlayerConditionChange(CT_ITEM);

	return true;
}

bool GItemAdder::CheckItemAddable(GEntityPlayer* pPlayer, const CHECK_ADDABLE_ITEM& checkAddableItem)
{
	VALID_RET(pPlayer, false);

	vector<CHECK_ADDABLE_ITEM> vecCheckAddableItem;
	vecCheckAddableItem.push_back(checkAddableItem);

	return CheckItemAddable(pPlayer, vecCheckAddableItem);
}

bool GItemAdder::CheckItemAddable(GEntityPlayer* pPlayer, const vector<CHECK_ADDABLE_ITEM>& vecCheckAddableItem)
{
	VALID_RET(pPlayer, false);

	if (false == CheckInvenSpace(pPlayer, vecCheckAddableItem)) return pPlayer->FailAndRouteAdviceMsg(CR_FAIL_SYSTEM_FEW_INVENTORY);
	if (false == CheckUnique(pPlayer, vecCheckAddableItem)) return pPlayer->FailAndRouteAdviceMsg(CR_FAIL_SYSTEM_CANT_HAVE_TWO_UNIQUE_ITEM);

	for each (const CHECK_ADDABLE_ITEM& checkAddableItem in vecCheckAddableItem)
	{
		if (false == CheckAmount(pPlayer, checkAddableItem)) return pPlayer->FailAndRouteAdviceMsg(CR_FAIL_ITEM_NOT_ADD_OVER_MAXSTACK);
	}

	return true;
}

bool GItemAdder::CheckInvenSpace( GEntityPlayer* pPlayer, const vector<CHECK_ADDABLE_ITEM>& vecCheckAddableItem)
{
	if (static_cast<int>(vecCheckAddableItem.size()) <= pPlayer->GetItemHolder()->GetEmptySlotCount(SLOTTYPE_INVENTORY)) return true;

	return pPlayer->GetItemHolder()->HasItemAddableSlot(SLOTTYPE_INVENTORY, vecCheckAddableItem);
}

bool GItemAdder::CheckAmount( GEntityPlayer* pPlayer, const CHECK_ADDABLE_ITEM& checkAddableItem )
{
	GItemData* pItemData = gmgr.pItemManager->GetItemData(checkAddableItem.m_nItemID);
	if (NULL == pItemData) return false;

	if (0 >= checkAddableItem.m_nAmount) return false;
	if (pItemData->m_nStackAmount < checkAddableItem.m_nAmount)	return false;

	return true;
}

bool GItemAdder::CheckUnique(GEntityPlayer* pPlayer, const vector<CHECK_ADDABLE_ITEM>& vecCheckAddableItem)
{
	set<int> setItemID;
	for each (const CHECK_ADDABLE_ITEM& checkAddableItem in vecCheckAddableItem)
	{
		GItemData* pItemData = gmgr.pItemManager->GetItemData(checkAddableItem.m_nItemID);
		if (NULL == pItemData) return false;

		if (pItemData->m_bUnique)
		{
			if (setItemID.end() != setItemID.find(checkAddableItem.m_nItemID))
			{
				return false;
			}
			else
			{
				setItemID.insert(checkAddableItem.m_nItemID);
			}
		}
	}

	for each (const CHECK_ADDABLE_ITEM& checkAddableItem in vecCheckAddableItem)
	{
		GItemData* pItemData = gmgr.pItemManager->GetItemData(checkAddableItem.m_nItemID);
		if (NULL == pItemData) return false;

		if (pItemData->m_bUnique)
		{
			if (1 < checkAddableItem.m_nAmount) return false;
			GItemHolder* pItemHolder = pPlayer->GetItemHolder();
			if (pItemHolder->IsExist(SLOTTYPE_EQUIP, pItemData->m_nID))	return false;
			if (pItemHolder->IsExist(SLOTTYPE_INVENTORY, pItemData->m_nID))	return false;
			if (pItemHolder->IsExist(SLOTTYPE_STORAGE, pItemData->m_nID))	return false;
		}
	}
	
	return true;
}

bool GItemAdder::DB_Insert( GEntityPlayer* pPlayer, int nItemID, int nInsertAmount, int nDurability, const uint8 nTier)
{
	GItemData* pItemData = gmgr.pItemManager->GetItemData(nItemID);
	if (NULL == pItemData)
		return false;

	GDBT_ITEM_DATA data(pPlayer->GetAID(), pPlayer->GetUID(), pPlayer->GetPlayerInfo()->GetDeltaPlayTime(), pPlayer->GetLevel(), pPlayer->GetMoney(), nInsertAmount, -1, nTier, 0);

	data.m_Item.Set(pPlayer->GetCID()
		, SLOTTYPE_INVENTORY
		, pPlayer->GetItemHolder()->ReserveEmptySlot(SLOTTYPE_INVENTORY, 0)
		, nItemID
		, nInsertAmount
		, pPlayer->GetPlayerInfo()->nPlayTimeSec
		, 0
		, 0
		, nDurability
		, nDurability
		, 0
		, pItemData->m_nTexColor
		, !pItemData->m_bTradable
		, false
		, 0
		, L"NULL");
		
	return gsys.pDBManager->ItemInsert(data);
}

bool GItemAdder::DB_Increase(GEntityPlayer* pPlayer, GItem* pItem, int nIncreaseAmount)
{
	GDBT_ITEM_DATA data(pPlayer->GetAID(), pPlayer->GetUID(), pPlayer->GetPlayerInfo()->GetDeltaPlayTime(), pPlayer->GetLevel(), pPlayer->GetMoney(), nIncreaseAmount, -1, pItem->m_pItemData->m_nTier, 0);

	data.m_Item.Set(pPlayer->GetCID()
		, pItem->m_nSlotType
		, pItem->m_nSlotID
		, pItem->m_pItemData->m_nID
		, pItem->GetAmount() + nIncreaseAmount
		, pPlayer->GetPlayerInfo()->nPlayTimeSec
		, pItem->m_nIUID
		, 0
		, pItem->m_nDurability
		, pItem->m_pItemData->m_nMaxDurability
		, pItem->GetEnchantCount()
		, pItem->m_pItemData->m_nTexColor
		, pItem->m_bBind
		, pItem->m_UsagePeriod.IsPeriod() || pItem->m_ExpirationPeriod.IsPeriod()
		, 0
		, pItem->m_strExpiDt);

	return gsys.pDBManager->ItemIncStackAmt(data);
}

GItem* GItemAdder::Server_Insert(GEntityPlayer* pPlayer, uint8 nSlotType, const int nSlotID, const int nInsertAmount, const int64 nIUID, const int nItemID, const int nAddExpiMin/*=-1*/, const int nUsagePeriod/*=0*/, const int nDurablity/*=-1*/, const int nColor/*=-1*/ )
{
	GItem* pItem = gmgr.pItemManager->MakeNewItem(nItemID, nInsertAmount);
	if (NULL == pItem)	return NULL;

	pItem->m_nIUID = nIUID;

	if (-1 != nColor)
	{
		pItem->m_nDyedColor = nColor;
	}

	if (-1 != nDurablity)
	{
		pItem->m_nDurability = nDurablity;
	}

	if (0 < nUsagePeriod)
		pItem->m_UsagePeriod.Set(true, true, (gsys.pSystem->GetNowTime() / 1000) + nUsagePeriod, pPlayer->GetPlayerInfo()->nPlayTimeSec);

	if (-1 < nAddExpiMin)
		pItem->m_ExpirationPeriod.Set(true, true, (gsys.pSystem->GetNowTime() / 1000) + nAddExpiMin * 60, pPlayer->GetPlayerInfo()->nPlayTimeSec);

	GItemHolder* pItemHolder = pPlayer->GetItemHolder();
	if (!pItemHolder->PushItem((SH_ITEM_SLOT_TYPE) nSlotType, nSlotID, pItem))
	{
		delete pItem;
		return NULL;
	}

	gsys.pDBCacheSystem->CreateItem(pPlayer->GetUID(), nSlotType, nSlotID, nIUID);

	return pItem;
}

void GItemAdder::Server_Increase( GEntityPlayer* pPlayer, uint8 nSlotType, const int nSlotID, int nIncreaseAmount )
{
	pPlayer->GetItemHolder()->IncreaseAmount((SH_ITEM_SLOT_TYPE) nSlotType, nSlotID, nIncreaseAmount);	
}

void GItemAdder::Route(GEntityPlayer* pPlayer, vector<ADD_ITEM>& vecAddItem)
{
	if (NULL == pPlayer) return;

	vector<TD_ITEM_INSERT>		vecTDItemInsert;
	vector<TD_ITEM_INCREASE>	vecTDItemIncrease;

	for (vector<ADD_ITEM>::iterator itor = vecAddItem.begin(); itor != vecAddItem.end(); itor++)  
	{
		if (itor->m_nStackModAmt == itor->m_nToStackAmt)
		{
			vecTDItemInsert.push_back(TD_ITEM_INSERT(itor->m_nItemID, itor->m_nSlotID, itor->m_nStackModAmt, itor->m_nDurability, itor->m_nColor, 0));
		}
		else
		{
			vecTDItemIncrease.push_back(TD_ITEM_INCREASE(itor->m_nSlotID, itor->m_nStackModAmt));
		}
	}

	MCommand* pNewCmd = MakeNewCommand(MC_ITEM_ADD, 
		2,
		NEW_BLOB(vecTDItemInsert),
		NEW_BLOB(vecTDItemIncrease));

	pPlayer->RouteToMe(pNewCmd);	
}

