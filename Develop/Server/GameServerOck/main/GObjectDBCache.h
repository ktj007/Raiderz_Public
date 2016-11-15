#ifndef _GDBCACHE_OBJECT_H
#define _GDBCACHE_OBJECT_H


const static int	DBCACHE_MAX_CACHE_COUNT = 2;
const static DWORD	DBCACHE_SYNC_ELAPSED	= 10 * 1000;	// 10초.
const static int	DBCACHE_SKEEP			= -1;

/*
 * cache_value_type 의 데이터 타입은 singed로 해주세요!!
 */
class GPlayerDBCache;

class GObjectDBCache
{
public :
	GObjectDBCache(const MUID& uidPlayer);
	virtual ~GObjectDBCache() {}

	virtual bool			BeginCacheSync();
	virtual void			EndCacheSync(const bool bSetZero);
	virtual bool			IsSyncing();

	void					Lock();
	void					Unlock();

	const MUID&				GetPlayerUID();

	virtual bool			IsOverflow();
	
protected :
	virtual GPlayerDBCache*	GetPlayerDBCache();
	
	virtual bool			IsModified()						= 0;
	virtual void			SetZero()							= 0;
	virtual bool			IsOverflowModifiedRange()			= 0;
	virtual bool			IsOverflowModifiedCnt()				= 0;
	virtual bool			IsOverflowSyncDelayTm(const uint32 nNowTm);

	void					SetSyncTm(const uint32 nNowTm);
	uint32					GetLastSyncTm();
	virtual uint32			GetLocalTm();

protected :
	const MUID				m_uidPlayer;	
	uint32					m_nLastSyncTm;
	bool					m_bLock;
};


#endif