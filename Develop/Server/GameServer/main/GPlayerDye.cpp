#include "StdAfx.h"
#include "GPlayerDye.h"
#include "GItem.h"

GPlayerDye::GPlayerDye()
: m_nTargetSlotType(SLOTTYPE_NONE)
, m_nTargetSlotID(-1)
, m_nDyeSlotID(-1)
{

}


void GPlayerDye::Prepare( SH_ITEM_SLOT_TYPE nTargetSlotType, int nTargetSlotID, int nDyeSlotID )
{
	m_nTargetSlotType = nTargetSlotType;
	m_nTargetSlotID = nTargetSlotID;
	m_nDyeSlotID = nDyeSlotID;
}

void GPlayerDye::Clear()
{
	m_nTargetSlotType = SLOTTYPE_NONE;
	m_nTargetSlotID = -1;
	m_nDyeSlotID = -1;
}

bool GPlayerDye::IsDyeingItem(const GItem* pItem )
{
	if (NULL == pItem) return false;

	if (m_nTargetSlotType == pItem->m_nSlotType && m_nTargetSlotID == pItem->m_nSlotID)
		return true;

	if (SLOTTYPE_INVENTORY == pItem->m_nSlotType && m_nDyeSlotID == pItem->m_nSlotID)
		return true;

	return false;
}