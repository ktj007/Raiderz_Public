#include "StdAfx.h"
#include "XItemHelper.h"
#include "XItemManager.h"
#include "XTalentInfo.h"
#include "XTalentInfoMgr.h"
#include "XMyPlayer.h"
#include "XTalentAvailable.h"


XTalentInfo* XItemHelper::GetTalentUsableItem( int ItemID )
{
	XItemData* pItemData = info.item->GetItemData(ItemID);
	if (NULL == pItemData) return NULL;

	if (USABLE_TALENT_USE != pItemData->m_nUsableType) return NULL;
	if (1 != pItemData->m_vecUsableParam.size()) return NULL;

	XTalentInfo* pTalentInfo = info.talent->Get(pItemData->m_vecUsableParam.front());
	return pTalentInfo;
}

bool XItemHelper::IsUsableItemAtDead( int ItemID )
{
	XTalentInfo* pTalentInfo = GetTalentUsableItem(ItemID);
	if (NULL == pTalentInfo) return false;

	if (false  ==  pTalentInfo->m_bUsableAtDead) return false;

	return true;
}

bool XItemHelper::IsUsableSelfRebirthItem( int ItemID )
{
	XTalentInfo* pTalentInfo = GetTalentUsableItem(ItemID);
	if (NULL == pTalentInfo) return false;

	if (false == pTalentInfo->IsSelfRebirth()) return false;
	if (true == gvar.MyInfo.Talent.CoolTime.IsCoolDown(pTalentInfo->m_nID)) return false;

	return true;
}

bool XItemHelper::IsAvailable(int ItemID )
{
	if (gvar.Game.pMyPlayer->IsDead())
	{
		if (IsUsableItemAtDead(ItemID) == true &&
			IsUsableSelfRebirthItem(ItemID) == true)
			return true;

		return false;
	}

	XItemData* pItemData = info.item->GetItemData(ItemID);
	if (pItemData == NULL) return false;

	if (USABLE_TALENT_USE != pItemData->m_nUsableType) return true;

	XTalentInfo* pTalentInfo = GetTalentUsableItem(ItemID);
	if (NULL == pTalentInfo) return false;

	XTalentAvailable	TalentAvailable;
	return TalentAvailable.IsAvailable(pTalentInfo);
}

int XItemHelper::GetInvenItemReuseTimeRemaining(int ItemID)
{
	XItemData* pItemData = info.item->GetItemData(ItemID);
	if (pItemData == NULL)		return 0;

	if (USABLE_TALENT_USE != pItemData->m_nUsableType)	return 0;

	XTalentInfo* pTalentInfo = GetTalentUsableItem(ItemID);
	if (NULL == pTalentInfo)	return 0;

	return (int)(gvar.MyInfo.Talent.CoolTime.GetRemainedTime(pTalentInfo->m_nID) + 0.5f);
}

XItem* XItemHelper::GetItem( SH_ITEM_SLOT_TYPE nSlotType, int nSlotID )
{
	if (SLOTTYPE_EQUIP == nSlotType)
	{
		return gvar.MyInfo.EquipmentSlot.GetItem((SH_ITEM_SLOT)nSlotID);
	}
	else if (SLOTTYPE_INVENTORY == nSlotType)
	{
		return gvar.MyInfo.Inventory.GetItem(nSlotID);
	}
	else if (SLOTTYPE_STORAGE == nSlotType)
	{
		return gvar.MyInfo.Storage.GetItem(nSlotID);
	}
	else if (SLOTTYPE_GUILDSTORAGE == nSlotType)
	{
		return gvar.MyInfo.GuildStorage.GetItem(nSlotID);
	} 

	return NULL;
}

void XItemHelper::PushItem( SH_ITEM_SLOT_TYPE nSlotType, int nSlotID, XItem* pItem )
{
	if (SLOTTYPE_INVENTORY == nSlotType)
	{
		return gvar.MyInfo.Inventory.PushItem(nSlotID, pItem);
	}
	else if (SLOTTYPE_STORAGE == nSlotType)
	{
		return gvar.MyInfo.Storage.PushItem(nSlotID, pItem);
	} 
	else if (SLOTTYPE_GUILDSTORAGE == nSlotType)
	{
		return gvar.MyInfo.GuildStorage.PushItem(nSlotID, pItem);
	} 
}

