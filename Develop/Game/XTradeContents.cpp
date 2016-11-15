#include "stdafx.h"
#include "XTradeContents.h"
#include "XGameFrameID.h"
#include "XGlobal.h"
#include "XScript.h"
#include "XGameFrameManager.h"
#include "XItemManager.h"

XTradeContainer::XTradeContainer() : XTradeItemContainer()
{
	m_vContains.reserve(MAX_TRADE_ITEM_SPACE);
}

XTradeContainer::~XTradeContainer()
{
	DeleteAll();
}

void XTradeContainer::DeleteAll()
{
	for (size_t i=0; i<m_vContains.size(); i++)
	{
		SAFE_DELETE(m_vContains[i]);
	}

	m_vContains.clear();
	m_nNowItemIdx = 0;
}

void XTradeContainer::PushItem(XItem* item)
{
	_ASSERT(NULL != item && "XTradeContainer::PushItem item is NULL");
	if (NULL == item) return;
	if (MAX_TRADE_ITEM_SPACE <= m_vContains.size()) return;

	m_vContains.push_back(item);
}

XItem* XTradeContainer::InsertItem(const TD_TRADE_ITEM_PUTUP& tdItemPutUp)
{
	if (0 > tdItemPutUp.m_nAmount) return NULL;

	XItem* pGetItem = FindTradeItemForInvenSlotID(tdItemPutUp.m_nSlotID);
	if (NULL == pGetItem)
	{
		XItem* pNewItem = new XItem();
		if (pNewItem == NULL)
		{
			_ASSERT(0 && "new alloc failed!");
			return NULL;
		}

		pNewItem->m_pItemData = info.item->GetItemData(tdItemPutUp.m_nID);
		pNewItem->m_nSlotID = tdItemPutUp.m_nSlotID;
		pNewItem->m_nAmount = tdItemPutUp.m_nAmount;
		pNewItem->m_nDurability = tdItemPutUp.m_nDurability;
		pNewItem->m_nDyedColor = tdItemPutUp.m_nColor;

		PushItem(pNewItem);

		return pNewItem;
	}
	else
	{
		if (INT_MAX - tdItemPutUp.m_nAmount < pGetItem->m_nAmount) return false;

		pGetItem->m_nAmount += tdItemPutUp.m_nAmount;

		return pGetItem;
	}
}

bool XTradeContainer::DecItem(const TD_TRADE_ITEM_PUTDOWN& tdItemPutDown)
{
	if (0 > tdItemPutDown.m_nAmount) return false;

	XItem* pGetItem = FindTradeItemForInvenSlotID(tdItemPutDown.m_nSlotID);
	if (NULL == pGetItem) return false;

	if (tdItemPutDown.m_nAmount > pGetItem->m_nAmount) return false;

	pGetItem->m_nAmount -= tdItemPutDown.m_nAmount;

	if (0 == pGetItem->m_nAmount)
	{
		XTradeContainer::PopItem(pGetItem);
	}

	return true;
}

void XTradeContainer::MakeTD_Item(vector<TD_ITEM>& vecTDItem)
{
	for(XItem* item = FirstItem();
		item != NULL;
		item = NextItem())
	{
		TD_ITEM tdItem;
		tdItem.m_nDurability = item->m_nDurability;
		tdItem.m_nQuantity = item->m_nAmount;
		tdItem.m_nSlotID = item->m_nSlotID;
		tdItem.m_nItemID = item->m_pItemData->m_nID;

		vecTDItem.push_back(tdItem);
	}
}

MWLua::table XTradeContainer::GetTradeItemTable()
{
	MWLua::table t(global.script->GetLua()->GetState());
	if( m_vContains.size() == 0) return t;

	char buffer[32];
	int i =1;
	XItemVector::iterator itor = m_vContains.begin();
	for (; itor != m_vContains.end(); itor++, i++)
	{
		XItem* pItem = (*itor);
		MWLua::table item(global.script->GetLua()->GetState());
		item.set("id"		, pItem->m_pItemData->m_nID);
		item.set("amount"	, pItem->m_nAmount);

		sprintf_s(buffer,"%d", i);
		t.set(buffer, item);
	}

	return t;
}

XItem* XTradeContainer::FindTradeItemForInvenSlotID(int nSlotID)
{
	for(XItem* pItem = FirstItem(); pItem != NULL; pItem = NextItem())
	{
		if (pItem->m_nSlotID == nSlotID)
		{
			return pItem;
		}
	}

	return NULL;
}

//////////////////////////////////////
XTradeContents::XTradeContents()
: m_bMyConfirm(false)
, m_bOtherConfirm(false)
, m_nMySilver(0)
, m_nOtherSilver(0)
, m_bTrade(false)
{
}

XTradeContents::~XTradeContents()
{
	Reset();
}

void XTradeContents::Reset()
{
	m_MyTradeContainer.DeleteAll();
	m_OtherTradeContainer.DeleteAll();
	m_nMySilver = 0;
	m_nOtherSilver = 0;
	m_bMyConfirm = false;
	m_bOtherConfirm = false;

	m_bTrade = false;
}