#include "StdAfx.h"
#include "GItemSorter.h"
#include "GItem.h"
#include "GEntityPlayer.h"
#include "GItemHolder.h"
#include "GDBTaskDataItemSort.h"
#include "AStlUtil.h"
#include "GGlobal.h"
#include "GPlayerObjectManager.h"
#include "CCommandTable.h"
#include "GCommand.h"
#include "GDBManager.h"
#include "GItemRemover.h"

bool GItemSorter::Sort(GEntityPlayer* pPlayer)
{
	VALID_RET(pPlayer, false);

	vector<GItem*> vecAllItem;
	pPlayer->GetItemHolder()->CollectAllItem(SLOTTYPE_INVENTORY, vecAllItem);

	vector<GItem*> vecSortedAllItem = GetSortedAllItem(vecAllItem);

	GDBT_ITEM_SORT data;
	if (false == MakeDBTaskData(pPlayer, SLOTTYPE_INVENTORY, vecSortedAllItem, 0, data)) return false;

	// SortForDBTask(data);
	return gsys.pDBManager->ItemSort(data);
}

bool GItemSorter::SortByTD(GEntityPlayer* pPlayer, SH_ITEM_SLOT_TYPE nSlotType, const vector<TD_ITEM_INVENTORY_SORT>& vectdItemSort)
{
	VALID_RET(pPlayer, false);

	if (vectdItemSort.empty())
		return true;

	if (!SortByTD_Check(pPlayer, nSlotType, vectdItemSort))
		return false;

	int nStartSlotIndex = INT_MAX;
	vector<GDBT_ITEM_SORT_SLOT> vecDBSortSlot;

	for (const TD_ITEM_INVENTORY_SORT& item : vectdItemSort)
	{
		if (nStartSlotIndex > item.m_nToSlotID)
			nStartSlotIndex = item.m_nToSlotID;

		vecDBSortSlot.push_back(GDBT_ITEM_SORT_SLOT(item.m_nFromSlotID, item.m_nToSlotID));
	}

	// TODO: nStartSlotIndex validation...

	GDBT_ITEM_SORT data(
		pPlayer->GetUID(), 
		pPlayer->GetCID(), 
		nSlotType, 
		vecDBSortSlot, 
		nStartSlotIndex, 
		MakeDBTaskData_MergeAll(pPlayer, nSlotType, vecDBSortSlot));

	return gsys.pDBManager->ItemSort(data);
}

bool GItemSorter::SortByTD_Check(GEntityPlayer* pPlayer, SH_ITEM_SLOT_TYPE nSlotType, const vector<TD_ITEM_INVENTORY_SORT>& vectdItemSort) const
{
	VALID_RET(pPlayer, false);
	VALID_RET(nSlotType == SLOTTYPE_INVENTORY || nSlotType == SLOTTYPE_STORAGE, false);

	// SLOTTYPE_INVENTORY
	int nStartSlot = 0;
	int nEndSlot = pPlayer->GetItemHolder()->GetSlotSize(nSlotType);

	// TODO: vectdItemSort.size() validation... (would always be max size of inventory to be sorted, i.e. 80)
	if (vectdItemSort.size() > (size_t)nEndSlot)
		return false;

	set<int> setFromSlot;
	set<int> setToSlot;

	for (const TD_ITEM_INVENTORY_SORT& item : vectdItemSort)
	{
		if ((item.m_nFromSlotID < 0 || item.m_nFromSlotID >= nEndSlot) ||
			(item.m_nToSlotID < 0 || item.m_nToSlotID >= nEndSlot))
			return false;

		if (setFromSlot.find(item.m_nFromSlotID) != setFromSlot.end() ||
			setToSlot.find(item.m_nToSlotID) != setToSlot.end())
			return false;

		setFromSlot.insert(item.m_nFromSlotID);
		setToSlot.insert(item.m_nToSlotID);
	}

	return true;
}

