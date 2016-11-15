#include "stdafx.h"
#include "XGlueGame_GuildStorage.h"
#include "XPost_Guild.h"
#include "XPost_Item.h"
#include "XMyPlayer.h"


int XGlueGameGuildStorage::GetGuildStorageItemID( int nSlotID ) const
{
	XItem* pItem =gvar.MyInfo.GuildStorage.GetItem( nSlotID);
	if(pItem == NULL) return 0;

	return pItem->m_pItemData->m_nID;
}

int XGlueGameGuildStorage::GetGuildStorageItemQuantity( int nSlotID ) const
{
	XItem* pItem =gvar.MyInfo.GuildStorage.GetItem( nSlotID);
	if(pItem == NULL) return 0;

	return pItem->m_nAmount;
}

bool XGlueGameGuildStorage::DepositGuildMoney( int nMoney ) const
{
	XPostGuild_DepositStorageMoney(nMoney);
	return true;
}

bool XGlueGameGuildStorage::WithdrawGuildMoney( int nMoney ) const
{
	XPostGuild_WithdrawStorageMoney(nMoney);
	return true;
}

bool XGlueGameGuildStorage::MoveGuildStorageItem( int nSrcSlotID, int nTarSlotID) const
{
	XItem* pSrcItem = gvar.MyInfo.GuildStorage.GetItem( nSrcSlotID);
	if ( pSrcItem == NULL)	return false;

	int nQuantity = pSrcItem->m_nAmount;

	int nTarItemID = 0;
	XItem* pTarItem = gvar.MyInfo.GuildStorage.GetItem( nTarSlotID);
	if ( pTarItem)
	{
		nTarItemID = pTarItem->GetID();
		if (pSrcItem->GetID() == pTarItem->GetID())
		{
			if (pTarItem->m_pItemData->m_nStackAmount < pTarItem->m_nAmount + nQuantity)
			{
				nQuantity = pTarItem->m_pItemData->m_nStackAmount - pTarItem->m_nAmount;
			}
		}
	}

	XPostItem_MoveItem(SLOTTYPE_GUILDSTORAGE, nSrcSlotID, pSrcItem->GetID(), SLOTTYPE_GUILDSTORAGE, nTarSlotID, nTarItemID, nQuantity);	

	return true;
}

bool XGlueGameGuildStorage::DepositGuildItem( int nSrcSlotID, int nTarSlotID, int nQuantity) const
{
	XItem* pSrcItem = gvar.MyInfo.Inventory.GetItem( nSrcSlotID);
	if ( pSrcItem == NULL)	return false;

	int nTarItemID = 0;
	XItem* pTarItem = gvar.MyInfo.GuildStorage.GetItem( nTarSlotID);
	if ( pTarItem)
	{
		nTarItemID = pTarItem->GetID();
		if (pSrcItem->GetID() == pTarItem->GetID())
		{
			if (pTarItem->m_pItemData->m_nStackAmount < pTarItem->m_nAmount + nQuantity)
			{
				nQuantity = pTarItem->m_pItemData->m_nStackAmount - pTarItem->m_nAmount;
			}
		}
	}

	XPostItem_MoveItem(SLOTTYPE_INVENTORY, nSrcSlotID, pSrcItem->GetID(), SLOTTYPE_GUILDSTORAGE, nTarSlotID, nTarItemID, nQuantity);	

	return true;
}

bool XGlueGameGuildStorage::WithdrawGuildItem( int nSrcSlotID, int nTarSlotID, int nQuantity) const
{
	XItem* pSrcItem = gvar.MyInfo.GuildStorage.GetItem( nSrcSlotID);
	if ( pSrcItem == NULL)	return false;

	int nTarItemID = 0;
	XItem* pTarItem = gvar.MyInfo.Inventory.GetItem( nTarSlotID);
	if ( pTarItem)
	{
		nTarItemID = pTarItem->GetID();
		if (pSrcItem->GetID() == pTarItem->GetID())
		{
			if (pTarItem->m_pItemData->m_nStackAmount < pTarItem->m_nAmount + nQuantity)
			{
				nQuantity = pTarItem->m_pItemData->m_nStackAmount - pTarItem->m_nAmount;
			}
		}
	}

	XPostItem_MoveItem(SLOTTYPE_GUILDSTORAGE, nSrcSlotID, pSrcItem->GetID(), SLOTTYPE_INVENTORY, nTarSlotID, nTarItemID, nQuantity);	

	return true;
}

//bool XGlueGameGuildStorage::DropGuildStorageItem(int nSlotID) const
//{
//	if(XGetMyPlayer()->IsUsableBreakablePartsWeapon())
//		return false;
//
//	XItem* pItem = gvar.MyInfo.GuildStorage.GetItem( nSlotID);
//	if ( pItem)		XPostItem_DropItem(SLOTTYPE_GUILDSTORAGE, pItem->m_nSlotID, pItem->m_nAmount);
//
//	return true;
//}