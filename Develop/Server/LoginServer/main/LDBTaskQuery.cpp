#include "stdafx.h"
#include "LDBTaskQuery.h"


LAccountDBTaskQuery::LAccountDBTaskQuery( const SDBTASK_ID TaskID ) : LDBAsyncTask(SDBT_DBTYPE_ACCOUNTDB, TaskID )
{

}

LAccountDBTaskQuery::~LAccountDBTaskQuery()
{

}

void LAccountDBTaskQuery::OnExecute( mdb::MDatabase& rfDB )
{
	_ASSERT(1 == GetQuerySize());

	if (1 != GetQuerySize())
		return;

	mdb::MDatabaseQuery dbq(&rfDB);
	ExecuteW(dbq, GetSQLW(0));
}

mdb::MDB_THRTASK_RESULT LAccountDBTaskQuery::_OnCompleted()
{
	return mdb::MDBTR_SUCESS;
}

LGameDBTaskQuery::LGameDBTaskQuery( const SDBTASK_ID TaskID ) : LDBAsyncTask(SDBT_DBTYPE_GAMEDB, TaskID)
{

}

LGameDBTaskQuery::~LGameDBTaskQuery()
{

}

void LGameDBTaskQuery::OnExecute( mdb::MDatabase& rfDB )
{
	_ASSERT(1 == GetQuerySize());

	if (1 != GetQuerySize())
		return;

	mdb::MDatabaseQuery dbq(&rfDB);
	ExecuteW(dbq, GetSQLW(0));
}

mdb::MDB_THRTASK_RESULT LGameDBTaskQuery::_OnCompleted()
{
	return mdb::MDBTR_SUCESS;
}

LLogDBTaskQuery::LLogDBTaskQuery( const SDBTASK_ID TaskID ) : LDBAsyncTask(SDBT_DBTYPE_LOGDB, TaskID)
{

}

LLogDBTaskQuery::~LLogDBTaskQuery()
{

}

void LLogDBTaskQuery::OnExecute( mdb::MDatabase& rfDB )
{
	_ASSERT(1 == GetQuerySize());

	if (1 != GetQuerySize())
		return;

	mdb::MDatabaseQuery dbq(&rfDB);
	ExecuteW(dbq, GetSQLW(0));
}

mdb::MDB_THRTASK_RESULT LLogDBTaskQuery::_OnCompleted()
{
	return mdb::MDBTR_SUCESS;
}