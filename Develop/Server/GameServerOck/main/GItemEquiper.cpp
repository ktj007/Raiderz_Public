#include "StdAfx.h"
#include "GItemEquiper.h"
#include "GEntityPlayer.h"
#include "GGlobal.h"
#include "GConditionsSystem.h"
#include "CSItemHelper.h"
#include "GItemUnequiper.h"
#include "GDBManager.h"
#include "GInteractionSystem.h"
#include "GPlayerNPCIconSender.h"
#include "GConditionInfo.h"
#include "GCommand.h"
#include "GItemActableChecker.h"
#include "GItemHolder.h"
#include "CCommandResultTable.h"
#include "CCommandTable.h"
#include "AStlUtil.h"
#include "GDBTaskDataItemEquip.h"
#include "GEquipmentSlot.h"
#include "GOverlappedEquipmentSlot.h"


bool GItemEquiper::Equip(GEntityPlayer* pPlayer, GItem* pItem, SH_ITEM_SLOT nSlot)
{
	VALID_RET(pPlayer, false);
	VALID_RET(pItem, false);

	SH_ITEM_SLOT nSubSlot = CSItemHelper::GetSubSlot(nSlot, pItem->m_pItemData);
	if (false == Check(pPlayer, pItem, nSlot, nSubSlot)) return false;
	
	GDBT_ITEM_EQUIP data;
	if (false == MakeDBTaskData(pPlayer, pItem, nSlot, nSubSlot, pItem->m_pItemData->m_bClaimRequired, data)) return false;
		
	if (false == gsys.pDBManager->ItemEquip(data)) return false;

	return true;
}

bool GItemEquiper::Check( GEntityPlayer* pPlayer, GItem* pItem, SH_ITEM_SLOT nSlot, SH_ITEM_SLOT nSubSlot )
{
	if (pPlayer->GetItemHolder()->GetEquipment().GetOverlapped().IsOverlapped())
		return false;

	if (false == CheckSlot(pPlayer, pItem, nSlot)) return false;
	if (false == CheckReservedEquipSlot(pPlayer, nSlot, nSubSlot)) return false;
	if (false == CheckReservedDummySlot(pPlayer, nSlot, nSubSlot)) return false;

	if (false == CheckCondition(pPlayer, pItem)) return pPlayer->FailAndRouteSystemMsg(CR_FAIL_EQUIP_NEED_CONDITION);
	if (false == CheckHasPassiveTalent(pPlayer, pItem)) return pPlayer->FailAndRouteSystemMsg(CR_FAIL_EQUIP_NEED_LICENSE);
	if (false == CheckActableItem(pPlayer, pItem)) return false;

	return true;
}

bool GItemEquiper::MakeDBTaskData( GEntityPlayer* pPlayer, GItem* pItem, SH_ITEM_SLOT nSlot, SH_ITEM_SLOT nSubSlot, bool bBind, GDBT_ITEM_EQUIP& data )
{
	//GDBT_ITEM_EQUIP_SLOT equipData = GDBT_ITEM_EQUIP_SLOT(pItem->m_nIUID, pItem->m_nSlotType, pItem->m_nSlotID, nSlot);
	GDBT_EQUIP_ITEM_SLOT	equipData(pItem->m_nSlotType, pItem->m_nSlotID, pItem->m_nIUID, pItem->m_nDurability, bBind, nSlot);
	GDBT_UNEQUIP_ITEM_SLOT unequipData1; 
	GDBT_UNEQUIP_ITEM_SLOT unequipData2;
	// equipData.Item.Build(pItem);

	GItem* pUnequipItem1 = NULL;
	GItem* pUnequipItem2 = NULL;
	int nUnequipItemCount = SelectUnequipItem(pPlayer, pItem, nSlot, nSubSlot, pUnequipItem1, pUnequipItem2);

	if (true == IsUnequipSwapItem(pUnequipItem1, pItem))
	{
		if (nUnequipItemCount > pPlayer->GetItemHolder()->GetEmptySlotCount(SLOTTYPE_INVENTORY) +1)
		{
			return false;
		}
	}
	else
	{
		if (nUnequipItemCount > pPlayer->GetItemHolder()->GetEmptySlotCount(SLOTTYPE_INVENTORY))
		{
			return false;
		}
	}

	pPlayer->GetItemHolder()->ReserveSlot(SLOTTYPE_EQUIP, equipData.m_nToSlotID);	

	if (NULL != pUnequipItem1)
	{
		int nUnequipSlotIDTo = 0;
		if (IsUnequipSwapItem(pUnequipItem1, pItem))
		{
			// 인벤 아이템을 장착 했을때 장착해제 되는 아이템은 인벤 아이템의 슬롯으로 보낸다.
			nUnequipSlotIDTo = pItem->m_nSlotID;
		}
		else
		{
			nUnequipSlotIDTo = pPlayer->GetItemHolder()->ReserveEmptySlot(SLOTTYPE_INVENTORY, 0);
		}

		unequipData1 = GDBT_UNEQUIP_ITEM_SLOT(pUnequipItem1->m_nSlotType, (SH_ITEM_SLOT)pUnequipItem1->m_nSlotID, pUnequipItem1->m_nIUID, pUnequipItem1->m_nDurability, nUnequipSlotIDTo);
		pPlayer->GetItemHolder()->ReserveSlot(SLOTTYPE_EQUIP, unequipData1.m_nSlotID);
	}

	if (NULL != pUnequipItem2)
	{
		int nUnequipSlotIDTo = pPlayer->GetItemHolder()->ReserveEmptySlot(SLOTTYPE_INVENTORY, 0);
		unequipData2 = GDBT_UNEQUIP_ITEM_SLOT(pUnequipItem2->m_nSlotType, (SH_ITEM_SLOT)pUnequipItem2->m_nSlotID, pUnequipItem2->m_nIUID, pUnequipItem2->m_nDurability, nUnequipSlotIDTo);
		pPlayer->GetItemHolder()->ReserveSlot(SLOTTYPE_EQUIP, unequipData2.m_nSlotID);
	}

	data = GDBT_ITEM_EQUIP(pPlayer->GetUID(), (int64)pPlayer->GetCID(), equipData, unequipData1, unequipData2);

	return true;
}

