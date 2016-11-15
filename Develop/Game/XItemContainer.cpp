#include "stdafx.h"
#include "XItemContainer.h"
#include "XItemManager.h"

XTradeItemContainer::XTradeItemContainer() : m_nNowItemIdx(0)
{
}


XTradeItemContainer::~XTradeItemContainer()
{
}

XTradeItemContainer& XTradeItemContainer::operator =(const XTradeItemContainer& rhs)
{
	_ASSERT(0);

	if (this == &rhs)
		return *this;

	m_uidOwner = rhs.m_uidOwner;

	m_vContains = rhs.m_vContains;

	return *this;
}

XItem* XTradeItemContainer::FirstItem()
{
	m_nNowItemIdx = 0;
	if (m_nNowItemIdx < m_vContains.size())
		return m_vContains[m_nNowItemIdx];
	return NULL;
}

XItem* XTradeItemContainer::NextItem()
{
	m_nNowItemIdx++;
	if (m_nNowItemIdx < m_vContains.size())
		return m_vContains[m_nNowItemIdx];
	return NULL;
}

size_t XTradeItemContainer::GetSize()
{
	return  m_vContains.size();
}

void XTradeItemContainer::PushItem(XItem *item)
{
	if (item == NULL || item->m_pItemData == NULL)
	{
		_ASSERT(0 && "XItemContainer::PushItem - item is NULL!");
		return;
	}

	m_vContains.push_back(item);
}

bool XTradeItemContainer::PopItem(XItem* item)
{
	if (item == NULL)
	{
		_ASSERT(0 && "XItemContainer::PopItem - item is NULL!");
		return false;
	}

	for (XItemVector::iterator i=m_vContains.begin(); i!=m_vContains.end(); ++i)
	{
		if (*i == item)
		{
			m_vContains.erase(i);
			return true;
		}
	}
	return false;
}

XItem* XTradeItemContainer::InsertItem(const TD_ITEM& tdItem)
{
	if (0 > tdItem.m_nQuantity) return NULL;

	XItem* pGetItem = GetItem(tdItem.m_nSlotID);
	if (NULL != pGetItem) return NULL;

	XItem* pNewItem = info.item->MakeNewItem(tdItem.m_nItemID);
	pNewItem->m_pItemData = info.item->GetItemData(tdItem.m_nItemID);
	pNewItem->m_nSlotID = tdItem.m_nSlotID;
	PushItem(pNewItem);

	return pNewItem;
}

XItem* XTradeItemContainer::InsertItem(const TD_ITEM_INSERT& tdItemInsert)
{
	if (0 > tdItemInsert.m_nAmount) return NULL;

	XItem* pGetItem = GetItem(tdItemInsert.m_nSlotID);
	if (NULL != pGetItem) return NULL;

	XItem* pNewItem = info.item->MakeNewItem(tdItemInsert.m_nID);
	pNewItem->m_pItemData = info.item->GetItemData(tdItemInsert.m_nID);
	pNewItem->m_nSlotID = tdItemInsert.m_nSlotID;
	pNewItem->m_nAmount = tdItemInsert.m_nAmount;
	pNewItem->m_nDurability = tdItemInsert.m_nDurability;
	pNewItem->m_nDyedColor = tdItemInsert.m_nColor;
	pNewItem->m_nSoulCount = tdItemInsert.m_nSoulAmount;

	PushItem(pNewItem);

	return pNewItem;
}

bool XTradeItemContainer::IncreaseItemAmount(int nSlotID, int nAmount)
{
	XItem* pGetItem = GetItem(nSlotID);
	if (NULL == pGetItem) return false;

	pGetItem->m_nAmount += nAmount;

	return true;
}

XItem* XTradeItemContainer::GetItem(int index)
{
	if( index < 0 || index >= (int)m_vContains.size()) return NULL;

	return m_vContains[index];
}

void XTradeItemContainer::Reset()
{
	m_nNowItemIdx=0;
	m_vContains.clear();
	m_uidOwner=0;
}

XItemContainer::XItemContainer( size_t slotSize )
: m_vecItem(slotSize)
{
	Clear();
}

XItemContainer::~XItemContainer()
{

}

void XItemContainer::Clear()
{
	for (int i=0; i<GetSlotSize(); i++)
		SetSlotItem(i,  NULL);
}

void XItemContainer::SetSlotItem( int nSlotID, XItem* pItem )
{
	if (nSlotID < 0 || nSlotID >= GetSlotSize()) return;

	if (pItem != NULL)	pItem->m_nSlotID = nSlotID;
	m_vecItem[nSlotID] = pItem;
}

void XItemContainer::PushItem( int nSlotID, XItem* pItem )
{
	if (pItem == NULL) return;

	SetSlotItem(nSlotID, pItem);
}

XItem* XItemContainer::PopItem(int nSlotID)
{
	if (nSlotID < 0 || nSlotID >= BASE_INVENTORY_SIZE)
	{
		mlog("슬롯ID값이 %d 입니다.", nSlotID);
		return NULL;
	}

	XItem* pItem = m_vecItem[nSlotID];
	if(pItem == NULL) return NULL;

	m_vecItem[nSlotID] = NULL;
	pItem->m_nSlotID = INVALID_ID;

	return pItem;
}

