#ifndef _GXP_LOG_CACHE_H
#define _GXP_LOG_CACHE_H


#include "GLogCache.h"
#include "GLogCacheData.h"


class GXPLogCache : public GLogCache<GLOG_DATA_XP>
{
public :
	GXPLogCache();
	virtual ~GXPLogCache() {}

protected :
	virtual void OnPostLog() override;

	virtual void PostString();
};

#endif