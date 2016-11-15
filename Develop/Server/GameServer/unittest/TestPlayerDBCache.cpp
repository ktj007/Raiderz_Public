#include "stdafx.h"
#include "GPlayerDBCache.h"




SUITE(TestDBCache_PlayerDBCache)
{
	TEST(PlayerCacheGetUID)
	{
		const MUID	uidPlayer	= 1;
		const int	nCID		= 2;

		GPlayerDBCache pc(uidPlayer, nCID);

		CHECK_EQUAL(uidPlayer, pc.GetUID());
		CHECK_EQUAL(0, pc.GetSyncingCnt());
	}

	class MockPlayerDBCacheSyncCnt : public GPlayerDBCache 
	{
	public :
		MockPlayerDBCacheSyncCnt() : GPlayerDBCache(0, 0) {}

		void SetSyncingCnt(const size_t nCnt)
		{
			for (size_t i = 0; i < nCnt; ++i)
				IncreaseSyncCnt();			
		}
	};	
}
