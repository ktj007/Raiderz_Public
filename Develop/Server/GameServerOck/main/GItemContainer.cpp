#include "stdafx.h"
#include "GItemContainer.h"
#include "GItem.h"
#include "GEntityPlayer.h"
#include "AStlUtil.h"
#include "GItemAdder.h"

GItemContainer::GItemContainer(size_t slotSize, GEntityPlayer* pOwnerPlayer)
: m_vecItemSlot(slotSize)
, m_nItemCount(0)
, m_pOwnerPlayer(pOwnerPlayer)
, m_vecSlotReserved(MAX_INVENTORY_SIZE)
, m_nReserveCount(0)
{
	// do nothing
}

GItemContainer::~GItemContainer()
{
	Clear();
}

void GItemContainer::Clear(void)
{
	for(size_t i = 0; i < m_vecItemSlot.size(); i++)
	{
		SAFE_DELETE(m_vecItemSlot[i]);
	}

	m_nItemCount = 0;

	for(size_t i = 0; i < m_vecSlotReserved.size(); i++)
	{
		m_vecSlotReserved[i] = false;
	}

	m_nReserveCount = 0;
}

GItem* GItemContainer::IncreaseAmount( int nSlotID, int nAmount )
{
	GItem* pItem = GetItem(nSlotID);
	if (NULL == pItem) return NULL;

	pItem->m_nAmount += nAmount;

	return pItem;
}

GItem* GItemContainer::DecreaseAmount( int nSlotID, int nAmount )
{
	GItem* pItem = GetItem(nSlotID);
	if (NULL == pItem) return NULL;

	pItem->m_nAmount -= nAmount;

	return pItem;
}

GItem* GItemContainer::SetAmount( int nSlotID, int nAmount )
{
	GItem* pItem = GetItem(nSlotID);
	if (NULL == pItem) return NULL;

	pItem->m_nAmount = nAmount;

	return pItem;
}

bool GItemContainer::PushItem(int nSlotID, GItem* pItem)
{
	if (pItem == NULL) return false;
	if (CheckSlotID(nSlotID) == false) return false;

#if defined _DEBUG

	// pItem 중복 검사
	vector<GItem*>::iterator it = find(m_vecItemSlot.begin(), m_vecItemSlot.end(), pItem);
	VALID_RET(it == m_vecItemSlot.end(), false);

#endif

	pItem->m_nSlotID = nSlotID;	

	m_vecItemSlot[nSlotID] = pItem;
	m_nItemCount++;

	return true;
}

GItem* GItemContainer::PopItem(int nSlotID)
{
	if (CheckSlotID(nSlotID) == false) return NULL;
	if (m_vecItemSlot[nSlotID] == NULL)	return NULL;

	GItem* pItem = m_vecItemSlot[nSlotID];
	pItem->m_nSlotID = INVALID_ID;
	
	m_vecItemSlot[nSlotID] = NULL;
	m_nItemCount--;

	return pItem;
}

GItem* GItemContainer::GetItem(int nSlotID)  const
{
	if (CheckSlotID(nSlotID) == false) return NULL;
	if (m_vecItemSlot[nSlotID] == NULL)	return NULL;

	return m_vecItemSlot[nSlotID];
}

int GItemContainer::GetItemCount(void) const
{
	return m_nItemCount;
}

int GItemContainer::GetSlotSize(void) const
{
	return m_vecItemSlot.size();
}

bool GItemContainer::IsEmpty(void) const
{
	return m_nItemCount == 0;
}

bool GItemContainer::IsFull(void) const
{
	return GetSlotSize() <= (GetItemCount() + GetReserveSlotCount());
}

GEntityPlayer* GItemContainer::GetOwnerPlayer(void)
{
	return m_pOwnerPlayer;
}

GItem* GItemContainer::GetItem(int nItemID, int nFindStartSlotID)
{
	VALID_RET(0 <= nFindStartSlotID, NULL);

	if (IsEmpty())	return NULL;

	if (GetSlotSize() <= nFindStartSlotID)
	{
		return NULL;
	}

	for (int i = nFindStartSlotID; i < GetSlotSize(); i++)
	{
		GItem* pItem = GetItem(i);
		if (pItem == NULL)	continue;

		if (pItem->m_pItemData->m_nID == nItemID)
		{
			return pItem;
		}
	}

	return NULL;
}

