#include "stdafx.h"
#include "GItemDBCache.h"
#include "GPlayerDBCache.h"


SUITE(TestDBCache_ItemStackAmt)
{
	const size_t TEST_MAX_MOD_CONT = 10;
	const size_t TEST_MAX_MOD_STACK_AMT = 100;
	const size_t TEST_MAX_MOD_DURA = 100;

	class MockItemDBCache_StackAmt : public GItemDBCache
	{
	public : 
		MockItemDBCache_StackAmt(const MUID& uidPlayer = 0, const IUID nIUID = 0) : GItemDBCache(uidPlayer, nIUID, TEST_MAX_MOD_CONT, TEST_MAX_MOD_STACK_AMT, TEST_MAX_MOD_DURA) {}

		cache_StackAmt GetStackAmt() { return m_cacheStackAmt; }

		using GItemDBCache::IsOverflowModifiedRange;
		using GItemDBCache::IsOverflowModifiedCnt;
		using GItemDBCache::SetSyncTm;
		using GItemDBCache::GetLastSyncTm;
		using GItemDBCache::CalcModifiedCnt;
		using GItemDBCache::StartCaching;

		void SetTestIncStackAmtCnt(const size_t nCnt)
		{
			for (size_t i = 0; i < nCnt; ++i)
				IncStackAmt(1, false);			
		}

		void SetTestDecStackAmtCnt(const size_t nCnt)
		{
			for (size_t i = 0; i < nCnt; ++i)
				DecStackAmt(1, false);			
		}
	};

	TEST(TestItemDBCache_IncStackAmt)
	{
		MockItemDBCache_StackAmt item;
		const int16 nAmt = 10;
		const int16 nIncAmt = 1;

		CHECK(item.IncStackAmt(nIncAmt));
		CHECK(item.IsSyncing());

		CHECK_EQUAL(nIncAmt, item.GetStackAmt().GetIncrease());
		CHECK_EQUAL(0, item.GetStackAmt().GetDecrease());
		CHECK(!item.IsOverflow());
		CHECK(!item.IsOverflowModifiedRange());
		CHECK(item.IsModified());
	}

	TEST(TestItemDBCache_IncStackAmt_Fail)
	{
		MockItemDBCache_StackAmt item;
	
		CHECK(item.IncStackAmt(1));
		CHECK(!item.IncStackAmt(1));		
	}

	TEST(TestItemDBCache_DecStackAmt)
	{
		MockItemDBCache_StackAmt item;
		const int16 nStackAmt = 10;
		const int16 nDecAmt = 1;

		CHECK(item.DecStackAmt(nDecAmt));
		CHECK(item.IsSyncing());

		CHECK_EQUAL(0, item.GetStackAmt().GetIncrease());
		CHECK_EQUAL(nDecAmt, item.GetStackAmt().GetDecrease());
		CHECK(!item.IsOverflow());
		CHECK(!item.IsOverflowModifiedRange());
		CHECK(item.IsModified());
	}

	TEST(TestItemDBCache_DecStackAmt_Fail)
	{
		MockItemDBCache_StackAmt item;
		
		CHECK(item.DecStackAmt(1));
		CHECK(!item.DecStackAmt(1));		
	}

	TEST(TestItemDBCache_IncStackAmt_Overflow_Range)
	{
		MockItemDBCache_StackAmt item;
		const int16 nAmt = 10;
		const int16 nIncAmt = TEST_MAX_MOD_STACK_AMT;

		item.IncStackAmt(nIncAmt);

		CHECK_EQUAL(nIncAmt, item.GetStackAmt().GetIncrease());
		CHECK_EQUAL(0, item.GetStackAmt().GetDecrease());
		CHECK(item.IsOverflow());
		CHECK(item.IsOverflowModifiedRange());
		CHECK(item.IsModified());
	}

	TEST(TestItemDBCache_DecStackAmt_Overflow_Range)
	{
		MockItemDBCache_StackAmt item;
		const int16 nAmt = 103;
		const int16 nDecAmt = TEST_MAX_MOD_STACK_AMT;

		item.DecStackAmt(nDecAmt);

		CHECK_EQUAL(0, item.GetStackAmt().GetIncrease());
		CHECK_EQUAL(nDecAmt, item.GetStackAmt().GetDecrease());
		CHECK(item.IsOverflow());
		CHECK(item.IsOverflowModifiedRange());
		CHECK(item.IsModified());
	}

	TEST(TestItemDBCache_IncStackAtm_AND_DecStackAmt_Overflow_Range)
	{
		MockItemDBCache_StackAmt item;
		const int16 nAmt = 10;
		const int16 nIncAmt = TEST_MAX_MOD_STACK_AMT - 1;
		const int16 nDecAnt = TEST_MAX_MOD_STACK_AMT - nIncAmt;

		item.IncStackAmt(nIncAmt, false);
		item.DecStackAmt(nDecAnt, false);

		CHECK_EQUAL(nIncAmt, item.GetStackAmt().GetIncrease());
		CHECK_EQUAL(nDecAnt, item.GetStackAmt().GetDecrease());
		CHECK(item.IsOverflow());
		CHECK(item.IsOverflowModifiedRange());
		CHECK(item.IsModified());
	}

	TEST(TestItemDBCache_IncStackAmt_Overflow_Cnt)
	{
		MockItemDBCache_StackAmt item;
		item.SetMaxModifyCnt(TEST_MAX_MOD_CONT);
		item.SetTestIncStackAmtCnt(TEST_MAX_MOD_CONT);
		
		item.IncStackAmt(1, false);

		CHECK(item.IsOverflow());
		CHECK(item.IsOverflowModifiedCnt());
		CHECK(item.IsModified());		
	}

	TEST(TestItemDBCache_DecStackAmt_Overflow_Cnt)
	{
		MockItemDBCache_StackAmt item;
		item.SetMaxModifyCnt(TEST_MAX_MOD_CONT);
		item.SetTestIncStackAmtCnt(TEST_MAX_MOD_CONT);

		item.DecStackAmt(1, false);

		CHECK(item.IsOverflow());
		CHECK(item.IsOverflowModifiedCnt());
		CHECK(item.IsModified());
	}

	TEST(TestItemDBCache_IncStackAmt_StartCaching_Success)
	{
		MockItemDBCache_StackAmt item;
		item.SetSyncTm(0);

		item.IncStackAmt(1);

		CHECK(0 < item.GetLastSyncTm());

		const uint32 nSyncTm = item.GetLastSyncTm();
		Sleep(1);
		item.IncStackAmt(1);
		CHECK_EQUAL(nSyncTm, item.GetLastSyncTm());
	}

	TEST(TestItemDBCache_DecStackAmt_StartCaching_Success)
	{
		MockItemDBCache_StackAmt item;
		item.SetSyncTm(0);

		item.DecStackAmt(1);

		CHECK(0 < item.GetLastSyncTm());

		const uint32 nSyncTm = item.GetLastSyncTm();
		Sleep(1);
		item.IncStackAmt(1);
		CHECK_EQUAL(nSyncTm, item.GetLastSyncTm());
	}

	TEST(TestItemDBCache_IncStackAmt_StartCaching_Fail)
	{
		MockItemDBCache_StackAmt item;
		item.IncStackAmt(1);
		item.SetSyncTm(0);

		item.IncStackAmt(1);

		CHECK_EQUAL(0, item.GetLastSyncTm());
	}

	TEST(TestItemDBCache_DecStackAmt_StartCaching_Fail)
	{
		MockItemDBCache_StackAmt item;
		item.DecStackAmt(1);
		item.SetSyncTm(0);

		item.DecStackAmt(1);

		CHECK_EQUAL(0, item.GetLastSyncTm());
	}
}
