#ifndef _GNPC_SHOP_LOG_CACHE_H
#define _GNPC_SHOP_LOG_CACHE_H


#include "GLogCache.h"
#include "GLogCacheData.h"


class GNpcShopLogCache : public GLogCache<GLOG_DATA_NPC_SHOP>
{
public :
	GNpcShopLogCache() : GLogCache<GLOG_DATA_NPC_SHOP>() {}
	virtual ~GNpcShopLogCache() {}

protected :
	virtual void OnPostLog() override;

	virtual void PostString();
};


#endif