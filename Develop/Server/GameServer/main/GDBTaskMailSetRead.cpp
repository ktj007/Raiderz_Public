#include "stdafx.h"
#include "GDBTaskMailSetRead.h"

GDBTaskMailSetRead::GDBTaskMailSetRead(const MUID& uidReqPlayer)
: GDBAsyncTask(uidReqPlayer, SDBT_DBTYPE_GAMEDB, SDBTID_MAIL_SET_READ)
{	
}

GDBTaskMailSetRead::~GDBTaskMailSetRead()
{
}

void GDBTaskMailSetRead::OnExecute(mdb::MDatabase& rfDB)
{
	mdb::MDBRecordSet rs(&rfDB);
	ExecuteW(rs, GetSQLW(SQL_MAIL_SET_READ));
}

mdb::MDB_THRTASK_RESULT GDBTaskMailSetRead::_OnCompleted()
{
	// do nothing
	return mdb::MDBTR_SUCESS;
}

mdb::MDB_THRTASK_RESULT	GDBTaskMailSetRead::_OnFailed()
{
	// do nothing
	return mdb::MDBTR_SUCESS;
}
