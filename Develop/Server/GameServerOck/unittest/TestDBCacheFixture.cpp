#include "stdafx.h"
#include "TestDBCacheFixture.h"


namespace test_dbcache_fixture
{
	SUITE(TestDBCache_Fixture)
	{
		TEST_FIXTURE(FIXTURE, test_SetTestCharCc)
		{
			GCharacterDBCache ch(1, 1);

			SetTestCharCc(&ch);

			CHECK_EQUAL(1, ch.GetXP().GetIncrease());
			CHECK_EQUAL(1, ch.GetXP().GetDecrease());
			CHECK_EQUAL(1, ch.GetMoney().GetIncrease());
			CHECK_EQUAL(1, ch.GetMoney().GetDecrease());
		}

		TEST_FIXTURE(FIXTURE, test_SetItemCc)
		{
			GItemDBCache_Leaf itm(1, 1);

			SetTestItemCc(&itm);

			CHECK_EQUAL(1, itm.GetStackAmt().GetIncrease());
			CHECK_EQUAL(1, itm.GetStackAmt().GetDecrease());			
		}

		TEST_FIXTURE(FIXTURE, test_CHECK_CHAR_CACHE_Syncing)
		{
			GCharacterDBCache ch(1, 1);

			ch.IncXP(1, false);
			ch.DecXP(1, false);
			ch.IncMoney(1, false);
			ch.DecMoney(1, false);

			ch.BeginCacheSync();

			CHECK_CHAR_CACHE(&ch, 1, 2, true);
		}

		TEST_FIXTURE(FIXTURE, test_CHECK_CHAR_CACHE_NotSyncing)
		{
			GCharacterDBCache ch(1, 1);

			ch.IncXP(1, false);
			ch.DecXP(1, false);
			ch.IncMoney(1, false);
			ch.DecMoney(1, false);

			CHECK_CHAR_CACHE(&ch, 1, 2, false);
		}

		TEST_FIXTURE(FIXTURE, test_CHECK_CHAR_BEGIN_CACHE_NotSyncing)
		{
			GCharacterDBCache ch(1, 1);

			ch.IncXP(1, false);
			ch.DecXP(1, false);
			ch.IncMoney(1, false);
			ch.DecMoney(1, false);

			CHECK_CHAR_BEGIN_CACHE(&ch, false);
		}

		TEST_FIXTURE(FIXTURE, test_CHECK_CHAR_BEGIN_CACHE_Syncing)
		{
			GCharacterDBCache ch(1, 1);

			ch.IncXP(1, false);
			ch.DecXP(1, false);
			ch.IncMoney(1, false);
			ch.DecMoney(1, false);

			ch.BeginCacheSync();

			CHECK_CHAR_BEGIN_CACHE(&ch, true);
		}

		TEST_FIXTURE(FIXTURE, test_CHECK_CHAR_END_CACHE)
		{
			GCharacterDBCache ch(1, 1);

			ch.SetZero();
			
			CHECK_CHAR_END_CACHE(&ch);
		}

		TEST_FIXTURE(FIXTURE, test_CHECK_ITEM_CACHE)
		{
			GItemDBCache_Leaf itm(1, 1);

			itm.IncStackAmt(1, false);
			itm.DecStackAmt(1, false);
			
			CHECK_ITEM_CACHE(&itm, 1, 2, false);

			itm.BeginCacheSync();

			CHECK_ITEM_CACHE(&itm, 1, 2, true);
		}

		TEST_FIXTURE(FIXTURE, test_CHECK_ITEM_BEGIN_CACHE_NotSyncing)
		{
			GItemDBCache_Leaf itm(1, 1);

			itm.IncStackAmt(1, false);
			itm.DecStackAmt(1, false);
			
			CHECK_ITEM_BEGIN_CACHE(&itm, false);
		}

		TEST_FIXTURE(FIXTURE, test_CHECK_ITEM_BEGIN_CACHE_Syncing)
		{
			GItemDBCache_Leaf itm(1, 0, 1);

			itm.IncStackAmt(1, false);
			itm.DecStackAmt(1, false);
			
			itm.BeginCacheSync();

			CHECK_ITEM_BEGIN_CACHE(&itm, true);
		}

		TEST_FIXTURE(FIXTURE, test_CHECK_ITEM_END_CACHE)
		{
			GItemDBCache_Leaf itm(1, 1);

			itm.SetZero();

			CHECK_ITEM_END_CACHE(&itm);
		}
	}
}