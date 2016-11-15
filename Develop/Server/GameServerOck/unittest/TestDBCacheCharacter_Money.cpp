#include "stdafx.h"
#include "GCharacterDBCache.h"


SUITE(TestDBCache_CharacterMoney)
{
	class MockDBCacheCharacterMoney : public GCharacterDBCache
	{
	public :
		MockDBCacheCharacterMoney() : GCharacterDBCache(0, 0) {}

		cache_Money GetMoney() { return m_cacheMoney; }

		bool IsOverflowModifiedCnt() override { return false; }

		using GCharacterDBCache::IsOverflowModifiedRange;
		using GCharacterDBCache::IsOverflowModifiedCnt;
		using GCharacterDBCache::SetSyncTm;
		using GCharacterDBCache::GetLastSyncTm;
		using GCharacterDBCache::CalcModifiedCnt;
		using GCharacterDBCache::StartCaching;
	};

	TEST(DBCacheCharacterIncMoney)
	{
		const int nMoney = 20;
		const int nIncMoney = 10;

		MockDBCacheCharacterMoney cc;

		CHECK(cc.IncMoney(nIncMoney));
		CHECK(cc.IsSyncing());

		CHECK_EQUAL(nIncMoney, cc.GetMoney().GetIncrease());
		CHECK_EQUAL(0, cc.GetMoney().GetDecrease());
	}

	TEST(DBCacheCharacterIncMoney_Fail)
	{
		MockDBCacheCharacterMoney cc;

		CHECK(cc.IncMoney(1));
		CHECK(!cc.IncMoney(1));		
	}

	TEST(DBCacheCharacterDecMoney)
	{
		const int nMoney = 20;
		const int nDecMoney = 10;

		MockDBCacheCharacterMoney cc;

		CHECK(cc.DecMoney(nDecMoney));
		CHECK(cc.IsSyncing());

		CHECK_EQUAL(0, cc.GetMoney().GetIncrease());
		CHECK_EQUAL(nDecMoney, cc.GetMoney().GetDecrease());		
	}

	TEST(DBCacheCharacterDecMoneyFail)
	{
		MockDBCacheCharacterMoney cc;

		CHECK(cc.DecMoney(1));
		CHECK(!cc.DecMoney(1));
		CHECK(cc.IsSyncing());		
	}

	//TEST(DBCacheCharacterOverflowIncMoney)
	//{
	//	const int nMoney = 20;
	//	const int nIncMoney = DBCACHE_MAX_MOD_MONEY + 1;

	//	MockDBCacheCharacterMoney cc;

	//	cc.IncMoney(nIncMoney);
	//	CHECK(cc.IsSyncing());		

	//	CHECK(cc.IsOverflow());
	//}

	//TEST(DBCacheCharacterOverflowDecMoney)
	//{
	//	const int nMoney = 20;
	//	const int nDecMoney = DBCACHE_MAX_MOD_MONEY + 1;

	//	MockDBCacheCharacterMoney cc;

	//	cc.DecMoney(nDecMoney);
	//	CHECK(cc.IsSyncing());

	//	CHECK(cc.IsOverflow());		
	//}

	//TEST(DBCacheCharacterOverflowIncAndDecMoney)
	//{
	//	const int nIncMoney = DBCACHE_MAX_MOD_MONEY - 1;
	//	const int nDecMoney = DBCACHE_MAX_MOD_MONEY - nIncMoney + 1;

	//	MockDBCacheCharacterMoney cc;

	//	cc.IncMoney(nIncMoney, false);
	//	cc.DecMoney(nDecMoney, false);

	//	CHECK(cc.IsOverflow());		
	//}

	TEST(TestCharacterDBCache_IncMoney_StartCaching_Success)
	{
		MockDBCacheCharacterMoney ch;
		ch.SetSyncTm(0);

		ch.IncMoney(1);

		CHECK(0 < ch.GetLastSyncTm());

		const uint32 nSyncTm = ch.GetLastSyncTm();
		Sleep(1);
		ch.IncMoney(1);
		CHECK_EQUAL(nSyncTm, ch.GetLastSyncTm());
	}

	TEST(TestCharacterDBCache_DecMoney_StartCaching_Success)
	{
		MockDBCacheCharacterMoney ch;
		ch.SetSyncTm(0);

		ch.DecMoney(1);

		CHECK(0 < ch.GetLastSyncTm());

		const uint32 nSyncTm = ch.GetLastSyncTm();
		Sleep(1);
		ch.DecMoney(1);
		CHECK_EQUAL(nSyncTm, ch.GetLastSyncTm());
	}

	TEST(TestCharacterDBCache_IncMoney_StartCaching_Fail)
	{
		MockDBCacheCharacterMoney ch;
		ch.IncMoney(1);
		ch.SetSyncTm(0);		

		ch.IncMoney(1);

		CHECK_EQUAL(0, ch.GetLastSyncTm());
	}

	TEST(TestCharacterDBCache_DecMoney_StartCaching_Fail)
	{
		MockDBCacheCharacterMoney ch;
		ch.DecMoney(1);
		ch.SetSyncTm(0);		

		ch.DecMoney(1);

		CHECK_EQUAL(0, ch.GetLastSyncTm());
	}

	class MockDBCacheCharacterCnt : public GCharacterDBCache
	{
	public :
		MockDBCacheCharacterCnt() : GCharacterDBCache(0, 0) {}

		void MakeModifiedCntOverflow()
		{
			for (int i = 0; i <= DBCACHE_CHAR_MAX_MOD_COUNT; ++i)
			{
				IncMoney(1, false);
			}
		}

		bool IsOverflowModifiedRange() override { return false; }
	};

	TEST(DBCacheCharacterOverflowModifiedCnt)
	{
		MockDBCacheCharacterCnt cc;

		cc.MakeModifiedCntOverflow();

		CHECK(cc.IsOverflow());
	}
}