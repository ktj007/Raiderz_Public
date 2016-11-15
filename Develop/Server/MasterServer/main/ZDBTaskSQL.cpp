#include "stdafx.h"
#include "ZDBTaskSQL.h"



LAccountDBTaskQuery::LAccountDBTaskQuery( const SDBTASK_ID TaskID)
: SDBAsyncTask(SDBT_DBTYPE_ACCOUNTDB, TaskID)
{
}

LAccountDBTaskQuery::~LAccountDBTaskQuery()
{
}

void LAccountDBTaskQuery::OnExecute( mdb::MDatabase& rfDB)
{
	_ASSERT(1 == GetQuerySize());

	if (1 != GetQuerySize())
		return;

	mdb::MDatabaseQuery dbq(&rfDB);
	ExecuteW(dbq, GetSQLW(0));
}

mdb::MDB_THRTASK_RESULT LAccountDBTaskQuery::OnCompleted()
{
	return mdb::MDBTR_SUCESS;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////

LGameDBTaskQuery::LGameDBTaskQuery( const SDBTASK_ID TaskID )
: SDBAsyncTask(SDBT_DBTYPE_GAMEDB, TaskID)
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

mdb::MDB_THRTASK_RESULT LGameDBTaskQuery::OnCompleted()
{
	return mdb::MDBTR_SUCESS;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////

LLogDBTaskQuery::LLogDBTaskQuery( const SDBTASK_ID TaskID )
: SDBAsyncTask(SDBT_DBTYPE_LOGDB, TaskID)
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

mdb::MDB_THRTASK_RESULT LLogDBTaskQuery::OnCompleted()
{
	return mdb::MDBTR_SUCESS;
}