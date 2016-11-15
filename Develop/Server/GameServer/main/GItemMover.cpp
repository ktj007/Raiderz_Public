#include "StdAfx.h"
#include "GItemMover.h"
#include "GItem.h"
#include "GEntityPlayer.h"
#include "GItemHolder.h"
#include "GGlobal.h"
#include "GDBManager.h"
#include "GDBTaskItemData.h"
#include "GItemActableChecker.h"
#include "CCommandResultTable.h"
#include "GDB_CODE.h"

bool GItemMover::Move(GEntityPlayer* pPlayer, SH_ITEM_SLOT_TYPE nFromSlotType, int nFromSlotID, int nFromItemID, SH_ITEM_SLOT_TYPE nToSlotType, int nToSlotID, int nToItemID, int nAmount)
{
	if (NULL == pPlayer) return false;	

	if (false == Check(pPlayer, nFromSlotType, nFromSlotID, nFromItemID, nToSlotType, nToSlotID, nToItemID, nAmount)) return false;	
	if (false == DB(pPlayer, nFromSlotType, nFromSlotID, nToSlotType, nToSlotID, nAmount)) return false;

	return true;	
}

bool GItemMover::Check(GEntityPlayer* pPlayer, SH_ITEM_SLOT_TYPE nFromSlotType, int nFromSlotID, int nFromItemID, SH_ITEM_SLOT_TYPE nToSlotType, int nToSlotID, int nToItemID, int nAmount)
{
	if (NULL == pPlayer) return false;
	if (0 >= nAmount) return false;
	if (nFromSlotType == nToSlotID && nFromSlotID == nToSlotID) return false;
	GItem* pFromItem = pPlayer->GetItemHolder()->GetItem(nFromSlotType, nFromSlotID);
	if (NULL == pFromItem) return false;
	if (pFromItem->GetID() != nFromItemID) return false;

	// 아이템 장착및 해제는 Move가 아닌  Equip, Unequip을 통해서만 처리한다.
	if (SLOTTYPE_EQUIP == nFromSlotType || SLOTTYPE_EQUIP == nToSlotType)
	{
		return CR_FALSE;
	}

	if (SLOTTYPE_GUILDSTORAGE != nFromSlotType && SLOTTYPE_GUILDSTORAGE == nToSlotType)
	{
		if (pFromItem->m_bBind)
			return pPlayer->FailAndRouteSystemMsg(CR_FAIL_GUILD_CANNOT_MOVE_BINDITEM_TO_STORAGE);

		if (pFromItem->IsPeriod())
			return pPlayer->FailAndRouteSystemMsg(CR_FAIL_GUILD_CANNOT_MOVE_PERIODITEM_TO_STORAGE);
	}

	if (SLOTTYPE_STORAGE != nFromSlotType && SLOTTYPE_STORAGE == nToSlotType)
	{
		if (pFromItem->IsPeriod())
			return pPlayer->FailAndRouteSystemMsg(CR_FAIL_GUILD_CANNOT_MOVE_PERIODITEM_TO_STORAGE);
	}

	GItem* pToItem = pPlayer->GetItemHolder()->GetItem(nToSlotType, nToSlotID);
	if (NULL == pToItem)
	{
		if (pFromItem->GetAmount() == nAmount)
		{
			return CheckMove(pPlayer, pFromItem, nToSlotType, nToSlotID);
		}
		else
		{
			return CheckSplit(pPlayer, pFromItem, nToSlotType, nToSlotID);
		}
	}
	else
	{
		if (pToItem->GetID() != nToItemID) return false;

		if (pFromItem->GetID() == pToItem->GetID() && pToItem->GetAmount() < pToItem->m_pItemData->m_nStackAmount)
		{
			return CheckMerge(pPlayer, pFromItem, pToItem, nAmount);
		}
		else
		{
			return CheckSwap(pPlayer, pFromItem, pToItem);
		}		
	}	
	
	return true;
}

