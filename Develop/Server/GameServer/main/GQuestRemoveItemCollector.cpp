#include "StdAfx.h"
#include "GQuestRemoveItemCollector.h"
#include "GItem.h"
#include "GQuestInfo.h"
#include "GItemHolder.h"
#include "GEntityPlayer.h"
#include "GItemAddRemoveInfo.h"
#include "GQObjectiveInfo.h"

void GQuestRemoveItemCollector::CollectForCancel(GEntityPlayer* pPlayer, GQuestInfo* pQuestInfo, vector<GITEM_STACK_AMT>& vecItemStackAmt)
{	
	CollectDestroyItem(pPlayer, pQuestInfo, vecItemStackAmt);
	CollectAddItem(pPlayer, pQuestInfo, vecItemStackAmt);
	
	FilterSameItem(vecItemStackAmt);
}

bool GQuestRemoveItemCollector::CollectForReward(GEntityPlayer* pPlayer, GQuestInfo* pQuestInfo, vector<GITEM_STACK_AMT>& vecItemStackAmt)
{
	CollectDestroyItem(pPlayer, pQuestInfo, vecItemStackAmt);
	CollectAddItem(pPlayer, pQuestInfo, vecItemStackAmt);	
	if (false == CollectQObjectiveItem(pPlayer, pQuestInfo, vecItemStackAmt)) return false;

	FilterSameItem(vecItemStackAmt);

	return true;
}

void GQuestRemoveItemCollector::CollectForGive( GEntityPlayer* pPlayer, GQuestInfo* pQuestInfo, vector<GITEM_STACK_AMT>& vecItemStackAmt )
{
	CollectRemoveItem(pPlayer, pQuestInfo, vecItemStackAmt);
	FilterSameItem(vecItemStackAmt);
}


void GQuestRemoveItemCollector::CollectDestroyItem(GEntityPlayer* pPlayer, GQuestInfo* pQuestInfo, vector<GITEM_STACK_AMT>& vecItemStackAmt)
{
	vector<GItem*> vecItem;
	for (size_t i = 0; i < pQuestInfo->vecDestroyItemID.size(); ++i)
	{
		GItemHolder* pItemHolder = pPlayer->GetItemHolder();
		pItemHolder->CollectItemByID(SLOTTYPE_EQUIP, pQuestInfo->vecDestroyItemID[i], vecItem);
		pItemHolder->CollectItemByID(SLOTTYPE_INVENTORY, pQuestInfo->vecDestroyItemID[i], vecItem);
		pItemHolder->CollectItemByID(SLOTTYPE_STORAGE, pQuestInfo->vecDestroyItemID[i], vecItem);
	}

	for each (GItem* pItem in vecItem)
	{
		vecItemStackAmt.push_back(GITEM_STACK_AMT(pItem));
	}
}

void GQuestRemoveItemCollector::CollectAddItem(GEntityPlayer* pPlayer, GQuestInfo* pQuestInfo, vector<GITEM_STACK_AMT>& vecItemStackAmt)
{
	if (NULL == pQuestInfo->pItemAddRemoveInfo) return;

	vector<GItemAddInfo*> vecItemAddInfo = pQuestInfo->pItemAddRemoveInfo->GetItemAddInfo();
	vector<GItem*> vecItem;		
	for each (GItemAddInfo* val in vecItemAddInfo)
	{
		GItemHolder* pItemHolder = pPlayer->GetItemHolder();
		pItemHolder->CollectItemByID(SLOTTYPE_EQUIP, val->GetID(), vecItem);
		pItemHolder->CollectItemByID(SLOTTYPE_INVENTORY, val->GetID(), vecItem);
		pItemHolder->CollectItemByID(SLOTTYPE_STORAGE, val->GetID(), vecItem);
	}

	for each (GItem* pItem in vecItem)
	{
		vecItemStackAmt.push_back(GITEM_STACK_AMT(pItem));
	}
}

