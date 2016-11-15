#include "stdafx.h"
#include "XGlueGameItemFunction.h"
#include "XItemManager.h"
#include "XStrings.h"
#include "XCommonFunction.h"
#include "XMyPlayer.h"
#include "XPost_Item.h"
#include "XPost_NPCShop.h"

//아이템 인덱스로 아이템 정보찾기.
const char* XGlueGameItemFunction::GetItemName( int nItemID )
{
	gvar.Temp.szTemp = MLocale::ConvUTF16ToAnsi(XCommonFunction::GetItemName(nItemID));

	return gvar.Temp.szTemp.c_str();
}

int XGlueGameItemFunction::GetItemPrice( int nItemID )
{
	XItemData* pItemData = info.item->GetItemData(nItemID);
	if( pItemData == NULL) return 0;

	return pItemData->m_nBaseBuyingPrice;
}

const char* XGlueGameItemFunction::GetItemType( int nItemID )
{
	XItemData* pItemData = info.item->GetItemData(nItemID);
	
	if( pItemData == NULL ||
		pItemData->m_ItemType <= ITEMTYPE_NONE ||
		pItemData->m_ItemType >= ITEMTYPE_MAX)
	{
		gvar.Temp.szTemp = MLocale::ConvUTF16ToAnsi(XStrings::ItemTypeToString(ITEMTYPE_NONE));
	}
	else
	{
		gvar.Temp.szTemp = MLocale::ConvUTF16ToAnsi(XStrings::ItemTypeToString(pItemData->m_ItemType));
	}

	return gvar.Temp.szTemp.c_str();
}

const char* XGlueGameItemFunction::GetItemEquipmentType( int nItemID )
{
	XItemData* pItemData = info.item->GetItemData(nItemID);
	if (pItemData == NULL ||
		pItemData->m_nArmorType >= ARMOR_MAX ||
		pItemData->m_nArmorType <= ARMOR_NONE)
	{
		gvar.Temp.szTemp = MLocale::ConvUTF16ToAnsi(XStrings::ArmorTypeToString(ARMOR_NONE));
	}
	else
	{
		gvar.Temp.szTemp = MLocale::ConvUTF16ToAnsi(XStrings::ArmorTypeToString(pItemData->m_nArmorType));
	}

	return gvar.Temp.szTemp.c_str();
}

int XGlueGameItemFunction::GetItemAP(int nItemID)
{
	XItemData* pItemData = info.item->GetItemData(nItemID);
	if( pItemData == NULL) return 0;

	return pItemData->m_nAP;
}

int XGlueGameItemFunction::GetItemSlotType( int nItemID )
{
	XItemData* pItemData = info.item->GetItemData(nItemID);
	if( pItemData == NULL || 
		pItemData->m_nItemSlot < ITEMSLOT_HEAD ||
		pItemData->m_nItemSlot > ITEMSLOT_MAX)
	{
		_ASSERT(0);
		return ITEMSLOT_HEAD;
	}

	return pItemData->m_nItemSlot;
}

int XGlueGameItemFunction::GetItemStackSize( int nItemID)
{
	XItemData* pItemData = info.item->GetItemData(nItemID);
	if( pItemData == NULL) return 0;

	return pItemData->m_nStackAmount;
}

int XGlueGameItemFunction::GetItemMaxDurability( int nItemID)
{
	XItemData* pItemData = info.item->GetItemData(nItemID);
	if( pItemData == NULL) return 0;

	return pItemData->m_nMaxDurability;
}

void XGlueGameItemFunction::UseItem(const char* szuid)
{	
	// ! GetItem(MUID) 함수가 제거되어, slotID로만 접근할 수 있습니다. - praptor, 2010.03.03
	// ! 사용하지 않는 함수로 판단되어 빌드에러 막고자, 이하 주석처리합니다. - praptor, 2010.03.03

	//if( szuid == NULL) return;

	//MUID uidItem = _atoi64(szuid);
	//XItem* pItem = gvar.MyInfo.Inventory.GetItem(uidItem);
	//if( pItem == NULL) return;

	//XModuleMyControl* pModuleMyControl = gvar.Game.pMyPlayer->GetModuleMyControl();
	//if (NULL == pModuleMyControl) return;

	//pModuleMyControl->DoActionUseItem(pItem->m_nSlotID);
}

void XGlueGameItemFunction::DropItem(const char* szuid)
{
	// ! GetItem(MUID) 함수가 제거되어, slotID로만 접근할 수 있습니다. - praptor, 2010.03.03
	// ! 사용하지 않는 함수로 판단되어 빌드에러 막고자, 이하 주석처리합니다. - praptor, 2010.03.03

	//if( szuid == NULL) return;

	//MUID uidItem = _atoi64(szuid);
	//XItem* pItem = gvar.MyInfo.Inventory.GetItem(uidItem);
	//if( pItem == NULL) return;

	//XPostItem_DropItem(pItem->m_nSlotID, 1);
}

void XGlueGameItemFunction::BuyItem( int nItemID, int nCount )
{
	_ASSERT(nItemID);
	if( nItemID == 0) return;

	XPostNPCShop_Buy(nItemID, nCount);
}

void XGlueGameItemFunction::SellItem( const char* szuid, int nCount )
{
	// ! GetItem(MUID) 함수가 제거되어, slotID로만 접근할 수 있습니다. - praptor, 2010.03.03
	// ! 사용하지 않는 함수로 판단되어 빌드에러 막고자, 이하 주석처리합니다. - praptor, 2010.03.03

	//if( szuid == NULL) return;

	//if (true == XGetInteractionInfo().InteractionTargetUID.IsInvalid()) return;

	//MUID uidItem = _atoi64(szuid);
	//XItem* pInvenItem = gvar.MyInfo.Inventory.GetItem(uidItem);
	//if (NULL == pInvenItem) return;

	//XPostNPCShop_Sell(pInvenItem->m_nSlotID, nCount);
}

const char* XGlueGameItemFunction::GetItemIconName( int nItemID )
{
	XItemData* pItemData = info.item->GetItemData(nItemID);
	if( pItemData == NULL) return "";
	
	gvar.Temp.szTemp = MLocale::ConvUTF16ToAnsi(pItemData->m_strIconName.c_str());

	return gvar.Temp.szTemp.c_str();
}