#include "stdafx.h"
#include "GCharacterDBCache.h"
#include "GPlayerDBCache.h"


SUITE(TestDBCache_XP)
{
	class MockCharacterDBCache_XP : public GCharacterDBCache
	{
	public :
		MockCharacterDBCache_XP() : GCharacterDBCache(1, 2) {}

		cache_XP GetXP() { return m_cacheXP; }

		using GCharacterDBCache::IsOverflowModifiedRange;
		using GCharacterDBCache::IsOverflowModifiedCnt;
		using GCharacterDBCache::SetSyncTm;
		using GCharacterDBCache::GetLastSyncTm;
		using GCharacterDBCache::CalcModifiedCnt;
		using GCharacterDBCache::StartCaching;

		void SetTestIncXPCnt(const size_t nCnt)
		{
			for (size_t i = 0; i < nCnt; ++i)
				IncXP(1, false);			
		}

		void SetTestDecXPCnt(const size_t nCnt)
		{
			for (size_t i = 0; i < nCnt; ++i)
				DecXP(1, false);			
		}
	};

	TEST(TestCharacterDBCache_IncXP)
	{
		MockCharacterDBCache_XP ch;
		const int nXP = 10;
		const int nIncXP = 1;

		CHECK(ch.IncXP(nIncXP));
		CHECK(ch.IsSyncing());

		CHECK_EQUAL(nIncXP, ch.GetXP().GetIncrease());
		CHECK_EQUAL(0, ch.GetXP().GetDecrease());
		CHECK(!ch.IsOverflow());
		CHECK(!ch.IsOverflowModifiedRange());		
		CHECK(ch.IsModified());		
	}

	TEST(TestCharacterDBCache_IncXP_Fail)
	{
		MockCharacterDBCache_XP ch;
		
		CHECK(ch.IncXP(1));
		CHECK(!ch.IncXP(1));				
	}

	TEST(TestCharacterDBCache_DecXP)
	{
		MockCharacterDBCache_XP ch;
		const int nXP = 10;
		const int nDecXP = 1;

		CHECK(ch.DecXP(nDecXP));
		CHECK(ch.IsSyncing());

		CHECK_EQUAL(0, ch.GetXP().GetIncrease());
		CHECK_EQUAL(nDecXP, ch.GetXP().GetDecrease());
		CHECK(!ch.IsOverflow());
		CHECK(!ch.IsOverflowModifiedRange());
		CHECK(ch.IsModified());
	}

	TEST(TestCharacterDBCache_DecXP_Fail)
	{
		MockCharacterDBCache_XP ch;
		
		CHECK(ch.DecXP(1));
		CHECK(!ch.DecXP(1));		
	}

	//TEST(TestCharacterDBCache_IncXP_Overflow_Range)
	//{
	//	MockCharacterDBCache_XP ch;
	//	const int nIncXP = DBCACHE_MAX_MOD_XP;
	//	const int nXP = 20;

	//	ch.IncXP(nIncXP);
	//	
	//	CHECK_EQUAL(nIncXP , ch.GetXP().GetIncrease());
	//	CHECK_EQUAL(0, ch.GetXP().GetDecrease());
	//	CHECK(ch.IsOverflow());
	//	CHECK(ch.IsOverflowModifiedRange());
	//	CHECK(ch.IsModified());
	//}

	//TEST(TestCharacterDBCache_DecXP_Overflow_Range)
	//{
	//	MockCharacterDBCache_XP ch;
	//	const int nDecXP = DBCACHE_MAX_MOD_XP;
	//	const int nXP = 1;

	//	ch.DecXP(nDecXP);

	//	CHECK_EQUAL(0, ch.GetXP().GetIncrease());
	//	CHECK_EQUAL(nDecXP , ch.GetXP().GetDecrease());		
	//	CHECK(ch.IsOverflow());
	//	CHECK(ch.IsOverflowModifiedRange());
	//	CHECK(ch.IsModified());
	//}

	//TEST(TestCharacterDBCache_IncXP_And_DecXP_Overflow_Range)
	//{
	//	MockCharacterDBCache_XP ch;
	//	const int nIncXP = DBCACHE_MAX_MOD_XP - 1;
	//	const int nDecXP = DBCACHE_MAX_MOD_XP - nIncXP;

	//	ch.IncXP(nIncXP, false);
	//	ch.DecXP(nDecXP, false);

	//	CHECK_EQUAL(nIncXP, ch.GetXP().GetIncrease());
	//	CHECK_EQUAL(nDecXP, ch.GetXP().GetDecrease());
	//	CHECK(ch.IsOverflow());
	//	CHECK(ch.IsOverflowModifiedRange());
	//	CHECK(ch.IsModified());				
	//}

	TEST(TestCharacterDBCache_IncXP_Overflow_Cnt)
	{
		MockCharacterDBCache_XP ch;
		ch.SetTestIncXPCnt(DBCACHE_CHAR_MAX_MOD_COUNT);
		ch.IncXP(1, false);
		CHECK(ch.IsOverflow());
		CHECK(ch.IsOverflowModifiedCnt());
		CHECK(ch.IsModified());		
	}

	TEST(TestCharacterDBCache_DecXP_Overflow_Cnt)
	{
		MockCharacterDBCache_XP ch;
		ch.SetTestDecXPCnt(DBCACHE_CHAR_MAX_MOD_COUNT);
		ch.DecXP(1, false);
		CHECK(ch.IsOverflow());
		CHECK(ch.IsOverflowModifiedCnt());
		CHECK(ch.IsModified());		
	}

	TEST(TestCharacterDBCache_IncXP_StartCaching_Success)
	{
		MockCharacterDBCache_XP ch;
		ch.SetSyncTm(0);

		ch.IncXP(1);

		CHECK(0 < ch.GetLastSyncTm());

		const uint32 nSyncTm = ch.GetLastSyncTm();
		Sleep(1);
		ch.IncXP(1);
		CHECK_EQUAL(nSyncTm, ch.GetLastSyncTm());
	}

	TEST(TestCharacterDBCache_DecXP_StartCaching_Success)
	{
		MockCharacterDBCache_XP ch;
		ch.SetSyncTm(0);

		ch.DecXP(1);

		CHECK(0 < ch.GetLastSyncTm());

		const uint32 nSyncTm = ch.GetLastSyncTm();
		Sleep(1);
		ch.DecMoney(1);
		CHECK_EQUAL(nSyncTm, ch.GetLastSyncTm());
	}

	TEST(TestCharacterDBCache_IncXP_StartCaching_Fail)
	{
		MockCharacterDBCache_XP ch;
		ch.IncXP(1);
		ch.SetSyncTm(0);		

		ch.IncXP(1);

		CHECK_EQUAL(0, ch.GetLastSyncTm());
	}

	TEST(TestCharacterDBCache_DecXP_StartCaching_Fail)
	{
		MockCharacterDBCache_XP ch;
		ch.DecXP(1);
		ch.SetSyncTm(0);		

		ch.DecXP(1);

		CHECK_EQUAL(0, ch.GetLastSyncTm());
	}
}