#include "stdafx.h"
#include "GDBTaskGlobal.h"
#include "GDBTaskTalentResetAll.h"
#include "GPlayerTalent.h"
#include "GPaletteSystem.h"
#include "GInteractionSystem.h"
#include "GPlayerNPCIconSender.h"
#include "CSConditionInfo.h"
#include "GCommand.h"
#include "CCommandTable.h"
#include "GTrainingSystem.h"
#include "GDBManager.h"
#include "GDBTaskItemData.h"


GDBTaskTalentResetAll::GDBTaskTalentResetAll( const MUID& uidPlayer ) 
: GDBAsyncTask(uidPlayer, SDBT_DBTYPE_GAMEDB, SDBTID_TALENTRESETALL)
{

}

GDBTaskTalentResetAll::~GDBTaskTalentResetAll()
{

}

void GDBTaskTalentResetAll::Input( GDBT_TALENT& data )
{
	m_Data.Set(data);
}

void GDBTaskTalentResetAll::OnExecute( mdb::MDatabase& rfDB )
{
	mdb::MDatabaseQuery q(&rfDB);
	ExecuteW(q, GetSQLW(TALENT_RESET_ALL));
}

mdb::MDB_THRTASK_RESULT GDBTaskTalentResetAll::_OnCompleted()
{
	Completer completer(m_Data);
	completer.Do();

	return mdb::MDBTR_SUCESS;
}

void GDBTaskTalentResetAll::Completer::Do()
{
	GDBT_ITEM_DEC_STACK_AMT_DATA dec_stack_amt(m_Data.m_uidPlayer, m_Data.m_nAID, m_Data.m_nCID, m_Data.m_nCharPtm, m_Data.m_nDeltaCharPtm
		, m_Data.m_nLevel, m_Data.m_nMoney, SLOTTYPE_INVENTORY, m_Data.m_nSlotID, m_Data.m_nIUID, m_Data.m_nItemID, 0, -1
		, ITEM_TIER_RARE /*초기화 아이템은 무조건 남기기 위해서.*/, 0);

	gsys.pDBManager->ItemDecStackAmtLog(dec_stack_amt);

	gsys.pTrainingSystem->UntrainAllForDBTask(m_Data);	
}