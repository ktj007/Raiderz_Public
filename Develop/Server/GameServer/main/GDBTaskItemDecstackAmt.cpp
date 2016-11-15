#include "stdafx.h"
#include "GDBTaskItemDecstackAmt.h"
#include "GItemRemover.h"
#include "GGlobal.h"
#include "GItemSystem.h"
#include "GDBManager.h"


GDBTaskItemDecStackAmt::GDBTaskItemDecStackAmt( const MUID& uidPlayer )
: GDBTaskItemStackAmt(uidPlayer, SDBTID_ITEM_DEC_STACKAMT)
{

}

bool GDBTaskItemDecStackAmt::Input( GDBT_ITEM_DEC_STACK_AMT_DATA& data )
{
	if (!DecCache(data.m_uidPlayer, data.m_nSlotType, data.m_nSlotID, data.m_nIUID, data.m_nModStackAmt, data.m_nStackAmt, data.m_nTier))
		return false;

	m_Data = data;

	return true;
}

void GDBTaskItemDecStackAmt::OnExecute( mdb::MDatabase& rfDB )
{
	mdb::MDatabaseQuery q(&rfDB);
	ExecuteW(q, GetSQLW(ITEM_DEC_STACK_AMT));
}

mdb::MDB_THRTASK_RESULT GDBTaskItemDecStackAmt::_OnCompleted()
{
	// if (ITEM_TIER_RARE <= m_Data.m_nTier)
	gsys.pDBManager->ItemDecStackAmtLog(m_Data);

	REMOVE_ITEM removeItem = REMOVE_ITEM(m_Data.m_nSlotType, m_Data.m_nSlotID, m_Data.m_nStackAmt, m_Data.m_nModStackAmt);
	gsys.pItemSystem->GetRemover().RemoveForDBTask(m_Data.m_uidPlayer, removeItem);

	return mdb::MDBTR_SUCESS;
}