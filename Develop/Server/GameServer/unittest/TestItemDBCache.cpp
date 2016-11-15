#include "stdafx.h"
#include "GItemDBCache.h"
#include "GPlayerDBCache.h"


SUITE(TestDBCache)
{
	class MockItemDBCache : public GItemDBCache
	{
	public :
		MockItemDBCache() : GItemDBCache(1, 2), m_pPlayerCc(NULL) {}

		void SetPlayerDBCache(GPlayerDBCache* pc)
		{
			m_pPlayerCc = pc;
		}

		GPlayerDBCache*	GetPlayerDBCache() override
		{
			return m_pPlayerCc;
		}

		using GItemDBCache::SetSyncTm;
		using GItemDBCache::GetLastSyncTm;
		using GItemDBCache::CalcModifiedCnt;
		using GItemDBCache::StartCaching;

	public :
		GPlayerDBCache* m_pPlayerCc;
	};

	TEST(TestItemDBCacheSync)
	{
		MockItemDBCache ic;
		GPlayerDBCache pc(ic.GetPlayerUID(), 1);
		ic.SetPlayerDBCache(&pc);

		ic.BeginCacheSync();

		CHECK(ic.IsSyncing());
		CHECK_EQUAL(1, pc.GetSyncingCnt());
	}

	TEST(TestItemDBCache_StackAmt_EndSync_SetZero_True)
	{
		MockItemDBCache item;
		GPlayerDBCache pc(item.GetPlayerUID(), 1);
		item.SetPlayerDBCache(&pc);

		item.IncStackAmt(1);
		item.DecStackAmt(1);
		item.SetSyncTm(0);

		item.EndCacheSync(true);

		CHECK_EQUAL(0, item.GetDura().GetIncrease());
		CHECK_EQUAL(0, item.GetDura().GetDecrease());
		CHECK_EQUAL(0, item.GetStackAmt().GetIncrease());
		CHECK_EQUAL(0, item.GetStackAmt().GetDecrease());
		CHECK(0 < item.GetLastSyncTm());
	}

	TEST(TestItemDBCache_Dura_EndSync_SetZero_True)
	{
		MockItemDBCache item;
		GPlayerDBCache pc(item.GetPlayerUID(), 1);
		item.SetPlayerDBCache(&pc);

		item.IncDura(1);
		item.DecDura(1);
		item.SetSyncTm(0);

		item.EndCacheSync(true);

		CHECK_EQUAL(0, item.GetDura().GetIncrease());
		CHECK_EQUAL(0, item.GetDura().GetDecrease());
		CHECK_EQUAL(0, item.GetStackAmt().GetIncrease());
		CHECK_EQUAL(0, item.GetStackAmt().GetDecrease());
		CHECK(0 < item.GetLastSyncTm());
	}

	TEST(TestItemDBCache_StackAmt_EndSync_SetZero_False)
	{
		MockItemDBCache item;
		const int16 nStackAmt = 10;
		GPlayerDBCache pc(item.GetPlayerUID(), 1);
		item.SetPlayerDBCache(&pc);

		item.IncStackAmt(1, false);
		item.DecStackAmt(1, false);
		item.SetSyncTm(0);

		pc.IncreaseSyncCnt();
		item.EndCacheSync(false);

		CHECK_EQUAL(0, item.GetDura().GetIncrease());
		CHECK_EQUAL(0, item.GetDura().GetDecrease());
		CHECK_EQUAL(1, item.GetStackAmt().GetIncrease());
		CHECK_EQUAL(1, item.GetStackAmt().GetDecrease());
		CHECK_EQUAL(0, item.GetLastSyncTm());
	}

	TEST(TestItemDBCache_Dura_EndSync_SetZero_False)
	{
		MockItemDBCache item;
		GPlayerDBCache pc(item.GetPlayerUID(), 1);
		item.SetPlayerDBCache(&pc);

		item.IncDura(1, false);
		item.DecDura(1, false);
		item.SetSyncTm(0);

		pc.IncreaseSyncCnt();
		item.EndCacheSync(false);

		CHECK_EQUAL(1, item.GetDura().GetIncrease());
		CHECK_EQUAL(1, item.GetDura().GetDecrease());
		CHECK_EQUAL(0, item.GetStackAmt().GetIncrease());
		CHECK_EQUAL(0, item.GetStackAmt().GetDecrease());
		CHECK_EQUAL(0, item.GetLastSyncTm());		
	}

	TEST(TestItemDBCache_StartCaching_Success)
	{
		MockItemDBCache cc;
		cc.SetSyncTm(0);
		cc.StartCaching();

		CHECK(0 < cc.GetLastSyncTm());
	}

	TEST(TestItemDBCache_StartCaching_Fail)
	{
		MockItemDBCache cc;
		cc.IncStackAmt(1);
		cc.SetSyncTm(0);
		cc.StartCaching();

		CHECK(0 == cc.GetLastSyncTm());
	}

	TEST(TestItemDBCache_CheckVaildModifyStackAmt_StackAmt_Success)
	{
		GItemDBCache_Leaf i(1, 1);

		CHECK(i.CheckValidModifyCnt(1, 0));
	}

	TEST(TestItemDBCache_CheckVaildModifyStackAmt_Dura_Success)
	{
		GItemDBCache_Leaf i(1, 1);

		CHECK(i.CheckValidModifyCnt(0, 1));
	}

	TEST(TestItemDBCache_CheckVaildModifyStackAmt_Fail)
	{
		GItemDBCache_Leaf i(1, 1);

		CHECK(!i.CheckValidModifyCnt(1, 1));
	}
}