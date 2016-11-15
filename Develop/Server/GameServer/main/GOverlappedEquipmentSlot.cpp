#include "StdAfx.h"
#include "GOverlappedEquipmentSlot.h"
#include "AStlUtil.h"
#include "GItem.h"

GOverlappedEquipmentSlot::GOverlappedEquipmentSlot(void)
: m_bOverlapped(false)
, m_vecEquipItems(ITEMSLOT_MAX, NULL)
{
	Reset();
}

GOverlappedEquipmentSlot::~GOverlappedEquipmentSlot(void)
{
	Reset();
}

GItem* GOverlappedEquipmentSlot::GetLeftWeapon() const
{
	return GetItem(ITEMSLOT_LWEAPON);
}

GItem* GOverlappedEquipmentSlot::GetRightWeapon() const
{
	return GetItem(ITEMSLOT_RWEAPON);
}

void GOverlappedEquipmentSlot::Reset()
{
	m_bOverlapped = false;
	for (size_t i=0; i<m_vecEquipItems.size(); ++i)
	{
		SAFE_DELETE(m_vecEquipItems[i]);
		m_vecEquipItems[i] = NULL;
	}
}

GItem* GOverlappedEquipmentSlot::GetItem( SH_ITEM_SLOT nSlot ) const
{
	VALID_RET(nSlot >= ITEMSLOT_ARMOR_START, NULL);
	VALID_RET(nSlot < ITEMSLOT_MAX, NULL);
	return m_vecEquipItems[nSlot];
}

void GOverlappedEquipmentSlot::SetItem( SH_ITEM_SLOT nSlot, GItem* pItem )
{
	VALID(pItem);
	VALID(nSlot >= ITEMSLOT_ARMOR_START);
	VALID(nSlot < ITEMSLOT_MAX);
	SAFE_DELETE(m_vecEquipItems[nSlot]);
	m_vecEquipItems[nSlot] = pItem;
	m_bOverlapped = true;
}

bool GOverlappedEquipmentSlot::IsOverlapped() const
{
	return m_bOverlapped;
}
