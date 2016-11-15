#ifndef _GDBTASK_ITEM_LOOT_INSERT_H
#define _GDBTASK_ITEM_LOOT_INSERT_H


#include "GDBAsyncTask.h"
#include "GDBTaskItemData.h"
#include "MMemPool.h"


class GDBTaskItemLootInsert : public GDBAsyncTask, public MMemPool<GDBTaskItemLootInsert>
{
public :
	GDBTaskItemLootInsert(const MUID& uidPlayer);
	~GDBTaskItemLootInsert();


	enum
	{
		ITEM_LOOT_INSERT = 0,
	};


	void Input(const GDBT_ITEM_DATA& data);


	void					OnExecute(mdb::MDatabase& rfDB) override;
	mdb::MDB_THRTASK_RESULT	_OnCompleted() override;
	mdb::MDB_THRTASK_RESULT	_OnFailed() override;	


protected :
	struct _RESULT
	{
		IUID m_nIUID;
	};


protected :
	GDBT_ITEM_DATA	m_Data;
	_RESULT			m_Result;
};


#endif