bool GItemEquiper::CheckSlot(GEntityPlayer* pPlayer, GItem* pItem, SH_ITEM_SLOT nSlot )
{
	if (NULL == pItem) return false;
	GItemData* pItemData = pItem->m_pItemData;
	if (NULL == pItemData) return false;

	if (SLOTTYPE_INVENTORY != pItem->m_nSlotType && SLOTTYPE_EQUIP != pItem->m_nSlotType) return false;
	if (SLOTTYPE_EQUIP == pItem->m_nSlotType && pItem->m_nSlotID == nSlot) return false;	

	if (nSlot == ITEMSLOT_LWEAPON || nSlot == ITEMSLOT_LWEAPON2)
	{
		// 방패가 아닌 무기아이템은 오른속에 아이템이 착용되지 않은 상태에서, 왼손에 착용할수 없다.
		if (ARMOR_SHIELD != pItem->m_pItemData->m_nArmorType)
		{
			if (false == pPlayer->GetItemHolder()->GetEquipment().IsEquipItem(CSItemHelper::GetOppositeWeaponSlot(nSlot)))
			{
				return false;
			}
		}		
	}

	if (false == CSItemHelper::IsEquipableSlot(nSlot, pItemData)) return false;

	return true;
}

bool GItemEquiper::CheckReservedDummySlot(GEntityPlayer* pPlayer, SH_ITEM_SLOT nSlot, SH_ITEM_SLOT nSubSlot)
{
	if (true == pPlayer->GetItemHolder()->IsReservedDummySlot(SLOTTYPE_EQUIP, nSlot)) return false;
	if (true == pPlayer->GetItemHolder()->IsReservedDummySlot(SLOTTYPE_EQUIP, nSubSlot)) return false;	

	return true;
}

bool GItemEquiper::CheckReservedEquipSlot( GEntityPlayer* pPlayer, SH_ITEM_SLOT nSlot, SH_ITEM_SLOT nSubSlot )
{
	if (true == pPlayer->GetItemHolder()->IsReservedSlot(SLOTTYPE_EQUIP, nSlot)) return false;
	if (true == pPlayer->GetItemHolder()->IsReservedSlot(SLOTTYPE_EQUIP, nSubSlot)) return false;	

	return true;
}

bool GItemEquiper::CheckCondition( GEntityPlayer* pPlayer, GItem* pItem )
{
	return gsys.pConditionsSystem->CheckForItem(pPlayer, pItem->m_pItemData->m_nConditionsInfoID);
}

bool GItemEquiper::CheckHasPassiveTalent( GEntityPlayer* pPlayer, GItem* pItem )
{
	if (NULL == pPlayer) return false;
	if (NULL == pItem) return false;

	TALENT_EXTRA_PASSIVE_TYPE nNeedTEPT = CSItemHelper::GetTEPTForEquip(pItem->m_pItemData);
	if (nNeedTEPT == TEPT_NONE)
		return true;

	if (!pPlayer->HasPassiveTalent(nNeedTEPT))
		return false;

	return true;
}


