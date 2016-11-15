#ifndef _GPLAYER_DB_CACHE_H
#define _GPLAYER_DB_CACHE_H


#include "GItemDBCacheMgr.h"
#include "MMemPool.h"

#include <set>


class GCharacterDBCache;


class GPlayerDBCache : public MMemPool<GPlayerDBCache>
{
public :
	GPlayerDBCache(const MUID& uidPlayer, const int nCID, GCharacterDBCache* pCacheChar = NULL);
	~GPlayerDBCache();

	const MUID& GetUID() { return m_UID; }

	GCharacterDBCache*	GetCharCc();
	GItemDBCacheMgr&	GetItemCcMgr();

	
	void				IncreaseSyncCnt();
	void				DecreaseSyncCnt(const uint32 nLastSyncTm, const uint32 nSyncTm);

	void				IncreaseCachingCnt();
	void				DecreaseCachingCnt();

	size_t				GetSyncingCnt();

	TEST_VIRTUAL void	BeginFlush();
	void				EndFlush();
	bool				IsFlushing();

	bool				IsNeedFlush(uint32 nFlushTime);
	uint32				GetLastFlushTime();
	

protected :
	const MUID			m_UID;
	GCharacterDBCache*	m_pCharCc;
	GItemDBCacheMgr		m_ItemCcMgr;

	bool				m_bFlushing;

	size_t				m_SyncingCnt;		// 현재 DB 반영중인 캐시 개수
	size_t				m_nCachingCnt;		// 현재 DB 반영이 필요한 캐시 개수(DB 반영후에도 가지고 있는 캐시 개수는 포함하지 않음)

	uint32				m_nLastFlushTm;	
};


#endif