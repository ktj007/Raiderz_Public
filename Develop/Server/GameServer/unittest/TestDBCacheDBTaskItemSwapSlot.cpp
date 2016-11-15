#include "stdafx.h"
#include "TestDBCacheFixture.h"
#include "GDBTaskItemSwapSlot.h"
#include "GDB_CODE.h"

namespace test_dbcache_fixture
{
	SUITE(TestDBCache_ItemSwapSlot)
	{
		GDBT_ITEM_MOVE CreateSwapData(GEntityPlayer* pPlayer, GItem* pItem, GItem* pToItem)
		{
			GDBT_ITEM_MOVE data(pPlayer->GetUID()
				, pPlayer->GetPlayerInfo()->nPlayTimeSec
				, pPlayer->GetPlayerInfo()->GetDeltaPlayTime()
				, pPlayer->GetLevel()
				, pPlayer->GetMoney()
				, pItem->m_pItemData->m_nID);

			data.m_FromSlot = GDBT_ITEM_MOVE_SLOT(pPlayer->GetAID(), pPlayer->GetCID(), pItem->m_nSlotType, pItem->m_nSlotID, pItem->m_nIUID, pItem->GetAmount());
			data.m_ToSlot = GDBT_ITEM_MOVE_SLOT(pPlayer->GetAID(), pPlayer->GetCID(), pToItem->m_nSlotType, pToItem->m_nSlotID, pToItem->m_nIUID, pToItem->GetAmount());
			data.MakeDBCode();
	
			return data;
		}

		TEST_FIXTURE(FIXTURE, TestItemSwapSlotSuccess)
		{
			MockDBManager			db;
			GEntityPlayer*			pPlayer		= CreateTestPlayer();
			GItem*					pItem		= GiveTestItem(pPlayer, 1);
			GItem*					pToItem		= GiveTestItem(pPlayer, 2);
			GItemDBCache*			ic			= gsys.pDBCacheSystem->FindItemCache(pPlayer->GetUID(), pItem->m_nSlotType, pItem->m_nSlotID, pItem->m_nIUID);
			GItemDBCache*			to_ic		= gsys.pDBCacheSystem->FindItemCache(pPlayer->GetUID(), pToItem->m_nSlotType, pToItem->m_nSlotID, pToItem->m_nIUID);
			GDBT_ITEM_MOVE			data		= CreateSwapData(pPlayer, pItem, pToItem);

			SetTestItemCc(ic);
			SetTestItemCc(to_ic);

			GDBTaskItemSwapSlot* pTask = new GDBTaskItemSwapSlot(pPlayer->GetUID());
			CHECK(pTask->Input(data));

			CHECK_ITEM_BEGIN_CACHE(ic, true);
			CHECK_ITEM_BEGIN_CACHE(to_ic, true);

			CHECK(db.Post(pTask));

			CHECK_ITEM_END_CACHE(ic);
			CHECK_ITEM_END_CACHE(to_ic);

			CHECK_EQUAL(ic->GetSlotType(), pItem->m_nSlotType);
			CHECK_EQUAL(ic->GetSlotID(), pItem->m_nSlotID);

			CHECK_EQUAL(to_ic->GetSlotType(), pToItem->m_nSlotType);
			CHECK_EQUAL(to_ic->GetSlotID(), pToItem->m_nSlotID);
		}

		TEST_FIXTURE(FIXTURE, TestItemSwapSlotFirstItemFail)
		{
			MockDBManager			db;
			GEntityPlayer*			pPlayer		= CreateTestPlayer();
			GItem*					pItem		= GiveTestItem(pPlayer);
			GItem*					pToItem		= GiveTestItem(pPlayer);
			GItemDBCache*			ic			= gsys.pDBCacheSystem->FindItemCache(pPlayer->GetUID(), pItem->m_nSlotType, pItem->m_nSlotID, pItem->m_nIUID);
			GItemDBCache*			to_ic		= gsys.pDBCacheSystem->FindItemCache(pPlayer->GetUID(), pToItem->m_nSlotType, pToItem->m_nSlotID, pToItem->m_nIUID);
			GDBT_ITEM_MOVE			data		= CreateSwapData(pPlayer, pItem, pToItem);

			SetTestItemCc(ic);
			SetTestItemCc(to_ic);

			ic->BeginCacheSync();

			GDBTaskItemSwapSlot* pTask = new GDBTaskItemSwapSlot(pPlayer->GetUID());
			CHECK(!pTask->Input(data));

			CHECK_ITEM_BEGIN_CACHE(ic, true);
			CHECK_ITEM_BEGIN_CACHE(to_ic, false);			

			CHECK_EQUAL(ic->GetSlotType(), pItem->m_nSlotType);
			CHECK_EQUAL(ic->GetSlotID(), pItem->m_nSlotID);

			CHECK_EQUAL(to_ic->GetSlotType(), pToItem->m_nSlotType);
			CHECK_EQUAL(to_ic->GetSlotID(), pToItem->m_nSlotID);
		}

		TEST_FIXTURE(FIXTURE, TestItemSwapSlotSecondItemFail)
		{
			MockDBManager			db;
			GEntityPlayer*			pPlayer		= CreateTestPlayer();
			GItem*					pItem		= GiveTestItem(pPlayer);
			GItem*					pToItem		= GiveTestItem(pPlayer);
			GItemDBCache*			ic			= gsys.pDBCacheSystem->FindItemCache(pPlayer->GetUID(), pItem->m_nSlotType, pItem->m_nSlotID, pItem->m_nIUID);
			GItemDBCache*			to_ic		= gsys.pDBCacheSystem->FindItemCache(pPlayer->GetUID(), pToItem->m_nSlotType, pToItem->m_nSlotID, pToItem->m_nIUID);
			GDBT_ITEM_MOVE			data		= CreateSwapData(pPlayer, pItem, pToItem);

			SetTestItemCc(ic);
			SetTestItemCc(to_ic);

			to_ic->BeginCacheSync();

			GDBTaskItemSwapSlot* pTask = new GDBTaskItemSwapSlot(pPlayer->GetUID());
			CHECK(!pTask->Input(data));

			CHECK_ITEM_BEGIN_CACHE(ic, false);
			CHECK_ITEM_BEGIN_CACHE(to_ic, true);			

			CHECK_EQUAL(ic->GetSlotType(), pItem->m_nSlotType);
			CHECK_EQUAL(ic->GetSlotID(), pItem->m_nSlotID);

			CHECK_EQUAL(to_ic->GetSlotType(), pToItem->m_nSlotType);
			CHECK_EQUAL(to_ic->GetSlotID(), pToItem->m_nSlotID);
		}
	}
}