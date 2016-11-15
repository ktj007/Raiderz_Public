#ifndef _GDBTASK_ITEM_DEC_DURA_H
#define _GDBTASK_ITEM_DEC_DURA_H


#include "GDBAsyncTask.h"
#include "GDBTaskItemData.h"
#include "GSyncingCacheList.h"
#include "MMemPool.h"


class GDBTaskItemDecDura : public GDBAsyncTask, public MMemPool<GDBTaskItemDecDura>
{
public :
	GDBTaskItemDecDura(const MUID& uidReqPlayer);
	~GDBTaskItemDecDura();

	enum
	{
		ITEM_UPDATE_DURA = 0,
	};

	bool Input(GDBT_ITEM_DEC_DURA_DATA& data);

	void					OnExecute(mdb::MDatabase& rfDB) override;
	mdb::MDB_THRTASK_RESULT	_OnCompleted() override;
	virtual void			_OnPreCompleted() override;

	bool DecCache(GDBT_ITEM_DEC_DURA_DATA& data);

protected :
	GDBT_ITEM_DEC_DURA_DATA	m_Data;
	GSyncingCacheList		m_SyncingList;
};

#endif
