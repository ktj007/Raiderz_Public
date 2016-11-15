#include "stdafx.h"
#include "GItemUnequiper.h"
#include "GGlobal.h"
#include "GEntityPlayer.h"
#include "CCommandResultTable.h"
#include "GDBManager.h"
#include "GInteractionSystem.h"
#include "CCommandTable.h"
#include "GCommand.h"
#include "GConditionInfo.h"
#include "GPlayerNPCIconSender.h"
#include "CSItemHelper.h"
#include "GItemEquiper.h"
#include "GItemHolder.h"
#include "GDBTaskDataItemEquip.h"
#include "GPlayerObjectManager.h"
#include "GDBCacheSystem.h"
#include "GItemActableChecker.h"
#include "GEquipmentSlot.h"
#include "GOverlappedEquipmentSlot.h"
#include "GPlayerStance.h"


bool GItemUnequiper::Unequip(GEntityPlayer* pPlayer, GItem* pItem)
{
	VALID_RET(pPlayer, false);
	VALID_RET(pItem, false);

	SH_ITEM_SLOT nSlot = static_cast<SH_ITEM_SLOT>(pItem->m_nSlotID);
	SH_ITEM_SLOT nSubSlot = CSItemHelper::GetSubSlot(nSlot, pItem->m_pItemData);

	if (false == Check(pPlayer, pItem, nSlot, nSubSlot)) return false;
	
	GDBT_ITEM_UNEQUIP data;
	MakeDBTaskData(pPlayer, pItem, data);
	if (false == gsys.pDBManager->ItemUnequip(data)) return false;	

	return true;
}

bool GItemUnequiper::Check(GEntityPlayer* pPlayer, GItem* pItem, SH_ITEM_SLOT nSlot, SH_ITEM_SLOT nSubSlot)
{
	VALID_RET(pPlayer, false);
	VALID_RET(pItem, false);

	if (pPlayer->GetItemHolder()->GetEquipment().GetOverlapped().IsOverlapped())
		return false;

	if (false == CheckReservedDummySlot(pPlayer, nSlot, nSubSlot)) return false;
	if (false == CheckReservedEquipSlot(pPlayer, nSlot, nSubSlot)) return false;
	if (true == pPlayer->GetItemHolder()->IsFull(SLOTTYPE_INVENTORY)) return false;
	if (SLOTTYPE_EQUIP != pItem->m_nSlotType) return false;

	if (false == gsys.pItemSystem->GetActableChecker().IsUnequipable(pPlayer, pItem)) return false;

	return true;
}

bool GItemUnequiper::CheckReservedDummySlot(GEntityPlayer* pPlayer, SH_ITEM_SLOT nSlot, SH_ITEM_SLOT nSubSlot)
{
	if (true == pPlayer->GetItemHolder()->IsReservedDummySlot(SLOTTYPE_EQUIP, nSlot)) return false;
	if (true == pPlayer->GetItemHolder()->IsReservedDummySlot(SLOTTYPE_EQUIP, nSubSlot)) return false;

	return true;
}

bool GItemUnequiper::CheckReservedEquipSlot( GEntityPlayer* pPlayer, SH_ITEM_SLOT nSlot, SH_ITEM_SLOT nSubSlot )
{
	if (true == pPlayer->GetItemHolder()->IsReservedSlot(SLOTTYPE_EQUIP, nSlot)) return false;
	if (true == pPlayer->GetItemHolder()->IsReservedSlot(SLOTTYPE_EQUIP, nSubSlot)) return false;	

	return true;
}

bool GItemUnequiper::MakeDBTaskData(GEntityPlayer* pPlayer, GItem* pItem, GDBT_ITEM_UNEQUIP& data )
{
	VALID_RET(pPlayer, false);
	VALID_RET(pItem, false);

	GDBT_UNEQUIP_ITEM_SLOT unequipData1;
	GDBT_UNEQUIP_ITEM_SLOT unequipData2;

	int nUnequipItemCount = 1;
	GItem* pExtraUnequipItem = SelectExtraUnequipItem(pPlayer, pItem);
	if (NULL != pExtraUnequipItem)
	{
		nUnequipItemCount = 2;
	}

	if (nUnequipItemCount > pPlayer->GetItemHolder()->GetEmptySlotCount(SLOTTYPE_INVENTORY))
	{
		return false;
	}

	int nUnequipSlotIDTo = pPlayer->GetItemHolder()->ReserveEmptySlot(SLOTTYPE_INVENTORY, 0);

	unequipData1 = GDBT_UNEQUIP_ITEM_SLOT(pItem->m_nSlotType, (SH_ITEM_SLOT)pItem->m_nSlotID, pItem->m_nIUID, pItem->m_nDurability, nUnequipSlotIDTo); 
	pPlayer->GetItemHolder()->ReserveSlot(SLOTTYPE_EQUIP, unequipData1.m_nSlotID);

	if (NULL != pExtraUnequipItem)
	{
		int nExtraUnequipSlotIDTo = pPlayer->GetItemHolder()->ReserveEmptySlot(SLOTTYPE_INVENTORY, 0);

		unequipData2 = GDBT_UNEQUIP_ITEM_SLOT(pExtraUnequipItem->m_nSlotType, (SH_ITEM_SLOT)pExtraUnequipItem->m_nSlotID, pExtraUnequipItem->m_nIUID, pExtraUnequipItem->m_nDurability
			, nExtraUnequipSlotIDTo);
		pPlayer->GetItemHolder()->ReserveSlot(SLOTTYPE_EQUIP, unequipData2.m_nSlotID);
	}

	data = GDBT_ITEM_UNEQUIP(pPlayer->GetUID(), pPlayer->GetCID(), unequipData1, unequipData2);

	return true;
}

