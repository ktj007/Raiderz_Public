#ifndef _GITEM_DELETE_LOG_CACHE_H
#define _GITEM_DELETE_LOG_CACHE_H


#include "GLogCache.h"
#include "GLogCacheData.h"


class GItemDeleteLogCache : public GLogCache<GLOG_DATA_ITEM_DELETE>
{
public :
	GItemDeleteLogCache() : GLogCache<GLOG_DATA_ITEM_DELETE>() {}
	virtual ~GItemDeleteLogCache() {}


protected :
	virtual void OnPostLog() override;

	virtual void PostString();	
};


#endif