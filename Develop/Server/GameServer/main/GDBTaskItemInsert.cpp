#include "stdafx.h"
#include "GDBTaskItemInsert.h"
#include "GDBTaskGlobal.h"
#include "GEntityPlayer.h"
#include "GGlobal.h"
#include "GPlayerObjectManager.h"
#include "GItemManager.h"
#include "GCommand.h"
#include "GSystem.h"
#include "GQuestSystem.h"
#include "GItemAdder.h"
#include "GItemHolder.h"
#include "GDBManager.h"

GDBTaskItemInsert::GDBTaskItemInsert( const MUID& uidReqPlayer ) 
: GDBAsyncTask(uidReqPlayer, SDBT_DBTYPE_GAMEDB, SDBTID_ITEMINSERT)
{
	
}

GDBTaskItemInsert::~GDBTaskItemInsert()
{

}

void GDBTaskItemInsert::Input( GDBT_ITEM_DATA& data )
{
	m_Data = data;
}

void GDBTaskItemInsert::OnExecute( mdb::MDatabase& rfDB )
{
	mdb::MDBRecordSet rs(&rfDB);
	if (!ExecuteW(rs, GetSQLW(ITEM_INSERT)))
		return;

	m_Result.m_nIUID = rs.FieldW(L"IUID").AsInt64();
	m_Data.m_Item.m_nIUID = m_Result.m_nIUID;
}

mdb::MDB_THRTASK_RESULT GDBTaskItemInsert::_OnCompleted()
{
	m_Data.m_Item.m_nIUID = m_Result.m_nIUID;

	Completer completer(m_Data, m_Result);
	completer.Do();

	return mdb::MDBTR_SUCESS;
}

mdb::MDB_THRTASK_RESULT	GDBTaskItemInsert::_OnFailed()
{
	GEntityPlayer* pPlayer = gmgr.pPlayerObjectManager->GetEntity(m_Data.m_uidPlayer);
	if (NULL != pPlayer)
	{
		GItemHolder* pItemHolder = pPlayer->GetItemHolder();
		pItemHolder->CancelReserveSlot(SLOTTYPE_INVENTORY, m_Data.m_Item.m_nSlotID);
	}

	return mdb::MDBTR_SUCESS;
}

void GDBTaskItemInsert::Completer::Do()
{
	//if (ITEM_TIER_RARE <= m_Data.m_nTier)
	gsys.pDBManager->ItemInsertLog(m_Data);

	ADD_ITEM addItem = ADD_ITEM(m_Data.m_Item.m_nSlotType, m_Data.m_Item.m_nSlotID, m_Data.m_Item.m_nStackAmt, m_Data.m_Item.m_nStackAmt
		, m_Result.m_nIUID, m_Data.m_Item.m_nItemID, m_Data.m_nAddExpiMin, m_Data.m_Item.m_nUsagePeriod, m_Data.m_Item.m_nDura, m_Data.m_Item.m_nColor);

	gsys.pItemSystem->GetAdder().AddForDBTask(m_Data.m_uidPlayer, addItem);	
}