#pragma once

#include "GDBTaskData.h"
#include "CSDef.h"

class GPlayerMailReserver : public MTestMemPool<GPlayerMailReserver>
{
public:
	typedef map<int, int>	map_appended;

public:
	GPlayerMailReserver(GEntityPlayer* pOwner);
	virtual ~GPlayerMailReserver();

	bool PutUpItem(int nSlotID, int nAmount);
	bool PutDownItem(int nSlotID, int nAmount);
	void Clear(void);

	bool IsAppended(int nSlotID);
	int GetAmount(int nSlotID);
	int GetAppendedSlotCount(void);
	int GetDefaultItemID(void);

	int MakeDBT(GDBT_MAIL_APPENDED_ITEM (&itemSlot)[MAX_MAIL_APPENDED_ITEM_COUNT]);
	
private:
	bool IsValidParameter(int nSlotID, int nAmount);

private:
	GEntityPlayer*	m_pOwner;
	map_appended	m_mapAppendedItem;	// nSlotID, nAmount
};
