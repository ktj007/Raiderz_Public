#include "stdafx.h"
#include "GObjectDBCache.h"
#include "GPlayerDBCache.h"
#include "GPlayerDBCacheMgr.h"
#include "GGlobal.h"
#include "GSystem.h"

GObjectDBCache::GObjectDBCache( const MUID& uidPlayer ) : m_uidPlayer(uidPlayer), m_bLock(false)
{
	SetSyncTm(GetLocalTm());	
}

GPlayerDBCache* GObjectDBCache::GetPlayerDBCache()
{
	return gmgr.pPlayerDBCacheMgr->Find(m_uidPlayer);
}

const MUID& GObjectDBCache::GetPlayerUID()
{
	return m_uidPlayer;
}

bool GObjectDBCache::IsOverflow()
{
	if (IsOverflowModifiedRange() || IsOverflowModifiedCnt() || IsOverflowSyncDelayTm(gsys.pSystem->GetNowTime()))
		return true;

	return false;
}

void GObjectDBCache::SetSyncTm( const uint32 nNowTm )
{
	m_nLastSyncTm = nNowTm;
}

uint32 GObjectDBCache::GetLastSyncTm()
{
	return m_nLastSyncTm;
}

bool GObjectDBCache::IsOverflowSyncDelayTm(const uint32 nNowTm)
{
	return DBCACHE_SYNC_ELAPSED < (nNowTm - GetLastSyncTm());
}

uint32 GObjectDBCache::GetLocalTm()
{
	return gsys.pSystem->GetNowTime();
}

bool GObjectDBCache::BeginCacheSync()
{
	if (IsSyncing())
		return false;	

	GPlayerDBCache* pc = GetPlayerDBCache();
	if (NULL != pc)
	{
		pc->IncreaseSyncCnt();
		pc->IncreaseCachingCnt();
	}
	
	Lock();

	return true;
}

void GObjectDBCache::EndCacheSync( const bool bSetZero )
{
	uint32 nSyncTm = GetLastSyncTm();
	if (bSetZero)
		nSyncTm = GetLocalTm();

	GPlayerDBCache* pc = GetPlayerDBCache();
	if (NULL != pc)
	{
		pc->DecreaseSyncCnt(GetLastSyncTm(), nSyncTm);
		if (bSetZero)
		{
			pc->DecreaseCachingCnt();
		}

	}

	if (bSetZero)
	{
		SetZero();
		SetSyncTm(GetLocalTm());
	}	

	Unlock();
}

bool GObjectDBCache::IsSyncing()
{
	return m_bLock;
}

void GObjectDBCache::Lock()
{
	m_bLock = true;
}

void GObjectDBCache::Unlock()
{
	m_bLock = false;
}