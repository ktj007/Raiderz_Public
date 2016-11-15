#include "stdafx.h"
#include "GDBTaskCharLevelUp.h"
#include "GDBTaskGlobal.h"
#include "GExpSystem.h"
#include "GPlayerTalent.h"
#include "CCommandTable.h"
#include "GCommand.h"
#include "GCharacterDBCache.h"
#include "GDBManager.h"
#include "GPlayerNPCIconSender.h"
#include "GConditionInfo.h"
#include "GPlayerLevelUpdater.h"


GDBTaskCharLevelUp::GDBTaskCharLevelUp( const MUID& uidReqPlayer )
: GDBAsyncTask(uidReqPlayer, SDBT_DBTYPE_GAMEDB, SDBTID_CHARLEVELUP), m_SyncingList(uidReqPlayer)
{

}

bool GDBTaskCharLevelUp::Input( GDBT_CHAR_LEVEL_UP_DATA& data )
{
	if (!SyncCharacter())
		return false;

	m_Data = data;

	return true;
}

bool GDBTaskCharLevelUp::SyncCharacter()
{
	GCharacterDBCache* cc = gsys.pDBCacheSystem->FindCharCache(GetReqPlayer());
	if (NULL == cc)
		return false;

	if (!cc->BeginCacheSync())
		return false;

	if (NULL == m_SyncingList.PushChar())
	{
		cc->EndCacheSync(false);
		return false;
	}

	return true;
}

void GDBTaskCharLevelUp::_OnPreCompleted()
{
	__super::_OnPreCompleted();
	m_SyncingList.EndCacheSync(IsSuccess());
}

void GDBTaskCharLevelUp::OnExecute( mdb::MDatabase& rfDB )
{
	mdb::MDatabaseQuery q(&rfDB);
	ExecuteW(q, GetSQLW(LEVEL_UP));
}

mdb::MDB_THRTASK_RESULT GDBTaskCharLevelUp::_OnCompleted()
{
	Completer completer(m_Data);
	completer.Do();

	return mdb::MDBTR_SUCESS;
}

void GDBTaskCharLevelUp::Completer::Do()
{
	gsys.pDBManager->CharLevelUpLog(m_Data);

	GEntityPlayer* pPlayer = gmgr.pPlayerObjectManager->GetEntity(m_Data.m_uidPlayer);
	if (NULL == pPlayer)
		return;

	GPlayerLevelUpdater levelUpdater(pPlayer);
	levelUpdater.LevelUpForDBTask(m_Data.m_nLevel, m_Data.m_nXP, m_Data.m_nModXP);
}