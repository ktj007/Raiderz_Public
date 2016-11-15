#ifndef _GDBTASK_ITEM_LOOT_UPDATE_H
#define _GDBTASK_ITEM_LOOT_UPDATE_H


#include "GDBTaskItemStackAmt.h"
#include "GDBTaskItemData.h"
#include "MMemPool.h"


class GDBTaskItemLootUpdate : public GDBTaskItemStackAmt, public MMemPool<GDBTaskItemLootUpdate>
{
public :
	GDBTaskItemLootUpdate(const MUID& uidPlayer);
	~GDBTaskItemLootUpdate();


	enum
	{
		ITEM_LOOT_UPDATE = 0,
	};


	bool Input(const GDBT_ITEM_DATA& data);

	void					OnExecute(mdb::MDatabase& rfDB);
	mdb::MDB_THRTASK_RESULT	_OnCompleted();


protected :
	GDBT_ITEM_DATA m_Data;
};


#endif