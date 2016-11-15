#include "stdafx.h"
#include "GItemDBCache.h"
#include "GPlayerDBCache.h"

SUITE(TestDBCache_ItemDura)
{
	const size_t TEST_MAX_MOD_CONT = 10;
	const size_t TEST_MAX_MOD_STACK_AMT = 100;
	const size_t TEST_MAX_MOD_DURA = 100;

	class MockItemDBCache_Dura : public GItemDBCache
	{
	public : 
		MockItemDBCache_Dura(const MUID& uidPlayer = 0, const IUID nIUID = 0) : GItemDBCache(uidPlayer, nIUID, TEST_MAX_MOD_CONT, TEST_MAX_MOD_STACK_AMT, TEST_MAX_MOD_DURA) {}

		cache_Dura GetDura() { return m_cacheDura; }

		using GItemDBCache::IsOverflowModifiedRange;
		using GItemDBCache::IsOverflowModifiedCnt;
		using GItemDBCache::SetSyncTm;
		using GItemDBCache::GetLastSyncTm;
		using GItemDBCache::CalcModifiedCnt;
		using GItemDBCache::StartCaching;

		void SetTestIncDuraCnt(const size_t nCnt)
		{
			for (size_t i = 0; i < nCnt; ++i)
				IncDura(1, false);			
		}

		void SetTestDecDuraCnt(const size_t nCnt)
		{
			for (size_t i = 0; i < nCnt; ++i)
				DecDura(1, false);			
		}
	};

	TEST(TestItemDBCache_IncDura)
	{
		MockItemDBCache_Dura item;
		const uint8 nDura = 10;
		const uint8 nIncDura = 1;

		CHECK(item.IncDura(nIncDura));
		CHECK(item.IsSyncing());

		CHECK_EQUAL(nIncDura, item.GetDura().GetIncrease());
		CHECK_EQUAL(0, item.GetDura().GetDecrease());
		CHECK(!item.IsOverflow());
		CHECK(!item.IsOverflowModifiedRange());
		CHECK(item.IsModified());
	}

	TEST(TestItemDBCache_IncDura_Fail)
	{
		MockItemDBCache_Dura item;
		
		CHECK(item.IncDura(1));
		CHECK(!item.IncDura(1));		
	}

	TEST(TestItemDBCache_DecDura)
	{
		MockItemDBCache_Dura item;
		const uint8 nDura = 10;
		const uint8 nDecDura = 1;

		CHECK(item.DecDura(nDecDura));
		CHECK(item.IsSyncing());

		CHECK_EQUAL(0, item.GetDura().GetIncrease());
		CHECK_EQUAL(nDecDura, item.GetDura().GetDecrease());
		CHECK(!item.IsOverflow());
		CHECK(!item.IsOverflowModifiedRange());
		CHECK(item.IsModified());
	}

	TEST(TestItemDBCache_DecDura_Fail)
	{
		MockItemDBCache_Dura item;
		
		CHECK(item.DecDura(1));
		CHECK(!item.DecDura(1));		
	}

	TEST(TestItemDBCache_IncDura_Overflow_Range)
	{
		MockItemDBCache_Dura item;
		const uint8 nDura = 20;
		const uint8 nIncDura = TEST_MAX_MOD_DURA;

		item.IncDura(nIncDura);

		CHECK_EQUAL(nIncDura, item.GetDura().GetIncrease());
		CHECK_EQUAL(0, item.GetDura().GetDecrease());
		CHECK(item.IsOverflow());
		CHECK(item.IsOverflowModifiedRange());
		CHECK(item.IsModified());
	}

	TEST(TestItemDBCache_DecDura_Overflow_Range)
	{
		MockItemDBCache_Dura item;
		const uint8 nDura = 10;
		const uint8 nDecDura = TEST_MAX_MOD_DURA;

		item.DecDura(nDecDura);

		CHECK_EQUAL(0, item.GetDura().GetIncrease());
		CHECK_EQUAL(nDecDura, item.GetDura().GetDecrease());
		CHECK(item.IsOverflow());
		CHECK(item.IsOverflowModifiedRange());
		CHECK(item.IsModified());
	}

	TEST(TestItemDBCache_IncDura_And_DecDura_Overflow_Range)
	{
		MockItemDBCache_Dura item;
		const uint8 nIncDura = TEST_MAX_MOD_DURA - 1;
		const uint8 nDecDura = TEST_MAX_MOD_DURA - nIncDura;
		const uint8 nDura	 = 10;

		item.IncDura(nIncDura, false);
		item.DecDura(nDecDura, false);

		CHECK_EQUAL(nIncDura, item.GetDura().GetIncrease());
		CHECK_EQUAL(nDecDura, item.GetDura().GetDecrease());
		CHECK(item.IsOverflow());
		CHECK(item.IsOverflowModifiedRange());
		CHECK(item.IsModified());
	}

	TEST(TestItemDBCache_IncDura_Overflow_Cnt)
	{
		MockItemDBCache_Dura item;
		item.SetMaxModifyCnt(TEST_MAX_MOD_CONT);
		item.SetTestIncDuraCnt(TEST_MAX_MOD_CONT);

		item.IncDura(1, false);

		CHECK(item.IsOverflow());
		CHECK(item.IsOverflowModifiedCnt());
		CHECK(item.IsModified());
	}

	TEST(TestItemDBCache_DecDura_Overflow_Cnt)
	{
		MockItemDBCache_Dura item;
		const uint8 nDura = 10;
		item.SetMaxModifyCnt(TEST_MAX_MOD_CONT);
		item.SetTestIncDuraCnt(TEST_MAX_MOD_CONT);

		item.DecDura(1, false);

		CHECK(item.IsOverflow());
		CHECK(item.IsOverflowModifiedCnt());
		CHECK(item.IsModified());		
	}

	TEST(TestItemDBCache_IncDura_StartCaching_Success)
	{
		MockItemDBCache_Dura item;
		item.SetSyncTm(0);

		item.IncDura(1);

		CHECK(0 < item.GetLastSyncTm());

		const uint32 nSyncTm = item.GetLastSyncTm();
		Sleep(1);
		item.IncDura(1);
		CHECK_EQUAL(nSyncTm, item.GetLastSyncTm());
	}

	TEST(TestItemDBCache_DecDura_StartCaching_Success)
	{
		MockItemDBCache_Dura item;
		item.SetSyncTm(0);

		item.DecDura(1);

		CHECK(0 < item.GetLastSyncTm());

		const uint32 nSyncTm = item.GetLastSyncTm();
		Sleep(1);
		item.DecDura(1);
		CHECK_EQUAL(nSyncTm, item.GetLastSyncTm());
	}

	TEST(TestItemDBCache_IncDura_StartCaching_Fail)
	{
		MockItemDBCache_Dura item;
		item.IncDura(1);
		item.SetSyncTm(0);		

		item.IncDura(1);

		CHECK_EQUAL(0, item.GetLastSyncTm());
	}

	TEST(TestItemDBCache_DecDura_StartCaching_Fail)
	{
		MockItemDBCache_Dura item;
		item.DecDura(1);
		item.SetSyncTm(0);		

		item.DecDura(1);

		CHECK_EQUAL(0, item.GetLastSyncTm());
	}
}