vector<GItem*> GItemSorter::GetSortedAllItem(const vector<GItem*>& vecAllItem)
{
	set<GItem*> setAllItem = VectorToSet(vecAllItem);
	vector<GItem*> vecEquipItem = SelectEquipItemForSort(setAllItem);
	vector<GItem*> vecUsableItem = SelectUsableItemForSort(setAllItem);
	vector<GItem*> vecQuestItem = SelectQuestItemForSort(setAllItem);
	vector<GItem*> vecMaterialItem = SelectMaterialItemForSort(setAllItem);
	vector<GItem*> vecEtcItem = SetToVector(setAllItem);

	vector<GItem*> vecSortedItem;
	vecSortedItem.insert(vecSortedItem.end(), vecEquipItem.begin(), vecEquipItem.end());
	vecSortedItem.insert(vecSortedItem.end(), vecUsableItem.begin(), vecUsableItem.end());
	vecSortedItem.insert(vecSortedItem.end(), vecQuestItem.begin(), vecQuestItem.end());
	vecSortedItem.insert(vecSortedItem.end(), vecMaterialItem.begin(), vecMaterialItem.end());
	vecSortedItem.insert(vecSortedItem.end(), vecEtcItem.begin(), vecEtcItem.end());

	return vecSortedItem;
}

vector<GItem*> GItemSorter::SelectEquipItemForSort(set<GItem*>& setItem)
{
	map<int, vector<GItem*>> mapSelectItem;
	for each (GItem* pItem in setItem)
	{
		if (ITEMTYPE_WEAPON != pItem->m_pItemData->m_ItemType &&
			ITEMTYPE_ARMOR != pItem->m_pItemData->m_ItemType) continue;

		MapSecondVectorInserter(mapSelectItem, pItem->m_pItemData->m_nID, pItem);
	}

	vector<GItem*> vecSelectItem;
	for each (const map<int, vector<GItem*>>::value_type& data in mapSelectItem)
	{
		const vector<GItem*> vecItem = data.second;
		for each(GItem* pItem in vecItem)
		{
			setItem.erase(pItem);
			vecSelectItem.push_back(pItem);
		}		
	}

	return vecSelectItem;
}

vector<GItem*> GItemSorter::SelectUsableItemForSort(set<GItem*>& setItem)
{
	map<int, vector<GItem*>> mapSelectItem;
	for each (GItem* pItem in setItem)
	{
		if (ITEMTYPE_USABLE != pItem->m_pItemData->m_ItemType) continue;

		MapSecondVectorInserter(mapSelectItem, pItem->m_pItemData->m_nID, pItem);
	}

	vector<GItem*> vecSelectItem;
	for each (const map<int, vector<GItem*>>::value_type& data in mapSelectItem)
	{
		const vector<GItem*> vecItem = data.second;
		for each(GItem* pItem in vecItem)
		{
			setItem.erase(pItem);
			vecSelectItem.push_back(pItem);
		}		
	}

	return vecSelectItem;
}

vector<GItem*> GItemSorter::SelectQuestItemForSort(set<GItem*>& setItem)
{
	map<int, vector<GItem*>> mapSelectItem;
	for each (GItem* pItem in setItem)
	{
		if (false == pItem->m_pItemData->m_bQuestRelated) continue;

		MapSecondVectorInserter(mapSelectItem, pItem->m_pItemData->m_nID, pItem);
	}

	vector<GItem*> vecSelectItem;
	for each (const map<int, vector<GItem*>>::value_type& data in mapSelectItem)
	{
		const vector<GItem*> vecItem = data.second;
		for each(GItem* pItem in vecItem)
		{
			setItem.erase(pItem);
			vecSelectItem.push_back(pItem);
		}		
	}

	return vecSelectItem;
}

vector<GItem*> GItemSorter::SelectMaterialItemForSort(set<GItem*>& setItem)
{
	map<int, vector<GItem*>> mapSelectItem;
	for each (GItem* pItem in setItem)
	{
		if (false == pItem->m_pItemData->m_bRecipeMaterial) continue;

		MapSecondVectorInserter(mapSelectItem, pItem->m_pItemData->m_nID, pItem);
	}

	vector<GItem*> vecSelectItem;
	for each (const map<int, vector<GItem*>>::value_type& data in mapSelectItem)
	{
		const vector<GItem*> vecItem = data.second;
		for each(GItem* pItem in vecItem)
		{
			setItem.erase(pItem);
			vecSelectItem.push_back(pItem);
		}		
	}

	return vecSelectItem;
}

