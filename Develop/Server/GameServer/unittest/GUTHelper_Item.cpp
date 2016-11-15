#include "StdAfx.h"
#include "GUTHelper_Item.h"
#include "SUnitTestUtil.h"
#include "GItemManager.h"
#include "GEntityPlayer.h"
#include "GItemEquiper.h"
#include "GQuestSystem.h"
#include "GTestForward.h"
#include "GItemAdder.h"
#include "GItemHolder.h"
#include "GItemUnequiper.h"

GItemData* GUTHelper_Item::NewItemData(int nID, SH_ITEM_SLOT nFirstSlot, SH_ITEM_SLOT nSecondSlot)
{
	if (nID == INVALID_ID)	nID = SUnitTestUtil::NewID();

	GItemData* pNewItemData = gmgr.pItemManager->GetItemData(nID);
	if (NULL != pNewItemData)
		return pNewItemData;

	pNewItemData = gmgr.pItemManager->NewItemData();

	pNewItemData->m_nID = nID;
	pNewItemData->m_nItemSlot = nFirstSlot;
	pNewItemData->m_nSubSlot = nSecondSlot;
	pNewItemData->m_nStackAmount = 100;

	gmgr.pItemManager->InsertItemData(pNewItemData);

	return pNewItemData;
}


GItemData* GUTHelper_Item::NewWeaponItemData( int nID, WEAPON_TYPE nWeaponType )
{
	GItemData* pItemData = NewItemData(nID, ITEMSLOT_RWEAPON);	// 기본을 오른손 장착으로...
	pItemData->m_ItemType = ITEMTYPE_WEAPON;
	pItemData->m_nWeaponType = nWeaponType;
	pItemData->m_nMaxDurability = 100; // 무기 장착이 안되는 문제를 피하기 위해서

	if (WEAPON_TWO_HANDED == nWeaponType)
	{
		pItemData->m_nSubSlot = ITEMSLOT_LWEAPON;
	}

	return pItemData;
}

GItemData* GUTHelper_Item::NewArmorItemData( int nID, ARMOR_TYPE nArmorType )
{
	GItemData* pItemData = NewItemData(nID, ITEMSLOT_HEAD);	// 기본을 머리 장착으로...
	pItemData->m_ItemType = ITEMTYPE_ARMOR;
	pItemData->m_nArmorType = nArmorType;
	pItemData->m_nMaxDurability = 100; // 무기 장착이 안되는 문제를 피하기 위해서

	return pItemData;
}

GItemData* GUTHelper_Item::NewShieldItemData( int nID )
{
	GItemData* pItemData = NewItemData(nID, ITEMSLOT_LWEAPON);	// 기본을 왼손 장착으로...
	pItemData->m_ItemType = ITEMTYPE_ARMOR;
	pItemData->m_nArmorType = ARMOR_SHIELD;
	pItemData->m_nMaxDurability = 100; // 무기 장착이 안되는 문제를 피하기 위해서

	return pItemData;
}

GItem* GUTHelper_Item::NewItem(int nItemID, int nAmount)
{	
	GItemData* pItemData = gmgr.pItemManager->GetItemData(nItemID);
	if (NULL == pItemData)
	{
		pItemData = NewItemData();
	}

	GItem* pItem = 
		gmgr.pItemManager->MakeNewItem(pItemData->m_nID, nAmount);

	pItem->m_nIUID = SUnitTestUtil::NewIUID();

	return pItem;
}

GItem* GUTHelper_Item::GiveNewItem(GEntityPlayer* pPlayer, int nItemID, int nItemQuantity, SH_ITEM_SLOT_TYPE nSlotType)
{
	if (NULL == pPlayer) return NULL;

	GItemData* pItemData = gmgr.pItemManager->GetItemData(nItemID);
	if (NULL == pItemData)
	{
		pItemData = NewItemData();
	}

	GItemHolder* pItemHolder = pPlayer->GetItemHolder();
	int nSlotID = pItemHolder->ReserveEmptySlot(nSlotType, 0);
	if (false == gsys.pItemSystem->GetAdder().AddForDBTask(pPlayer->GetUID(), ADD_ITEM(nSlotType, nSlotID, nItemQuantity, nItemQuantity, SUnitTestUtil::NewIUID(), pItemData->m_nID), false))
	{
		return NULL;
	}

	GItem* pItem = pPlayer->GetItemHolder()->GetItem(nSlotType, nSlotID);
	
	return pItem;
}

GItem* GUTHelper_Item::GiveNewItemToStorage( GEntityPlayer* pPlayer, int nItemID/*=INVALID_ID*/, int nItemQuantity/*=1*/ )
{
	return GiveNewItem(pPlayer, nItemID, nItemQuantity, SLOTTYPE_STORAGE);
}

GItem* GUTHelper_Item::GiveNewItemToGuildStorage( GEntityPlayer* pPlayer, int nItemID/*=INVALID_ID*/, int nItemQuantity/*=1*/ )
{
	return GiveNewItem(pPlayer, nItemID, nItemQuantity, SLOTTYPE_GUILDSTORAGE);
}