void GQuestRemoveItemCollector::CollectRemoveItem(GEntityPlayer* pPlayer, GQuestInfo* pQuestInfo, vector<GITEM_STACK_AMT>& vecItemStackAmt)
{
	if (NULL == pQuestInfo->pItemAddRemoveInfo) return;

	vector<GItem*> vecItem;
	vector<GItemRemoveAllInfo*> vecItemRemoveAllInfo = pQuestInfo->pItemAddRemoveInfo->GetItemRemoveAllInfo();		
	for each (GItemRemoveAllInfo* val in vecItemRemoveAllInfo)
	{
		GItemHolder* pItemHolder = pPlayer->GetItemHolder();
		pItemHolder->CollectItemByID(SLOTTYPE_EQUIP, val->GetID(), vecItem);
		pItemHolder->CollectItemByID(SLOTTYPE_INVENTORY, val->GetID(), vecItem);
		pItemHolder->CollectItemByID(SLOTTYPE_STORAGE, val->GetID(), vecItem);
	}
	for each (GItem* pItem in vecItem)
	{
		vecItemStackAmt.push_back(GITEM_STACK_AMT(pItem));
	}

	vector<GItemRemoveInfo*> vecItemRemoveInfo = pQuestInfo->pItemAddRemoveInfo->GetItemRemoveInfo();
	for each (GItemRemoveInfo* val in vecItemRemoveInfo)
	{
		int nCollectRemainAmount = val->GetAmount();

		nCollectRemainAmount = pPlayer->GetItemHolder()->CollectItemUntilAmount(SLOTTYPE_EQUIP, val->GetID(), nCollectRemainAmount, vecItemStackAmt);
		if (0 != nCollectRemainAmount)
		{
			nCollectRemainAmount = pPlayer->GetItemHolder()->CollectItemUntilAmount(SLOTTYPE_INVENTORY, val->GetID(), nCollectRemainAmount, vecItemStackAmt);
			if (0 != nCollectRemainAmount)
			{
				nCollectRemainAmount = pPlayer->GetItemHolder()->CollectItemUntilAmount(SLOTTYPE_STORAGE, val->GetID(), nCollectRemainAmount, vecItemStackAmt);
			}
		}
	}
}


bool GQuestRemoveItemCollector::CollectQObjectiveItem(GEntityPlayer* pPlayer, GQuestInfo* pQuestInfo, vector<GITEM_STACK_AMT>& vecItemStackAmt)
{
	vector<GQObjectiveInfo*> vecQObjectiveInfo = pQuestInfo->GetAllQObjectiveInfo();

	for each (GQObjectiveInfo* pQObjectiveInfo in vecQObjectiveInfo)
	{
		if (QOT_COLLECT != pQObjectiveInfo->nType)
			continue;

		GItemHolder* pItemHolder = pPlayer->GetItemHolder();

		int nItemID = pQObjectiveInfo->vecParam1[0];
		int nStackAmt = pQObjectiveInfo->nParam2;
		
		int nCollectRemainAmount = nStackAmt;
		nCollectRemainAmount = pItemHolder->CollectItemUntilAmount(SLOTTYPE_EQUIP, nItemID, nCollectRemainAmount, vecItemStackAmt);
		if (0 != nCollectRemainAmount)
		{
			nCollectRemainAmount = pItemHolder->CollectItemUntilAmount(SLOTTYPE_INVENTORY, nItemID, nCollectRemainAmount, vecItemStackAmt);
			if (0 != nCollectRemainAmount)
				return false;
		}
	}

	return true;
}

bool GQuestRemoveItemCollector::CheckAlreadyExistItem(vector<GITEM_STACK_AMT>&vecItemStackAmt, const int64 nIUID)
{
	for (size_t k = 0; k < vecItemStackAmt.size(); ++k)
	{
		if (vecItemStackAmt[k].nIUID == nIUID)
			return true;
	}

	return false;
}

void GQuestRemoveItemCollector::FilterSameItem(vector<GITEM_STACK_AMT> &vecItemStackAmt)
{
	vector<GITEM_STACK_AMT> vecTempItem = vecItemStackAmt;
	vecItemStackAmt.clear();

	for each (const GITEM_STACK_AMT& itemStackAmt in vecTempItem)
	{	
		if (CheckAlreadyExistItem(vecItemStackAmt, itemStackAmt.nIUID))
			continue;

		vecItemStackAmt.push_back(itemStackAmt);
	}
}