#include "stdafx.h"
#include "GDBTaskNPCShop.h"
#include "GDBTaskGlobal.h"
#include "GCharacterDBCache.h"


GDBTaskNPCShopTrade::GDBTaskNPCShopTrade( const MUID& uidPlayer, const SDBTASK_ID TaskID )
: GDBAsyncTask(uidPlayer, SDBT_DBTYPE_GAMEDB, TaskID), m_SyncingList(uidPlayer)
{

}

void GDBTaskNPCShopTrade::_OnPreCompleted()
{
	__super::_OnPreCompleted();
	m_SyncingList.EndCacheSync(IsSuccess());
}

bool GDBTaskNPCShopTrade::BeginCharCacheSync(const MUID& uidPlayer)
{
	GCharacterDBCache* cc = gsys.pDBCacheSystem->FindCharCache(uidPlayer);
	if (NULL == cc)
		return true;

	if (!cc->BeginCacheSync())
		return false;

	if (NULL == m_SyncingList.PushChar())
	{
		cc->EndCacheSync(false);
		return false;
	}

	return true;
}

bool GDBTaskNPCShopTrade::BeginItemCacheSync( const MUID& uidPlayer, const uint8 nSlotType, const int16 nSlotID, const IUID nIUID )
{
	GItemDBCache* ic = gsys.pDBCacheSystem->FindItemCache(uidPlayer, (SH_ITEM_SLOT_TYPE)nSlotType, nSlotID, nIUID);
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