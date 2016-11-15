#ifndef _GMONEY_LOOT_LOG_CACHE_H
#define _GMONEY_LOOT_LOG_CACHE_H


#include "GLogCache.h"
#include "GLogCacheData.h"


class GMoneyLogCache : public GLogCache<GLOG_DATA_MONEY>
{
public :
	GMoneyLogCache() : GLogCache<GLOG_DATA_MONEY>() {}
	~GMoneyLogCache() {}

protected :
	virtual void OnPostLog() override;

	virtual void PostString();
};


#endif