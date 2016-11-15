#include "stdafx.h"
#include "FBaseGame.h"
#include "GTestForward.h"
#include "GQuestInfo.h"
#include "GQuestSystem.h"
#include "GQuestCanceler.h"
#include "GPlayerQuests.h"
#include "GItemHolder.h"

SUITE(QuestCanceler)
{
	struct Fixture : FBaseGame
	{
		Fixture()
		{
			m_pQuestInfo = test.quest.NewQuestInfo();

			test.quest.GiveNewPlayerQuest(m_pPlayer, m_pQuestInfo->nID);
		}

		GTestMgrWrapper<GQuestInfoMgr>	m_QuestInfoMgr;		
		GQuestInfo*		m_pQuestInfo;
		GQuestCanceler	m_QuestCanceler;
	};

	TEST_FIXTURE(Fixture, InvalidQuestID)
	{
		CHECK_EQUAL(false, m_QuestCanceler.Cancel(m_pPlayer, 0));
	}

	TEST_FIXTURE(Fixture, NotHaveQuest)
	{
		m_pPlayer->GetQuests().Cancel(m_pQuestInfo->nID);
		CHECK_EQUAL(false, m_QuestCanceler.Cancel(m_pPlayer, m_pQuestInfo->nID));
	}

	TEST_FIXTURE(Fixture, NotCancelableQuest)
	{
		m_pQuestInfo->bCancelable = false;
		CHECK_EQUAL(false, m_QuestCanceler.Cancel(m_pPlayer, m_pQuestInfo->nID));
	}

	TEST_FIXTURE(Fixture, CancelQuestState)
	{
		CHECK_EQUAL(true, m_QuestCanceler.Cancel(m_pPlayer, m_pQuestInfo->nID));
		CHECK(false == m_pPlayer->GetQuests().IsDoingQuest(m_pQuestInfo->nID));
	}

	TEST_FIXTURE(Fixture, DestroyItem)
	{
		GItem* pDestroyItem = test.item.GiveNewItem(m_pPlayer);
		m_pQuestInfo->vecDestroyItemID.push_back(pDestroyItem->m_pItemData->m_nID);

		CHECK_EQUAL(true, m_QuestCanceler.Cancel(m_pPlayer, m_pQuestInfo->nID));
		
		GItemHolder* pItemHolder = m_pPlayer->GetItemHolder();
		CHECK_EQUAL(0, pItemHolder->GetItemCount(SLOTTYPE_INVENTORY, pDestroyItem->m_pItemData->m_nID));
	}
}