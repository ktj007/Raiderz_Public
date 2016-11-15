#pragma once

#include "GDBAsyncTask.h"
#include "MMemPool.h"

class GDBTaskMailSetRead: public GDBAsyncTask, public MMemPool<GDBTaskMailSetRead>
{
public:
	GDBTaskMailSetRead(const MUID& uidReqPlayer);
	virtual ~GDBTaskMailSetRead();

	enum
	{
		SQL_MAIL_SET_READ = 0
	};

	void					OnExecute(mdb::MDatabase& rfDB) override;
	mdb::MDB_THRTASK_RESULT _OnCompleted() override;
	mdb::MDB_THRTASK_RESULT	_OnFailed() override;
};
