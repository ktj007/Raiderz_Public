#include "stdafx.h"
#include "GDBTaskTalentLearn.h"
#include "GDBTaskGlobal.h"
#include "GTrainingSystem.h"
#include "GDBManager.h"

enum GDBTaskTalentLearnResult
{
	GDBTASK_TALENT_LEARN_RESULT_CRITICAL_ERROR = -1,
	GDBTASK_TALENT_LEARN_RESULT_SUCCESS = 0,
	GDBTASK_TALENT_LEARN_RESULT_NOT_ENOUGH_TP = 1,
};


GDBTaskTalentLearn::GDBTaskTalentLearn( const MUID& uidReqPlayer ) 
: GDBAsyncTask(uidReqPlayer, SDBT_DBTYPE_GAMEDB, SDBTID_TALENTLEARN)
, m_eResult(GDBTASK_TALENT_LEARN_RESULT_CRITICAL_ERROR)
{

}


void GDBTaskTalentLearn::Input(GDBT_TALENT& data)
{
	m_Data.Set(data);
}

void GDBTaskTalentLearn::OnExecute(mdb::MDatabase& rfDB)
{
	mdb::MDBRecordSet rs(&rfDB);
	if (!ExecuteW(rs, GetSQLW(TALENT_LEAN)))
		return;

	if (0 == rs.GetFetchedCount())
	{
		SetTaskFail();
		return;
	}

	m_eResult = static_cast<GDBTaskTalentLearnResult>(rs.FieldW(L"RESULT").AsInt());
}


mdb::MDB_THRTASK_RESULT GDBTaskTalentLearn::_OnCompleted()
{
	Completer completer(m_Data, m_eResult);
	completer.Do();

	return mdb::MDBTR_SUCESS;
}

void GDBTaskTalentLearn::Completer::Do()
{
	if (m_eResult == GDBTASK_TALENT_LEARN_RESULT_SUCCESS)
	{
		gsys.pDBManager->TalentLearnLog(m_Data);
		gsys.pTrainingSystem->TrainForDBTask(m_Data);
	}
	else if (m_eResult == GDBTASK_TALENT_LEARN_RESULT_NOT_ENOUGH_TP)
	{
		gsys.pTrainingSystem->TrainForDBTaskFail_NotEnoughTP(m_Data);
	}
}