vector<GItem*> GItemSorter::SelectEtcItemForSort( set<GItem*>& setItem )
{
	map<int, vector<GItem*>> mapSelectItem;
	for each (GItem* pItem in setItem)
	{
		MapSecondVectorInserter(mapSelectItem, pItem->m_pItemData->m_nID, pItem);
	}

	vector<GItem*> vecSelectItem;
	for each (const map<int, vector<GItem*>>::value_type& data in mapSelectItem)
	{
		const vector<GItem*> vecItem = data.second;
		for each(GItem* pItem in vecItem)
		{
			setItem.erase(pItem);
			vecSelectItem.push_back(pItem);
		}		
	}

	return vecSelectItem;
}

bool GItemSorter::MakeDBTaskData(GEntityPlayer* pPlayer, SH_ITEM_SLOT_TYPE nSlotType, const vector<GItem*>& vecSortedAllItem, int nStartSlotIndex, GDBT_ITEM_SORT& data)
{
	VALID_RET(pPlayer, false);

	vector<GDBT_ITEM_SORT_SLOT> vecSortSlot;

	int nSortSlotID = nStartSlotIndex;
	for each (GItem* pItem in vecSortedAllItem)
	{
		if (pItem->m_nSlotID != nSortSlotID)
		{
			vecSortSlot.push_back(GDBT_ITEM_SORT_SLOT(pItem->m_nSlotID, nSortSlotID));
		}
		nSortSlotID++;
	}

	data = GDBT_ITEM_SORT(
		pPlayer->GetUID(), 
		pPlayer->GetCID(), 
		nSlotType, 
		vecSortSlot, 
		nStartSlotIndex, 
		MakeDBTaskData_MergeAll(pPlayer, nSlotType, vecSortSlot));

	return true;
}

vector<GDBT_ITEM_MERGE_ALL_SLOT> GItemSorter::MakeDBTaskData_MergeAll(GEntityPlayer* pPlayer, SH_ITEM_SLOT_TYPE nSlotType, const vector<GDBT_ITEM_SORT_SLOT>& vecSortSlot)
{
	VALID_RET(pPlayer, vector<GDBT_ITEM_MERGE_ALL_SLOT>());

	GItemHolder* pItemHolder = pPlayer->GetItemHolder();
	VALID_RET(pItemHolder, vector<GDBT_ITEM_MERGE_ALL_SLOT>());

	/* Collect All Items, with grouping by Item ID to find duplicated same items */
	map<int, vector<pair<const GDBT_ITEM_SORT_SLOT&, GItem*>>> mapAllItem; // <Item ID, array of same items (Slot ID, GItem*)>

	for (const GDBT_ITEM_SORT_SLOT& slot : vecSortSlot)
	{
		GItem* pItem = pItemHolder->GetItem(nSlotType, slot.m_nFromSlotID);
		if (!pItem) continue;

		mapAllItem[pItem->GetID()].push_back(pair<const GDBT_ITEM_SORT_SLOT&, GItem*>(slot, pItem));
	}

	/* Merge Stack Amount */
	vector<GDBT_ITEM_MERGE_ALL_SLOT> vecMergeSlot;

	for (const map<int, vector<pair<const GDBT_ITEM_SORT_SLOT&, GItem*>>>::value_type& dup : mapAllItem)
	{
		const vector<pair<const GDBT_ITEM_SORT_SLOT&, GItem*>>& vecItem = dup.second;

		if (vecItem.size() <= 1)
			continue;	// no same items on other slots - merging isn't needed. skip.

		int nTotalStackAmount = SumStackAmount(vecItem);
		for (const pair<const GDBT_ITEM_SORT_SLOT&, GItem*>& item : vecItem)
		{
			const GDBT_ITEM_SORT_SLOT& Slot = item.first;
			GItem* pItem = item.second;

			if (nTotalStackAmount <= 0)
			{
				vecMergeSlot.push_back(GDBT_ITEM_MERGE_ALL_SLOT(Slot, 0));
			}
			else if (nTotalStackAmount > pItem->m_pItemData->m_nStackAmount)
			{
				vecMergeSlot.push_back(GDBT_ITEM_MERGE_ALL_SLOT(Slot, pItem->m_pItemData->m_nStackAmount));
				nTotalStackAmount -= pItem->m_pItemData->m_nStackAmount;
			}
			else
			{
				vecMergeSlot.push_back(GDBT_ITEM_MERGE_ALL_SLOT(Slot, nTotalStackAmount));
				nTotalStackAmount = 0;
			}
		}
	}

	return vecMergeSlot;
}

