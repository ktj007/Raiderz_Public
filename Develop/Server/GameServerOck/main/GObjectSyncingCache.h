#ifndef _GOBJECT_SYNCING_CACHE_H
#define _GOBJECT_SYNCING_CACHE_H


class GPlayerDBCache;


class GObjectSyncingCache
{
public :
	GObjectSyncingCache() {}
	virtual ~GObjectSyncingCache() {}

	virtual void EndCacheSync(GPlayerDBCache* pc, const bool bZero) = 0;		
};


#endif