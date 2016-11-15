#pragma once


#include "GDBTaskItem.h"


enum SH_ITEM_SLOT;
enum SH_ITEM_SLOT_TYPE;

class GDBT_UNEQUIP_ITEM_SLOT
{
public :
	GDBT_UNEQUIP_ITEM_SLOT() : m_nSlotType(SLOTTYPE_NONE), m_nSlotID(ITEMSLOT_NONE), m_nIUID(0), m_nDura(0), m_nToSlotID(-1) {}
	GDBT_UNEQUIP_ITEM_SLOT(SH_ITEM_SLOT_TYPE nSlotType, SH_ITEM_SLOT nSlotID, int64 nIUID, uint8 nDura, int16 nToSlotID)
		: m_nSlotType(nSlotType), m_nSlotID(nSlotID), m_nIUID(nIUID), m_nDura(nDura), m_nToSlotID(nToSlotID) {}

	bool IsValid()
	{
		return m_nIUID > 0;
	}

	SH_ITEM_SLOT_TYPE	m_nSlotType;
	SH_ITEM_SLOT		m_nSlotID;
	int64				m_nIUID;
	uint8				m_nDura;
	int16				m_nToSlotID;
};

class GDBT_EQUIP_ITEM_SLOT
{
public :
	GDBT_EQUIP_ITEM_SLOT() : m_nSlotType(SLOTTYPE_NONE), m_nSlotID(-1), m_nIUID(0), m_nDura(0), m_nToSlotID(ITEMSLOT_NONE) {}
	GDBT_EQUIP_ITEM_SLOT(SH_ITEM_SLOT_TYPE nSlotType, int16 nSlotID, int64 nIUID, uint8 nDura, bool bClaimed, SH_ITEM_SLOT nToSlotID)
		: m_nSlotType(nSlotType), m_nSlotID(nSlotID), m_nIUID(nIUID), m_nDura(nDura), m_bClaimed(bClaimed), m_nToSlotID(nToSlotID) {}

	SH_ITEM_SLOT_TYPE	m_nSlotType;
	int16				m_nSlotID;
	int64				m_nIUID;
	uint8				m_nDura;
	bool				m_bClaimed;
	SH_ITEM_SLOT		m_nToSlotID;
};

struct GDBT_ITEM_EQUIP
{
	MUID uidPlayer;
	int64 nCID;
	
	GDBT_EQUIP_ITEM_SLOT	equip;
	GDBT_UNEQUIP_ITEM_SLOT unequip1;
	GDBT_UNEQUIP_ITEM_SLOT unequip2;

	GDBT_ITEM_EQUIP();

	GDBT_ITEM_EQUIP(MUID uidPlayer, int64 nCID, GDBT_EQUIP_ITEM_SLOT& equip, GDBT_UNEQUIP_ITEM_SLOT& unequip1, GDBT_UNEQUIP_ITEM_SLOT& unequip2);
};

struct GDBT_ITEM_UNEQUIP
{
	MUID uidPlayer;
	int64 nCID;
	
	GDBT_UNEQUIP_ITEM_SLOT unequip1;
	GDBT_UNEQUIP_ITEM_SLOT unequip2;
	
	GDBT_ITEM_UNEQUIP();

	GDBT_ITEM_UNEQUIP(MUID uidPlayer, int64 nCID, GDBT_UNEQUIP_ITEM_SLOT& unequip1, GDBT_UNEQUIP_ITEM_SLOT& unequip2);
};