int GItemSorter::SumStackAmount(const vector<pair<const GDBT_ITEM_SORT_SLOT&, GItem*>>& vecItem) const
{
	int nTotalStackAmount = 0;

	for (const pair<const GDBT_ITEM_SORT_SLOT&, GItem*>& item : vecItem)
		nTotalStackAmount += item.second->GetAmount();

	return nTotalStackAmount;
}

void GItemSorter::SortForDBTask(const GDBT_ITEM_SORT& data)
{
	GEntityPlayer* pPlayer = gmgr.pPlayerObjectManager->GetEntity(data.m_uidPlayer);
	if (NULL == pPlayer) return;
	GItemHolder* pItemHolder = pPlayer->GetItemHolder();
	if (NULL == pItemHolder) return;

	// 1st: Pop All Items and Sort
	map<int, GItem*> mapSortedItem;	// <To Slot ID, GItem Pointer>
	for (const GDBT_ITEM_SORT_SLOT& sortSlot : data.m_vecSortSlot)
	{
		GItem* pItem = pItemHolder->PopItem(data.m_nSlotType, sortSlot.m_nFromSlotID);
		mapSortedItem[sortSlot.m_nToSlotID] = pItem;
	}

	// 2nd: Push All Sorted Items
	for (const map<int, GItem*>::value_type& item : mapSortedItem)
	{
		int nToSlot = item.first;
		GItem* pItem = item.second;

		pItemHolder->PushItem(data.m_nSlotType, nToSlot, pItem);
	}

	// 3rd: Merge Stack Amounts
	for (const GDBT_ITEM_MERGE_ALL_SLOT& merge : data.m_vecMergeSlot)
	{
		if (merge.m_nStackAmount <= 0)
		{
			GItem* pItem = pItemHolder->GetItem(data.m_nSlotType, merge.m_Slot.m_nToSlotID);
			if (pItem)
			{
				gsys.pItemSystem->GetRemover().RemoveForDBTask(
					pPlayer->GetUID(),
					REMOVE_ITEM(data.m_nSlotType, merge.m_Slot.m_nToSlotID, 0, pItem->GetAmount()),
					GItemRemover::DONT_ROUTE);
			}

			mapSortedItem[merge.m_Slot.m_nToSlotID] = NULL;
		}
		else
		{
			pItemHolder->SetAmount(data.m_nSlotType, merge.m_Slot.m_nToSlotID, merge.m_nStackAmount);
		}
	}

	// 4th: Route to Player
	Route(pPlayer, data.m_nSlotType, data.m_nStartSlotIndex, MakeTD(mapSortedItem));
}

void GItemSorter::Route(GEntityPlayer* pPlayer, SH_ITEM_SLOT_TYPE nSlotType, int nInvenIndex, const vector<TD_ITEM>& vectdItem)
{
	VALID(pPlayer);

	MCommand* pNewCommand = MakeNewCommand(MC_ITEM_SORT,
		3,
		NEW_INT(nSlotType),
		NEW_INT(nInvenIndex),
		NEW_BLOB(vectdItem));

	pPlayer->RouteToMe(pNewCommand);
}

vector<TD_ITEM> GItemSorter::MakeTD(const map<int, GItem*>& mapSortedItem)
{
	vector<TD_ITEM> vectdItem;
	for (const map<int, GItem*>::value_type& item : mapSortedItem)
	{
		GItem* pItem = item.second;

		if (pItem)
			vectdItem.push_back(pItem->ToTD_ITEM());
	}

	return vectdItem;
}
