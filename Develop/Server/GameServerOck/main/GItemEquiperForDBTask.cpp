#include "StdAfx.h"
#include "GItemEquiperForDBTask.h"
#include "GGlobal.h"
#include "GPlayerObjectManager.h"
#include "GDBTaskItemEquip.h"
#include "GItemHolder.h"
#include "GEntityPlayer.h"
#include "GDBCacheSystem.h"
#include "GItemUnequiper.h"
#include "GPlayerNPCIconSender.h"
#include "CSConditionsInfo.h"
#include "CCommandTable.h"
#include "GCommand.h"
#include "GEquipmentSlot.h"
#include "GPlayerStance.h"

void GItemEquiperForDBTask::Equip(GDBT_ITEM_EQUIP& data)
{
	GEntityPlayer* pPlayer = gmgr.pPlayerObjectManager->GetEntity(data.uidPlayer);
	if (NULL == pPlayer) return;
	GItemHolder* pItemHolder = pPlayer->GetItemHolder();
	if (NULL == pItemHolder) return;

	bool bBeforeUnarmed = pPlayer->GetItemHolder()->GetEquipment().IsUnarmed();

	GItem* pEquipItem = NULL;
	if (SLOTTYPE_INVENTORY == data.equip.m_nSlotType && true == data.unequip1.IsValid())
	{
		// 인벤 아이템을 장착 했을때 장착해제 되는 아이템을 인벤 아이템의 슬롯으로 보내기 위해, 인벤 아이템을 미리 빼놓는다.
		pEquipItem = pItemHolder->PopItem(data.equip.m_nSlotType, data.equip.m_nSlotID);
		if (NULL == pEquipItem) return;

		UnequipItem1(pPlayer, pItemHolder, data, false);
		EquipItem(pPlayer, pItemHolder, pEquipItem, data, false);
		RouteEquipSwapToInven(pPlayer, data);
	}
	else
	{
		pEquipItem = pItemHolder->GetItem(data.equip.m_nSlotType, data.equip.m_nSlotID);
		if (NULL == pEquipItem) return;
		
		UnequipItem1(pPlayer, pItemHolder, data);
		EquipItem(pPlayer, pItemHolder, pEquipItem, data);		
	}
	
	Bind(pEquipItem);
	UnequipItem2(pPlayer, pItemHolder, data);

	bool bAfterUnarmed = pPlayer->GetItemHolder()->GetEquipment().IsUnarmed();
	if (false == bBeforeUnarmed && true == bAfterUnarmed)
	{
		pPlayer->GetStance().OnUnarmed();
	}
	
	pPlayer->GetNPCIconSender().SendByPlayerConditionChange(CT_EQUIP);
}

void GItemEquiperForDBTask::Fail( GDBT_ITEM_EQUIP& data )
{
	GEntityPlayer* pPlayer = gmgr.pPlayerObjectManager->GetEntity(data.uidPlayer);
	if (NULL == pPlayer) return;
	GItemHolder* pItemHolder = pPlayer->GetItemHolder();
	if (NULL == pItemHolder) return;

	pItemHolder->CancelReserveSlot(SLOTTYPE_EQUIP, data.equip.m_nToSlotID);

	if (true == data.unequip1.IsValid())
	{
		pItemHolder->CancelReserveSlot(SLOTTYPE_EQUIP, data.unequip1.m_nSlotID);
	}

	if (true == data.unequip2.IsValid())
	{
		pItemHolder->CancelReserveSlot(SLOTTYPE_EQUIP, data.unequip2.m_nSlotID);
	}	
}

void GItemEquiperForDBTask::RouteToMe(GEntityPlayer* pPlayer, SH_ITEM_SLOT_TYPE nSlotTypeFrom, int nSlotIDFrom, SH_ITEM_SLOT nSlotIDTo)
{
	MCommand* pNewCommand = MakeNewCommand(MC_ITEM_EQUIP, 
		3, NEW_INT(nSlotTypeFrom), NEW_INT(nSlotIDFrom), NEW_CHAR(nSlotIDTo));

	pPlayer->RouteToMe(pNewCommand);
}

void GItemEquiperForDBTask::RouteToOther(GEntityPlayer* pPlayer, SH_ITEM_SLOT_TYPE nSlotTypeFrom, int nSlotIDFrom, SH_ITEM_SLOT nSlotIDTo, int nItemID, int nColor, int nEnchantBufFID)
{
	MCommand* pNewCommand = MakeNewCommand(MC_ITEM_CHANGE_LOOK_EQUIP_ITEM, 
		7, NEW_INT(nSlotTypeFrom), NEW_INT(nSlotIDFrom), NEW_CHAR(nSlotIDTo), NEW_UID(pPlayer->GetUID()), NEW_INT(nItemID), NEW_INT(nColor), NEW_INT(nEnchantBufFID));

	pPlayer->RouteToThisCellExceptMe(pNewCommand);
}

