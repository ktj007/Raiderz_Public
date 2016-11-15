#include "stdafx.h"
#include "LDBAsyncTask.h"
#include "LGlobal.h"
#include "LDBManager.h"


LDBAsyncTask::LDBAsyncTask( const SDBT_DBTYPE DBType, const SDBTASK_ID TaskID, const size_t nReserveSQLSize /*= 1*/ ) 
: SDBAsyncTask(DBType, TaskID, nReserveSQLSize)
{

}

LDBAsyncTask::~LDBAsyncTask()
{

}

mdb::MDB_THRTASK_RESULT LDBAsyncTask::OnCompleted()
{
	_OnPreCheck();

	mdb::MDB_THRTASK_RESULT Res = mdb::MDBTR_SUCESS;

	if (!CheckResult())
		Res = _OnFailed();
	else
		Res = _OnCompleted();

	_OnProfile();

	return Res;
}