#ifndef _GITEM_MOVE_LOG_CACHE_H
#define _GITEM_MOVE_LOG_CACHE_H

#include "GLogCache.h"
#include "GLogCacheData.h"

class GItemMoveLogCache : public GLogCache<GLOG_DATA_ITEM_MOVE>
{
public :
	GItemMoveLogCache() : GLogCache<GLOG_DATA_ITEM_MOVE>() {}
	virtual ~GItemMoveLogCache() {}

protected :
	virtual void OnPostLog() override;

	virtual void PostString();
};

#endif