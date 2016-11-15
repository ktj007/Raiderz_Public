#include "stdafx.h"
#include "GDBTaskItemLootUpdate.h"
#include "GDBTaskGlobal.h"
#include "GItemAdder.h"
#include "GItemHolder.h"
#include "GLootSystem.h"
#include "GLootItemGetter.h"
#include "GDBManager.h"

GDBTaskItemLootUpdate::GDBTaskItemLootUpdate( const MUID& uidPlayer ) 
: GDBTaskItemStackAmt(uidPlayer, SDBTID_ITEMLOOTUPDATE)
{
	
}

GDBTaskItemLootUpdate::~GDBTaskItemLootUpdate()
{

}

bool GDBTaskItemLootUpdate::Input(const GDBT_ITEM_DATA& data)
{
	if (!IncCache(data.m_uidPlayer, data.m_Item.m_nSlotType, data.m_Item.m_nSlotID, data.m_Item.m_nIUID, data.m_nModStackAmt, data.m_Item.m_nStackAmt, data.m_nTier))
		return false;

	m_Data = data;

	return true;
}

void GDBTaskItemLootUpdate::OnExecute( mdb::MDatabase& rfDB )
{
	mdb::MDatabaseQuery q(&rfDB);
	ExecuteW(q, GetSQLW(ITEM_LOOT_UPDATE));
}

mdb::MDB_THRTASK_RESULT GDBTaskItemLootUpdate::_OnCompleted()
{
	// if (m_bWriteLog)
	gsys.pDBManager->ItemLootLog(m_Data);

	ADD_ITEM addItem = ADD_ITEM(SLOTTYPE_INVENTORY, m_Data.m_Item.m_nSlotID, m_Data.m_Item.m_nStackAmt, m_Data.m_nModStackAmt);
	gsys.pItemSystem->GetAdder().AddForDBTask(m_Data.m_uidPlayer, addItem);

	return mdb::MDBTR_SUCESS;
}