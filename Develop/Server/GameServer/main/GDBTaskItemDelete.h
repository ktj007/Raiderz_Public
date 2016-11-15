#ifndef _GDBTASK_ITEM_DELETE_H
#define _GDBTASK_ITEM_DELETE_H


#include "GDBAsyncTask.h"
//#include "GDBTaskDataItemDelete.h"
#include "GDBTaskItemData.h"
#include "GItemDBCacheSyncer.h"
#include "MMemPool.h"


class GDBTaskItemDelete : public GDBAsyncTask,  public MMemPool<GDBTaskItemDelete>
{
public :
	GDBTaskItemDelete(const MUID& uidReqPlayer);
	~GDBTaskItemDelete();

	enum
	{
		ITEM_DELETE = 0,
	};

	bool Input(const GDBT_ITEM_DATA& data);

	void					OnExecute(mdb::MDatabase& rfDB) override;
	virtual void			_OnPreCompleted()	override;
	mdb::MDB_THRTASK_RESULT	_OnCompleted() override;

protected :
	GDBT_ITEM_DATA		m_Data;
	GItemDBCacheSyncer  m_Syncer;
};

#endif