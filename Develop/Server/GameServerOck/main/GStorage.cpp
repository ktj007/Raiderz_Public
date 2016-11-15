#include "StdAfx.h"
#include "GStorage.h"
#include "GItem.h"

GStorage::GStorage(GEntityPlayer* pOwner)
: GItemContainer(MAX_STORAGE_SIZE, pOwner)
, m_isExpended(false), m_bSerialized(false)
{
}

GStorage::~GStorage(void)
{
}

void GStorage::Clear( void )
{
	__super::Clear();
	m_bSerialized = false;
}

int GStorage::GetSlotSize( void ) const 
{
	return m_isExpended ? MAX_STORAGE_SIZE : BASE_STORAGE_SIZE;
}

bool GStorage::PushItem( int slotID, GItem* pItem )
{
	if (!__super::PushItem(slotID, pItem))	return false;

	pItem->m_nSlotType = SLOTTYPE_STORAGE;

	// 예약 슬롯이였으면 예약해제
	if (IsReservedSlot(slotID))
	{
		CancelReserveSlot(slotID);
	}

	return true;
}