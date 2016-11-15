#include "stdafx.h"
#include "GDBTaskGlobal.h"
#include "GDBTaskCharIncMoney.h"
#include "GEntityPlayer.h"
#include "GGlobal.h"
#include "GMoneySystem.h"
#include "GSystem.h"
#include "GCharacterDBCache.h"
#include "GDBManager.h"
#include "GTimeCalculator.h"


GDBTaskCharIncMoney::GDBTaskCharIncMoney(const MUID& uidPlayer, SDBTASK_ID TaskID)
: GDBAsyncTask(uidPlayer, SDBT_DBTYPE_GAMEDB, TaskID)
, m_nAID(0), m_nCID(0), m_nCharPtm(0), m_nDeltaCharPtm(0), m_nXP(0), m_nIncMoney(0), m_SyncingList(uidPlayer), m_nMoney(0), m_nLogCode(0)
{

}

bool GDBTaskCharIncMoney::Input( const GDBT_CHAR_MONEY_INC_DEC& data )
{
	if (!UpdateCache(data.m_nDeltaMoney, data.m_nMoney))
		return false;

	m_Data = data;

	return true;
}

void GDBTaskCharIncMoney::OnExecute( mdb::MDatabase& rfDB )
{
	mdb::MDatabaseQuery q(&rfDB);
	ExecuteW(q, GetSQLW(LOOT_MONEY));
}

mdb::MDB_THRTASK_RESULT GDBTaskCharIncMoney::_OnCompleted()
{
	gsys.pDBManager->CharUpdateMoneyLog(GLOG_DATA_MONEY(m_Data.m_nAID, m_Data.m_nCID, m_Data.m_nCode, m_Data.m_nCharPtm, m_Data.m_nDeltaCharPtm
		, m_Data.m_nLevel, m_Data.m_nMoney, m_Data.m_nDeltaMoney, GTimeCalculator::GetTimeAsString(GTimeSec(0)), m_Data.m_nNpcID));
	
	GEntityPlayer* pPlayer = gmgr.pPlayerObjectManager->GetEntity(GetReqPlayer());
	if (NULL == pPlayer)
		return mdb::MDBTR_SUCESS;

	gsys.pMoneySystem->IncreaseForDBTask(pPlayer, m_Data.m_nDeltaMoney);

	if (SDBTID_CHARLOOTMONEY == m_TaskID)
	{
		pPlayer->GetPlayerInfo()->nLootGainMoney += m_Data.m_nDeltaMoney;
	}	

	return mdb::MDBTR_SUCESS;
}

void GDBTaskCharIncMoney::_OnPreCompleted()
{
	__super::_OnPreCompleted();
	m_SyncingList.EndCacheSync(IsSuccess());
}

bool GDBTaskCharIncMoney::UpdateCache( const int nIncMoney, const int nMoney )
{
	GCharacterDBCache* cc = gsys.pDBCacheSystem->FindCharCache(GetReqPlayer());
	if (NULL == cc)
		return true;

	if (!cc->IncMoney(nIncMoney))
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