#ifndef _GPARTY_LOG_CACHE_H
#define _GPARTY_LOG_CACHE_H


#include "GLogCache.h"
#include "GLogCacheData.h"


class GPartyLogCache : public GLogCache<GLOG_DATA_PARTY>
{
public :
	GPartyLogCache() : GLogCache<GLOG_DATA_PARTY> () {}
	~GPartyLogCache() {}

protected :
	virtual void OnPostLog() override;

	virtual void PostString();
};


#endif