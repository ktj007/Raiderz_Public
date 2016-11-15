#ifndef _GCHARACTER_SYNCING_CACHE_H
#define _GCHARACTER_SYNCING_CACHE_H


#include "GObjectSyncingCache.h"
#include "MMemPool.h"


class GCharacterSyncingCache : public GObjectSyncingCache, public MMemPool<GCharacterSyncingCache>
{
public :
	GCharacterSyncingCache() {}		

	void EndCacheSync(GPlayerDBCache* pc, const bool bZero) override;
};


#endif