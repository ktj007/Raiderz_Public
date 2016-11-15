#include "StdAfx.h"
#include "GQuestAddItemCollector.h"
#include "GQuestInfo.h"
#include "GItemAddRemoveInfo.h"
#include "GItemData.h"
#include "GGlobal.h"
#include "GItemManager.h"
#include "GEntityPlayer.h"
#include "GItemHolder.h"
#include "GDBTaskQuestAccept.h"
#include "AStlUtil.h"


void GQuestAddItemCollector::CollectForGive(GEntityPlayer* pPlayer, GQuestInfo* pQuestInfo, vector<GDBT_QUEST_ADDITEM>& vecAddItem)
{
	if (NULL == pPlayer) return;
	if (NULL == pQuestInfo) return;
	if (NULL == pQuestInfo->pItemAddRemoveInfo) return;

	vector<GItemAddInfo*> vecItemAddInfo = pQuestInfo->pItemAddRemoveInfo->GetItemAddInfo();
	for each (GItemAddInfo* pItemAddInfo in vecItemAddInfo)
	{
		Collect(pPlayer, pItemAddInfo->GetID(), pItemAddInfo->GetAmount(), vecAddItem);
	}

	vector<GItemFillInfo*> vecItemFillInfo = pQuestInfo->pItemAddRemoveInfo->GetItemFillInfo();	
	for each (GItemFillInfo* pItemFillInfo in vecItemFillInfo)
	{
		int nInvenAmount = pPlayer->GetItemHolder()->GetItemCount(SLOTTYPE_INVENTORY, pItemFillInfo->GetID());
		if (pItemFillInfo->GetAmount() <= nInvenAmount) continue;

		Collect(pPlayer, pItemFillInfo->GetID(), pItemFillInfo->GetAmount() - nInvenAmount, vecAddItem);
	}	
}

void GQuestAddItemCollector::CollectForReward( GEntityPlayer* pPlayer, GQuestInfo* pQuestInfo, QUEST_REWARD selectedReward, vector<GDBT_QUEST_ADDITEM>& vecAddItem )
{
	if (NULL == pPlayer) return;
	if (NULL == pQuestInfo) return;

	for each (const QUEST_REWARD& reward in  pQuestInfo->vecReward)
	{
		if (QRT_ITEM != reward.nType) continue;

		Collect(pPlayer, reward.nParam1, reward.nParam2, vecAddItem);
	}
	
	if (QRT_ITEM == selectedReward.nType)
	{
		Collect(pPlayer, selectedReward.nParam1, selectedReward.nParam2, vecAddItem);
	}
}

void GQuestAddItemCollector::Collect( GEntityPlayer* pPlayer, int nID, int nAddAmount, vector<GDBT_QUEST_ADDITEM> &vecAddItem )
{
	GItemData* pItemData = gmgr.pItemManager->GetItemData(nID);
	if (NULL == pItemData) return;

	GItem* pItem = pPlayer->GetItemHolder()->GetMergeableItem(SLOTTYPE_INVENTORY, nID, nAddAmount);
	if (NULL != pItem)
	{
		GDBT_QUEST_ADDITEM addItem(pItem->m_nSlotID, pItem->m_nIUID, nID, pItem->GetAmount() + nAddAmount, nAddAmount, pItemData->m_nMaxDurability, pItem->m_bBind, pItem->m_ExpirationPeriod.IsPeriod());
		vecAddItem.push_back(addItem);
	}
	else
	{
		GDBT_QUEST_ADDITEM addItem(pPlayer->GetItemHolder()->ReserveEmptySlot(SLOTTYPE_INVENTORY, 0), 0, nID, nAddAmount, nAddAmount, pItemData->m_nMaxDurability, false, false);
		vecAddItem.push_back(addItem);
	}
}

void GQuestAddItemCollector::RemoveDuplicateCollection(vector<GDBT_QUEST_ADDITEM>& vecAddItem, vector<GITEM_STACK_AMT>& vecRemoveItem)
{
	set<int16> setDuplicatedAddItem;
	set<uint16> setDuplicatedRemoveItem;

	for (vector<GITEM_STACK_AMT>::iterator removeItor = vecRemoveItem.begin(); removeItor != vecRemoveItem.end(); removeItor++)
	{
		GITEM_STACK_AMT& removeItem = *removeItor;
		if (SLOTTYPE_INVENTORY != removeItem.nSlotType)	continue;
		

		for (vector<GDBT_QUEST_ADDITEM>::iterator addItor = vecAddItem.begin(); addItor != vecAddItem.end(); addItor++)
		{
			GDBT_QUEST_ADDITEM& addItem = *addItor;
			if (addItem.m_nSlotID != removeItem.nSlotID) continue;
			
			int nDiffAmt = addItem.m_nModStackAmt - removeItem.nModStackAmt;
			if (0 == nDiffAmt)
			{
				setDuplicatedAddItem.insert(addItem.m_nSlotID);
				setDuplicatedRemoveItem.insert(removeItem.nSlotID);
			}
			else if(0 < nDiffAmt)
			{
				setDuplicatedRemoveItem.insert(removeItem.nSlotID);

				addItem.m_nStackAmt = addItem.m_nStackAmt - addItem.m_nModStackAmt + nDiffAmt;
				addItem.m_nModStackAmt = nDiffAmt;				
			}
			else if(0 > nDiffAmt)
			{
				setDuplicatedAddItem.insert(addItem.m_nSlotID);

				removeItem.nToStackAmt = removeItem.nToStackAmt + removeItem.nModStackAmt + nDiffAmt;
				removeItem.nModStackAmt= -nDiffAmt;				
			}
		}
	}

	for (vector<GDBT_QUEST_ADDITEM>::iterator addItor = vecAddItem.begin(); addItor != vecAddItem.end();)
	{
		if (true == IsExist(setDuplicatedAddItem, addItor->m_nSlotID))
		{
			addItor = vecAddItem.erase(addItor);
		}
		else
		{
			addItor++;
		}
	}

	for (vector<GITEM_STACK_AMT>::iterator removeItor = vecRemoveItem.begin(); removeItor != vecRemoveItem.end();)
	{
		if (true == IsExist(setDuplicatedRemoveItem, removeItor->nSlotID))
		{
			removeItor = vecRemoveItem.erase(removeItor);
		}
		else
		{
			removeItor++;
		}
	}
}
