#include "stdafx.h"
#include "GInventory.h"
#include "GItem.h"
#include "AStlUtil.h"
#include "GItemAdder.h"
#include "GGlobal.h"
#include "GQuestSystem.h"
#include "GQObjUpdater.h"


GInventory::GInventory(GEntityPlayer* pOwner) 
: GItemContainer(MAX_INVENTORY_SIZE, pOwner)
, m_vecHadGatherTools(MAX_GATHER_TYPE)
, m_isExpended(false)
{
	// do nothing
}

GInventory::~GInventory()
{
	Clear();
}

void GInventory::Clear(void)
{
	GItemContainer::Clear();

	for(size_t i = 0; i < m_vecHadGatherTools.size(); i++)
	{
		m_vecHadGatherTools[i] = 0;
	}
}

GItem* GInventory::IncreaseAmount( int nSlotID, int nAmount )
{
	GItem* pItem = __super::IncreaseAmount(nSlotID, nAmount);
	if (NULL == pItem) return NULL;
	
	gsys.pQuestSystem->GetQObjUpdater().IncCollectQObj(m_pOwnerPlayer, pItem->GetID(), nAmount);

	return pItem;
}

GItem* GInventory::DecreaseAmount( int nSlotID, int nAmount )
{
	GItem* pItem = __super::DecreaseAmount(nSlotID, nAmount);
	if (NULL == pItem) return NULL;

	gsys.pQuestSystem->GetQObjUpdater().DecCollectQObj(m_pOwnerPlayer, pItem->GetID(), nAmount);

	return pItem;
}

GItem* GInventory::SetAmount( int nSlotID, int nAmount )
{
	GItem* pItem = GetItem(nSlotID);
	if (NULL == pItem) return NULL;

	int nBeforeAmount = pItem->GetAmount();

	__super::SetAmount(nSlotID, nAmount);

	int nAfterAmount = pItem->GetAmount();

	if (nBeforeAmount < nAfterAmount)
	{
		gsys.pQuestSystem->GetQObjUpdater().IncCollectQObj(m_pOwnerPlayer, pItem->GetID(), nAfterAmount - nBeforeAmount);
	}
	else if (nBeforeAmount > nAfterAmount)
	{
		gsys.pQuestSystem->GetQObjUpdater().DecCollectQObj(m_pOwnerPlayer, pItem->GetID(), nBeforeAmount - nAfterAmount);
	}	

	return pItem;
}


bool GInventory::PushItem(int slotID, GItem* pItem)
{
	if (!__super::PushItem(slotID, pItem))	return false;
	
	pItem->m_nSlotType = SLOTTYPE_INVENTORY;

	// 예약 슬롯이였으면 예약해제
	if (IsReservedSlot(slotID))
	{
		CancelReserveSlot(slotID);
	}

	/// 아이템이 채집도구라면 등록
	if (GATHER_NONE != pItem->m_pItemData->m_nGatherType)
	{
		m_vecHadGatherTools[pItem->m_pItemData->m_nGatherType]++;
	}

	gsys.pQuestSystem->GetQObjUpdater().IncCollectQObj(m_pOwnerPlayer, pItem->GetID(), pItem->GetAmount());

	return true;
}

GItem* GInventory::PopItem(int slotID)
{
	/// 아이템이 채집도구라면 등록해제
	GItem* pItem = __super::PopItem(slotID);
	if (NULL == pItem) return NULL;

	if (GATHER_NONE != pItem->m_pItemData->m_nGatherType)
	{
		m_vecHadGatherTools[pItem->m_pItemData->m_nGatherType]--;
		pItem->m_nSlotType = SLOTTYPE_NONE;
	}

	gsys.pQuestSystem->GetQObjUpdater().DecCollectQObj(m_pOwnerPlayer, pItem->GetID(), pItem->GetAmount());

	return pItem;
}

int GInventory::GetSlotSize( void ) const
{
	return m_isExpended ? MAX_INVENTORY_SIZE : BASE_INVENTORY_SIZE;
}

bool GInventory::HasGatherTool(GATHER_TYPE eGatherType)
{
	if (GATHER_NONE == eGatherType) return false;

	if (0 < m_vecHadGatherTools[eGatherType])
	{
		return true;
	}
	else
	{	
		return false;
	}
}

GItem* GInventory::GetItemByGatherType(GATHER_TYPE gatherType, int nStartSlotID)
{
	if (IsEmpty())	return NULL;
	if (nStartSlotID < 0)	return NULL;

	for (int i = nStartSlotID; i < GetSlotSize(); i++)
	{
		GItem* pItem = GetItem(i);

		if (pItem == NULL)	continue;
		if (gatherType == pItem->m_pItemData->m_nGatherType)
		{	
			return pItem;		
		}
	}

	return NULL;
}

void GInventory::CollectVeryCommonItem(vector<GItem*>& outvecitem)
{
	if (IsEmpty())	return;

	for (int i = 0; i < GetSlotSize(); i++)
	{
		GItem* pItem = GetItem(i);
		if (pItem == NULL)	continue;

		if (pItem->m_pItemData->m_nTier == ITEM_TIER_VERY_COMMON)
		{
			outvecitem.push_back(pItem);
		}
	}
}