GItem* GItemContainer::GetItem(MUID uidItem) const
{
	if (IsEmpty())	return NULL;

	for (int i = 0; i < GetSlotSize(); i++)
	{
		GItem* pItem = GetItem(i);
		if (pItem == NULL)	continue;
		
		if (uidItem == pItem->m_uidID)
		{
			return pItem;
		}
	}

	return NULL;
}

GItem* GItemContainer::GetItem(int64 nIUID) const
{
	if (IsEmpty())	return NULL;

	for (int i = 0; i < GetSlotSize(); i++)
	{
		GItem* pItem = GetItem(i);
		if (pItem == NULL)	continue;
		
		if (nIUID == pItem->m_nIUID)
		{
			return pItem;
		}
	}

	return NULL;
}

void GItemContainer::CollectExpiringItem(int nCharPlayTimeMin, vector<GItem*>& outvecItem)
{
	if (IsEmpty())	return;

	int nSize = (int)m_vecItemSlot.size();

	for (int i = 0; i < nSize; i++)
	{
		GItem* pItem = GetItem(i);
		if (pItem == NULL)	continue;

		if (pItem->IsExpired(nCharPlayTimeMin))
		{
			outvecItem.push_back(pItem);
		}
	}
}

void GItemContainer::CollectItemByID(int nID, vector<GItem*>& outvecItem)
{
	if (IsEmpty())	return;

	for (int i = 0; i < GetSlotSize(); i++)
	{
		GItem* pItem = GetItem(i);
		if (pItem == NULL)	continue;

		if (nID == pItem->m_pItemData->m_nID)
		{
			outvecItem.push_back(pItem);
		}
	}
}

void GItemContainer::CollectAllItem( vector<GItem*>& outvecitem )
{
	if (IsEmpty())	return;

	for (int i = 0; i < GetSlotSize(); i++)
	{
		GItem* pItem = GetItem(i);
		if (pItem == NULL)	continue;

		outvecitem.push_back(pItem);
	}
}

int GItemContainer::CollectItemUntilAmount(int nItemID, int nNeedAmount, GITEM_STACK_AMT_VEC& outvecItemStackAmt)
{
	int				nRemainAmt = nNeedAmount;
	int				RemainStackAmt;
	GITEM_STACK_AMT ItemStackAmt;

	if (IsEmpty())	return nRemainAmt;

	for (int i = 0; i < GetSlotSize(); i++)
	{
		GItem* pItem = GetItem(i);
		
		if (pItem == NULL)	continue;
		if (nItemID != pItem->m_pItemData->m_nID)	continue;
		
		if (nRemainAmt <= pItem->GetAmount())
		{
			RemainStackAmt				= pItem->GetAmount() - nRemainAmt;
			ItemStackAmt.nModStackAmt	= nRemainAmt;
			nRemainAmt					= 0;			
		}
		else
		{
			RemainStackAmt	= 0;
			nRemainAmt		-= pItem->GetAmount();

			ItemStackAmt.nModStackAmt = pItem->GetAmount();
		}

		ItemStackAmt.nIUID			= pItem->m_nIUID;
		ItemStackAmt.nSlotType		= pItem->m_nSlotType;
		ItemStackAmt.nSlotID		= pItem->m_nSlotID;
		ItemStackAmt.nItemID		= pItem->m_pItemData->m_nID;
		ItemStackAmt.nToStackAmt	= RemainStackAmt;

		outvecItemStackAmt.push_back(ItemStackAmt);

		if (nRemainAmt <= 0)
		{
			return nRemainAmt;
		}	
	}

	return nRemainAmt;
}

GItem* GItemContainer::GetMergeableItem( int nItemID, int nMergeAmt )
{
	if (IsEmpty())	return NULL;

	for (int i = 0; i < GetSlotSize(); i++)
	{
		GItem* pItem = GetItem(i);
		
		if (pItem == NULL)	continue;
		if (nItemID != pItem->m_pItemData->m_nID)	continue;

		int nMarginAmount = pItem->m_pItemData->m_nStackAmount - pItem->GetAmount();		
		if (nMergeAmt <= nMarginAmount)
		{
			return pItem;
		}
	}

	return NULL;
}

int GItemContainer::GetItemCountByID( int id )
{
	int nSumQuantity = 0;

	if (IsEmpty())	return nSumQuantity;

	for (int i = 0; i < GetSlotSize(); i++)
	{
		GItem* pItem = GetItem(i);
		
		if (pItem == NULL)	continue;
		if (id != pItem->m_pItemData->m_nID)	continue;

		if (INT_MAX - pItem->GetAmount() < nSumQuantity) return INT_MAX;

		nSumQuantity += pItem->GetAmount();
	}

	return nSumQuantity;
}

