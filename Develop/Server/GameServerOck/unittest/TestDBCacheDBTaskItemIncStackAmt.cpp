#include "stdafx.h"
#include "TestDBCacheFixture.h"
#include "GDBTaskItemIncStackAmt.h"


namespace test_dbcache_fixture
{
	SUITE(TestDBCache_ItemIncStackAmt)
	{
		GDBT_ITEM_DATA CreateItemUpdateData(GEntityPlayer* pPlayer, GItem* pItem, const int16 nModStackAmt)
		{
			GDBT_ITEM_DATA data(pPlayer->GetAID(), pPlayer->GetUID(), pPlayer->GetPlayerInfo()->GetDeltaPlayTime(), pPlayer->GetLevel(), pPlayer->GetMoney(), pItem->m_pItemData->m_nTier, nModStackAmt, -1, 0);

			data.m_Item.Set(pPlayer->GetCID()
				, pItem->m_nSlotType
				, pItem->m_nSlotID
				, pItem->m_pItemData->m_nID
				, pItem->GetAmount() + nModStackAmt
				, pPlayer->GetPlayerInfo()->nPlayTimeSec
				, 0
				, 0
				, pItem->m_pItemData->m_nMaxDurability
				, pItem->m_pItemData->m_nMaxDurability);

			return data;

			/*GDBT_ITEM_UPDATE_STACKAMT_DATA data(pPlayer->GetUID()
				, pPlayer->GetCID()
				, pItem->m_nSlotType
				, pItem->m_nSlotID
				, pItem->m_nIUID
				, pItem->GetAmount() + nModStackAmt
				, nModStackAmt
				, 0, 0, 0, 0);

			return data;*/
		}

		TEST_FIXTURE(FIXTURE, TestIncItemStackAmtNotSyncSuccess)
		{
			//MockDBManager			db;
			//GEntityPlayer*			pPlayer			= CreateTestPlayer();
			//GItem*					pItem			= GiveTestItem(pPlayer);
			//const int16				nModStackAmt	= 1;
			//GItemDBCache*			pi				= gsys.pDBCacheSystem->FindItemCache(pPlayer->GetUID(), pItem->m_nSlotType, pItem->m_nSlotID, pItem->m_nIUID);
			//GDBT_ITEM_UPDATE_STACKAMT_DATA	data			= CreateItemUpdateData(pPlayer, pItem, nModStackAmt);

			//GDBTaskItemIncStackAmt* pTask = new GDBTaskItemIncStackAmt(pPlayer->GetUID());
			//CHECK(pTask->Input(data));
			//CHECK(!pi->IsSyncing());

			//CHECK(db.Post(pTask));

			//CHECK_EQUAL(nModStackAmt, pi->GetStackAmt().GetIncrease());
			//CHECK_EQUAL(1, pi->GetStackAmt().GetModifiedCnt());
			//CHECK_EQUAL(data.m_nStackAmt, pItem->GetAmount());
		}

		TEST_FIXTURE(FIXTURE, TestIncItemStackAmtNotSyncFail)
		{
			GEntityPlayer*			pPlayer			= CreateTestPlayer();
			GItem*					pItem			= GiveTestItem(pPlayer);
			const int16				nStackAmt		= pItem->GetAmount();
			const int16				nModStackAmt	= 1;
			GItemDBCache*			pi				= gsys.pDBCacheSystem->FindItemCache(pPlayer->GetUID(), pItem->m_nSlotType, pItem->m_nSlotID, pItem->m_nIUID);
			GDBT_ITEM_DATA	data			= CreateItemUpdateData(pPlayer, pItem, nModStackAmt);

			pi->BeginCacheSync();

			GDBTaskItemIncStackAmt Task(pPlayer->GetUID());
			CHECK(!Task.Input(data));
			CHECK(pi->IsSyncing());

			CHECK_EQUAL(0, pi->GetStackAmt().GetIncrease());
			CHECK_EQUAL(0, pi->GetStackAmt().GetModifiedCnt());
			CHECK_EQUAL(nStackAmt, pItem->GetAmount());
		}

		TEST_FIXTURE(FIXTURE, TestIncItemStackAmtOverflowStackAmtSuccess)
		{
			//MockDBManager			db;
			//GEntityPlayer*			pPlayer			= CreateTestPlayer();
			//GItem*					pItem			= GiveTestItem(pPlayer);
			//const int16				nModStackAmt	= 1;
			//const int16				nStackAmt		= pItem->GetAmount() + nModStackAmt;
			//GItemDBCache*			pi				= gsys.pDBCacheSystem->FindItemCache(pPlayer->GetUID(), pItem->m_nSlotType, pItem->m_nSlotID, pItem->m_nIUID);
			//GDBT_ITEM_INSERT_DATA	data			= CreateItemUpdateData(pPlayer, pItem, nModStackAmt);

			//pi->IncStackAmt(DBCACHE_MAX_MOD_STACKAMT, false);

			//GDBTaskItemIncStackAmt* pTask = new GDBTaskItemIncStackAmt(pPlayer->GetUID());
			//CHECK(pTask->Input(data));
			//CHECK(pi->IsSyncing());

			//CHECK(db.Post(pTask));

			//CHECK_EQUAL(0, pi->GetStackAmt().GetIncrease());
			//CHECK_EQUAL(0, pi->GetStackAmt().GetModifiedCnt());
			//CHECK_EQUAL(data.m_Item.m_nStackAmt, pItem->GetAmount());
		}

		TEST_FIXTURE(FIXTURE, TestIncItemStackAmtOverflowModifiedCntSuccess)
		{
			//MockDBManager			db;
			//GEntityPlayer*			pPlayer			= CreateTestPlayer();
			//GItem*					pItem			= GiveTestItem(pPlayer);
			//const int16				nModStackAmt	= 1;
			//GItemDBCache*			pMockItemCc		= SetItemCache(pPlayer->GetUID(), new GItemDBCache_Leaf(pPlayer->GetUID(), pItem->m_nIUID, 0));
			//GDBT_ITEM_INSERT_DATA	data			= CreateItemUpdateData(pPlayer, pItem, nModStackAmt);

			//GDBTaskItemIncStackAmt* pTask = new GDBTaskItemIncStackAmt(pPlayer->GetUID());
			//CHECK(pTask->Input(data));
			//CHECK(pMockItemCc->IsSyncing());

			//CHECK(db.Post(pTask));

			//CHECK_EQUAL(0, pMockItemCc->GetStackAmt().GetIncrease());
			//CHECK_EQUAL(0, pMockItemCc->GetStackAmt().GetModifiedCnt());
			//CHECK_EQUAL(data.m_Item.m_nStackAmt, pItem->GetAmount());
		}

		TEST_FIXTURE(FIXTURE, TestIncItemStackAmtSyncFail)
		{
			GEntityPlayer*			pPlayer			= CreateTestPlayer();
			GItem*					pItem			= GiveTestItem(pPlayer);
			const int16				nStackAmt		= pItem->GetAmount();
			const int16				nModStackAmt	= 1;
			GItemDBCache*			pMockItemCc		= SetItemCache(pPlayer->GetUID(), new GItemDBCache_Leaf(pPlayer->GetUID(), pItem->m_nIUID, 0));
			GDBT_ITEM_DATA	data			= CreateItemUpdateData(pPlayer, pItem, nModStackAmt);

			pMockItemCc->SetMaxModifyCnt(0);
			pMockItemCc->BeginCacheSync();

			GDBTaskItemIncStackAmt Task(pPlayer->GetUID());
			CHECK(!Task.Input(data));
			CHECK(pMockItemCc->IsSyncing());

			CHECK_EQUAL(0, pMockItemCc->GetStackAmt().GetIncrease());
			CHECK_EQUAL(0, pMockItemCc->GetStackAmt().GetModifiedCnt());
			CHECK_EQUAL(nStackAmt, pItem->GetAmount());
		}
	}
}