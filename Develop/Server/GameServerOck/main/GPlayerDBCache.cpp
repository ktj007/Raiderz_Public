#include "stdafx.h"
#include "GDBTaskGlobal.h"
#include "GPlayerDBCache.h"
#include "GCharacterDBCache.h"
#include "GSystem.h"


GPlayerDBCache::GPlayerDBCache( const MUID& uidPlayer, const int nCID, GCharacterDBCache* pCacheChar ) 
: m_UID(uidPlayer), m_bFlushing(false), m_SyncingCnt(0), m_nCachingCnt(0), m_nLastFlushTm(0)
{
	if (NULL != pCacheChar)
	{
		m_pCharCc = pCacheChar;
	}
	else
	{
		m_pCharCc = new GCharacterDBCache_Leaf(uidPlayer, nCID);
	}
}

GPlayerDBCache::~GPlayerDBCache()
{
	if (NULL != m_pCharCc)
		SAFE_DELETE(m_pCharCc);
}

GCharacterDBCache* GPlayerDBCache::GetCharCc()
{
	return m_pCharCc;
}

void GPlayerDBCache::IncreaseSyncCnt()
{
	++m_SyncingCnt;
}

void GPlayerDBCache::DecreaseSyncCnt(const uint32 nLastSyncTm, const uint32 nSyncTm)
{
	_ASSERT(0 < m_SyncingCnt);
	
	--m_SyncingCnt;
}

void GPlayerDBCache::IncreaseCachingCnt()
{
	++m_nCachingCnt;
}

void GPlayerDBCache::DecreaseCachingCnt()
{
	_ASSERT(0 < m_nCachingCnt);

	--m_nCachingCnt;
}

size_t GPlayerDBCache::GetSyncingCnt()
{
	return m_SyncingCnt;
}

GItemDBCacheMgr& GPlayerDBCache::GetItemCcMgr()
{
	return m_ItemCcMgr;
}

void GPlayerDBCache::BeginFlush()
{
	_ASSERT(!m_bFlushing);
	m_bFlushing = true;	
}

void GPlayerDBCache::EndFlush()
{
	_ASSERT(m_bFlushing);
	m_bFlushing = false;
	m_nLastFlushTm = gsys.pSystem->GetNowTime();
}

bool GPlayerDBCache::IsFlushing()
{
	return m_bFlushing;
}

bool GPlayerDBCache::IsNeedFlush( uint32 nFlushTime )
{
	if (IsFlushing())
		return false;

	if (nFlushTime > gsys.pSystem->GetNowTime() - m_nLastFlushTm)
		return false;

	if (0 >= m_nCachingCnt)
		return false;

	return true;
}

uint32 GPlayerDBCache::GetLastFlushTime()
{
	return m_nLastFlushTm;
}