XItem* XItemHelper::PopItem( SH_ITEM_SLOT_TYPE nSlotType, int nSlotID )
{
	if (SLOTTYPE_EQUIP == nSlotType)
	{
		return gvar.Game.pMyPlayer->RemoveEquipItem((SH_ITEM_SLOT)nSlotID);
	}
	else if (SLOTTYPE_INVENTORY == nSlotType)
	{
		return gvar.MyInfo.Inventory.PopItem(nSlotID);
	}
	else if (SLOTTYPE_STORAGE == nSlotType)
	{
		return gvar.MyInfo.Storage.PopItem(nSlotID);
	} 
	else if (SLOTTYPE_GUILDSTORAGE == nSlotType)
	{
		return gvar.MyInfo.GuildStorage.PopItem(nSlotID);
	} 

	return NULL;
}

void XItemHelper::MoveItem( XItem* pFromItem, SH_ITEM_SLOT_TYPE nFromSlotType, SH_ITEM_SLOT_TYPE nToSlotType, int nToSlotID, int nAmount )
{
	PopItem(nFromSlotType, pFromItem->m_nSlotID);
	PushItem(nToSlotType, nToSlotID, pFromItem);
}

void XItemHelper::MergeItem( XItem* pFromItem, SH_ITEM_SLOT_TYPE nFromSlotType, XItem* pToItem, SH_ITEM_SLOT_TYPE nToSlotType, int nAmount )
{
	pFromItem->m_nAmount -= nAmount;
	pToItem->m_nAmount += nAmount;

	if (0 == pFromItem->m_nAmount)
	{
		PopItem(nFromSlotType, pFromItem->m_nSlotID);
	}
}

void XItemHelper::SwapItem( XItem* pFromItem, SH_ITEM_SLOT_TYPE nFromSlotType, int nFromSlotID, XItem* pToItem, SH_ITEM_SLOT_TYPE nToSlotType, int nToSlotID )
{
	PopItem(nFromSlotType, nFromSlotID);
	PopItem(nToSlotType, nToSlotID);
	PushItem(nToSlotType, nToSlotID, pFromItem);
	PushItem(nFromSlotType, nFromSlotID, pToItem);
}

bool XItemHelper::DecreaseItemAmount( SH_ITEM_SLOT_TYPE nSlotType, int nSlotID, int nAmount )
{
	if (SLOTTYPE_INVENTORY == nSlotType)
	{
		return gvar.MyInfo.Inventory.DecreaseItemAmount(nSlotID, nAmount);
	}
	else if (SLOTTYPE_STORAGE == nSlotType)
	{
		return gvar.MyInfo.Storage.DecreaseItemAmount(nSlotID, nAmount);
	}
	else if (SLOTTYPE_GUILDSTORAGE == nSlotType)
	{
		return gvar.MyInfo.GuildStorage.DecreaseItemAmount(nSlotID, nAmount);
	}

	return false;
}

XItem* XItemHelper::SplitItem( XItem* pFromItem, SH_ITEM_SLOT_TYPE nFromSlotType, SH_ITEM_SLOT_TYPE nToSlotType, int nToSlotID, int nAmount )
{
	pFromItem->m_nAmount -= nAmount;

	if (SLOTTYPE_INVENTORY == nToSlotType)
	{
		return gvar.MyInfo.Inventory.InsertItem(nToSlotID, pFromItem, nAmount);
	}
	else if (SLOTTYPE_STORAGE == nToSlotType)
	{
		return gvar.MyInfo.Storage.InsertItem(nToSlotID, pFromItem, nAmount);
	}
	else if (SLOTTYPE_GUILDSTORAGE == nToSlotType)
	{
		return gvar.MyInfo.GuildStorage.InsertItem(nToSlotID, pFromItem, nAmount);
	}

	return NULL;
}