bool GItemContainer::CheckSlotID( int nSlotID ) const
{
	if (nSlotID == INVALID_ID)	return false;
	if (nSlotID < 0)			return false;
	if (static_cast<int>(m_vecItemSlot.size()) <= nSlotID)	return false;	

	return true;	
}

int GItemContainer::ReserveEmptySlot(int nFindStartSlotID)
{
	VALID_RET(0 <= nFindStartSlotID, INVALID_ID);

	if (GetSlotSize() <= nFindStartSlotID)
	{
		return INVALID_ID;
	}

	if (IsFull())
	{
		return INVALID_ID;
	}

	for(int i = nFindStartSlotID; i < GetSlotSize(); i++)
	{
		// 비어있고 예약없으면, 예약하고 알려줌
		if (NULL == GetItem(i) && !IsReservedSlot(i))
		{
			ReserveSlot(i);
			return i;
		}
	}

	return INVALID_ID;
}

void GItemContainer::ReserveSlot( int nSlotID )
{
	if (nSlotID < 0 || GItemContainer::GetSlotSize() <= nSlotID)	return;
	
	if (m_vecSlotReserved[nSlotID])
	{
		return;
	}

	m_vecSlotReserved[nSlotID] = true;
	m_nReserveCount++;
}

void GItemContainer::CancelReserveSlot( int nSlotID )
{
	if (nSlotID < 0 || GItemContainer::GetSlotSize() <= nSlotID)	return;
	
	if (!m_vecSlotReserved[nSlotID])
	{
		return;
	}

	m_vecSlotReserved[nSlotID] = false;
	m_nReserveCount--;
}

bool GItemContainer::IsReservedSlot(int nSlotID)
{
	if (nSlotID < 0 || GItemContainer::GetSlotSize() <= nSlotID)	return false;
	
	return m_vecSlotReserved[nSlotID];
}

int GItemContainer::GetEmptySlotCount(void) const
{
	return GetSlotSize() - (GetItemCount() + GetReserveSlotCount());
}

bool GItemContainer::HasItemAddableSlot(const vector<CHECK_ADDABLE_ITEM>& vecCheckAddableItem)
{
	map<int,int> mapMergeReservedSlot;
	int nMergeReserervedCount = 0;

	for each(const CHECK_ADDABLE_ITEM& checkAddableItem in vecCheckAddableItem)
	{
		for (int i = 0; i < GetSlotSize(); i++)
		{
			GItem* pItem = GetItem(i);
			if (pItem == NULL)	continue;
			if (checkAddableItem.m_nItemID != pItem->m_pItemData->m_nID) continue;

			map<int,int>::iterator itor = mapMergeReservedSlot.find(i);
			if (mapMergeReservedSlot.end() == itor)
			{
				if (pItem->m_pItemData->m_nStackAmount >= pItem->GetAmount() + checkAddableItem.m_nAmount)
				{
					mapMergeReservedSlot.insert(map<int,int>::value_type(i, checkAddableItem.m_nAmount));
					nMergeReserervedCount++;
				}
			}
			else
			{
				int& nMergeReservedAmount = itor->second;
				if (pItem->m_pItemData->m_nStackAmount >= pItem->GetAmount() + checkAddableItem.m_nAmount + nMergeReservedAmount)
				{
					nMergeReservedAmount += checkAddableItem.m_nAmount;
					nMergeReserervedCount++;
				}
			}
		}
	}

	return static_cast<int>(vecCheckAddableItem.size()) <= nMergeReserervedCount + GetEmptySlotCount();
}

void GItemContainer::ReserveDummySlot( int nSlotID )
{
	ReserveSlot(nSlotID);
	m_setDummySlotReserved.insert(nSlotID);
}

bool GItemContainer::IsReservedDummySlot( int nSlotID )
{
	return m_setDummySlotReserved.end() != m_setDummySlotReserved.find(nSlotID);
}

void GItemContainer::CollectReservedDummySlot(vector<int>& vecDummySlotID)
{
	for each (int nReservedDummyslotID in m_setDummySlotReserved)
	{
		vecDummySlotID.push_back(nReservedDummyslotID);
	}	
}

bool GItemContainer::IsExist( int nItemID )
{
	if (NULL == GetItem(nItemID, 0)) return false;

	return true;
}