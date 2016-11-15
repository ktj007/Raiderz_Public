#include "stdafx.h"
#include "TestDBCacheFixture.h"
#include "GDBTaskItemMerge.h"
#include "GDB_CODE.h"


namespace test_dbcache_fixture
{
	SUITE(TestDBCache_ItemMerge)
	{
		GDBT_ITEM_MERGE_AND_SPLIT CreateMergeData(GEntityPlayer* pPlayer, GItem* pItem , GItem* pToItem, const int16 nMergeAmount)
		{
			GDBT_ITEM_MERGE_AND_SPLIT data(pPlayer->GetUID()
				, pPlayer->GetPlayerInfo()->nPlayTimeSec
				, pPlayer->GetPlayerInfo()->GetDeltaPlayTime()
				, pPlayer->GetLevel()
				, pPlayer->GetPlayerInfo()->GetMoney()				
				, nMergeAmount
				, GDBT_ITEM_MERGE_AND_SPLIT::MERGE);

			data.m_FromSlot = GDBT_ITEM_MERGE_AND_SPLIT_SLOT(pPlayer->GetAID(), pPlayer->GetCID(), pItem->m_nSlotType, pItem->m_nSlotID, pItem->m_nIUID, pItem->GetAmount()-nMergeAmount, pItem->GetID());
			data.m_ToSlot = GDBT_ITEM_MERGE_AND_SPLIT_SLOT(pPlayer->GetAID(), pPlayer->GetCID(), pToItem->m_nSlotType, pToItem->m_nSlotID, pToItem->m_nIUID, pToItem->GetAmount()+nMergeAmount, pToItem->GetID());
			data.MakeDBCode();

			return data;
		}

		TEST_FIXTURE(FIXTURE, TestDBCache_ItemMergeSuccess_MadeOne)
		{
			MockDBManager			db;
			GEntityPlayer*			pPlayer		= CreateTestPlayer();
			GItem*					pItem		= GiveTestItem(pPlayer);
			GItem*					pToItem		= GiveTestItem(pPlayer);
			GItemDBCache*			ic			= gsys.pDBCacheSystem->FindItemCache(pPlayer->GetUID(), pItem->m_nSlotType, pItem->m_nSlotID, pItem->m_nIUID);
			GItemDBCache*			to_ic		= gsys.pDBCacheSystem->FindItemCache(pPlayer->GetUID(), pToItem->m_nSlotType, pToItem->m_nSlotID, pToItem->m_nIUID);
			const IUID				nIUID		= pItem->m_nIUID;
			const int16				nMergeAmt	= pItem->GetAmount();
			GDBT_ITEM_MERGE_AND_SPLIT			data		= CreateMergeData(pPlayer, pItem, pToItem, nMergeAmt);

			SetTestItemCc(ic);
			SetTestItemCc(to_ic);

			GDBTaskItemMerge* pTask = new GDBTaskItemMerge(pPlayer->GetUID());
			CHECK(pTask->Input(data));

			CHECK_ITEM_BEGIN_CACHE(ic, true);
			CHECK_ITEM_BEGIN_CACHE(to_ic, true);

			CHECK(db.Post(pTask));

			CHECK_ITEM_END_CACHE(to_ic);			
			CHECK(NULL == gmgr.pPlayerDBCacheMgr->Find(pPlayer->GetUID())->GetItemCcMgr().Find(nIUID));
		}


		TEST_FIXTURE(FIXTURE, TestDBCache_ItemMergeSuccess_Split)
		{
			MockDBManager			db;
			GEntityPlayer*			pPlayer		= CreateTestPlayer();
			GItem*					pItem		= GiveTestItem(pPlayer);
			GItem*					pToItem		= GiveTestItem(pPlayer);
			GItemDBCache*			ic			= gsys.pDBCacheSystem->FindItemCache(pPlayer->GetUID(), pItem->m_nSlotType, pItem->m_nSlotID, pItem->m_nIUID);
			GItemDBCache*			to_ic		= gsys.pDBCacheSystem->FindItemCache(pPlayer->GetUID(), pToItem->m_nSlotType, pToItem->m_nSlotID, pToItem->m_nIUID);
			const IUID				nIUID		= pItem->m_nIUID;
			const int16				nMergeAmt	= pItem->GetAmount() - 1;
			GDBT_ITEM_MERGE_AND_SPLIT			data		= CreateMergeData(pPlayer, pItem, pToItem, nMergeAmt);

			SetTestItemCc(ic);
			SetTestItemCc(to_ic);

			GDBTaskItemMerge* pTask = new GDBTaskItemMerge(pPlayer->GetUID());
			CHECK(pTask->Input(data));

			CHECK_ITEM_BEGIN_CACHE(ic, true);
			CHECK_ITEM_BEGIN_CACHE(to_ic, true);

			CHECK(db.Post(pTask));

			CHECK_ITEM_END_CACHE(ic);			
			CHECK_ITEM_END_CACHE(to_ic);
		}

		TEST_FIXTURE(FIXTURE, TestDBCache_ItemMergeFail_FirstItem)
		{
			GEntityPlayer*			pPlayer		= CreateTestPlayer();
			GItem*					pItem		= GiveTestItem(pPlayer);
			GItem*					pToItem		= GiveTestItem(pPlayer);
			GItemDBCache*			ic			= gsys.pDBCacheSystem->FindItemCache(pPlayer->GetUID(), pItem->m_nSlotType, pItem->m_nSlotID, pItem->m_nIUID);
			GItemDBCache*			to_ic		= gsys.pDBCacheSystem->FindItemCache(pPlayer->GetUID(), pToItem->m_nSlotType, pToItem->m_nSlotID, pToItem->m_nIUID);
			const IUID				nIUID		= pItem->m_nIUID;
			const int16				nMergeAmt	= pItem->GetAmount();
			GDBT_ITEM_MERGE_AND_SPLIT			data		= CreateMergeData(pPlayer, pItem, pToItem, nMergeAmt);

			SetTestItemCc(ic);
			SetTestItemCc(to_ic);

			ic->BeginCacheSync();

			GDBTaskItemMerge pTask(pPlayer->GetUID());
			CHECK(!pTask.Input(data));

			CHECK_ITEM_BEGIN_CACHE(ic, true);
			CHECK_ITEM_BEGIN_CACHE(to_ic, false);			
		}

		TEST_FIXTURE(FIXTURE, TestDBCache_ItemMergeFail_SecondItem)
		{
			GEntityPlayer*			pPlayer		= CreateTestPlayer();
			GItem*					pItem		= GiveTestItem(pPlayer);
			GItem*					pToItem		= GiveTestItem(pPlayer);
			GItemDBCache*			ic			= gsys.pDBCacheSystem->FindItemCache(pPlayer->GetUID(), pItem->m_nSlotType, pItem->m_nSlotID, pItem->m_nIUID);
			GItemDBCache*			to_ic		= gsys.pDBCacheSystem->FindItemCache(pPlayer->GetUID(), pToItem->m_nSlotType, pToItem->m_nSlotID, pToItem->m_nIUID);
			const IUID				nIUID		= pItem->m_nIUID;
			const int16				nMergeAmt	= pItem->GetAmount();
			GDBT_ITEM_MERGE_AND_SPLIT			data		= CreateMergeData(pPlayer, pItem, pToItem, nMergeAmt);

			SetTestItemCc(ic);
			SetTestItemCc(to_ic);

			to_ic->BeginCacheSync();

			GDBTaskItemMerge pTask(pPlayer->GetUID());
			CHECK(!pTask.Input(data));

			CHECK_ITEM_BEGIN_CACHE(ic, false);
			CHECK_ITEM_BEGIN_CACHE(to_ic, true);			
		}
	}
}
