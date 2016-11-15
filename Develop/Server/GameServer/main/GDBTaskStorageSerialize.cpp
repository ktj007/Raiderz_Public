#include "StdAfx.h"
#include "GDBTaskStorageSerialize.h"
#include "GGlobal.h"
#include "GStorageSystem.h"

GDBTaskStorageSerialize::GDBTaskStorageSerialize( const MUID& uidReqPlayer )
: GDBAsyncTask(uidReqPlayer, SDBT_DBTYPE_GAMEDB, SDBTID_STORAGE_SERIALIZE, 1)
{
	m_data.m_uidPlayer = uidReqPlayer;
}

GDBTaskStorageSerialize::~GDBTaskStorageSerialize(void)
{
}

void GDBTaskStorageSerialize::OnExecute( mdb::MDatabase& rfDB )
{
	mdb::MDBRecordSet rs(&rfDB);
	if (!ExecuteW(rs, GetSQLW(0)))
		return;

	if (false == m_data.m_itemInstanceQ.GetFromDB(rs))
		return;
}

mdb::MDB_THRTASK_RESULT GDBTaskStorageSerialize::_OnCompleted()
{
	gsys.pStorageSystem->ShowForDBTask(m_data);

	return mdb::MDBTR_SUCESS;
}
