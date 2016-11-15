#include "stdafx.h"
#include "TestDBCacheFixture.h"
#include "GDBTaskItemLootUpdate.h"
#include "GDBTaskItemData.h"


namespace test_dbcache_fixture
{
	SUITE(TestDBCache_ItemLootUpdate)
	{
		TEST_FIXTURE(FIXTURE, DBTaskItemUpdateStackAmt_Increase_Success)
		{
			const int		nItemID		= 1;
			const int16		nStackAmt	= 1;
			GEntityPlayer*	pPlayer		= CreateTestPlayer();
			GItem*			pItem		= GiveTestItem(pPlayer, nItemID, nStackAmt); 			
		}

		TEST_FIXTURE(FIXTURE, DBTaskItemUpdateStackAmt_Increase_False)
		{

		}

		TEST_FIXTURE(FIXTURE, DBTaskItemUpdateStackAmt_Decrease_Success)
		{

		}

		TEST_FIXTURE(FIXTURE, DBTaskItemUpdateStackAmt_Decrease_False)
		{

		}
	}
}