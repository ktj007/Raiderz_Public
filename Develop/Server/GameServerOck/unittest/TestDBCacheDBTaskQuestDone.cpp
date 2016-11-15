#include "stdafx.h"
#include "TestDBCacheFixture.h"
#include "GDBTaskQuestDone.h"
#include "GUTHelper_Quest.h"


namespace test_dbcache_fixture
{
	SUITE(TestDBCache_QuestDone)
	{
		GDBT_QEUST_DONE CreateDoneData(GEntityPlayer* pPlayer, GItem* pItem1, GItem* pItem2, const int nModXP, const int nModMoney)
		{
			GDBT_QEUST_DONE data(pPlayer->GetAID()
				, pPlayer->GetUID()
				, pPlayer->GetCID()
				, pPlayer->GetPlayerInfo()->nLevel
				, 0
				, 0
				, 1
				, QUEST_REWARD()
				, pPlayer->GetPlayerInfo()->nXP + nModXP
				, nModXP
				, pPlayer->GetMoney() + nModMoney
				, nModMoney
				, L""
				, L""
				, pPlayer->GetPlayerInfo()->nPlayTimeSec
				, pPlayer->GetPlayerInfo()->GetDeltaPlayTime()
				, MUID::ZERO
				, 0
				, vec3());

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

			data.m_vecRemoveItem.push_back(amt1);
			data.m_vecRemoveItem.push_back(amt2);

			return data;
		}

		TEST_FIXTURE(FIXTURE, TestDBCacheQuestDoneSuccess)
		{
			MockDBManager			db;
			GEntityPlayer*			pPlayer		= CreateTestPlayer();
			GItem*					pItem1		= GiveTestItem(pPlayer);
			GItem*					pItem2		= GiveTestItem(pPlayer);
			const int				nQID		= 1;
			GCharacterDBCache*		pch			= gsys.pDBCacheSystem->FindCharCache(pPlayer->GetUID());
			GItemDBCache*			pi1			= gsys.pDBCacheSystem->FindItemCache(pPlayer->GetUID(), pItem1->m_nSlotType, pItem1->m_nSlotID, pItem1->m_nIUID);
			GItemDBCache*			pi2			= gsys.pDBCacheSystem->FindItemCache(pPlayer->GetUID(), pItem2->m_nSlotType, pItem2->m_nSlotID, pItem2->m_nIUID);
			const int				nModXP		= 1;
			const int				nModMoney	= 1;
			GDBT_QEUST_DONE	data	= CreateDoneData(pPlayer, pItem1, pItem2, nModXP, nModMoney);

			GUTHelper_Quest::GiveNewPlayerQuest(pPlayer, nQID);
			SetTestCharCc(pch);
			SetTestItemCc(pi1);
			SetTestItemCc(pi2);

			GDBTaskQuestDone* pTask = new GDBTaskQuestDone(pPlayer->GetUID());
			CHECK(pTask->Input(data));
			
			CHECK_CHAR_BEGIN_CACHE(pch, true);
			CHECK_ITEM_BEGIN_CACHE(pi1, true);
			CHECK_ITEM_BEGIN_CACHE(pi2, true);

			CHECK(db.Post(pTask));

			CHECK(!pch->IsSyncing());
			CHECK_EQUAL(0, pch->GetMoney().GetModifiedCnt());

			CHECK_EQUAL(0, gsys.pDBCacheSystem->FindPlayerCache(pPlayer->GetUID())->GetItemCcMgr().size());			
		}

		TEST_FIXTURE(FIXTURE, TestDBCacheQuestDoneFailChar)
		{
			MockDBManager			db;
			GEntityPlayer*			pPlayer		= CreateTestPlayer();
			GItem*					pItem1		= GiveTestItem(pPlayer);
			GItem*					pItem2		= GiveTestItem(pPlayer);
			const int				nQID		= 1;
			GCharacterDBCache*		pch			= gsys.pDBCacheSystem->FindCharCache(pPlayer->GetUID());
			GItemDBCache*			pi1			= gsys.pDBCacheSystem->FindItemCache(pPlayer->GetUID(), pItem1->m_nSlotType, pItem1->m_nSlotID, pItem1->m_nIUID);
			GItemDBCache*			pi2			= gsys.pDBCacheSystem->FindItemCache(pPlayer->GetUID(), pItem2->m_nSlotType, pItem2->m_nSlotID, pItem2->m_nIUID);
			const int				nModXP		= 1;
			const int				nModMoney	= 1;
			GDBT_QEUST_DONE	data		= CreateDoneData(pPlayer, pItem1, pItem2, nModXP, nModMoney);

			GUTHelper_Quest::GiveNewPlayerQuest(pPlayer, nQID);
			SetTestCharCc(pch);
			SetTestItemCc(pi1);
			SetTestItemCc(pi2);

			pch->BeginCacheSync();

			GDBTaskQuestDone* pTask = new GDBTaskQuestDone(pPlayer->GetUID());
			CHECK(!pTask->Input(data));
			
			CHECK_CHAR_BEGIN_CACHE(pch, true);
			CHECK_ITEM_BEGIN_CACHE(pi1, false);
			CHECK_ITEM_BEGIN_CACHE(pi2, false);
		}

		TEST_FIXTURE(FIXTURE, TestDBCacheQuestDoneFailFirstItem)
		{
			MockDBManager			db;
			GEntityPlayer*			pPlayer		= CreateTestPlayer();
			GItem*					pItem1		= GiveTestItem(pPlayer);
			GItem*					pItem2		= GiveTestItem(pPlayer);
			const int				nQID		= 1;
			GCharacterDBCache*		pch			= gsys.pDBCacheSystem->FindCharCache(pPlayer->GetUID());
			GItemDBCache*			pi1			= gsys.pDBCacheSystem->FindItemCache(pPlayer->GetUID(), pItem1->m_nSlotType, pItem1->m_nSlotID, pItem1->m_nIUID);
			GItemDBCache*			pi2			= gsys.pDBCacheSystem->FindItemCache(pPlayer->GetUID(), pItem2->m_nSlotType, pItem2->m_nSlotID, pItem2->m_nIUID);
			const int				nModXP		= 1;
			const int				nModMoney	= 1;
			GDBT_QEUST_DONE	data		= CreateDoneData(pPlayer, pItem1, pItem2, nModXP, nModMoney);

			GUTHelper_Quest::GiveNewPlayerQuest(pPlayer, nQID);
			SetTestCharCc(pch);
			SetTestItemCc(pi1);
			SetTestItemCc(pi2);

			pi1->BeginCacheSync();

			GDBTaskQuestDone* pTask = new GDBTaskQuestDone(pPlayer->GetUID());
			CHECK(!pTask->Input(data));

			CHECK_CHAR_BEGIN_CACHE(pch, false);
			CHECK_ITEM_BEGIN_CACHE(pi1, true);
			CHECK_ITEM_BEGIN_CACHE(pi2, false);
		}

		TEST_FIXTURE(FIXTURE, TestDBCacheQuestDoneFailSecondItem)
		{
			MockDBManager			db;
			GEntityPlayer*			pPlayer		= CreateTestPlayer();
			GItem*					pItem1		= GiveTestItem(pPlayer);
			GItem*					pItem2		= GiveTestItem(pPlayer);
			const int				nQID		= 1;
			GCharacterDBCache*		pch			= gsys.pDBCacheSystem->FindCharCache(pPlayer->GetUID());
			GItemDBCache*			pi1			= gsys.pDBCacheSystem->FindItemCache(pPlayer->GetUID(), pItem1->m_nSlotType, pItem1->m_nSlotID, pItem1->m_nIUID);
			GItemDBCache*			pi2			= gsys.pDBCacheSystem->FindItemCache(pPlayer->GetUID(), pItem2->m_nSlotType, pItem2->m_nSlotID, pItem2->m_nIUID);
			const int				nModXP		= 1;
			const int				nModMoney	= 1;
			GDBT_QEUST_DONE	data		= CreateDoneData(pPlayer, pItem1, pItem2, nModXP, nModMoney);

			GUTHelper_Quest::GiveNewPlayerQuest(pPlayer, nQID);
			SetTestCharCc(pch);
			SetTestItemCc(pi1);
			SetTestItemCc(pi2);

			pi2->BeginCacheSync();

			GDBTaskQuestDone* pTask = new GDBTaskQuestDone(pPlayer->GetUID());
			CHECK(!pTask->Input(data));

			CHECK_CHAR_BEGIN_CACHE(pch, false);
			CHECK_ITEM_BEGIN_CACHE(pi1, false);
			CHECK_ITEM_BEGIN_CACHE(pi2, true);
		}
	}
}