#include "stdafx.h"
#include "XGlueGame_Storage.h"
#include "XPost_Storage.h"
#include "XPost_Item.h"
#include "XMyPlayer.h"


//////////////////////////////////////////////////////////////////////////
// Storage

int XGlueGameStorage::GetStorageItemID( int nSlotID ) const
{
	XItem* pItem =gvar.MyInfo.Storage.GetItem( nSlotID);
	if(pItem == NULL) return 0;

	return pItem->m_pItemData->m_nID;
}

int XGlueGameStorage::GetStorageItemQuantity( int nSlotID ) const
{
	XItem* pItem =gvar.MyInfo.Storage.GetItem( nSlotID);
	if(pItem == NULL) return 0;

	return pItem->m_nAmount;
}

bool XGlueGameStorage::DepositMoney( int nMoney ) const
{
	XPostStorage_DepositMoney(nMoney);
	return true;
}

bool XGlueGameStorage::WithdrawMoney( int nMoney ) const
{
	XPostStorage_WithdrawMoney(nMoney);
	return true;
}

bool XGlueGameStorage::MoveStorageItem( int nSrcSlotID, int nTarSlotID) const
{
	XItem* pSrcItem = gvar.MyInfo.Storage.GetItem( nSrcSlotID);
	if ( pSrcItem == NULL)	return false;

	int nQuantity = pSrcItem->m_nAmount;

	int nTarItemID = 0;
	XItem* pTarItem = gvar.MyInfo.Storage.GetItem( nTarSlotID);
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

	XPostItem_MoveItem(SLOTTYPE_STORAGE, nSrcSlotID, pSrcItem->GetID(), SLOTTYPE_STORAGE, nTarSlotID, nTarItemID, nQuantity);	

	return true;
}

bool XGlueGameStorage::DepositItem( int nSrcSlotID, int nTarSlotID, int nQuantity) const
{
	XItem* pSrcItem = gvar.MyInfo.Inventory.GetItem( nSrcSlotID);
	if ( pSrcItem == NULL)	return false;

	int nTarItemID = 0;
	XItem* pTarItem = gvar.MyInfo.Storage.GetItem( nTarSlotID);
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

	XPostItem_MoveItem(SLOTTYPE_INVENTORY, nSrcSlotID, pSrcItem->GetID(), SLOTTYPE_STORAGE, nTarSlotID, nTarItemID, nQuantity);	

	return true;
}

bool XGlueGameStorage::WithdrawItem( int nSrcSlotID, int nTarSlotID, int nQuantity) const
{
	XItem* pSrcItem = gvar.MyInfo.Storage.GetItem( nSrcSlotID);
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

	XPostItem_MoveItem(SLOTTYPE_STORAGE, nSrcSlotID, pSrcItem->GetID(), SLOTTYPE_INVENTORY, nTarSlotID, nTarItemID, nQuantity);	

	return true;
}

bool XGlueGameStorage::DropStorageItem(int nSlotID) const
{
	if(XGetMyPlayer()->IsUsableBreakablePartsWeapon())
		return false;

	XItem* pItem = gvar.MyInfo.Storage.GetItem( nSlotID);
	if ( pItem)		XPostItem_DropItem(SLOTTYPE_STORAGE, pItem->m_nSlotID, pItem->m_nAmount);

	return true;
}