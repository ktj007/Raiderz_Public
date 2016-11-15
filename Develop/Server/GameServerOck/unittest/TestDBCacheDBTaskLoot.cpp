#include "stdafx.h"
#include "TestDBCacheFixture.h"
#include "GDBTaskItemLootInsert.h"
#include "GDBTaskItemLootUpdate.h"


namespace test_dbcache_fixture
{
	SUITE(TestDBCache_Loot)
	{
		GDBT_ITEM_DATA CreateLootUpdateData(GEntityPlayer* pPlayer, GItem* pItem, const int16 nModStackAmt)
		{
			GDBT_ITEM_DATA data(pPlayer->GetAID(), pPlayer->GetUID(), pPlayer->GetPlayerInfo()->GetDeltaPlayTime(), pPlayer->GetLevel(), pPlayer->GetMoney(), nModStackAmt, -1, 0, 0);

			data.m_Item.Set(pPlayer->GetCID()
				, SLOTTYPE_INVENTORY
				, pItem->m_nSlotID
				, pItem->m_pItemData->m_nID
				, pItem->GetAmount() + nModStackAmt
				, pPlayer->GetPlayerInfo()->nPlayTimeSec
				, pItem->m_nIUID
				, 0
				, pItem->m_nDurability
				, pItem->m_nDurability);

			return data;
		}

		TEST_FIXTURE(FIXTURE, TestLootUpdateNotSyncSuccess)
		{
			//MockDBManager				db;
			//GEntityPlayer*				pPlayer		= CreateTestPlayer();
			//GItem*						pItem		= GiveTestItem(pPlayer);
			//const int16					nLootAmt	= 1;
			//GItemDBCache*				pi			= gsys.pDBCacheSystem->FindItemCache(pPlayer->GetUID(), pItem->m_nSlotType, pItem->m_nSlotID, pItem->m_nIUID);
			//GDBT_ITEM_LOOT_DATA	data		= CreateLootUpdateData(pPlayer, pItem, nLootAmt);

			//GDBTaskItemLootUpdate* pTask = new GDBTaskItemLootUpdate(pPlayer->GetUID());
			//CHECK(pTask->Input(data));
			//CHECK(!pi->IsSyncing());

			//CHECK(db.Post(pTask));

			//CHECK_EQUAL(nLootAmt, pi->GetStackAmt().GetIncrease());
			//CHECK_EQUAL(1, pi->GetStackAmt().GetModifiedCnt());
			//CHECK_EQUAL(data.m_Item.m_nStackAmt, pItem->GetAmount());
		}

		TEST_FIXTURE(FIXTURE, TestLootUpdateNotSyncFail)
		{
			GEntityPlayer*				pPlayer		= CreateTestPlayer();
			GItem*						pItem		= GiveTestItem(pPlayer);
			const int16					nStackAmt	= pItem->GetAmount();
			const int16					nLootAmt	= 1;
			GItemDBCache*				pi			= gsys.pDBCacheSystem->FindItemCache(pPlayer->GetUID(), pItem->m_nSlotType, pItem->m_nSlotID, pItem->m_nIUID);
			GDBT_ITEM_DATA				data		= CreateLootUpdateData(pPlayer, pItem, nLootAmt);

			pi->BeginCacheSync();

			GDBTaskItemLootUpdate Task(pPlayer->GetUID());
			CHECK(!Task.Input(data));
			CHECK(pi->IsSyncing());

			CHECK_EQUAL(0, pi->GetStackAmt().GetIncrease());
			CHECK_EQUAL(0, pi->GetStackAmt().GetModifiedCnt());
			CHECK_EQUAL(nStackAmt, pItem->GetAmount());
		}

		TEST_FIXTURE(FIXTURE, TestLootUpdateStackAmtOverflowSyncSuccess)
		{
			MockDBManager				db;
			GEntityPlayer*				pPlayer		= CreateTestPlayer();
			GItem*						pItem		= GiveTestItem(pPlayer);
			const int16					nLootAmt	= 1;
			GItemDBCache*				pi			= gsys.pDBCacheSystem->FindItemCache(pPlayer->GetUID(), pItem->m_nSlotType, pItem->m_nSlotID, pItem->m_nIUID);
			GDBT_ITEM_DATA				data		= CreateLootUpdateData(pPlayer, pItem, nLootAmt);

			pi->IncStackAmt(DBCACHE_MAX_MOD_STACKAMT, false);

			GDBTaskItemLootUpdate* pTask = new GDBTaskItemLootUpdate(pPlayer->GetUID());
			CHECK(pTask->Input(data));

			CHECK(pi->IsSyncing());

			CHECK(db.Post(pTask));

			CHECK_EQUAL(0, pi->GetStackAmt().GetIncrease());
			CHECK_EQUAL(0, pi->GetStackAmt().GetModifiedCnt());
			CHECK_EQUAL(data.m_Item.m_nStackAmt, pItem->GetAmount());
		}

		TEST_FIXTURE(FIXTURE, TestLootUpdateModifiedCntOverflowSyncSuccess)
		{
			MockDBManager				db;
			GEntityPlayer*				pPlayer		= CreateTestPlayer();
			GItem*						pItem		= GiveTestItem(pPlayer);
			const int16					nLootAmt	= 1;
			GItemDBCache*				pMockItemCc	= SetItemCache(pPlayer->GetUID(), new GItemDBCache_Leaf(pPlayer->GetUID(), pItem->m_nIUID, 0));
			GDBT_ITEM_DATA				data		= CreateLootUpdateData(pPlayer, pItem, nLootAmt);

			GDBTaskItemLootUpdate* pTask = new GDBTaskItemLootUpdate(pPlayer->GetUID());
			CHECK(pTask->Input(data));

			CHECK(pMockItemCc->IsSyncing());

			CHECK(db.Post(pTask));

			CHECK_EQUAL(0, pMockItemCc->GetStackAmt().GetIncrease());
			CHECK_EQUAL(0, pMockItemCc->GetStackAmt().GetModifiedCnt());
			CHECK_EQUAL(data.m_Item.m_nStackAmt, pItem->GetAmount());
		}

		TEST_FIXTURE(FIXTURE, TestLootUpdateSyncFail)
		{
			GEntityPlayer*				pPlayer		= CreateTestPlayer();
			GItem*						pItem		= GiveTestItem(pPlayer);
			const int16					nStackAmt	= pItem->GetAmount();
			const int16					nLootAmt	= 1;
			GItemDBCache*				pMockItemCc	= SetItemCache(pPlayer->GetUID(), new GItemDBCache_Leaf(pPlayer->GetUID(), pItem->m_nIUID, 0));
			GDBT_ITEM_DATA				data		= CreateLootUpdateData(pPlayer, pItem, nLootAmt);
			
			pMockItemCc->SetMaxModifyCnt(0);
			pMockItemCc->BeginCacheSync();

			GDBTaskItemLootUpdate Task(pPlayer->GetUID());
			CHECK(!Task.Input(data));
			CHECK(pMockItemCc->IsSyncing());

			CHECK_EQUAL(0, pMockItemCc->GetStackAmt().GetIncrease());
			CHECK_EQUAL(0, pMockItemCc->GetStackAmt().GetModifiedCnt());
			CHECK_EQUAL(nStackAmt, pItem->GetAmount());
		}
	}	
}