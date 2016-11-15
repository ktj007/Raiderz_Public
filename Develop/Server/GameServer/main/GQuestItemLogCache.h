#ifndef _GLOG_QUEST_ACCEPT_ITEM_LOG_CACHE_H
#define _GLOG_QUEST_ACCEPT_ITEM_LOG_CACHE_H


#include "GLogCache.h"
#include "GLogCacheData.h"

class GQuestItemLogCache : public GLogCache<GLOG_DATA_QUEST_ITEM>
{
public :
	GQuestItemLogCache() : GLogCache<GLOG_DATA_QUEST_ITEM>() {}
	virtual ~GQuestItemLogCache() {}

protected :
	virtual void OnPostLog() override;

	virtual void PostString();
};


#endif