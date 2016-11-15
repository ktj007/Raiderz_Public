#include "stdafx.h"
#include "GDBTaskItemLootInsert.h"
#include "GDBTaskGlobal.h"
#include "GItemAdder.h"
#include "GItemHolder.h"
#include "GLootSystem.h"
#include "GLootItemGetter.h"
#include "GDBManager.h"

GDBTaskItemLootInsert::GDBTaskItemLootInsert( const MUID& uidPlayer ) 
: GDBAsyncTask(uidPlayer, SDBT_DBTYPE_GAMEDB, SDBTID_ITEMLOOTINSERT)
{
	
}

GDBTaskItemLootInsert::~GDBTaskItemLootInsert()
{

}

void GDBTaskItemLootInsert::OnExecute( mdb::MDatabase& rfDB )
{
	mdb::MDBRecordSet rs(&rfDB);
	if (!ExecuteW(rs, GetSQLW(ITEM_LOOT_INSERT)))
		return;

	if (0 == rs.GetFetchedCount())
	{
		SetTaskFail();
		return;
	}

	m_Result.m_nIUID = rs.FieldW(L"IUID").AsInt64();
	m_Data.m_Item.m_nIUID = m_Result.m_nIUID;
}

mdb::MDB_THRTASK_RESULT GDBTaskItemLootInsert::_OnCompleted()
{
	// if (ITEM_TIER_RARE <= m_Data.m_nTier)
	gsys.pDBManager->ItemLootLog(m_Data);

	ADD_ITEM addItem = ADD_ITEM(SLOTTYPE_INVENTORY, m_Data.m_Item.m_nSlotID, m_Data.m_Item.m_nStackAmt, m_Data.m_Item.m_nStackAmt, m_Result.m_nIUID, m_Data.m_Item.m_nItemID, -1, m_Data.m_Item.m_nUsagePeriod, m_Data.m_Item.m_nDura, m_Data.m_Item.m_nColor);
	gsys.pItemSystem->GetAdder().AddForDBTask(m_Data.m_uidPlayer, addItem);	

	return mdb::MDBTR_SUCESS;
}

mdb::MDB_THRTASK_RESULT	GDBTaskItemLootInsert::_OnFailed()
{
	GEntityPlayer* pPlayer = gmgr.pPlayerObjectManager->GetEntity(m_Data.m_uidPlayer);
	if (NULL != pPlayer)
	{
		GItemHolder* pItemHolder = pPlayer->GetItemHolder();
		pItemHolder->CancelReserveSlot(SLOTTYPE_INVENTORY, m_Data.m_Item.m_nSlotID);
	}

	return mdb::MDBTR_SUCESS;
}

void GDBTaskItemLootInsert::Input( const GDBT_ITEM_DATA& data )
{
	m_Data = data;
}