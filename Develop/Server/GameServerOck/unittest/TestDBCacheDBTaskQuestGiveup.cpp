#include "stdafx.h"
#include "TestDBCacheFixture.h"
#include "GDBTaskQuestGiveup.h"
#include "GUTHelper_Quest.h"


namespace test_dbcache_fixture
{
	SUITE(TestDBCache_QuestGiveup)
	{
		GDBT_QUEST_GIVEUP CreateGiveupData(GEntityPlayer* pPlayer, const int nQuestID, GItem* pItem1, GItem* pItem2)
		{
			GDBT_QUEST_GIVEUP data(pPlayer->GetAID()
				, pPlayer->GetUID()
				, pPlayer->GetCID()
				, pPlayer->GetPlayerInfo()->nLevel
				, 0
				, nQuestID
				, pPlayer->GetPlayerInfo()->nPlayTimeSec
				, pPlayer->GetPlayerInfo()->GetDeltaPlayTime()
				, L"2010-12-1 1:1:0"
				, pPlayer->GetFieldID());

			GITEM_STACK_AMT amt1;
			amt1.nIUID = pItem1->m_nIUID;
			amt1.nToStackAmt = 0;
			amt1.nModStackAmt = pItem1->GetAmount();
			amt1.nSlotType = pItem1->m_nSlotType;
			amt1.nSlotID = pItem1->m_nSlotID;

			GITEM_STACK_AMT amt2;
			amt2.nIUID = pItem2->m_nIUID;
			amt2.nToStackAmt = 0;
			amt2.nModStackAmt = pItem2->GetAmount();
			amt2.nSlotType = pItem2->m_nSlotType;
			amt2.nSlotID = pItem2->m_nSlotID;

			data.m_vecQItems.push_back(amt1);
			data.m_vecQItems.push_back(amt2);

			return data;
		}

		TEST_FIXTURE(FIXTURE, TestDBQuestGiveupSuccess)
		{
			MockDBManager			db;
			GEntityPlayer*			pPlayer		= CreateTestPlayer();
			GItem*					pItem1		= GiveTestItem(pPlayer);
			GItem*					pItem2		= GiveTestItem(pPlayer);
			const int				nQID		= 1;
			GCharacterDBCache*		pch			= gsys.pDBCacheSystem->FindCharCache(pPlayer->GetUID());
			GItemDBCache*			pi1			= gsys.pDBCacheSystem->FindItemCache(pPlayer->GetUID(), pItem1->m_nSlotType, pItem1->m_nSlotID, pItem1->m_nIUID);
			GItemDBCache*			pi2			= gsys.pDBCacheSystem->FindItemCache(pPlayer->GetUID(), pItem2->m_nSlotType, pItem2->m_nSlotID, pItem2->m_nIUID);
			GDBT_QUEST_GIVEUP		data		= CreateGiveupData(pPlayer, nQID, pItem1, pItem2);

			GUTHelper_Quest::GiveNewPlayerQuest(pPlayer, nQID);

			SetTestItemCc(pi1);
			SetTestItemCc(pi2);

			GDBTaskQuestGiveup* pTask = new GDBTaskQuestGiveup(pPlayer->GetUID());
			CHECK(pTask->Input(data));

			CHECK_ITEM_BEGIN_CACHE(pi1, true);
			CHECK_ITEM_BEGIN_CACHE(pi2, true);

			CHECK(db.Post(pTask));

			CHECK_EQUAL(0, gsys.pDBCacheSystem->FindPlayerCache(pPlayer->GetUID())->GetItemCcMgr().size());	
		}

		TEST_FIXTURE(FIXTURE, TestDBQuestGiveupFail_FirstItem)
		{
			GEntityPlayer*			pPlayer		= CreateTestPlayer();
			GItem*					pItem1		= GiveTestItem(pPlayer);
			GItem*					pItem2		= GiveTestItem(pPlayer);
			const int				nQID		= 1;
			GCharacterDBCache*		pch			= gsys.pDBCacheSystem->FindCharCache(pPlayer->GetUID());
			GItemDBCache*			pi1			= gsys.pDBCacheSystem->FindItemCache(pPlayer->GetUID(), pItem1->m_nSlotType, pItem1->m_nSlotID, pItem1->m_nIUID);
			GItemDBCache*			pi2			= gsys.pDBCacheSystem->FindItemCache(pPlayer->GetUID(), pItem2->m_nSlotType, pItem2->m_nSlotID, pItem2->m_nIUID);
			GDBT_QUEST_GIVEUP		data		= CreateGiveupData(pPlayer, nQID, pItem1, pItem2);

			GUTHelper_Quest::GiveNewPlayerQuest(pPlayer, nQID);

			SetTestItemCc(pi1);
			SetTestItemCc(pi2);

			pi1->BeginCacheSync();

			GDBTaskQuestGiveup Task(pPlayer->GetUID());
			CHECK(!Task.Input(data));

			CHECK_ITEM_BEGIN_CACHE(pi1, true);
			CHECK_ITEM_BEGIN_CACHE(pi2, false);
		}

		TEST_FIXTURE(FIXTURE, TestDBQuestGiveupFail_SecondItem)
		{
			GEntityPlayer*			pPlayer		= CreateTestPlayer();
			GItem*					pItem1		= GiveTestItem(pPlayer);
			GItem*					pItem2		= GiveTestItem(pPlayer);
			const int				nQID		= 1;
			GCharacterDBCache*		pch			= gsys.pDBCacheSystem->FindCharCache(pPlayer->GetUID());
			GItemDBCache*			pi1			= gsys.pDBCacheSystem->FindItemCache(pPlayer->GetUID(), pItem1->m_nSlotType, pItem1->m_nSlotID, pItem1->m_nIUID);
			GItemDBCache*			pi2			= gsys.pDBCacheSystem->FindItemCache(pPlayer->GetUID(), pItem2->m_nSlotType, pItem2->m_nSlotID, pItem2->m_nIUID);
			GDBT_QUEST_GIVEUP		data		= CreateGiveupData(pPlayer, nQID, pItem1, pItem2);

			GUTHelper_Quest::GiveNewPlayerQuest(pPlayer, nQID);

			SetTestItemCc(pi1);
			SetTestItemCc(pi2);

			pi2->BeginCacheSync();

			GDBTaskQuestGiveup Task(pPlayer->GetUID());
			CHECK(!Task.Input(data));

			CHECK_ITEM_BEGIN_CACHE(pi1, false);
			CHECK_ITEM_BEGIN_CACHE(pi2, true);
		}
	}
}