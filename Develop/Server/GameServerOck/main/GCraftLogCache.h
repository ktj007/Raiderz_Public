#ifndef _GCRAFT_LOG_CACHE_H
#define _GCRAFT_LOG_CACHE_H


#include "GLogCache.h"
#include "GLogCacheData.h"


class GCraftLogCache : public GLogCache<GLOG_DATA_CRAFT>
{
public :
	GCraftLogCache() : GLogCache<GLOG_DATA_CRAFT>() {}
	virtual ~GCraftLogCache() {}

protected :
	virtual void OnPostLog() override;

	virtual void PostString();
};



#endif