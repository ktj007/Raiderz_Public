#pragma once

#include "GDBTaskItem.h"
#include <vector>

using namespace std;
class GDBT_ITEM_SORT_SLOT;

class GDBT_ITEM_SORT
{
public :
	GDBT_ITEM_SORT() : m_uidPlayer(0), m_nCID(0) {}
	GDBT_ITEM_SORT(const MUID& uidPlayer, const CID nCID, const vector<GDBT_ITEM_SORT_SLOT>& vecSortSlot) : m_uidPlayer(uidPlayer), m_nCID(nCID), m_vecSortSlot(vecSortSlot) {}

	MUID						m_uidPlayer;
	int							m_nCID;
	vector<GDBT_ITEM_SORT_SLOT>	m_vecSortSlot;
};

class GDBT_ITEM_SORT_SLOT
{
public:
	GDBT_ITEM_SORT_SLOT() : m_nFromSlotID(0), m_nToSlotID(0) {}
	GDBT_ITEM_SORT_SLOT(int16 nFromSlotID, int16 nToSlotID) : m_nFromSlotID(nFromSlotID), m_nToSlotID(nToSlotID) {}

	int16		m_nFromSlotID;
	int16		m_nToSlotID;
};