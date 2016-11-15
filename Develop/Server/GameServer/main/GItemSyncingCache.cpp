#include "stdafx.h"
#include "GItemSyncingCache.h"
#include "GPlayerDBCache.h"
#include "GItemDBCache.h"


void GItemSyncingCache::EndCacheSync( GPlayerDBCache* pc, const bool bZero )
{
	GItemDBCache* ic = pc->GetItemCcMgr().Find(m_nIUID);
	if (NULL == ic)
		return;

	ic->EndCacheSync(bZero);
}