bool GItemEquiper::CheckActableItem( GEntityPlayer* pPlayer, GItem* pItem )
{
	return gsys.pItemSystem->GetActableChecker().IsEquipable(pPlayer, pItem);
}

int GItemEquiper::SelectUnequipItem(GEntityPlayer* pPlayer, GItem* pItem, SH_ITEM_SLOT nSlot, SH_ITEM_SLOT nSubSlot, GItem*& pUnequipItem1, GItem*& pUnequipItem2 )
{
	pUnequipItem1 = NULL;
	pUnequipItem2 = NULL;

	// 장착할 메인 슬롯을 메인슬롯으로 차지하고 있는 아이템 찾기
	GItem* pSlotOccupyItem = pPlayer->GetItemHolder()->GetItem(SLOTTYPE_EQUIP, nSlot);
	if (NULL == pSlotOccupyItem)	
	{
		// 장착할 메인슬롯을 서브슬롯으로 사용하고 있는 아이템 찾기
		pSlotOccupyItem = pPlayer->GetItemHolder()->GetEquipment().GetItemFromSubSlot(nSlot);
	}	
	// 장착할 서브슬롯을 메인슬롯으로 차지하고 있는 아이템 찾기
	GItem* pSubSlotOccupyItem = pPlayer->GetItemHolder()->GetItem(SLOTTYPE_EQUIP, nSubSlot);
	if (NULL == pSubSlotOccupyItem)
	{
		// 장착할 서브슬롯을 서브슬롯으로 차지하고 있는 아이템 찾기
		pSubSlotOccupyItem = pPlayer->GetItemHolder()->GetEquipment().GetItemFromSubSlot(nSubSlot);
	}

	// 함께 착용할수 없는 무기 아이템 찾기
	GItem* pTogetherUnequipableWeaponItem = SelectTogetherUnequipableWeaponItem(pPlayer, pItem, nSlot);

	int nUnequipItemCount = 0;
	if (NULL != pSlotOccupyItem)
	{
		if (NULL == pUnequipItem1 && pUnequipItem2 != pSlotOccupyItem)
		{
			pUnequipItem1 = pSlotOccupyItem;
			nUnequipItemCount++;
		}
		else if (NULL == pUnequipItem2 && pUnequipItem1 != pSlotOccupyItem)
		{
			pUnequipItem2 = pSlotOccupyItem;
			nUnequipItemCount++;
		}
	}

	if (NULL != pSubSlotOccupyItem)
	{
		if (NULL == pUnequipItem1 && pUnequipItem2 != pSubSlotOccupyItem)
		{
			pUnequipItem1 = pSubSlotOccupyItem;
			nUnequipItemCount++;
		}
		else if (NULL == pUnequipItem2 && pUnequipItem1 != pSubSlotOccupyItem)
		{
			pUnequipItem2 = pSubSlotOccupyItem;
			nUnequipItemCount++;
		}
	}

	if (NULL != pTogetherUnequipableWeaponItem)
	{
		if (NULL == pUnequipItem1 && pUnequipItem2 != pTogetherUnequipableWeaponItem)
		{
			pUnequipItem1 = pTogetherUnequipableWeaponItem;
			nUnequipItemCount++;
		}
		else if (NULL == pUnequipItem2 && pUnequipItem1 != pTogetherUnequipableWeaponItem)
		{
			pUnequipItem2 = pTogetherUnequipableWeaponItem;
			nUnequipItemCount++;
		}
	}

	return nUnequipItemCount;
}

GItem* GItemEquiper::SelectTogetherUnequipableWeaponItem(GEntityPlayer* pPlayer, GItem* pItem, SH_ITEM_SLOT nSlot)
{
	if (false == CSItemHelper::IsWeaponSlot(nSlot)) return NULL;

	SH_ITEM_SLOT nOppositeSlot = CSItemHelper::GetOppositeWeaponSlot(nSlot);
	GItem* pOppositeWeaponItem = pPlayer->GetItemHolder()->GetItem(SLOTTYPE_EQUIP, nOppositeSlot);
	if (NULL == pOppositeWeaponItem) return NULL;

	if (true == CSItemHelper::IsTogetherEquipableItemData(pItem->m_pItemData, pOppositeWeaponItem->m_pItemData))
	{
		return NULL;
	}

	return pOppositeWeaponItem;
}

bool GItemEquiper::IsUnequipSwapItem( GItem* pUnequipItem1, GItem* pEquipItem )
{
	if (NULL == pUnequipItem1) return false;
	if (SLOTTYPE_INVENTORY != pEquipItem->m_nSlotType) return false;

	return true;
}