#include "stdafx.h"
#include "GDBTaskTalentLearn.h"
#include "GDBTaskGlobal.h"
#include "GTrainingSystem.h"
#include "GDBManager.h"

GDBTaskTalentLearn::GDBTaskTalentLearn( const MUID& uidReqPlayer ) 
: GDBAsyncTask(uidReqPlayer, SDBT_DBTYPE_GAMEDB, SDBTID_TALENTLEARN)
{

}


void GDBTaskTalentLearn::Input(GDBT_TALENT& data)
{
	m_Data.Set(data);
}

void GDBTaskTalentLearn::OnExecute(mdb::MDatabase& rfDB)
{
	mdb::MDatabaseQuery dbq(&rfDB);
	if (!ExecuteW(dbq, GetSQLW(TALENT_LEAN)))
		return;	
}


mdb::MDB_THRTASK_RESULT GDBTaskTalentLearn::_OnCompleted()
{
	Completer completer(m_Data);
	completer.Do();

	return mdb::MDBTR_SUCESS;
}

void GDBTaskTalentLearn::Completer::Do()
{
	gsys.pDBManager->TalentLearnLog(m_Data);
	gsys.pTrainingSystem->TrainForDBTask(m_Data);
}