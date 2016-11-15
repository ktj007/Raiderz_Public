#include "stdafx.h"
#include "GDBTaskQuestFail.h"
#include "GDBTaskGlobal.h"
#include "GPlayerObject.h"
#include "GPlayerObjectManager.h"
#include "GPlayerQuest.h"
#include "GPlayerQuests.h"
#include "GScriptGroup_Quest.h"
#include "GScriptManager.h"
#include "GScriptUtil.h"
#include "GNullGlueQuest.h"
#include "GInteractionSystem.h"
#include "GConditionInfo.h"
#include "GQuestSystem.h"
#include "GCommand.h"
#include "CCommandTable.h"
#include "GPartySystem.h"
#include "GPlayerNPCIconSender.h"
#include "GDBManager.h"
#include "GQuestFailer.h"


GDBTaskQuestFail::GDBTaskQuestFail(const MUID& uidReqPlayer) 
: GDBAsyncTask(uidReqPlayer, SDBT_DBTYPE_GAMEDB, SDBTID_QUESTFAIL)
{
}

GDBTaskQuestFail::~GDBTaskQuestFail()
{

}

void GDBTaskQuestFail::Input( const GDBT_QUEST_COMMON& data )
{
	m_Data = data;
}

void GDBTaskQuestFail::OnExecute( mdb::MDatabase& rfDB )
{
	mdb::MDatabaseQuery dbq(&rfDB);
	if (!ExecuteW(dbq, GetSQLW(QUEST_FAIL)))
		return;
}

mdb::MDB_THRTASK_RESULT GDBTaskQuestFail::_OnCompleted()
{
	gsys.pQuestSystem->GetFailer().FailForDBTask(m_Data);

	return mdb::MDBTR_SUCESS;
}