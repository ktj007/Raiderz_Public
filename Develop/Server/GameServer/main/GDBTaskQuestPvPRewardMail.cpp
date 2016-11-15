#include "stdafx.h"
#include "GDBTaskQuestPvPRewardMail.h"
#include "GEntityPlayer.h"
#include "GMoneySystem.h"
#include "GExpSystem.h"
#include "GGlobal.h"
#include "GPlayerObjectManager.h"
#include "GCharacterDBCache.h"
#include "GItemDBCache.h"
#include "GDBCacheSystem.h"
#include "GMailSystem.h"
#include "GDBManager.h"
#include "GPlayerLevelUpdater.h"

GDBTaskQuestPvPRewardMail::GDBTaskQuestPvPRewardMail( const MUID& uidPlayer ) 
: GDBAsyncTask(uidPlayer, SDBT_DBTYPE_GAMEDB, SDBTID_QUEST_PVP_REWARD_MAIL)
, m_SyncingList(uidPlayer)
{

}

void GDBTaskQuestPvPRewardMail::_OnPreCompleted()
{
	__super::_OnPreCompleted();

	m_SyncingList.EndCacheSync(IsSuccess());
}

void GDBTaskQuestPvPRewardMail::OnExecute( mdb::MDatabase& rfDB )
{
	mdb::MDBRecordSet rs(&rfDB);
	if (!ExecuteW(rs, GetSQLW(QPVP_REWARD_MAIL)))
		return;

	m_Data.m_nMailUID = rs.FieldW(L"MAIL_ID").AsInt64();
}

mdb::MDB_THRTASK_RESULT GDBTaskQuestPvPRewardMail::_OnCompleted()
{
	gsys.pDBManager->QuestPVPERewardMailLog(m_Data);

	GEntityPlayer* pPlayer = gmgr.pPlayerObjectManager->GetEntity(m_Data.m_uidPlayer);
	if (NULL == pPlayer)
		return mdb::MDBTR_SUCESS;

	if (0 < m_Data.m_nDeltaLevel)
	{
		GPlayerLevelUpdater levelUpdater(pPlayer);
		levelUpdater.LevelUpForDBTask(m_Data.m_nLevel, m_Data.m_nXP, m_Data.m_nDeltaXP);
	}
	else
	{
		gsys.pExpSystem->AddExpForDB(pPlayer, m_Data.m_nDeltaXP);
	}

	gsys.pMoneySystem->IncreaseForDBTask(pPlayer, m_Data.m_nDeltaMoney);

	if (0 < m_Data.m_nMailUID)
	{
		TD_MAIL_MAILBOX_MAIL_INFO info;
		info.uidMail = m_Data.m_nMailUID;
		info.nType = MT_NPC;
		wcsncpy_s(info.strSenderName, m_Data.m_strSender.c_str(), _TRUNCATE);
		wcsncpy_s(info.strMailTitle, m_Data.m_strTitle.c_str(), _TRUNCATE);
		info.nAppendedItemID = m_Data.m_nDefaultItemID;
		info.bIsRead = false;
		info.nExpiringTimeSec = m_Data.m_nRemainDeleteSeconds;
		info.bHasText = false;	
		info.nAppendedMoney = 0;

		// AppServer¿¡ Àü´Þ
		gsys.pMailSystem->RouteNewMailToReceiver(pPlayer->GetCID(), &info);
	}
	else
	{
		MLog3("QPvP Reward to Mail. [CSN:%I64d, EventID:%d].\n"
			, m_Data.m_nCID, m_Data.m_nEventID);
	}

	return mdb::MDBTR_SUCESS;
}

mdb::MDB_THRTASK_RESULT GDBTaskQuestPvPRewardMail::_OnFailed()
{
	return mdb::MDBTR_SUCESS;
}



bool GDBTaskQuestPvPRewardMail::Input( GDBT_QPER_TOMAIL& data )
{
	if (!BeginCharCacheSync(data))
		return false;

	m_Data.Set(data);

	return true;
}

bool GDBTaskQuestPvPRewardMail::BeginCharCacheSync( GDBT_QPER_TOMAIL& data )
{
	GCharacterDBCache* pcc = gsys.pDBCacheSystem->FindCharCache(data.m_uidPlayer);
	if (NULL == pcc)
	{
		m_SyncingList.EndCacheSync(false);
		return false;
	}

	if (!m_SyncingList.PushChar())
	{
		m_SyncingList.EndCacheSync(false);
		return false;
	}

	return true;
}