bool XItemContainer::UpdateItemAmount( int nSlotID, int nAmount )
{
	if(m_vecItem[nSlotID] != NULL)
	{
		m_vecItem[nSlotID]->m_nAmount += nAmount;

		if (m_vecItem[nSlotID]->m_nAmount == 0)
		{
			PopItem(nSlotID);
		}

		return true;
	}

	return false;
}

// 아이템 아이디에 맞는 첫번째 아이템;;;
XItem* XItemContainer::GetItemByID(int nItemID)
{
	for (int i=0; i<GetSlotSize(); i++)
	{
		if(m_vecItem[i] != NULL && m_vecItem[i]->m_pItemData->m_nID == nItemID)
		{
			return m_vecItem[i];
		}
	}

	return NULL;
}

int XItemContainer::GetItemAmountByID(int nItemID)
{
	int nSum = 0;
	for (int i=0; i<GetSlotSize(); i++)
	{
		if(m_vecItem[i] != NULL && m_vecItem[i]->m_pItemData->m_nID == nItemID)
		{
			nSum += m_vecItem[i]->m_nAmount;
		}
	}

	return nSum;
}

XItem* XItemContainer::InsertItem( const TD_ITEM_INSERT& tdItemInsert )
{
	if (0 > tdItemInsert.m_nAmount) return NULL;

	XItem* pGetItem = GetItem(tdItemInsert.m_nSlotID);
	if (NULL != pGetItem) return NULL;

	XItem* pNewItem = info.item->MakeNewItem(tdItemInsert.m_nID);
	if ( pNewItem == NULL) return NULL;

	pNewItem->m_pItemData = info.item->GetItemData(tdItemInsert.m_nID);
	pNewItem->m_nSlotID	= tdItemInsert.m_nSlotID;
	pNewItem->m_nAmount = tdItemInsert.m_nAmount;
	pNewItem->m_nDurability = tdItemInsert.m_nDurability;
	pNewItem->m_nDyedColor = tdItemInsert.m_nColor;
	pNewItem->m_nSoulCount = tdItemInsert.m_nSoulAmount;
	pNewItem->m_bNewItem = true;

	PushItem(tdItemInsert.m_nSlotID, pNewItem);

	return pNewItem;
}

XItem* XItemContainer::InsertItem(int nSlotID, XItem* pItem, int nAmount )
{
	XItem* pNewItem = info.item->MakeNewItem(pItem, nAmount);
	if (NULL == pNewItem) return NULL;

	PushItem(nSlotID, pNewItem);

	return pNewItem;
}

bool XItemContainer::IncreaseItemAmount(int nSlotID, int nAmount )
{
	XItem* pGetItem = GetItem(nSlotID);
	if (NULL == pGetItem) return false;

	pGetItem->m_nAmount += nAmount;

	return true;
}

// 아이템의 갯수가 0이면 서버에서 아이템 삭제 커맨드가 온다.
bool XItemContainer::DecreaseItemAmount(int nSlotID, int nAmount )
{
	XItem* pGetItem = GetItem(nSlotID);
	if (NULL == pGetItem) return false;

	pGetItem->m_nAmount -= nAmount;

	return true;
}

int XItemContainer::GetSize()
{
	int count = 0;
	for (int i=0; i<GetSlotSize(); i++)
	{
		if(m_vecItem[i] != NULL)
		{
			++count;
		}
	}

	return count;
}

void XItemContainer::ChangeSlot(int nCurSlotID, int nTargetSlotID )
{
	XItem* pItem = GetItem(nCurSlotID);
	if (pItem == NULL) return;

	if (PopItem(nCurSlotID))
	{
		XItem* pTargetItem = GetItem(nTargetSlotID);
		if (pTargetItem)
		{
			PushItem(nCurSlotID, pTargetItem);
		}

		PushItem(nTargetSlotID, pItem);
	}
}

void XItemContainer::Merge(int nCurSlotID, int nTargetSlotID, int nMergeAmount )
{
	UpdateItemAmount(nCurSlotID, -nMergeAmount);
	UpdateItemAmount(nTargetSlotID, nMergeAmount);
}

XItem* XItemContainer::GetItem( int nSlotID )
{
	if (nSlotID < 0 || nSlotID >=GetSlotSize()) return NULL;

	return m_vecItem[nSlotID];
}


vector<XItem*> XItemContainer::GetAllItem()
{
	vector<XItem*> vecItem;

	for (int i=0; i<GetSlotSize(); i++)
	{
		if(m_vecItem[i])  
		{
			vecItem.push_back(m_vecItem[i]);
		}
	}

	return vecItem;
}

void XItemContainer::ClearAllNewItemFlag()
{
	const int nMaxSize = GetSlotSize();

	for (int i = 0; i < nMaxSize; i++)
	{
		if (m_vecItem[i])
		{
			m_vecItem[i]->m_bNewItem = false;
		}
	}
}

bool XItemContainer::IsEmpty()
{
	for (int i=0; i<GetSlotSize(); i++)
	{
		if(m_vecItem[i] != NULL)
		{
			return false;
		}
	}

	return true;
}

bool XItemContainer::IsHaveItem( int nItemID )
{
	XItem* pItem = GetItemByID(nItemID);
	if (pItem) return true;

	return false;
}