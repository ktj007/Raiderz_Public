#include "stdafx.h"
#include "GDBTaskEmblemInsert.h"

GDBTaskEmblemInsert::GDBTaskEmblemInsert( const MUID& uidPlayer ) : GDBAsyncTask(uidPlayer, SDBT_DBTYPE_GAMEDB, SDBTID_QUALIFYINSERT)
{
	m_Data.uidPlayer = uidPlayer;
}

GDBTaskEmblemInsert::~GDBTaskEmblemInsert()
{

}

void GDBTaskEmblemInsert::Input( const int nQualifyID )
{
	m_Data.nQualifyID = nQualifyID;
}

void GDBTaskEmblemInsert::OnExecute( mdb::MDatabase& rfDB )
{
	mdb::MDatabaseQuery q(&rfDB);
	ExecuteW(q, GetSQLW(QUALIFY_INSERT));
}

mdb::MDB_THRTASK_RESULT GDBTaskEmblemInsert::_OnCompleted()
{
	return mdb::MDBTR_SUCESS;
}