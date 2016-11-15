#include "stdafx.h"
#include "GObjectDBCache.h"
#include "GPlayerDBCache.h"


SUITE(TestDBCache_ObjectCache)
{
	class MockObjectDBCache : public GObjectDBCache
	{
	public :
		MockObjectDBCache(const MUID& uidPlayer = 0) : GObjectDBCache(uidPlayer) {}

		virtual bool			BeginCacheSync()					override { return true; }
		virtual void			EndCacheSync(const bool bSetZero)	override {}
		virtual bool			IsSyncing()							override { return true; }

		using					GObjectDBCache::IsOverflowSyncDelayTm;
		using					GObjectDBCache::SetSyncTm;
		using					GObjectDBCache::GetLastSyncTm;

	protected :
		virtual bool			IsModified()						override { return true; }
		virtual void			SetZero()							override {}
		virtual bool			IsOverflowModifiedRange()			override { return true; }
		virtual bool			IsOverflowModifiedCnt()				override { return true; }

	};

	TEST(TestObjectDBCache_OverflowUpdateTm_Success)
	{
		MockObjectDBCache objcache;

		objcache.SetSyncTm(0);
		
		CHECK(objcache.IsOverflowSyncDelayTm(DBCACHE_SYNC_ELAPSED + 1));
	}

	TEST(TestObjectDBCache_OverflowUpdateTm_Fail)
	{
		MockObjectDBCache objcache;

		objcache.SetSyncTm(0);

		CHECK(!objcache.IsOverflowSyncDelayTm(DBCACHE_SYNC_ELAPSED));
	}
}