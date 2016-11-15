#include "StdAfx.h"
#include "GGuildStorage.h"
#include "GItem.h"

GGuildStorage::GGuildStorage()
: GItemContainer(MAX_GUILDSTORAGE_SIZE, NULL)
, m_isExpended(false)
{
}

GGuildStorage::~GGuildStorage(void)
{
}

int GGuildStorage::GetSlotSize( void ) const 
{
	return m_isExpended ? MAX_GUILDSTORAGE_SIZE : BASE_GUILDSTORAGE_SIZE;
}

bool GGuildStorage::PushItem( int slotID, GItem* pItem )
{
	if (!__super::PushItem(slotID, pItem)) return false;

	pItem->m_nSlotType = SLOTTYPE_GUILDSTORAGE;

	// 예약 슬롯이였으면 예약해제
	if (IsReservedSlot(slotID))
	{
		CancelReserveSlot(slotID);
	}

	return true;
}