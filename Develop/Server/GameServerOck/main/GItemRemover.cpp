#include "StdAfx.h"
#include "GItemRemover.h"
#include "GEntityPlayer.h"
#include "GGlobal.h"
#include "GDBManager.h"
#include "GQuestSystem.h"
#include "CCommandResultTable.h"
#include "CCommandTable.h"
#include "GCommand.h"
#include "GInteractionSystem.h"
#include "GItemUnequiper.h"
#include "GItemRemover.h"
#include "GConditionInfo.h"
#include "GPlayerNPCIconSender.h"
#include "GItemManager.h"
#include "GItemHolder.h"
#include "GPlayerObjectManager.h"
#include "GItemActableChecker.h"
#include "GDBCacheSystem.h"
#include "GDBTaskItemData.h"
#include "GQObjUpdater.h"
#include "GDBTaskData.h"

REMOVE_ITEM::REMOVE_ITEM( const uint8 nSlotType, const int16 nSlotID, const int16 nToStackAmt, const int16 nStackModAmt )
: m_nSlotType(nSlotType), m_nSlotID(nSlotID), m_nToStackAmt(nToStackAmt), m_nStackModAmt(nStackModAmt)
{

}

REMOVE_ITEM::REMOVE_ITEM( const GITEM_STACK_AMT& itemStackAmt )
: m_nSlotType(itemStackAmt.nSlotType), m_nSlotID(itemStackAmt.nSlotID), m_nToStackAmt(itemStackAmt.nToStackAmt), m_nStackModAmt(itemStackAmt.nModStackAmt)
{

}


bool GItemRemover::Remove(GEntityPlayer* pPlayer, int nItemID, int nRemoveAmount)
{
	// 루아에서 사용하고 있는데, 루프가 아니라 프로시저 하나로 처리하도록 수정하자. sunge.
	// 다른데서는 사용하면 안됨. 	
	// 비동기로 바꿔줘야 함.

	if (NULL == pPlayer) return false;

	GItemHolder* pItemHolder = pPlayer->GetItemHolder();
	if (NULL == pItemHolder) return false;

	int nRemainStackAmt = nRemoveAmount;

	GITEM_STACK_AMT_VEC vecItemStackAmt;
	int nCollectRemainAmount = nRemoveAmount;
	nCollectRemainAmount = pItemHolder->CollectItemUntilAmount(SLOTTYPE_EQUIP, nItemID, nRemoveAmount, vecItemStackAmt);
	if (0 != nCollectRemainAmount)
	{
		nCollectRemainAmount = pItemHolder->CollectItemUntilAmount(SLOTTYPE_INVENTORY, nItemID, nRemoveAmount, vecItemStackAmt);
		if (0 != nCollectRemainAmount)
		{
			nCollectRemainAmount = pItemHolder->CollectItemUntilAmount(SLOTTYPE_STORAGE, nItemID, nRemoveAmount, vecItemStackAmt);
		}
	}
	
	for (size_t i = 0; i < vecItemStackAmt.size(); ++i)
	{
		Remove(pPlayer, (SH_ITEM_SLOT_TYPE)vecItemStackAmt[i].nSlotType, vecItemStackAmt[i].nSlotID, vecItemStackAmt[i].nModStackAmt);
	}

	return true;
}

bool GItemRemover::Remove(GEntityPlayer* pPlayer, SH_ITEM_SLOT_TYPE nSlotType, uint16 nSlotID, int nRemoveAmount)
{
	if (NULL == pPlayer) return false;
	GItemHolder* pItemHolder = pPlayer->GetItemHolder();
	if (NULL == pItemHolder) return false;
	GItem* pItem = pPlayer->GetItemHolder()->GetItem(nSlotType, nSlotID); 
	if (NULL == pItem) return false;

	if (false == Check(pPlayer, pItem, nRemoveAmount)) return false;

	vector<REMOVE_ITEM> vecRemoveItem;	
	if (nRemoveAmount == pItem->GetAmount())
	{		
		return DB_Delete(pPlayer, pItem);		
	}
	else if (nRemoveAmount < pItem->GetAmount())
	{		
		DB_Decrease(pPlayer, pItem, nRemoveAmount);
	}

	return true;
}

bool GItemRemover::RemoveForDBTask(const MUID& nPlayerUID, vector<REMOVE_ITEM>& vecRemoveItem)
{
	GEntityPlayer* pPlayer = gmgr.pPlayerObjectManager->GetEntity(nPlayerUID);
	if (NULL == pPlayer) return false;	
	GItemHolder* pItemHolder = pPlayer->GetItemHolder();
	if (NULL == pItemHolder) return false;

	Route(pPlayer, vecRemoveItem);

	for each(const REMOVE_ITEM& removeItem in vecRemoveItem)  
	{
		if (0 == removeItem.m_nToStackAmt)
		{
			Server_Delete(pPlayer, (SH_ITEM_SLOT_TYPE)removeItem.m_nSlotType, removeItem.m_nSlotID);		
		}
		else
		{
			Server_Decrease(pPlayer, (SH_ITEM_SLOT_TYPE)removeItem.m_nSlotType, removeItem.m_nSlotID, removeItem.m_nStackModAmt);
		}
	}	

	pPlayer->GetNPCIconSender().SendByPlayerConditionChange(CT_ITEM);

	return true;
}

