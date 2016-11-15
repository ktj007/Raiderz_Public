#ifndef _GDBTASK_DATA_PALETTE_H
#define _GDBTASK_DATA_PALETTE_H


#include "CSPalette.h"


class GDBT_PALETTE_SLOT
{
public :
	GDBT_PALETTE_SLOT() : m_nNum(PALETTENUM_1), m_nSlot(PALETTESLOT_1), m_nIdType(PIT_NONE), m_nID(0) {}
	GDBT_PALETTE_SLOT(const PALETTE_NUM nNum, const PALETTE_SLOT nSlot, const int nID, const PALETTE_ITEM_TYPE nIdType)
		: m_nNum(nNum), m_nSlot(nSlot), m_nID(nID), m_nIdType(nIdType) {}

	PALETTE_NUM			m_nNum;
	PALETTE_SLOT		m_nSlot;
	int					m_nID;
	PALETTE_ITEM_TYPE	m_nIdType;
};

class GDBT_PALETTE_SET
{
public :
	GDBT_PALETTE_SET() : m_nAID(0), m_uidPlayer(0), m_nCID(0) {}
	GDBT_PALETTE_SET(const int64 nAID, const MUID& uidPlayer, const int64 nCID, const GDBT_PALETTE_SLOT& Slot )
		: m_nAID(nAID), m_uidPlayer(uidPlayer), m_nCID(nCID), m_Slot(Slot) {}

	int64				m_nAID;
	MUID				m_uidPlayer;
	int64				m_nCID;
	GDBT_PALETTE_SLOT	m_Slot;
};

class GDBT_PALETTE_CHANGE
{
public :
	GDBT_PALETTE_CHANGE() : m_nAID(0), m_uidPlayer(0), m_nCID(0) {}
	GDBT_PALETTE_CHANGE(const int64 nAID, const MUID& uidPlayer, const int64 nCID
		, const GDBT_PALETTE_SLOT& Slot
		, const GDBT_PALETTE_SLOT& ToSlot)
		: m_nAID(nAID), m_uidPlayer(uidPlayer), m_nCID(nCID)
		, m_Slot(Slot)
		, m_ToSlot(ToSlot) {}

	int64				m_nAID;
	MUID				m_uidPlayer;
	int64				m_nCID;
	GDBT_PALETTE_SLOT	m_Slot;
	GDBT_PALETTE_SLOT	m_ToSlot;
};

class GDBT_PALETTE_RESET
{
public :
	GDBT_PALETTE_RESET() : m_nAID(0), m_uidPlayer(0), m_nCID(0), m_nNum(PALETTENUM_1), m_nSlot(PALETTESLOT_1) {}
	GDBT_PALETTE_RESET(const int64 nAID, const MUID& uidPlayer, const int64 nCID, const PALETTE_NUM nNum, const PALETTE_SLOT nSlot)
		: m_nAID(nAID), m_uidPlayer(uidPlayer), m_nCID(nCID), m_nNum(nNum), m_nSlot(nSlot) {}

	int64			m_nAID;
	MUID			m_uidPlayer;
	int64			m_nCID;
	PALETTE_NUM		m_nNum;
	PALETTE_SLOT	m_nSlot;
};


#endif