bool GItemMover::CheckMove( GEntityPlayer* pPlayer, GItem* pFromItem, SH_ITEM_SLOT_TYPE nToSlotType, int nToSlotID )
{
	if (NULL == pPlayer) return false;
	if (NULL == pFromItem) return false;

	if (false == gsys.pItemSystem->GetActableChecker().IsMovable(pPlayer, pFromItem)) return false;
	if (true == pPlayer->GetItemHolder()->IsReservedSlot(static_cast<SH_ITEM_SLOT_TYPE>(nToSlotType), nToSlotID)) return false;
	if (true == pPlayer->GetItemHolder()->IsReservedDummySlot(static_cast<SH_ITEM_SLOT_TYPE>(nToSlotType), nToSlotID)) return false;

	return true;
}

bool GItemMover::CheckSplit( GEntityPlayer* pPlayer, GItem* pFromItem, SH_ITEM_SLOT_TYPE nToSlotType, int nToSlotID )
{
	if (NULL == pPlayer) return false;
	if (NULL == pFromItem) return false;

	if (false == gsys.pItemSystem->GetActableChecker().IsMovable(pPlayer, pFromItem)) return false;
	if (true == pPlayer->GetItemHolder()->IsReservedSlot(static_cast<SH_ITEM_SLOT_TYPE>(nToSlotType), nToSlotID)) return false;
	if (true == pPlayer->GetItemHolder()->IsReservedDummySlot(static_cast<SH_ITEM_SLOT_TYPE>(nToSlotType), nToSlotID)) return false;
	if (true == pFromItem->IsPeriod()) return false;

	return true;
}

bool GItemMover::CheckMerge( GEntityPlayer* pPlayer, GItem* pFromItem, GItem* pToItem, int nAmount )
{
	if (NULL == pPlayer) return false;
	if (NULL == pFromItem) return false;
	if (NULL == pToItem) return false;
	
	if (false == gsys.pItemSystem->GetActableChecker().IsMovable(pPlayer, pFromItem)) return false;
	if (false == gsys.pItemSystem->GetActableChecker().IsMovable(pPlayer, pToItem)) return false;
	if (nAmount + pToItem->GetAmount() > pToItem->m_pItemData->m_nStackAmount) return false;
	if (true == pFromItem->IsPeriod()) return false;
	if (true == pToItem->IsPeriod()) return false;

	return true;
}

bool GItemMover::CheckSwap( GEntityPlayer* pPlayer, GItem* pFromItem, GItem* pToItem)
{
	if (NULL == pPlayer) return false;
	if (NULL == pFromItem) return false;
	if (NULL == pToItem) return false;

	if (false == gsys.pItemSystem->GetActableChecker().IsMovable(pPlayer, pFromItem)) return false;
	if (false == gsys.pItemSystem->GetActableChecker().IsMovable(pPlayer, pToItem)) return false;	

	return true;
}

bool GItemMover::DB(GEntityPlayer* pPlayer, SH_ITEM_SLOT_TYPE nFromSlotType, int nFromSlotID, SH_ITEM_SLOT_TYPE nToSlotType, int nToSlotID, int nAmount)
{
	if (NULL == pPlayer) return false;
	GItem* pFromItem = pPlayer->GetItemHolder()->GetItem(nFromSlotType, nFromSlotID);
	if (NULL == pFromItem) return false;

	GItem* pToItem = pPlayer->GetItemHolder()->GetItem(nToSlotType, nToSlotID);
	if (NULL == pToItem)
	{
		if (pFromItem->GetAmount() == nAmount)
		{
			return DBMove(pPlayer, pFromItem, nToSlotType, nToSlotID, nAmount);
		}
		else
		{
			return DBSplit(pPlayer, pFromItem, nToSlotType, nToSlotID, nAmount);
		}		
	}
	else
	{
		if (pFromItem->GetID() == pToItem->GetID() && pToItem->GetAmount() < pToItem->m_pItemData->m_nStackAmount)
		{
			return DBMerge(pPlayer, pFromItem, pToItem, nAmount);
		}
		else
		{
			return DBSwap(pPlayer, pFromItem, pToItem);
		}
	}	

	return true;	
}

