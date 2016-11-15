#include "StdAfx.h"
#include "GDBTaskStorageUpdateMoney.h"
#include "GCharacterDBCache.h"
#include "GGlobal.h"
#include "GDBCacheSystem.h"
#include "GStorageSystem.h"


GDBTaskStorageUpdateMoney::GDBTaskStorageUpdateMoney(const MUID& uidReqPlayer)
: GDBAsyncTask(uidReqPlayer, SDBT_DBTYPE_GAMEDB, SDBTID_STORAGE_UPDATE_MONEY, 1)
{
}

GDBTaskStorageUpdateMoney::~GDBTaskStorageUpdateMoney(void)
{
}

bool GDBTaskStorageUpdateMoney::Input( const GDBT_STORAGE_UPDATE_MONEY& data )
{
	GCharacterDBCache* cc = gsys.pDBCacheSystem->FindCharCache(GetReqPlayer());
	if (NULL == cc)
		return false;

	if (!cc->BeginCacheSync())
		return false;

	m_data = data;

	return true;
}

void GDBTaskStorageUpdateMoney::OnExecute( mdb::MDatabase& rfDB )
{
	mdb::MDBRecordSet rs(&rfDB);
	if (!ExecuteW(rs, GetSQLW(0)))
		return;
}

void GDBTaskStorageUpdateMoney::_OnPreCompleted()
{
	__super::_OnPreCompleted();

	GCharacterDBCache* cc = gsys.pDBCacheSystem->FindCharCache(GetReqPlayer());
	if (NULL == cc)
		return;

	cc->EndCacheSync(IsSuccess());
}

mdb::MDB_THRTASK_RESULT GDBTaskStorageUpdateMoney::_OnCompleted()
{
	if (GDBT_STORAGE_UPDATE_MONEY::DEPOSIT == m_data.m_nType)
	{
		gsys.pStorageSystem->DepositMoneyForDBTask(m_data);
	}
	else if (GDBT_STORAGE_UPDATE_MONEY::WITHDRAW == m_data.m_nType)
	{
		gsys.pStorageSystem->WithdrawForDBTask(m_data);
	}
	

	return mdb::MDBTR_SUCESS;
}