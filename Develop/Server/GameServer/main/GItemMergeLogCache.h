#ifndef _GITEM_MERGE_LOG_CACHE_H
#define _GITEM_MERGE_LOG_CACHE_H


#include "GLogCache.h"
#include "GLogCacheData.h"


class GItemMergeAndSplitLogCache : public GLogCache<GLOG_DATA_ITEM_MERGE_AND_SPLIT>
{
public :
	GItemMergeAndSplitLogCache() : GLogCache<GLOG_DATA_ITEM_MERGE_AND_SPLIT>() {}
	virtual ~GItemMergeAndSplitLogCache() {}

protected :
	virtual void OnPostLog() override;

	virtual void PostString();
};


#endif