bool GItemRemover::RemoveForDBTask( const MUID& nPlayerUID, const REMOVE_ITEM& removeItem )
{
	vector<REMOVE_ITEM> vecRemoveItem;
	vecRemoveItem.push_back(removeItem);

	return RemoveForDBTask(nPlayerUID, vecRemoveItem);	
}

bool GItemRemover::Check(GEntityPlayer* pPlayer, GItem* pItem, int nRemoveAmount)
{
	if (NULL == pItem) return false;

	if (0 >= nRemoveAmount) return false;
	if (pItem->GetAmount() < nRemoveAmount)	return false;	

	return true;
}

void GItemRemover::Server_Delete(GEntityPlayer* pPlayer, SH_ITEM_SLOT_TYPE nSlotType, uint16 nSlotID)
{
	if (NULL == pPlayer) return;

	GItemHolder* pItemHolder = pPlayer->GetItemHolder();
	if (NULL == pItemHolder) return;

	GItem* pItem = pItemHolder->GetItem(nSlotType, nSlotID);
	if (NULL == pItem) return;

	if (false == pItemHolder->PopItem(pItem->m_nSlotType, pItem->m_nSlotID)) return;
	gsys.pDBCacheSystem->DeleteItem(pPlayer->GetUID(), pItem->m_nIUID);

	int nItemID = pItem->m_pItemData->m_nID;
	int nAmount = pItem->GetAmount();
	SAFE_DELETE(pItem);

	gsys.pQuestSystem->GetQObjUpdater().DecCollectQObj(pPlayer, nItemID, nAmount);	
}

void GItemRemover::Server_Decrease(GEntityPlayer* pPlayer, SH_ITEM_SLOT_TYPE nSlotType, uint16 nSlotID, int nDecreaseAmount)
{
	if (NULL == pPlayer) return;
	GItemHolder* pItemHolder = pPlayer->GetItemHolder();
	if (NULL == pItemHolder) return;

	if (false == pItemHolder->DecreaseAmount(nSlotType, nSlotID, nDecreaseAmount))
		return;
}

bool GItemRemover::DB_Delete(GEntityPlayer* pPlayer, GItem* pItem)
{
	GDBT_ITEM_DATA data(pPlayer->GetAID(), pPlayer->GetUID(), pPlayer->GetPlayerInfo()->GetDeltaPlayTime(), pPlayer->GetLevel(), pPlayer->GetMoney(), pItem->GetAmount(), -1, pItem->m_pItemData->m_nTier, 0);

	data.m_Item.Build(pPlayer->GetCID(), pItem);
	data.m_Item.m_nStackAmt = 0;
	data.m_Item.m_nCharPtm = pPlayer->GetPlayerInfo()->nPlayTimeSec;

	return gsys.pDBManager->ItemDelete(data);
}

bool GItemRemover::DB_Decrease(GEntityPlayer* pPlayer, GItem* pItem, int nDecreaseAmount)
{
	GDBT_ITEM_DEC_STACK_AMT_DATA data(pPlayer->GetUID()
		, pPlayer->GetAID()
		, pPlayer->GetCID()
		, pPlayer->GetPlayerInfo()->nPlayTimeSec
		, pPlayer->GetPlayerInfo()->GetDeltaPlayTime()
		, pPlayer->GetLevel()
		, pPlayer->GetMoney()
		, pItem->m_nSlotType
		, pItem->m_nSlotID
		, pItem->m_nIUID
		, pItem->m_pItemData->m_nID
		, pItem->GetAmount() - nDecreaseAmount
		, nDecreaseAmount
		, pItem->m_pItemData->m_nTier
		, 0);
		
	return gsys.pDBManager->ItemDecStackAmt(data);
}

void GItemRemover::Route(GEntityPlayer* pPlayer, vector<REMOVE_ITEM>& vecRemoveItem)
{
	if (NULL == pPlayer) return;

	vector<TD_ITEM_DELETE>		vecTDItemDelete;
	vector<TD_ITEM_DECREASE>	vecTDItemDecrease;

	for (vector<REMOVE_ITEM>::iterator itor = vecRemoveItem.begin(); itor != vecRemoveItem.end(); itor++)  
	{
		if (0 == itor->m_nToStackAmt)
		{
			vecTDItemDelete.push_back(TD_ITEM_DELETE(itor->m_nSlotType, itor->m_nSlotID));
		}
		else
		{
			vecTDItemDecrease.push_back(TD_ITEM_DECREASE(itor->m_nSlotType, itor->m_nSlotID, itor->m_nStackModAmt));
		}
	}

	MCommand* pNewCmd = MakeNewCommand(MC_ITEM_REMOVE, 
		2,
		NEW_BLOB(vecTDItemDelete),
		NEW_BLOB(vecTDItemDecrease));

	pPlayer->RouteToMe(pNewCmd);	
}