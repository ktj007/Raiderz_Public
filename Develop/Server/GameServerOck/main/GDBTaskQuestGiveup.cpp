#include "stdafx.h"
#include "GDBTaskQuestGiveup.h"
#include "GDBTaskGlobal.h"
#include "GQuestSystem.h"
#include "GQuestCanceler.h"
#include "GDBManager.h"


GDBTaskQuestGiveup::GDBTaskQuestGiveup(const MUID& uidReqPlayer) 
: GDBAsyncTask(uidReqPlayer, SDBT_DBTYPE_GAMEDB, SDBTID_QUESTGIVEUP), m_SyncingList(uidReqPlayer)
{

}

GDBTaskQuestGiveup::~GDBTaskQuestGiveup()
{

}

bool GDBTaskQuestGiveup::Input( GDBT_QUEST_GIVEUP& data )
{
	if (!BeginCacheSync(data))
	{
		m_SyncingList.EndCacheSync(false);
		return false;
	}

	m_Data.Set(data);

	return true;
}

void GDBTaskQuestGiveup::OnExecute( mdb::MDatabase& rfDB )
{
	mdb::MDatabaseQuery dbq(&rfDB);
	if (!ExecuteW(dbq, GetSQLW(QUEST_GIVEUP)))
		return;
}

mdb::MDB_THRTASK_RESULT GDBTaskQuestGiveup::_OnCompleted()
{
	Completer completer(m_Data);
	completer.Do();

	return mdb::MDBTR_SUCESS;
}

bool GDBTaskQuestGiveup::BeginCacheSync( GDBT_QUEST_GIVEUP& data )
{
	for each (const GITEM_STACK_AMT& amt in data.m_vecQItems)
	{
		GItemDBCache* pi = gsys.pDBCacheSystem->FindItemCache(data.m_uidPlayer, amt.nSlotType, amt.nSlotID, amt.nIUID);
		if (NULL == pi)
			return false;

		if (!pi->BeginCacheSync())
			return false;
		
		if (NULL == m_SyncingList.PushItem(pi->GetIUID()))
		{
			pi->EndCacheSync(false);
			return false;
		}
	}

	return true;
}

void GDBTaskQuestGiveup::_OnPreCompleted()
{
	__super::_OnPreCompleted();
	m_SyncingList.EndCacheSync(IsSuccess());
}

GDBTaskQuestGiveup::Completer::Completer( GDBT_QUEST_GIVEUP& Data ) : m_Data(Data)
{

}

void GDBTaskQuestGiveup::Completer::Do()
{
	gsys.pDBManager->QuestGiveupLog(m_Data);
	gsys.pQuestSystem->GetCanceler().CancelForDBTask(m_Data);	
}