bool GItemMover::DBMove( GEntityPlayer* pPlayer, GItem* pFromItem, SH_ITEM_SLOT_TYPE nToSlotType, int nToSlotID, int nAmount )
{
	if (NULL == pPlayer) return false;
	if (NULL == pFromItem) return false;

	if (pFromItem->GetAmount() != nAmount)
		return false;

	PLAYER_RELATE_ID playerRelateID;
	GetPlayerRelateID(pPlayer, pFromItem->m_nSlotType, nToSlotType, playerRelateID);

	pPlayer->GetItemHolder()->ReserveSlot(nToSlotType, nToSlotID);

	GDBT_ITEM_MOVE data(pPlayer->GetUID()
		, pPlayer->GetPlayerInfo()->nPlayTimeSec
		, pPlayer->GetPlayerInfo()->GetDeltaPlayTime()
		, pPlayer->GetLevel()
		, pPlayer->GetMoney()
		, pFromItem->m_pItemData->m_nID);

	data.m_FromSlot = GDBT_ITEM_MOVE_SLOT(playerRelateID.nFromAID, playerRelateID.nFromCIDorGID, pFromItem->m_nSlotType, pFromItem->m_nSlotID, pFromItem->m_nIUID, pFromItem->GetAmount());
	data.m_ToSlot = GDBT_ITEM_MOVE_SLOT(playerRelateID.nToAID, playerRelateID.nToCIDorGID, nToSlotType, nToSlotID, 0, 0);
	data.MakeDBCode();

	return gsys.pDBManager->ItemMove(data);
}

bool GItemMover::DBSplit( GEntityPlayer* pPlayer, GItem* pFromItem, SH_ITEM_SLOT_TYPE nToSlotType, int nToSlotID, int nAmount )
{
	if (NULL == pPlayer) return false;
	if (NULL == pFromItem) return false;

	PLAYER_RELATE_ID playerRelateID;
	GetPlayerRelateID(pPlayer, pFromItem->m_nSlotType, nToSlotType, playerRelateID);

	pPlayer->GetItemHolder()->ReserveSlot(nToSlotType, nToSlotID);
	
	GDBT_ITEM_MERGE_AND_SPLIT data(pPlayer->GetUID()
		, pPlayer->GetPlayerInfo()->nPlayTimeSec
		, pPlayer->GetPlayerInfo()->GetDeltaPlayTime()
		, pPlayer->GetLevel()
		, pPlayer->GetPlayerInfo()->GetMoney()
		, nAmount
		, GDBT_ITEM_MERGE_AND_SPLIT::SPLIT);

	data.m_FromSlot = GDBT_ITEM_MERGE_AND_SPLIT_SLOT(playerRelateID.nFromAID, playerRelateID.nFromCIDorGID, pFromItem->m_nSlotType, pFromItem->m_nSlotID, pFromItem->m_nIUID, pFromItem->GetAmount()-nAmount, pFromItem->m_pItemData->m_nID);
	data.m_ToSlot = GDBT_ITEM_MERGE_AND_SPLIT_SLOT(playerRelateID.nToAID, playerRelateID.nToCIDorGID, nToSlotType, nToSlotID, 0, nAmount, pFromItem->m_pItemData->m_nID);
	data.MakeDBCode();

	return gsys.pDBManager->ItemSplit(data);
}

