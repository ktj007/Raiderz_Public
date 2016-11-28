#pragma once

#include "GDBTaskItem.h"
#include <vector>

using namespace std;
class GDBT_ITEM_SORT_SLOT;
class GDBT_ITEM_MERGE_ALL_SLOT;

enum SH_ITEM_SLOT_TYPE;

class GDBT_ITEM_SORT
{
public :
	GDBT_ITEM_SORT() : m_uidPlayer(0), m_nCID(0), m_nSlotType(SLOTTYPE_NONE), m_nStartSlotIndex(0) {}
	GDBT_ITEM_SORT(const MUID& uidPlayer, const CID nCID, const SH_ITEM_SLOT_TYPE nSlotType, const vector<GDBT_ITEM_SORT_SLOT>& vecSortSlot, int nStartSlotIndex, const vector<GDBT_ITEM_MERGE_ALL_SLOT>& vecMergeSlot)
		: m_uidPlayer(uidPlayer), m_nCID(nCID), m_nSlotType(nSlotType), m_vecSortSlot(vecSortSlot), m_nStartSlotIndex(nStartSlotIndex), m_vecMergeSlot(vecMergeSlot) {}

	MUID								m_uidPlayer;
	CID									m_nCID;
	SH_ITEM_SLOT_TYPE					m_nSlotType;
	vector<GDBT_ITEM_SORT_SLOT>			m_vecSortSlot;
	int									m_nStartSlotIndex;
	vector<GDBT_ITEM_MERGE_ALL_SLOT>	m_vecMergeSlot;
};

class GDBT_ITEM_SORT_SLOT
{
public:
	GDBT_ITEM_SORT_SLOT() : m_nFromSlotID(0), m_nToSlotID(0) {}
	GDBT_ITEM_SORT_SLOT(const int16 nFromSlotID, const int16 nToSlotID) : m_nFromSlotID(nFromSlotID), m_nToSlotID(nToSlotID) {}

	int16		m_nFromSlotID;
	int16		m_nToSlotID;
};

class GDBT_ITEM_MERGE_ALL_SLOT
{
public:
	GDBT_ITEM_MERGE_ALL_SLOT() : m_nStackAmount(0) {}
	GDBT_ITEM_MERGE_ALL_SLOT(const GDBT_ITEM_SORT_SLOT& Slot, const int16 nStackAmount) : m_Slot(Slot), m_nStackAmount(nStackAmount) {}

	GDBT_ITEM_SORT_SLOT		m_Slot;
	int16					m_nStackAmount;
};
