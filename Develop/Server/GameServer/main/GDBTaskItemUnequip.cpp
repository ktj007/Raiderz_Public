#include "stdafx.h"
#include "GDBTaskItemUnequip.h"
#include "GDBTaskGlobal.h"
#include "GItemSystem.h"
#include "GItemUnequiper.h"


GDBTaskItemUnequip::GDBTaskItemUnequip( const MUID& uidReqPlayer )
: GDBAsyncTask(uidReqPlayer, SDBT_DBTYPE_GAMEDB, SDBTID_ITEM_UNEQUIP), m_Syncer(uidReqPlayer)
{

}

GDBTaskItemUnequip::~GDBTaskItemUnequip()
{

}

void GDBTaskItemUnequip::_OnPreCompleted()
{
	__super::_OnPreCompleted();
	m_Syncer.EndCacheSync(IsSuccess());
}

bool GDBTaskItemUnequip::Input( GDBT_ITEM_UNEQUIP& data )
{
	GDBSYNC_ITEM_VEC vSyncItem;
	MakeSyncNeedIUID(data, vSyncItem);
	if (false == m_Syncer.BeginCacheSync(vSyncItem))
	{
		m_Syncer.EndCacheSync(false);
		return false;
	}

	m_Data = data;

	return true;
}

void GDBTaskItemUnequip::OnExecute( mdb::MDatabase& rfDB )
{
	mdb::MDatabaseQuery q(&rfDB);
	ExecuteW(q, GetSQLW(ITEM_UNEQUIP));
}

mdb::MDB_THRTASK_RESULT GDBTaskItemUnequip::_OnCompleted()
{
	gsys.pItemSystem->GetUnequiper().UnequipForDBTask(m_Data);
	return mdb::MDBTR_SUCESS;
}

mdb::MDB_THRTASK_RESULT GDBTaskItemUnequip::_OnFailed()
{
	gsys.pItemSystem->GetUnequiper().FailForDBTask(m_Data);
	return mdb::MDBTR_SUCESS;
}

void GDBTaskItemUnequip::MakeSyncNeedIUID( GDBT_ITEM_UNEQUIP& data, GDBSYNC_ITEM_VEC& vSyncItem)
{
	vSyncItem.push_back(GDBSYNC_ITEM((uint8)SLOTTYPE_EQUIP, data.unequip1.m_nSlotID, data.unequip1.m_nIUID));
	if (0 != data.unequip2.m_nIUID)
	{
		vSyncItem.push_back(GDBSYNC_ITEM((uint8)SLOTTYPE_EQUIP, data.unequip2.m_nSlotID, data.unequip2.m_nIUID));
	}
}