#ifndef _GITEM_SYNCING_CACHE_H
#define _GITEM_SYNCING_CACHE_H


#include "GObjectSyncingCache.h"
#include "MMemPool.h"


class GItemSyncingCache : public GObjectSyncingCache, public MMemPool<GItemSyncingCache>
{
public :
	GItemSyncingCache(const IUID nIUID) : m_nIUID(nIUID) {}

	void EndCacheSync(GPlayerDBCache* pc, const bool bZero) override;

protected :
	const IUID m_nIUID;
};


#endif