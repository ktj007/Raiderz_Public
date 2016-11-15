#include "stdafx.h"
#include "GDBTaskCraft.h"
#include "GDBTaskGlobal.h"
#include "GCharacterSyncingCache.h"
#include "GCharacterDBCache.h"


GDBTaskCraft::GDBTaskCraft( const MUID& uidReqPlayer, const SDBTASK_ID TaskID )
: GDBAsyncTask(uidReqPlayer, SDBT_DBTYPE_GAMEDB, TaskID), m_SyncingList(uidReqPlayer)
{

}

void GDBTaskCraft::_OnPreCompleted()
{
	__super::_OnPreCompleted();
	m_SyncingList.EndCacheSync(IsSuccess());
}

bool GDBTaskCraft::SyncCharacter(GPlayerDBCache* pc)
{
	if (NULL == pc)
		return true;

	GCharacterDBCache* cc = pc->GetCharCc();
	if (NULL == cc)
		return false;

	if (!cc->BeginCacheSync())
		return false;

	if (NULL == m_SyncingList.PushChar())
	{
		cc->EndCacheSync(false);
		return false;
	}

	return true;
}

bool GDBTaskCraft::SyncItem( const MUID& uidPlayer, GITEM_STACK_AMT_VEC& vItem )
{
	for each(const GITEM_STACK_AMT& amt in vItem)
	{
		GItemDBCache* pi = gsys.pDBCacheSystem->FindItemCache(uidPlayer, amt.nSlotType, amt.nSlotID, amt.nIUID);
		if (NULL == pi)
			return false;

		if (!pi->BeginCacheSync())
			return false;

		if (NULL == m_SyncingList.PushItem(amt.nIUID))
		{
			pi->EndCacheSync(false);
			return false;
		}
	}

	return true;
}

void GDBTaskCraft::EndCacheSync( const bool bZero )
{
	m_SyncingList.EndCacheSync(bZero);
}

bool GDBTaskCraft::BeginCacheSync( const MUID& uidPlayer, GITEM_STACK_AMT_VEC& vRecp )
{
	GPlayerDBCache* pc = gsys.pDBCacheSystem->FindPlayerCache(uidPlayer);
	if (NULL == pc)
		return true;

	if (!SyncCharacter(pc))
	{
		EndCacheSync(false);
		return false;
	}

	if (!SyncItem(uidPlayer, vRecp))
	{
		EndCacheSync(false);
		return false;
	}

	return true;
}