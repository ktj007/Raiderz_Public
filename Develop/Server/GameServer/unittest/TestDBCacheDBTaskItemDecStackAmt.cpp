#include "stdafx.h"
#include "TestDBCacheFixture.h"
#include "GDBTaskItemDecStackAmt.h"

namespace test_dbcache_fixture
{
	SUITE(TestDBCache_ItemDecStackAmt)
	{
		GDBT_ITEM_DEC_STACK_AMT_DATA CreateItemUpdateData(GEntityPlayer* pPlayer, GItem* pItem, const int16 nModStackAmt)
		{
			GDBT_ITEM_DEC_STACK_AMT_DATA data(pPlayer->GetUID()
				, pPlayer->GetAID()
				, pPlayer->GetCID()
				, 0
				, 0
				, 0
				, pPlayer->GetMoney()
				, pItem->m_nSlotType
				, pItem->m_nSlotID
				, pItem->m_nIUID
				, pItem->m_pItemData->m_nID
				, pItem->GetAmount() - nModStackAmt
				, nModStackAmt
				, 0
				, 0);

			return data;
		}

		TEST_FIXTURE(FIXTURE, TestDecItemStackAmtNotSyncSuccess)
		{
			//MockDBManager					db;
			//GEntityPlayer*					pPlayer			= CreateTestPlayer();
			//GItem*							pItem			= GiveTestItem(pPlayer);
			//const int16						nModStackAmt	= 1;
			//GItemDBCache*					pi				= gsys.pDBCacheSystem->FindItemCache(pPlayer->GetUID(), pItem->m_nSlotType, pItem->m_nSlotID, pItem->m_nIUID);
			//GDBT_ITEM_UPDATE_STACKAMT_DATA	data			= CreateItemUpdateData(pPlayer, pItem, nModStackAmt);

			//GDBTaskItemDecStackAmt* pTask = new GDBTaskItemDecStackAmt(pPlayer->GetUID());
			//CHECK(pTask->Input(data));
			//CHECK(!pi->IsSyncing());

			//CHECK(db.Post(pTask));

			//CHECK_EQUAL(nModStackAmt, pi->GetStackAmt().GetDecrease());
			//CHECK_EQUAL(1, pi->GetStackAmt().GetModifiedCnt());
			//CHECK_EQUAL(data.m_nStackAmt, pItem->GetAmount());
		}

		TEST_FIXTURE(FIXTURE, TestDecItemStackAmtNotSyncFail)
		{
			GEntityPlayer*					pPlayer			= CreateTestPlayer();
			GItem*							pItem			= GiveTestItem(pPlayer);
			const int16						nStackAmt		= pItem->GetAmount();
			const int16						nModStackAmt	= 1;
			GItemDBCache*					pi				= gsys.pDBCacheSystem->FindItemCache(pPlayer->GetUID(), pItem->m_nSlotType, pItem->m_nSlotID, pItem->m_nIUID);
			GDBT_ITEM_DEC_STACK_AMT_DATA	data			= CreateItemUpdateData(pPlayer, pItem, nModStackAmt);

			pi->BeginCacheSync();

			GDBTaskItemDecStackAmt Task(pPlayer->GetUID());
			CHECK(!Task.Input(data));
			CHECK(pi->IsSyncing());

			CHECK_EQUAL(0, pi->GetStackAmt().GetDecrease());
			CHECK_EQUAL(0, pi->GetStackAmt().GetModifiedCnt());
			CHECK_EQUAL(nStackAmt, pItem->GetAmount());
		}

		TEST_FIXTURE(FIXTURE, TestDecItemStackAmtOverflowStackAmtSuccess)
		{
			MockDBManager					db;
			GEntityPlayer*					pPlayer			= CreateTestPlayer();
			GItem*							pItem			= GiveTestItem(pPlayer);
			const int16						nModStackAmt	= 1;
			const int16						nStackAmt		= pItem->GetAmount() - nModStackAmt;
			GItemDBCache*					pi				= gsys.pDBCacheSystem->FindItemCache(pPlayer->GetUID(), pItem->m_nSlotType, pItem->m_nSlotID, pItem->m_nIUID);
			GDBT_ITEM_DEC_STACK_AMT_DATA	data			= CreateItemUpdateData(pPlayer, pItem, nModStackAmt);

			pi->DecStackAmt(DBCACHE_MAX_MOD_STACKAMT, false);

			GDBTaskItemDecStackAmt* pTask = new GDBTaskItemDecStackAmt(pPlayer->GetUID());
			CHECK(pTask->Input(data));
			CHECK(pi->IsSyncing());

			CHECK(db.Post(pTask));

			CHECK_EQUAL(0, pi->GetStackAmt().GetDecrease());
			CHECK_EQUAL(0, pi->GetStackAmt().GetModifiedCnt());
			CHECK_EQUAL(data.m_nStackAmt, pItem->GetAmount());
		}

		TEST_FIXTURE(FIXTURE, TestDecItemStackAmtOverflowModifiedCntSuccess)
		{
			MockDBManager					db;
			GEntityPlayer*					pPlayer			= CreateTestPlayer();
			GItem*							pItem			= GiveTestItem(pPlayer);
			const int16						nModStackAmt	= 1;
			GItemDBCache*					pMockItemCc		= SetItemCache(pPlayer->GetUID(), new GItemDBCache_Leaf(pPlayer->GetUID(), pItem->m_nIUID, 0));
			GDBT_ITEM_DEC_STACK_AMT_DATA	data			= CreateItemUpdateData(pPlayer, pItem, nModStackAmt);

			GDBTaskItemDecStackAmt* pTask = new GDBTaskItemDecStackAmt(pPlayer->GetUID());
			CHECK(pTask->Input(data));
			CHECK(pMockItemCc->IsSyncing());

			CHECK(db.Post(pTask));

			CHECK_EQUAL(0, pMockItemCc->GetStackAmt().GetDecrease());
			CHECK_EQUAL(0, pMockItemCc->GetStackAmt().GetModifiedCnt());
			CHECK_EQUAL(data.m_nStackAmt, pItem->GetAmount());
		}

		TEST_FIXTURE(FIXTURE, TestDecItemStackAmtSyncFail)
		{
			GEntityPlayer*					pPlayer			= CreateTestPlayer();
			GItem*							pItem			= GiveTestItem(pPlayer);
			const int16						nStackAmt		= pItem->GetAmount();
			const int16						nModStackAmt	= 1;
			GItemDBCache*					pMockItemCc		= SetItemCache(pPlayer->GetUID(), new GItemDBCache_Leaf(pPlayer->GetUID(), pItem->m_nIUID, 0));
			GDBT_ITEM_DEC_STACK_AMT_DATA	data			= CreateItemUpdateData(pPlayer, pItem, nModStackAmt);

			pMockItemCc->SetMaxModifyCnt(0);
			pMockItemCc->BeginCacheSync();

			GDBTaskItemDecStackAmt Task(pPlayer->GetUID());
			CHECK(!Task.Input(data));
			CHECK(pMockItemCc->IsSyncing());

			CHECK_EQUAL(0, pMockItemCc->GetStackAmt().GetDecrease());
			CHECK_EQUAL(0, pMockItemCc->GetStackAmt().GetModifiedCnt());
			CHECK_EQUAL(nStackAmt, pItem->GetAmount());
		}
	}
}