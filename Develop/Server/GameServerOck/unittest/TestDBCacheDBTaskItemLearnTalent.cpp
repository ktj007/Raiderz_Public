#include "stdafx.h"
#include "TestDBCacheFixture.h"
#include "GDBTaskItemLearnTalent.h"
#include "GPlayerTalent.h"



namespace test_dbcache_fixture
{
	SUITE(TestDBCache_ItemLearnTalent)
	{
		TEST_FIXTURE(FIXTURE, TestItemLearnTalentSuccess)
		{
			MockDBManager			db;
			GEntityPlayer*			pPlayer			= CreateTestPlayer();
			GItem*					pItem			= GiveTestItem(pPlayer);
			GItemDBCache*			ic				= gsys.pDBCacheSystem->FindItemCache(pPlayer->GetUID(), pItem->m_nSlotType, pItem->m_nSlotID, pItem->m_nIUID);

			GDBT_ITEM_LEARN_TALENT data(pPlayer->GetAID()
				, pPlayer->GetUID()
				, pPlayer->GetCID()
				, pItem->m_nSlotType
				, pItem->m_nSlotID
				, pItem->m_pItemData->m_nID
				, pItem->m_nIUID
				, pPlayer->GetTalent().GetTP()
				, pPlayer->GetPlayerInfo()->nPlayTimeSec
				, pPlayer->GetPlayerInfo()->GetDeltaPlayTime()
				, pPlayer->GetLevel()
				, pPlayer->GetMoney());

			GDBTaskItemLearnTalent* pTask = new GDBTaskItemLearnTalent(pPlayer->GetUID());
			CHECK(pTask->Input(data));

			CHECK(ic->IsSyncing());

			CHECK(db.Post(pTask));

			CHECK_EQUAL(0, gsys.pDBCacheSystem->GetItemCount(pPlayer->GetUID()));
		}

		TEST_FIXTURE(FIXTURE, TestItemLearnTalentFail)
		{
			MockDBManager			db;
			GEntityPlayer*			pPlayer			= CreateTestPlayer();
			GItem*					pItem			= GiveTestItem(pPlayer);
			GItemDBCache*			ic				= gsys.pDBCacheSystem->FindItemCache(pPlayer->GetUID(), pItem->m_nSlotType, pItem->m_nSlotID, pItem->m_nIUID);

			GDBT_ITEM_LEARN_TALENT data(pPlayer->GetAID()
				, pPlayer->GetUID()
				, pPlayer->GetCID()
				, pItem->m_nSlotType
				, pItem->m_nSlotID
				, pItem->m_pItemData->m_nID
				, pItem->m_nIUID
				, pPlayer->GetTalent().GetTP()
				, pPlayer->GetPlayerInfo()->nPlayTimeSec
				, pPlayer->GetPlayerInfo()->nPlayTimeSec
				, pPlayer->GetPlayerInfo()->GetDeltaPlayTime()
				, pPlayer->GetMoney());

			ic->BeginCacheSync();

			GDBTaskItemLearnTalent* pTask = new GDBTaskItemLearnTalent(pPlayer->GetUID());
			CHECK(!pTask->Input(data));

			CHECK(ic->IsSyncing());		

			SAFE_DELETE(pTask);
		}
	}
}