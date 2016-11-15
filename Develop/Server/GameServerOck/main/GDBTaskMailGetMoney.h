#pragma once

#include "GDBAsyncTask.h"
#include "MMemPool.h"
#include "GDBTaskData.h"
#include "GSyncingCacheList.h"

class GDBTaskMailGetMoney: public GDBAsyncTask, public MMemPool<GDBTaskMailGetMoney>
{
public:
	GDBTaskMailGetMoney(const MUID& uidPlayer);
	virtual ~GDBTaskMailGetMoney(void);

	enum
	{
		SQL_MAIL_GET_MONEY = 0
	};

	bool Input(const GDBT_MAIL_GET_MONEY& data);
	bool UpdateCache(void);

	void					OnExecute(mdb::MDatabase& rfDB) override;
	void					_OnPreCompleted() override;
	mdb::MDB_THRTASK_RESULT _OnCompleted() override;
	mdb::MDB_THRTASK_RESULT	_OnFailed() override;	

private :
	struct _DATA
	{
		GDBT_MAIL_GET_MONEY m_mailMoney;
	};

private:
	_DATA				m_Data;
	GSyncingCacheList	m_SyncingList;
};
