#ifndef _GITEM_LOOT_LOG_CACHE_H
#define _GITEM_LOOT_LOG_CACHE_H


#include "GLogCache.h"
#include "GLogCacheData.h"


class GItemLogCache : public GLogCache<GLOG_DATA_ITEM>
{
public :
	GItemLogCache();
	virtual ~GItemLogCache() {}	

protected :
	virtual void OnPostLog() override;

	virtual void PostString();
};

#endif