#include "stdafx.h"
#include "GItemStateSelector.h"
#include "GItem.h"
#include "GEntityPlayer.h"
#include "GPlayerDoing.h"
#include "GItemHolder.h"
#include "GPlayerTrade.h"
#include "GPlayerMailReserver.h"
#include "GPlayerDye.h"

ITEM_STATE_TYPE GItemStateSelector::Select(GEntityPlayer* pPlayer, const GItem* pItem)
{	
	if (true == IsStorageItem(pPlayer, pItem)) return IST_STORAGE;
	if (true == IsPostItem(pPlayer, pItem)) return IST_POST;
	if (true == IsDyeingItem(pPlayer)) return IST_DYEING;
	if (true == IsEnchant(pPlayer)) return IST_ENCNAHTING;
	if (true == IsEquipedItem(pPlayer, pItem)) return IST_EQUIPED;
	if (true == IsArrangedItem(pPlayer, pItem)) return IST_ARRANGED;
	if (true == IsTradingItem(pPlayer, pItem)) return IST_TRADING;	
	if (true == IsInventoryItem(pPlayer, pItem)) return IST_INVENTORY;
	
	return IST_NONE;
}

bool GItemStateSelector::IsInventoryItem(GEntityPlayer* pPlayer, const GItem* pItem)
{
	// TODO : 나중에 창고 생기면 추가 체크
	
	GItemHolder* pItemHolder = pPlayer->GetItemHolder();

	if (pItem != pItemHolder->GetItem(SLOTTYPE_INVENTORY, pItem->m_nSlotID)) return false;
	if (true == IsTradingItem(pPlayer, pItem)) return false;
	if (true == IsEquipedItem(pPlayer, pItem)) return false;
	if (true == IsArrangedItem(pPlayer, pItem)) return false;
	if (true == IsStorageItem(pPlayer, pItem))	return false;
	if (true == IsPostItem(pPlayer, pItem))	return false;

	return true;
}

bool GItemStateSelector::IsStorageItem(GEntityPlayer* pPlayer, const GItem* pItem)
{
	return SLOTTYPE_STORAGE == pItem->m_nSlotType;
}

bool GItemStateSelector::IsPostItem(GEntityPlayer* pPlayer, const GItem* pItem)
{
	VALID_RET(pPlayer != NULL, false);
	VALID_RET(pItem != NULL, false);

	if (pItem->m_nSlotType != SLOTTYPE_INVENTORY)	return false;
	if (false == pPlayer->GetDoing().IsNowInteracting()) return false;
	if (false == pPlayer->GetMailReserver().IsAppended(pItem->m_nSlotID)) return false;

	return true;
}

bool GItemStateSelector::IsEquipedItem(GEntityPlayer* pPlayer, const GItem* pItem)
{
	if (false == pItem->IsEquiped()) return false;
	if (ITEMTYPE_HOUSING == pItem->m_pItemData->m_ItemType) return false;

	return true;
}

bool GItemStateSelector::IsArrangedItem(GEntityPlayer* pPlayer, const GItem* pItem)
{
	if (false == pItem->IsEquiped()) return false;
	if (ITEMTYPE_HOUSING != pItem->m_pItemData->m_ItemType) return false;

	return true;
}
bool GItemStateSelector::IsTradingItem(GEntityPlayer* pPlayer, const GItem* pItem)
{
	if (pItem->m_nSlotType != SLOTTYPE_INVENTORY)	return false;
	if (false == pPlayer->GetDoing().IsNowTrading()) return false;
	if (false == pPlayer->GetPlayerTrade().IsExistItemBySlotID(pItem->m_nSlotID)) return false;

	return true;
}

bool GItemStateSelector::IsEnchant(GEntityPlayer* pPlayer)
{
	if (false == pPlayer->GetDoing().IsNowEnchanting()) return false;

	return true;
}

bool GItemStateSelector::IsDyeingItem( GEntityPlayer* pPlayer)
{
	if (false == pPlayer->GetDoing().IsNowDyeing()) return false;

	return true;
}

