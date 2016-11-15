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

bool GItemSorter::Sort(GEntityPlayer* pPlayer)
{
	VALID_RET(pPlayer, false);

	vector<GItem*> vecAllItem;
	pPlayer->GetItemHolder()->CollectAllItem(SLOTTYPE_INVENTORY, vecAllItem);

	vector<GItem*> vecSortedAllItem = GetSortedAllItem(vecAllItem);

	GDBT_ITEM_SORT data;
	if (false == MakeDBTaskData(pPlayer, vecSortedAllItem, data)) return false;

	SortForDBTask(data);

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

bool GItemSorter::MakeDBTaskData(GEntityPlayer* pPlayer, const vector<GItem*>& vecSortedAllItem, GDBT_ITEM_SORT& data)
{
	VALID_RET(pPlayer, false);

	vector<GDBT_ITEM_SORT_SLOT> vecSortSlot;

	int nSortSlotID = 0;
	for each (GItem* pItem in vecSortedAllItem)
	{
		if (pItem->m_nSlotID != nSortSlotID)
		{
			vecSortSlot.push_back(GDBT_ITEM_SORT_SLOT(pItem->m_nSlotID, nSortSlotID));
		}
		nSortSlotID++;
	}

	data = GDBT_ITEM_SORT(pPlayer->GetUID(), pPlayer->GetCID(), vecSortSlot);

	return true;
}

void GItemSorter::SortForDBTask(const GDBT_ITEM_SORT& data)
{
	GEntityPlayer* pPlayer = gmgr.pPlayerObjectManager->GetEntity(data.m_uidPlayer);
	if (NULL == pPlayer) return;
	GItemHolder* pItemHolder = pPlayer->GetItemHolder();
	if (NULL == pItemHolder) return;

	vector<GItem*> vecSortItem;
	for each (const GDBT_ITEM_SORT_SLOT& sortSlot in data.m_vecSortSlot)
	{
		vecSortItem.push_back(pItemHolder->GetItem(SLOTTYPE_INVENTORY, sortSlot.m_nFromSlotID));
	}

	int nIndex = 0;
	for each (GItem* pItem in vecSortItem)
	{
		int nSlot1 = pItem->m_nSlotID;
		int nSlot2 = data.m_vecSortSlot[nIndex].m_nToSlotID;

		GItem* pItem1 = pItemHolder->PopItem(SLOTTYPE_INVENTORY, nSlot1);
		GItem* pItem2 = pItemHolder->PopItem(SLOTTYPE_INVENTORY, nSlot2);

		pItemHolder->PushItem(SLOTTYPE_INVENTORY, nSlot2, pItem1);
		pItemHolder->PushItem(SLOTTYPE_INVENTORY, nSlot1, pItem2);
		nIndex++;
	}

	Route(pPlayer);
}

void GItemSorter::Route(GEntityPlayer* pPlayer)
{
	VALID(pPlayer);

	MCommand* pNewCommand = MakeNewCommand(MC_ITEM_SORT_INVEN_SLOT, 0, NULL);

	pPlayer->RouteToMe(pNewCommand);
}