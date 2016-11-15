#include "stdafx.h"
#include "GDBTaskItemMerge.h"
#include "GDBTaskGlobal.h"
#include "GItemMoverForDBTask.h"
#include "GDBManager.h"
#include "GGuildSystem.h"

GDBTaskItemMerge::GDBTaskItemMerge( const MUID& uidReqPlayer ) 
: GDBAsyncTask(uidReqPlayer, SDBT_DBTYPE_GAMEDB, SDBTID_ITEMMERGE), m_SyncingList(uidReqPlayer)
{
	
}

GDBTaskItemMerge::~GDBTaskItemMerge()
{

}

void GDBTaskItemMerge::OnExecute( mdb::MDatabase& rfDB )
{
	mdb::MDatabaseQuery q(&rfDB);
	ExecuteW(q, GetSQLW(ITEM_MERGE));
}

mdb::MDB_THRTASK_RESULT GDBTaskItemMerge::_OnCompleted()
{
	Completer completer(m_Data);
	completer.Do();

	return mdb::MDBTR_SUCESS;
}

bool GDBTaskItemMerge::Input( const GDBT_ITEM_MERGE_AND_SPLIT& data )
{
	if (!BeginItemCacheSync(data.m_uidPlayer, data.m_FromSlot.m_nSlotType, data.m_FromSlot.m_nSlotID, data.m_FromSlot.m_nIUID))
		return false;

	if (!BeginItemCacheSync(data.m_uidPlayer, data.m_ToSlot.m_nSlotType, data.m_ToSlot.m_nSlotID, data.m_ToSlot.m_nIUID))
	{
		m_SyncingList.EndCacheSync(false);
		return false;
	}

	m_Data = data;

	return true;
}

void GDBTaskItemMerge::_OnPreCompleted()
{
	__super::_OnPreCompleted();
	m_SyncingList.EndCacheSync(IsSuccess());

	gsys.pItemSystem->GetMoverForDBTask().OnPreCompleted(m_Data.m_uidPlayer
		, (SH_ITEM_SLOT_TYPE)m_Data.m_FromSlot.m_nSlotType, m_Data.m_FromSlot.m_nSlotID
		, (SH_ITEM_SLOT_TYPE)m_Data.m_ToSlot.m_nSlotType, m_Data.m_ToSlot.m_nSlotID, m_Data.m_nMergeAndSplitAmt);
}

bool GDBTaskItemMerge::BeginItemCacheSync(const MUID& uidPlaeyr, const uint8 nSlotType, const int16 nSlotID, const IUID nIUID )
{
	GItemDBCache* ic = gsys.pDBCacheSystem->FindItemCache(uidPlaeyr, nSlotType, nSlotID, nIUID);
	if (NULL == ic)
		return false;

	if (!ic->BeginCacheSync())
		return false;

	if (NULL == m_SyncingList.PushItem(nIUID))
	{
		ic->EndCacheSync(false);
		return false;
	}

	return true;
}

void GDBTaskItemMerge::Completer::Do()
{
	gsys.pDBManager->ItemMergeAndSplitLog(m_Data);

	gsys.pItemSystem->GetMoverForDBTask().Move(m_Data.m_uidPlayer
		, (SH_ITEM_SLOT_TYPE)m_Data.m_FromSlot.m_nSlotType, m_Data.m_FromSlot.m_nSlotID
		, (SH_ITEM_SLOT_TYPE)m_Data.m_ToSlot.m_nSlotType, m_Data.m_ToSlot.m_nSlotID, m_Data.m_nMergeAndSplitAmt);
}

mdb::MDB_THRTASK_RESULT GDBTaskItemMerge::_OnFailed()
{
	if (SLOTTYPE_GUILDSTORAGE == m_Data.m_FromSlot.m_nSlotType || SLOTTYPE_GUILDSTORAGE == m_Data.m_ToSlot.m_nSlotType)
	{
		gsys.pGuildSystem->RouteSyncFail(m_Data.m_uidPlayer);
	}	

	return mdb::MDBTR_SUCESS;
}
