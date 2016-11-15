#include "stdafx.h"
#include "GCharacterSyncingCache.h"
#include "GPlayerDBCache.h"
#include "GCharacterDBCache.h"


void GCharacterSyncingCache::EndCacheSync( GPlayerDBCache* pc, const bool bZero )
{
	if (NULL == pc)
		return;

	pc->GetCharCc()->EndCacheSync(bZero);
}