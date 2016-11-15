#pragma once

#include "GDBAsyncTask.h"
#include "GDBTaskItem.h"
#include "GItemDBCacheSyncer.h"

class GDBTaskItemSplit : public GDBAsyncTask, public MMemPool<GDBTaskItemSplit>
{
public:
	GDBTaskItemSplit(const MUID& uidPlayer);
	~GDBTaskItemSplit(void);

	bool Input(GDBT_ITEM_MERGE_AND_SPLIT& data);

	void					OnExecute(mdb::MDatabase& rfDB) override;
	void					_OnPreCompleted() override;
	mdb::MDB_THRTASK_RESULT	_OnCompleted() override;
	mdb::MDB_THRTASK_RESULT	_OnFailed() override;

protected:
	GDBT_ITEM_MERGE_AND_SPLIT		m_data;
	GItemDBCacheSyncer	m_syncer;
};
