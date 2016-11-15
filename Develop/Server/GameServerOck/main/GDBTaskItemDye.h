#ifndef _GDBTASK_ITEM_DYE_H
#define _GDBTASK_ITEM_DYE_H


#include "GDBAsyncTask.h"
#include "GDBTaskItemData.h"
#include "GItemDBCacheSyncer.h"
#include "MMemPool.h"


class GDBTaskItemDye : public GDBAsyncTask, public MMemPool<GDBTaskItemDye>
{
public :
	GDBTaskItemDye(const MUID& uidReqPlayer);

	enum
	{
		ITEM_DYE = 0,
	};

	bool Input(const GDBT_ITEM_DYE& data);

	void					OnExecute(mdb::MDatabase& rfDB) override;

	virtual void			_OnPreCompleted()	override;
	mdb::MDB_THRTASK_RESULT	_OnCompleted() override;

protected :
	bool SyncItem(const GDBT_ITEM_DYE& data);

protected :
	GDBT_ITEM_DYE		m_Data;
	GItemDBCacheSyncer  m_Syncer;
};


#endif