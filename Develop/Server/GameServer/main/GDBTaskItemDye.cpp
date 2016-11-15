#include "stdafx.h"
#include "GDBTaskItemDye.h"
#include "GEntityPlayer.h"
#include "GPlayerObjectManager.h"
#include "GGlobal.h"
#include "GItemHolder.h"
#include "GItemDyer.h"
#include "GDBManager.h"


GDBTaskItemDye::GDBTaskItemDye( const MUID& uidReqPlayer )
: GDBAsyncTask(uidReqPlayer, SDBT_DBTYPE_GAMEDB, SDBTID_ITEM_DYE), m_Syncer(uidReqPlayer)
{

}

bool GDBTaskItemDye::Input( const GDBT_ITEM_DYE& data )
{
	if (!SyncItem(data))
		return false;

	m_Data = data;

	return true;
}

bool GDBTaskItemDye::SyncItem( const GDBT_ITEM_DYE& data )
{
	if (false == m_Syncer.BeginCacheSync(SLOTTYPE_INVENTORY, data.m_nDyeItemSlotID, data.m_nDyeIUID))
	{
		m_Syncer.EndCacheSync(false);
		return false;
	}

	if (false == m_Syncer.BeginCacheSync(data.m_nTargetItemSlotType, data.m_nTargetItemSlotID, data.m_nTargetIUID))
	{
		m_Syncer.EndCacheSync(false);
		return false;
	}

	return true;
}

void GDBTaskItemDye::_OnPreCompleted()
{
	__super::_OnPreCompleted();
	m_Syncer.EndCacheSync(IsSuccess());
}

mdb::MDB_THRTASK_RESULT GDBTaskItemDye::_OnCompleted()
{
	gsys.pDBManager->ItemDyeLog(m_Data);
	gsys.pItemSystem->GetDyer().DyeForDBTask(m_Data);

	return mdb::MDBTR_SUCESS;
}

void GDBTaskItemDye::OnExecute( mdb::MDatabase& rfDB )
{
	mdb::MDatabaseQuery q(&rfDB);
	ExecuteW(q, GetSQLW(ITEM_DYE));	
}
