#include "StdAfx.h"
#include "GDBTaskItemSplit.h"
#include "GGlobal.h"
#include "GItemSystem.h"
#include "GItemMoverForDBTask.h"
#include "GPlayerObjectManager.h"
#include "GEntityPlayer.h"
#include "GItemHolder.h"
#include "GGuildSystem.h"
#include "GDBManager.h"

GDBTaskItemSplit::GDBTaskItemSplit(const MUID& uidPlayer)
: GDBAsyncTask(uidPlayer, SDBT_DBTYPE_GAMEDB, SDBTID_ITEMSPLIT), m_syncer(uidPlayer)
{
}

GDBTaskItemSplit::~GDBTaskItemSplit(void)
{
}

bool GDBTaskItemSplit::Input( GDBT_ITEM_MERGE_AND_SPLIT& data )
{
	if (!m_syncer.BeginCacheSync(data.m_FromSlot.m_nSlotType, data.m_FromSlot.m_nSlotID, data.m_FromSlot.m_nIUID))
		return false;

	m_data = data;

	return true;
}

void GDBTaskItemSplit::OnExecute( mdb::MDatabase& rfDB )
{
	mdb::MDBRecordSet rs(&rfDB);

	if (!ExecuteW(rs, GetSQLW(0)))
		return;

	if (1 == rs.GetFetchedCount())
		m_data.m_ToSlot.m_nIUID = rs.FieldW(L"NEW_IUID").AsInt64();		
}

void GDBTaskItemSplit::_OnPreCompleted()
{
	__super::_OnPreCompleted();
	m_syncer.EndCacheSync(IsSuccess());

	gsys.pItemSystem->GetMoverForDBTask().OnPreCompleted(m_data.m_uidPlayer,
		(SH_ITEM_SLOT_TYPE)m_data.m_FromSlot.m_nSlotType, m_data.m_FromSlot.m_nSlotID,
		(SH_ITEM_SLOT_TYPE)m_data.m_ToSlot.m_nSlotType, m_data.m_ToSlot.m_nSlotID, m_data.m_ToSlot.m_nStackAmt);
}

mdb::MDB_THRTASK_RESULT GDBTaskItemSplit::_OnCompleted()
{
	gsys.pDBManager->ItemMergeAndSplitLog(m_data);

	gsys.pItemSystem->GetMoverForDBTask().Move(m_data.m_uidPlayer, (SH_ITEM_SLOT_TYPE)m_data.m_FromSlot.m_nSlotType, m_data.m_FromSlot.m_nSlotID, (SH_ITEM_SLOT_TYPE)m_data.m_ToSlot.m_nSlotType, m_data.m_ToSlot.m_nSlotID, m_data.m_ToSlot.m_nStackAmt, m_data.m_ToSlot.m_nIUID);

	return mdb::MDBTR_SUCESS;
}

mdb::MDB_THRTASK_RESULT GDBTaskItemSplit::_OnFailed()
{
	GEntityPlayer* pPlayer = gmgr.pPlayerObjectManager->GetEntity(m_data.m_uidPlayer);
	if (NULL == pPlayer) return mdb::MDBTR_SUCESS;

	pPlayer->GetItemHolder()->CancelReserveSlot((SH_ITEM_SLOT_TYPE)m_data.m_ToSlot.m_nSlotType, m_data.m_ToSlot.m_nSlotID);

	if (SLOTTYPE_GUILDSTORAGE == m_data.m_FromSlot.m_nSlotType || SLOTTYPE_GUILDSTORAGE == m_data.m_ToSlot.m_nSlotType)
	{
		gsys.pGuildSystem->RouteSyncFail(m_data.m_uidPlayer);
	}

	return mdb::MDBTR_SUCESS;
}