bool GItemMover::DBMerge( GEntityPlayer* pPlayer, GItem* pFromItem, GItem* pToItem, int nAmount )
{
	if (NULL == pPlayer) return false;
	if (NULL == pFromItem) return false;
	if (NULL == pToItem) return false;

	PLAYER_RELATE_ID playerRelateID;
	GetPlayerRelateID(pPlayer, pFromItem->m_nSlotType, pToItem->m_nSlotType, playerRelateID);

	GDBT_ITEM_MERGE_AND_SPLIT data(pPlayer->GetUID()
		, pPlayer->GetPlayerInfo()->nPlayTimeSec
		, pPlayer->GetPlayerInfo()->GetDeltaPlayTime()
		, pPlayer->GetLevel()
		, pPlayer->GetPlayerInfo()->GetMoney()
		, nAmount
		, GDBT_ITEM_MERGE_AND_SPLIT::MERGE);

	data.m_FromSlot = GDBT_ITEM_MERGE_AND_SPLIT_SLOT(playerRelateID.nFromAID, playerRelateID.nFromCIDorGID, pFromItem->m_nSlotType, pFromItem->m_nSlotID, pFromItem->m_nIUID, pFromItem->GetAmount()-nAmount, pFromItem->GetID());
	data.m_ToSlot= GDBT_ITEM_MERGE_AND_SPLIT_SLOT(playerRelateID.nToAID, playerRelateID.nToCIDorGID, pToItem->m_nSlotType, pToItem->m_nSlotID, pToItem->m_nIUID, pToItem->GetAmount()+nAmount, pToItem->GetID());
	data.MakeDBCode();

	return gsys.pDBManager->ItemMerge(data);	
}

bool GItemMover::DBSwap( GEntityPlayer* pPlayer, GItem* pFromItem, GItem* pToItem )
{
	if (NULL == pPlayer) return false;
	if (NULL == pFromItem) return false;
	if (NULL == pToItem) return false;

	PLAYER_RELATE_ID playerRelateID;
	GetPlayerRelateID(pPlayer, pFromItem->m_nSlotType, pToItem->m_nSlotType, playerRelateID);

	GDBT_ITEM_MOVE data(pPlayer->GetUID()
		, pPlayer->GetPlayerInfo()->nPlayTimeSec
		, pPlayer->GetPlayerInfo()->GetDeltaPlayTime()
		, pPlayer->GetLevel()
		, pPlayer->GetMoney()
		, pFromItem->m_pItemData->m_nID);

	data.m_FromSlot = GDBT_ITEM_MOVE_SLOT(playerRelateID.nFromAID, playerRelateID.nFromCIDorGID, pFromItem->m_nSlotType, pFromItem->m_nSlotID, pFromItem->m_nIUID, pFromItem->GetAmount());
	data.m_ToSlot = GDBT_ITEM_MOVE_SLOT(playerRelateID.nToAID, playerRelateID.nToCIDorGID, pToItem->m_nSlotType, pToItem->m_nSlotID, pToItem->m_nIUID, pToItem->GetAmount());
	data.MakeDBCode();

	return gsys.pDBManager->ItemMove(data);
}

void GItemMover::GetPlayerRelateID(GEntityPlayer* pPlayer, SH_ITEM_SLOT_TYPE nFromSlotType, SH_ITEM_SLOT_TYPE nToSlotType, PLAYER_RELATE_ID& playerRelateID)
{
	if (NULL == pPlayer) return;

	playerRelateID.nFromAID = (SLOTTYPE_GUILDSTORAGE == nFromSlotType) ? 0 : pPlayer->GetAID();
	playerRelateID.nFromCIDorGID = (SLOTTYPE_GUILDSTORAGE == nFromSlotType) ? pPlayer->GetGID() : pPlayer->GetCID();
	playerRelateID.nToAID = (SLOTTYPE_GUILDSTORAGE == nToSlotType) ? 0 : pPlayer->GetAID();
	playerRelateID.nToCIDorGID = (SLOTTYPE_GUILDSTORAGE == nToSlotType) ? pPlayer->GetGID() : pPlayer->GetCID();	
}
