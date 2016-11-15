#pragma once

#include "GDBAsyncTask.h"

class MockDBTask : public GDBAsyncTask
{
public :
	MockDBTask(const MUID& uidReqPlayer) : GDBAsyncTask(uidReqPlayer, SDBT_DBTYPE_NONE, SDBTID_NONE) {}

	void					OnExecute(mdb::MDatabase& rfDB) {}
	mdb::MDB_THRTASK_RESULT	_OnCompleted() { return mdb::MDBTR_SUCESS; }

	using GDBAsyncTask::IncreaseRefCount;
	using GDBAsyncTask::DecreaseRefCount;
};
