#include "stdafx.h"
#include "GDBTaskItemEquip.h"
#include "GDBTaskGlobal.h"
#include "GItemSystem.h"
#include "GItemEquiperForDBTask.h"


GDBTaskItemEquip::GDBTaskItemEquip( const MUID& uidReqPlayer )
: GDBAsyncTask(uidReqPlayer, SDBT_DBTYPE_GAMEDB, SDBTID_ITEM_EQUIP), m_Syncer(uidReqPlayer)
{

}

GDBTaskItemEquip::~GDBTaskItemEquip()
{

}

void GDBTaskItemEquip::_OnPreCompleted()
{
	__super::_OnPreCompleted();
	m_Syncer.EndCacheSync(IsSuccess());
}

bool GDBTaskItemEquip::Input( GDBT_ITEM_EQUIP& data )
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

void GDBTaskItemEquip::OnExecute( mdb::MDatabase& rfDB )
{
	mdb::MDatabaseQuery q(&rfDB);
	ExecuteW(q, GetSQLW(ITEM_EQUIP));
}

mdb::MDB_THRTASK_RESULT GDBTaskItemEquip::_OnCompleted()
{
	gsys.pItemSystem->GetEquiperForDBTask().Equip(m_Data);
	return mdb::MDBTR_SUCESS;
}

mdb::MDB_THRTASK_RESULT GDBTaskItemEquip::_OnFailed()
{
	gsys.pItemSystem->GetEquiperForDBTask().Fail(m_Data);
	return mdb::MDBTR_SUCESS;
}

void GDBTaskItemEquip::MakeSyncNeedIUID( GDBT_ITEM_EQUIP &data, GDBSYNC_ITEM_VEC& vSyncItem )
{
	vSyncItem.push_back(GDBSYNC_ITEM(data.equip.m_nSlotType, data.equip.m_nSlotID, data.equip.m_nIUID));
	if (0 != data.unequip1.m_nIUID)
	{
		vSyncItem.push_back(GDBSYNC_ITEM(GDBSYNC_ITEM((uint8)SLOTTYPE_EQUIP, data.unequip1.m_nSlotID, data.unequip1.m_nIUID)));
	}
	if (0 != data.unequip2.m_nIUID)
	{
		vSyncItem.push_back(GDBSYNC_ITEM(GDBSYNC_ITEM((uint8)SLOTTYPE_EQUIP, data.unequip2.m_nSlotID, data.unequip2.m_nIUID)));
	}
}