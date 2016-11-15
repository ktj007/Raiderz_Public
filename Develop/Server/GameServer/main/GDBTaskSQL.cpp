#include "stdafx.h"
#include "GDBTaskSQL.h"


GGameDBTaskQuery::GGameDBTaskQuery( const MUID& uidReqPlayer, const SDBTASK_ID nTaskID) 
: GDBAsyncTask(uidReqPlayer, SDBT_DBTYPE_GAMEDB, nTaskID)
{

}

GGameDBTaskQuery::~GGameDBTaskQuery()
{

}

void GGameDBTaskQuery::OnExecute( mdb::MDatabase& rfDB )
{
	_ASSERT(1 == GetQuerySize());

	if (1 != GetQuerySize())
		return;

	mdb::MDatabaseQuery dbq(&rfDB);
	ExecuteW(dbq, GetSQLW(0));
}

mdb::MDB_THRTASK_RESULT GGameDBTaskQuery::_OnCompleted()
{
	return mdb::MDBTR_SUCESS;
}

GGameDBTaskQueryIter::GGameDBTaskQueryIter( const MUID& uidReqPlayer, const SDBTASK_ID nTaskID, const size_t nReserveSQLSize ) 
: GDBAsyncTask(uidReqPlayer, SDBT_DBTYPE_GAMEDB, nTaskID, nReserveSQLSize)
{

}

GGameDBTaskQueryIter::~GGameDBTaskQueryIter()
{

}

void GGameDBTaskQueryIter::OnExecute( mdb::MDatabase& rfDB )
{
	for (size_t i = 0; i < GetQuerySize(); ++i)
	{
		mdb::MDatabaseQuery dbq(&rfDB);

		if (!ExecuteW(dbq, GetSQLW(i)))
			return;

		dbq.Close();
	}
}

mdb::MDB_THRTASK_RESULT GGameDBTaskQueryIter::_OnCompleted()
{
	return mdb::MDBTR_SUCESS;
}

GLogDBTaskQuery::GLogDBTaskQuery( const MUID& uidReqPlayer, const SDBTASK_ID nTaskID ) 
: GDBAsyncTask(uidReqPlayer, SDBT_DBTYPE_LOGDB, nTaskID)
{

}

GLogDBTaskQuery::~GLogDBTaskQuery()
{

}

void GLogDBTaskQuery::OnExecute( mdb::MDatabase& rfDB )
{
	_ASSERT(1 == GetQuerySize());

	if (1 != GetQuerySize())
		return;

	mdb::MDatabaseQuery dbq(&rfDB);
	ExecuteW(dbq, GetSQLW(0));
}

mdb::MDB_THRTASK_RESULT GLogDBTaskQuery::_OnCompleted()
{
	return mdb::MDBTR_SUCESS;
}