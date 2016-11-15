#include "StdAfx.h"
#include "GItemDBCacheSyncer.h"
#include "GItemDBCache.h"
#include "GGlobal.h"
#include "GDBCacheSystem.h"


GItemDBCacheSyncer::GItemDBCacheSyncer(const MUID& uidPlayer)
: m_SyncingList(uidPlayer)
{
}

GItemDBCacheSyncer::~GItemDBCacheSyncer(void)
{
}

bool GItemDBCacheSyncer::BeginCacheSync(const uint8 nSlotType, const int16 nSlotID, const IUID nIUID)
{
	return SyncItem(nSlotType, nSlotID, nIUID);
}

bool GItemDBCacheSyncer::BeginCacheSync(const GDBSYNC_ITEM_VEC& vSyncItem)
{
	for each (const GDBSYNC_ITEM& si in vSyncItem)
	{
		if (false == SyncItem(si.m_nSlotType, si.m_nSlotID, si.m_nIUID))
		{
			return false;
		}
	}

	return true;
}

bool GItemDBCacheSyncer::SyncItem(const uint8 nSlotType, const int16 nSlotID, const IUID nIUID )
{
	GItemDBCache* ic = gsys.pDBCacheSystem->FindItemCache(m_SyncingList.GetPlayer(), (SH_ITEM_SLOT_TYPE)nSlotType, nSlotID, nIUID);
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

void GItemDBCacheSyncer::EndCacheSync( const bool bZero )
{
	m_SyncingList.EndCacheSync(bZero);
}