GItem* GUTHelper_Item::EquipNewItem(GEntityPlayer* pPlayer, int nItemID, SH_ITEM_SLOT nItemSlot)
{
	GItem* pItem = GiveNewItem(pPlayer, nItemID);
	if (NULL == pItem) return NULL;

	SH_ITEM_SLOT nItemEquipSlot = nItemSlot;

	if (ITEMSLOT_NONE == pItem->m_pItemData->m_nItemSlot &&
		ITEMSLOT_NONE != nItemSlot)
	{
		pItem->m_pItemData->m_nItemSlot = nItemSlot;
		nItemEquipSlot = nItemSlot;		

		if (ITEMSLOT_RWEAPON2 == nItemSlot)
		{
			pItem->m_pItemData->m_nItemSlot = ITEMSLOT_RWEAPON;
		}
		else if (ITEMSLOT_LWEAPON2 == nItemSlot)
		{
			pItem->m_pItemData->m_nItemSlot = ITEMSLOT_LWEAPON;
		}
	}
	else if(ITEMSLOT_NONE == nItemSlot &&
		ITEMSLOT_NONE != pItem->m_pItemData->m_nItemSlot)
	{
		nItemEquipSlot = pItem->m_pItemData->m_nItemSlot;
	}
	else if(ITEMSLOT_NONE == nItemSlot &&
		ITEMSLOT_NONE == pItem->m_pItemData->m_nItemSlot)
	{
		pItem->m_pItemData->m_nItemSlot = ITEMSLOT_RWEAPON;
		nItemEquipSlot = ITEMSLOT_RWEAPON;
	}

	EquipItem(pPlayer, pItem, nItemEquipSlot);

	return pItem;
}

GItem* GUTHelper_Item::GainWeaponItem(GEntityPlayer* pOwner, int nID, WEAPON_TYPE nWeaponType)
{
	GItemData* pItemData = NewWeaponItemData(nID, nWeaponType);

	return GiveNewItem(pOwner, pItemData->m_nID);
}

GItem* GUTHelper_Item::GainArmorItem( GEntityPlayer* pOwner, ARMOR_TYPE nArmorType )
{
	GItemData* pItemData = NewArmorItemData(INVALID_ID, nArmorType);

	return GiveNewItem(pOwner, pItemData->m_nID);
}

GItem* GUTHelper_Item::GainShieldItem( GEntityPlayer* pOwner)
{
	GItemData* pItemData = NewShieldItemData();

	return GiveNewItem(pOwner, pItemData->m_nID);
}

GItem* GUTHelper_Item::GainDyeItem( GEntityPlayer* pOwner, int nColor )
{
	vector<int> vecColor;
	vecColor.push_back(nColor);

	return GainDyeItem(pOwner, vecColor);
}

GItem* GUTHelper_Item::GainDyeItem(GEntityPlayer* pOwner, vector<int> vecColor )
{
	GItemData* pItemData = NewItemData();
	pItemData->m_ItemType = ITEMTYPE_DYE;
	pItemData->m_vecDyeColor = vecColor;

	return GiveNewItem(pOwner, pItemData->m_nID);
}

GItem* GUTHelper_Item::EquipNewWeapon(GEntityPlayer* pOwner, WEAPON_TYPE nWeaponType, SH_ITEM_SLOT nSlot)
{
	GItem* pWeapon = GainWeaponItem(pOwner, SUnitTestUtil::NewID(), nWeaponType);
	EquipItem(pOwner, pWeapon, nSlot);
	for (int i=0; i<ENCHANT_MAX_COUNT; ++i)
	{
		pWeapon->m_pItemData->m_vecEquipEnchantColor.push_back(ENCHANT_RED);
	}
	
	return pWeapon;
}

GItem* GUTHelper_Item::EquipNewShield(GEntityPlayer* pOwner)
{
	GItem* pItem = GainShieldItem(pOwner);
	EquipItem(pOwner, pItem, ITEMSLOT_LWEAPON);
	return pItem;
}

GItem* GUTHelper_Item::NewEnchantStoneItem(GEntityPlayer* pPlayer, int nLinkedBuffID)
{
	GItemData* pItemData = NewItemData();
	pItemData->m_ItemType = ITEMTYPE_ENCHANTSTONE;
	pItemData->m_EnchantStone.nLinkedBuffID = nLinkedBuffID;
	GItemHolder* pItemHolder = pPlayer->GetItemHolder();
	int nSlotID = pItemHolder->ReserveEmptySlot(SLOTTYPE_INVENTORY, 0);
	if (false == gsys.pItemSystem->GetAdder().AddForDBTask(pPlayer->GetUID(), ADD_ITEM(SLOTTYPE_INVENTORY, nSlotID, 1, 1, SUnitTestUtil::NewIUID(), pItemData->m_nID), false))
	{
		return NULL;
	}

	return pPlayer->GetItemHolder()->GetItem(SLOTTYPE_INVENTORY, nSlotID);
}


bool GUTHelper_Item::EquipItem( GEntityPlayer* pPlayer, GItem* pItem, SH_ITEM_SLOT nSlot )
{
	GTestHelpers testHelpers;
	testHelpers.talent.TrainEquipUsableAllTalent(pPlayer);	

	return gsys.pItemSystem->GetEquiper().Equip(pPlayer, pItem, nSlot);
}

bool GUTHelper_Item::UnequipItem( GEntityPlayer* pPlayer, GItem* pItem )
{
	return gsys.pItemSystem->GetUnequiper().Unequip(pPlayer, pItem);
}

bool GUTHelper_Item::FullItemToInven(GEntityPlayer* pPlayer, int nEmptySize)
{
	GItemHolder* pItemHolder = pPlayer->GetItemHolder();
	if (NULL == pItemHolder) return false;

	int nSize = BASE_INVENTORY_SIZE - nEmptySize;

	for (int i=0; i<nSize; i++)
	{
		GiveNewItem(pPlayer);
	}

	return true;
}