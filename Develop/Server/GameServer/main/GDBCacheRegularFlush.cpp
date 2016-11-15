#include "StdAfx.h"
#include "GDBCacheRegularFlush.h"
#include "GPlayerDBCacheMgr.h"
#include "GGlobal.h"
#include "GSystem.h"
#include "GPlayerDBCache.h"
#include "GDBCacheFlush.h"

GDBCacheRegularFlush::GDBCacheRegularFlush(void)
{
	m_Regulator.SetTickTime(_FLUSHCHECK_TIME);
}

GDBCacheRegularFlush::~GDBCacheRegularFlush(void)
{
}

void GDBCacheRegularFlush::Update( float fDelta )
{
	if (false == m_Regulator.IsReady(fDelta)) return;
	
	vector<GPlayerDBCache*> vecFlushNeedCache;
	CollectFlushNeedCache(vecFlushNeedCache);
	Flush(vecFlushNeedCache);
}

void GDBCacheRegularFlush::CollectFlushNeedCache(vector<GPlayerDBCache*>& vec)
{
	vector<GPlayerDBCache*> vecFlushNeedCache;
	for (GPLAYER_DBCACHE_HMAP::iterator itor = gmgr.pPlayerDBCacheMgr->begin(); itor != gmgr.pPlayerDBCacheMgr->end(); itor++)
	{
		GPlayerDBCache* pCache = itor->second;
		if (false == pCache->IsNeedFlush(_FLUSH_TIME))
			continue;

		vec.push_back(pCache);
	}
}

void GDBCacheRegularFlush::Flush(const vector<GPlayerDBCache*>& vecCache)
{
	for each (GPlayerDBCache* pCache in vecCache)
	{
		GDBCacheFlush flush(pCache);
		flush.Post();
	}
}