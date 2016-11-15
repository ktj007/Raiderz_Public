#include "stdafx.h"
#include "GDBTaskGlobal.h"
#include "GDBTaskCharDecMoney.h"
#include "GEntityPlayer.h"
#include "GGlobal.h"
#include "GMoneySystem.h"
#include "GSystem.h"
#include "GCharacterDBCache.h"
#include "GDBManager.h"
#include "GDB_CODE.h"
#include "GTimeCalculator.h"


GDBTaskCharDecMoney::GDBTaskCharDecMoney( const MUID& uidPlayer, SDBTASK_ID TaskID )
: GDBAsyncTask(uidPlayer, SDBT_DBTYPE_GAMEDB, TaskID), m_SyncingList(uidPlayer)
{

}

bool GDBTaskCharDecMoney::Input( const GDBT_CHAR_MONEY_INC_DEC& data )
{
	if (!UpdateCache(data.m_nDeltaMoney, data.m_nMoney))
		return false;

	m_Data = data;

	return true;
}

void GDBTaskCharDecMoney::OnExecute( mdb::MDatabase& rfDB )
{
	mdb::MDatabaseQuery q(&rfDB);
	ExecuteW(q, GetSQLW(LOOT_MONEY));
}

mdb::MDB_THRTASK_RESULT GDBTaskCharDecMoney::_OnCompleted()
{
	gsys.pDBManager->CharUpdateMoneyLog(GLOG_DATA_MONEY(m_Data.m_nAID, m_Data.m_nCID, m_Data.m_nCode, m_Data.m_nCharPtm, m_Data.m_nDeltaCharPtm
		, m_Data.m_nLevel, m_Data.m_nMoney, -m_Data.m_nDeltaMoney, GTimeCalculator::GetTimeAsString(GTimeSec(0)), 0));

	GEntityPlayer* pPlayer = gmgr.pPlayerObjectManager->GetEntity(GetReqPlayer());
	if (NULL == pPlayer)
		return mdb::MDBTR_SUCESS;

	gsys.pMoneySystem->DecreaseForDBTask(pPlayer, m_Data.m_nDeltaMoney);

	return mdb::MDBTR_SUCESS;
}

void GDBTaskCharDecMoney::_OnPreCompleted()
{
	__super::_OnPreCompleted();

	if (!IsSync())
		return;

	m_SyncingList.EndCacheSync(IsSuccess());
}

bool GDBTaskCharDecMoney::UpdateCache( const int nDecMoney, const int nMoney )
{
	GCharacterDBCache* cc = gsys.pDBCacheSystem->FindCharCache(GetReqPlayer());
	if (NULL == cc)
		return true;

	if (!cc->DecMoney(nDecMoney))
		return false;

	if (!cc->IsOverflow())
	{
		SetSync(false);	
		cc->EndCacheSync(false);
	}
	else
	{
		if (NULL == m_SyncingList.PushChar())
		{
			cc->EndCacheSync(false);
			return false;
		}
	}

	return true;
}