GItem* GItemUnequiper::SelectExtraUnequipItem(GEntityPlayer* pPlayer, GItem* pItem)
{
	VALID_RET(pPlayer, NULL);
	VALID_RET(pItem, NULL);

	if (ITEMSLOT_RWEAPON == pItem->m_nSlotID || ITEMSLOT_RWEAPON2 == pItem->m_nSlotID)
	{
		GItem* pOppositeWeaponItem = pPlayer->GetItemHolder()->GetItem(SLOTTYPE_EQUIP, CSItemHelper::GetOppositeWeaponSlot(static_cast<SH_ITEM_SLOT>(pItem->m_nSlotID)));
		if (NULL != pOppositeWeaponItem  && ARMOR_SHIELD != pOppositeWeaponItem->m_pItemData->m_nArmorType)
		{
			return pOppositeWeaponItem;
		}
	}

	return NULL;
}


void GItemUnequiper::UnequipForDBTask(GDBT_ITEM_UNEQUIP& data)
{
	GEntityPlayer* pPlayer = gmgr.pPlayerObjectManager->GetEntity(data.uidPlayer);
	VALID(pPlayer);

	bool bBeforeUnarmed = pPlayer->GetItemHolder()->GetEquipment().IsUnarmed();

	GItem* pUneqipItem1 = pPlayer->GetItemHolder()->PopItem(SLOTTYPE_EQUIP, data.unequip1.m_nSlotID);
	if (NULL != pUneqipItem1)
	{
		pPlayer->GetItemHolder()->CancelReserveSlot(SLOTTYPE_EQUIP, data.unequip1.m_nSlotID);
		pPlayer->GetItemHolder()->PushItem(SLOTTYPE_INVENTORY, data.unequip1.m_nToSlotID, pUneqipItem1);		
		gsys.pDBCacheSystem->SetItemSlot(pPlayer->GetUID(), SLOTTYPE_INVENTORY, data.unequip1.m_nToSlotID, data.unequip1.m_nIUID);
		Route(pPlayer, data.unequip1.m_nSlotID, data.unequip1.m_nToSlotID);
	}
	
	GItem* pUneqipItem2 = pPlayer->GetItemHolder()->PopItem(SLOTTYPE_EQUIP, data.unequip2.m_nSlotID);
	if (NULL != pUneqipItem2)
	{
		pPlayer->GetItemHolder()->CancelReserveSlot(SLOTTYPE_EQUIP, data.unequip2.m_nSlotID);
		pPlayer->GetItemHolder()->PushItem(SLOTTYPE_INVENTORY, data.unequip2.m_nToSlotID, pUneqipItem2);		
		gsys.pDBCacheSystem->SetItemSlot(pPlayer->GetUID(), SLOTTYPE_INVENTORY, data.unequip2.m_nToSlotID, data.unequip2.m_nIUID);
		Route(pPlayer, data.unequip2.m_nSlotID, data.unequip2.m_nToSlotID);	
	}

	bool bAfterUnarmed = pPlayer->GetItemHolder()->GetEquipment().IsUnarmed();
	if (false == bBeforeUnarmed && true == bAfterUnarmed)
	{
		pPlayer->GetStance().OnUnarmed();
	}
}

void GItemUnequiper::FailForDBTask( GDBT_ITEM_UNEQUIP& data )
{
	GEntityPlayer* pPlayer = gmgr.pPlayerObjectManager->GetEntity(data.uidPlayer);
	VALID(pPlayer);

	if (true == data.unequip1.IsValid())
	{
		pPlayer->GetItemHolder()->CancelReserveSlot(SLOTTYPE_EQUIP, data.unequip1.m_nSlotID);
	}

	if (true == data.unequip2.IsValid())
	{
		pPlayer->GetItemHolder()->CancelReserveSlot(SLOTTYPE_EQUIP, data.unequip2.m_nSlotID);
	}	
}

void GItemUnequiper::Route(GEntityPlayer* pPlayer, SH_ITEM_SLOT nSlotIDFrom, int nSlotIDTo)
{
	VALID(pPlayer);
	if (ITEMSLOT_NONE == nSlotIDFrom) return;

	RouteToMe(pPlayer, nSlotIDFrom, nSlotIDTo);
	RouteToOther(pPlayer, nSlotIDFrom);

	pPlayer->GetNPCIconSender().SendByPlayerConditionChange(CT_EQUIP);
}

void GItemUnequiper::RouteToMe(GEntityPlayer* pPlayer, SH_ITEM_SLOT nSlotIDFrom, int nSlotIDTo)
{
	VALID(pPlayer);

	MCommand* pNewCommand = MakeNewCommand(MC_ITEM_UNEQUIP, 
		2, NEW_CHAR(nSlotIDFrom), NEW_INT(nSlotIDTo));

	pPlayer->RouteToMe(pNewCommand);
}

void GItemUnequiper::RouteToOther(GEntityPlayer* pPlayer, SH_ITEM_SLOT nSlotIDFrom)
{
	VALID(pPlayer);

	MCommand* pNewCommand = MakeNewCommand(MC_ITEM_CHANGE_LOOK_UNEQUIP_ITEM, 
		2, NEW_UID(pPlayer->GetUID()), NEW_CHAR(nSlotIDFrom));

	pPlayer->RouteToThisCellExceptMe(pNewCommand);
}