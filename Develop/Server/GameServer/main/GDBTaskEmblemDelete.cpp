#include "stdafx.h"
#include "GDBTaskEmblemDelete.h"

GDBTaskEmblemDelete::GDBTaskEmblemDelete( const MUID& uidPlayer ) : GDBAsyncTask(uidPlayer, SDBT_DBTYPE_GAMEDB, SDBTID_QUALIFYDELETE)
{
	m_Data.uidPlayer = uidPlayer;
}

void GDBTaskEmblemDelete::Input( const int nQualifyID )
{
	m_Data.nQualifyID = nQualifyID;
}

void GDBTaskEmblemDelete::OnExecute( mdb::MDatabase& rfDB )
{
	mdb::MDatabaseQuery q(&rfDB);
	ExecuteW(q, GetSQLW(QUALIFY_DELETE));
}

mdb::MDB_THRTASK_RESULT GDBTaskEmblemDelete::_OnCompleted()
{
	return mdb::MDBTR_SUCESS;
}