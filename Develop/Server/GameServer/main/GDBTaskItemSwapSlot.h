#ifndef _GDBTASK_ITEM_SWAP_SLOT_H
#define _GDBTASK_ITEM_SWAP_SLOT_H


#include "GDBAsyncTask.h"
#include "GDBTaskItem.h"
#include "GItemDBCacheSyncer.h"
#include "MMemPool.h"


class GDBTaskItemSwapSlot : public GDBAsyncTask, public MMemPool<GDBTaskItemSwapSlot>
{
public :
	GDBTaskItemSwapSlot(const MUID& uidReqPlayer);
	~GDBTaskItemSwapSlot();

	enum
	{
		ITEM_SWAP_SLOT = 0,
	};

	bool Input(GDBT_ITEM_MOVE& swapData);

	void					OnExecute(mdb::MDatabase& rfDB) override;
	void					_OnPreCompleted() override;
	mdb::MDB_THRTASK_RESULT	_OnCompleted() override;
	mdb::MDB_THRTASK_RESULT	_OnFailed() override;

protected :
	GDBT_ITEM_MOVE		m_swapData;	
	GItemDBCacheSyncer	m_Syncer;
};


#endif