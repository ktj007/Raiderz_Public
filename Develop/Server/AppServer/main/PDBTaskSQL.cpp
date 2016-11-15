#include "stdafx.h"
#include "PDBTaskSQL.h"


PGameDBTaskQuery::PGameDBTaskQuery( const SDBTASK_ID TaskID ) : SDBAsyncTask(SDBT_DBTYPE_GAMEDB, TaskID)
{

}

mdb::MDB_THRTASK_RESULT PGameDBTaskQuery::OnCompleted()
{
	return mdb::MDBTR_SUCESS;
}

void PGameDBTaskQuery::OnExecute( mdb::MDatabase& rfDB )
{
	_ASSERT(1 == GetQuerySize());

	if (1 != GetQuerySize())
		return;

	mdb::MDatabaseQuery dbq(&rfDB);
	ExecuteW(dbq, GetSQLW(0));
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

PLogDBTaskQuery::PLogDBTaskQuery( const SDBTASK_ID TaskID )
: SDBAsyncTask(SDBT_DBTYPE_LOGDB, TaskID)
{

}

PLogDBTaskQuery::~PLogDBTaskQuery()
{

}

void PLogDBTaskQuery::OnExecute( mdb::MDatabase& rfDB )
{
	_ASSERT(1 == GetQuerySize());

	if (1 != GetQuerySize())
		return;

	mdb::MDatabaseQuery dbq(&rfDB);
	ExecuteW(dbq, GetSQLW(0));
}

mdb::MDB_THRTASK_RESULT PLogDBTaskQuery::OnCompleted()
{
	return mdb::MDBTR_SUCESS;
}