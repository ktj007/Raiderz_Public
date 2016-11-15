#include "stdafx.h"
#include "TestDBCacheFixture.h"
#include "GDBTaskItemDecDura.h"


namespace test_dbcache_fixture
{
	SUITE(TestDBCache_DecDura)
	{
		GDBT_ITEM_DEC_DURA_DATA CreateDecDuraData(GEntityPlayer* pPlayer, GItem* pItem, const uint8 nModDura)
		{
			GDBT_ITEM_DEC_DURA_DATA data(pPlayer->GetUID()
				, pPlayer->GetCID()
				, pItem->m_nSlotType
				, pItem->m_nSlotID
				, pItem->m_nIUID
				, pItem->m_nDurability - nModDura
				, nModDura
				, pItem->GetAmount());

			return data;
		}

		TEST_FIXTURE(FIXTURE, TestDecDuraNotSyncSuccess)
		{
			MockDBManager			db;
			GEntityPlayer*			pPlayer		= CreateTestPlayer();
			GItem*					pItem		= GiveTestItem(pPlayer);
			const int16				nModDura	= 1;
			GItemDBCache*			pi			= gsys.pDBCacheSystem->FindItemCache(pPlayer->GetUID(), pItem->m_nSlotType, pItem->m_nSlotID, pItem->m_nIUID);
			GDBT_ITEM_DEC_DURA_DATA	data		= CreateDecDuraData(pPlayer, pItem, nModDura);

			GDBTaskItemDecDura* pTask = new GDBTaskItemDecDura(pPlayer->GetUID());

			CHECK(pTask->Input(data));
			if (DBCACHE_ITEM_MAX_MOD_COUNT > 0 && DBCACHE_MAX_MOD_STACKAMT > 0 && DBCACHE_MAX_MOD_DURA > 0)
				CHECK(!pi->IsSyncing());
			else
				CHECK(pi->IsSyncing());

			db.Post(pTask);

			if (DBCACHE_ITEM_MAX_MOD_COUNT > 0 && DBCACHE_MAX_MOD_STACKAMT > 0 && DBCACHE_MAX_MOD_DURA > 0)
			{
				CHECK_EQUAL(nModDura, pi->GetDura().GetDecrease());
				CHECK_EQUAL(1, pi->GetDura().GetModifiedCnt());
			}			
			else
			{
				CHECK_EQUAL(0, pi->GetDura().GetDecrease());
				CHECK_EQUAL(0, pi->GetDura().GetModifiedCnt());
			}
			CHECK_EQUAL(data.m_nStackAmt, pItem->GetAmount());
		}

		TEST_FIXTURE(FIXTURE, TestDecDuraNotSyncFail)
		{
			GEntityPlayer*			pPlayer		= CreateTestPlayer();
			GItem*					pItem		= GiveTestItem(pPlayer);
			const int16				nDura		= pItem->m_nDurability;
			const int16				nModDura	= 1;
			GItemDBCache*			pi			= gsys.pDBCacheSystem->FindItemCache(pPlayer->GetUID(), pItem->m_nSlotType, pItem->m_nSlotID, pItem->m_nIUID);
			GDBT_ITEM_DEC_DURA_DATA	data		= CreateDecDuraData(pPlayer, pItem, nModDura);

			pi->BeginCacheSync();

			GDBTaskItemDecDura* pTask = new GDBTaskItemDecDura(pPlayer->GetUID());

			CHECK(!pTask->Input(data));
			CHECK(pi->IsSyncing());

			CHECK_EQUAL(0, pi->GetDura().GetDecrease());
			CHECK_EQUAL(0, pi->GetDura().GetModifiedCnt());
			CHECK_EQUAL(nDura, pItem->GetAmount());

			SAFE_DELETE(pTask);
		}

		TEST_FIXTURE(FIXTURE, TestDecItemDuraOverflowDuraSuccess)
		{
			MockDBManager			db;
			GEntityPlayer*			pPlayer		= CreateTestPlayer();
			GItem*					pItem		= GiveTestItem(pPlayer);
			const uint8				nDura		= 10;
			const uint8				nModDura	= 1;
			GItemDBCache*			pi			= gsys.pDBCacheSystem->FindItemCache(pPlayer->GetUID(), pItem->m_nSlotType, pItem->m_nSlotID, pItem->m_nIUID);
			GDBT_ITEM_DEC_DURA_DATA	data		= CreateDecDuraData(pPlayer, pItem, nModDura);

			pi->DecDura(DBCACHE_MAX_MOD_DURA, false);			

			GDBTaskItemDecDura* pTask = new GDBTaskItemDecDura(pPlayer->GetUID());

			CHECK(pTask->Input(data));
			CHECK(pi->IsSyncing());

			db.Post(pTask);

			CHECK_EQUAL(0, pi->GetDura().GetDecrease());
			CHECK_EQUAL(0, pi->GetDura().GetModifiedCnt());
			CHECK_EQUAL(data.m_nStackAmt, pItem->GetAmount());
		}

		TEST_FIXTURE(FIXTURE, TestDecItemDuraOverflowModifiedCntSuccess)
		{
			MockDBManager			db;
			GEntityPlayer*			pPlayer		= CreateTestPlayer();
			GItem*					pItem		= GiveTestItem(pPlayer);
			const int16				nModDura	= 1;
			GItemDBCache*			pi			= gsys.pDBCacheSystem->FindItemCache(pPlayer->GetUID(), pItem->m_nSlotType, pItem->m_nSlotID, pItem->m_nIUID);
			GDBT_ITEM_DEC_DURA_DATA	data		= CreateDecDuraData(pPlayer, pItem, nModDura);

			pi->SetMaxModifyCnt(0);

			GDBTaskItemDecDura* pTask = new GDBTaskItemDecDura(pPlayer->GetUID());

			CHECK(pTask->Input(data));
			CHECK(pi->IsSyncing());

			db.Post(pTask);

			CHECK_EQUAL(0, pi->GetDura().GetDecrease());
			CHECK_EQUAL(0, pi->GetDura().GetModifiedCnt());
			CHECK_EQUAL(data.m_nStackAmt, pItem->GetAmount());
		}

		TEST_FIXTURE(FIXTURE, TestDecItemDuraSyncFail)
		{
			GEntityPlayer*			pPlayer		= CreateTestPlayer();
			GItem*					pItem		= GiveTestItem(pPlayer);
			const int16				nDura		= pItem->m_nDurability;
			const int16				nModDura	= 1;
			GItemDBCache*			pMockItem	= SetItemCache(pPlayer->GetUID(), new GItemDBCache_Leaf(pPlayer->GetUID(), pItem->m_nIUID, 0));
			GDBT_ITEM_DEC_DURA_DATA	data		= CreateDecDuraData(pPlayer, pItem, nModDura);

			pMockItem->SetMaxModifyCnt(0);
			pMockItem->BeginCacheSync();

			GDBTaskItemDecDura* pTask = new GDBTaskItemDecDura(pPlayer->GetUID());

			CHECK(!pTask->Input(data));
			CHECK(pMockItem->IsSyncing());

			CHECK_EQUAL(0, pMockItem->GetDura().GetDecrease());
			CHECK_EQUAL(0, pMockItem->GetDura().GetModifiedCnt());
			CHECK_EQUAL(nDura, pItem->GetAmount());

			SAFE_DELETE(pTask);
		}
	}
}