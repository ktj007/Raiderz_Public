#include "StdAfx.h"
#include "GDBTaskGuildUpdateStorageMoney.h"
#include "GCharacterDBCache.h"
#include "GGlobal.h"
#include "GDBCacheSystem.h"
#include "GDBTaskDataStorage.h"
#include "GGuildSystem.h"
#include "GGuildSystemForDBTask.h"
#include "GGuildSystem.h"

GDBTaskGuildUpdateStorageMoney::GDBTaskGuildUpdateStorageMoney(const MUID& uidReqPlayer)
: GDBAsyncTask(uidReqPlayer, SDBT_DBTYPE_GAMEDB, SDBTID_GUILDUPDATESTORAGEMONEY, 1)
{
}

GDBTaskGuildUpdateStorageMoney::~GDBTaskGuildUpdateStorageMoney(void)
{
}

bool GDBTaskGuildUpdateStorageMoney::Input( const GDBT_GUILD_UPDATE_STORAGEMONEY& data )
{
	GCharacterDBCache* cc = gsys.pDBCacheSystem->FindCharCache(GetReqPlayer());
	if (NULL == cc)
		return false;

	if (!cc->BeginCacheSync())
		return false;

	m_data = data;

	return true;
}

void GDBTaskGuildUpdateStorageMoney::OnExecute( mdb::MDatabase& rfDB )
{
	mdb::MDBRecordSet rs(&rfDB);
	if (!ExecuteW(rs, GetSQLW(0)))
		return;
}

void GDBTaskGuildUpdateStorageMoney::_OnPreCompleted()
{
	__super::_OnPreCompleted();

	GCharacterDBCache* cc = gsys.pDBCacheSystem->FindCharCache(GetReqPlayer());
	if (NULL == cc)
		return;

	cc->EndCacheSync(IsSuccess());
}

mdb::MDB_THRTASK_RESULT GDBTaskGuildUpdateStorageMoney::_OnCompleted()
{
	if (GDBT_STORAGE_UPDATE_MONEY::DEPOSIT == m_data.m_nType)
	{
		gsys.pGuildSystem->ForDBTask().DepositStorageMoney(m_data);
	}
	else if (GDBT_STORAGE_UPDATE_MONEY::WITHDRAW == m_data.m_nType)
	{
		gsys.pGuildSystem->ForDBTask().WithdrawStorageMoney(m_data);
	}

	return mdb::MDBTR_SUCESS;
}

mdb::MDB_THRTASK_RESULT GDBTaskGuildUpdateStorageMoney::_OnFailed()
{
	gsys.pGuildSystem->RouteSyncFail(m_data.m_uidPlayer);

	return mdb::MDBTR_SUCESS;
}
