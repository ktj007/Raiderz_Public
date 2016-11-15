#include "stdafx.h"
#include "GDBTaskItemIncStackAmt.h"
#include "GItemAdder.h"
#include "GGlobal.h"
#include "GItemSystem.h"
#include "GDBManager.h"

GDBTaskItemIncStackAmt::GDBTaskItemIncStackAmt( const MUID& uidPlayer )
: GDBTaskItemStackAmt(uidPlayer, SDBTID_ITEM_INC_STACKAMT)
{
	
}

bool GDBTaskItemIncStackAmt::Input( GDBT_ITEM_DATA& data )
{
	if (!IncCache(data.m_uidPlayer, data.m_Item.m_nSlotType, data.m_Item.m_nSlotID, data.m_Item.m_nIUID, data.m_nModStackAmt, data.m_Item.m_nStackAmt, data.m_nTier))
		return false;

	m_Data = data;

	return true;
}

void GDBTaskItemIncStackAmt::OnExecute( mdb::MDatabase& rfDB )
{
	mdb::MDatabaseQuery q(&rfDB);
	ExecuteW(q, GetSQLW(ITEM_INC_STACK_AMT));
}

mdb::MDB_THRTASK_RESULT GDBTaskItemIncStackAmt::_OnCompleted()
{
	//if (ITEM_TIER_RARE <= m_Data.m_nTier)
	gsys.pDBManager->ItemInsertLog(m_Data);

	ADD_ITEM addItem = ADD_ITEM(SLOTTYPE_INVENTORY, m_Data.m_Item.m_nSlotID, m_Data.m_Item.m_nStackAmt, m_Data.m_nModStackAmt);
	gsys.pItemSystem->GetAdder().AddForDBTask(m_Data.m_uidPlayer, addItem);

	return mdb::MDBTR_SUCESS;
}