void GItemEquiperForDBTask::RouteEquipSwapToInven(GEntityPlayer* pPlayer, const GDBT_ITEM_EQUIP &data)
{
	MCommand* pNewCommand = MakeNewCommand(MC_ITEM_EQUIP_SWAPTOINVEN,
		4, 
		NEW_INT(data.equip.m_nSlotID), NEW_CHAR(static_cast<char>(data.equip.m_nToSlotID)),
		NEW_CHAR(static_cast<char>(data.unequip1.m_nSlotID)), NEW_INT(data.unequip1.m_nToSlotID));

	pPlayer->RouteToMe(pNewCommand);
}

void GItemEquiperForDBTask::EquipItem(GEntityPlayer* pPlayer, GItemHolder* pItemHolder, GItem* pEquipItem, const GDBT_ITEM_EQUIP &data, bool bRouteToMe)
{
	pItemHolder->PopItem(pEquipItem->m_nSlotType, pEquipItem->m_nSlotID);

	if (!pItemHolder->PushItem(SLOTTYPE_EQUIP, data.equip.m_nToSlotID, pEquipItem))
	{
		dlog("Fail Equip - item(id:%d, slotId:%d)\n", pEquipItem->m_pItemData->m_nID, pEquipItem->m_nSlotID);
		return;
	}

	gsys.pDBCacheSystem->SetItemSlot(pPlayer->GetUID(), SLOTTYPE_EQUIP, data.equip.m_nToSlotID, data.equip.m_nIUID);
	pItemHolder->CancelReserveSlot(SLOTTYPE_EQUIP, data.equip.m_nToSlotID);

	if (true == bRouteToMe)
	{
		RouteToMe(pPlayer, data.equip.m_nSlotType, data.equip.m_nSlotID, static_cast<SH_ITEM_SLOT>(data.equip.m_nToSlotID));
	}
	
	RouteToOther(pPlayer, data.equip.m_nSlotType, data.equip.m_nSlotID, static_cast<SH_ITEM_SLOT>(data.equip.m_nToSlotID), pEquipItem->m_pItemData->m_nID, pEquipItem->m_nDyedColor, pEquipItem->GetActiveEnchantBuffID());
}

void GItemEquiperForDBTask::UnequipItem1(GEntityPlayer* pPlayer, GItemHolder* pItemHolder, const GDBT_ITEM_EQUIP &data, bool bRoute)
{
	GItem* pUnequipItem1 = pItemHolder->PopItem(SLOTTYPE_EQUIP, data.unequip1.m_nSlotID);
	if (NULL == pUnequipItem1) return;

	if (!pItemHolder->PushItem(SLOTTYPE_INVENTORY, data.unequip1.m_nToSlotID, pUnequipItem1))
		return;

	gsys.pDBCacheSystem->SetItemSlot(pPlayer->GetUID(), SLOTTYPE_INVENTORY, data.unequip1.m_nToSlotID, data.unequip1.m_nIUID);
	pItemHolder->CancelReserveSlot(SLOTTYPE_EQUIP, data.unequip1.m_nSlotID);
	
	if (true == bRoute)
	{
		gsys.pItemSystem->GetUnequiper().Route(pPlayer, data.unequip1.m_nSlotID, data.unequip1.m_nToSlotID);
	}	
}

void GItemEquiperForDBTask::UnequipItem2( GEntityPlayer* pPlayer, GItemHolder* pItemHolder, const GDBT_ITEM_EQUIP &data)
{
	GItem* pUnequipItem2 = pItemHolder->PopItem(SLOTTYPE_EQUIP, data.unequip2.m_nSlotID);
	if (NULL == pUnequipItem2) return;

	if (!pItemHolder->PushItem(SLOTTYPE_INVENTORY, data.unequip2.m_nToSlotID, pUnequipItem2))
		return;
		
	gsys.pDBCacheSystem->SetItemSlot(pPlayer->GetUID(), SLOTTYPE_INVENTORY, data.unequip2.m_nToSlotID, data.unequip2.m_nIUID);
	pItemHolder->CancelReserveSlot(SLOTTYPE_EQUIP, data.unequip2.m_nSlotID);

	gsys.pItemSystem->GetUnequiper().Route(pPlayer, data.unequip2.m_nSlotID, data.unequip2.m_nToSlotID);
}

void GItemEquiperForDBTask::Bind( GItem* pEquipItem )
{
	if (true == pEquipItem->m_pItemData->m_bClaimRequired)
	{
		pEquipItem->m_bBind = true;
	}
}