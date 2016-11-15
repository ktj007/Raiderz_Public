#include "stdafx.h"
#include "FBaseGame.h"
#include "GTestForward.h"
#include "GQuestInfo.h"
#include "GQuestSystem.h"
#include "GQuestGiver.h"
#include "GItemAddRemoveInfo.h"
#include "GItemHolder.h"
#include "GPlayerQuest.h"
#include "GPlayerQuests.h"

SUITE(QuestGiver)
{
	struct Fixture : public FBaseGame
	{
		Fixture()
		{
			m_pQuestInfo = test.quest.NewQuestInfo();
		}

		GQuestInfo*		m_pQuestInfo;
	};

	TEST_FIXTURE(Fixture, NotRepeatableQuest)
	{
		m_pQuestInfo->bRepeatable = false;		
		test.quest.GiveNewPlayerDoneQuest(m_pPlayer, m_pQuestInfo->nID);

		CHECK_EQUAL(false, gsys.pQuestSystem->GetGiver().Give(m_pPlayer, m_pQuestInfo->nID));
	}

	TEST_FIXTURE(Fixture, ItemAddRemove)
	{
		GItemData* pItemData = test.item.NewItemData();
		int nAddAmount = 1;
		GItemAddRemoveInfo* pItemAddRemoveInfo = new GItemAddRemoveInfo;
		pItemAddRemoveInfo->InsertItemAddInfo(pItemData->m_nID, nAddAmount);
		m_pQuestInfo->pItemAddRemoveInfo = pItemAddRemoveInfo;		

		int nBeforeAmount = m_pPlayer->GetItemHolder()->GetItemCount(SLOTTYPE_INVENTORY, pItemData->m_nID);
		CHECK_EQUAL(true, gsys.pQuestSystem->GetGiver().Give(m_pPlayer, m_pQuestInfo->nID));

		CHECK_EQUAL(nBeforeAmount+nAddAmount, m_pPlayer->GetItemHolder()->GetItemCount(SLOTTYPE_INVENTORY, pItemData->m_nID));	
	}

	TEST_FIXTURE(Fixture, Give)
	{
		CHECK_EQUAL(true, gsys.pQuestSystem->GetGiver().Give(m_pPlayer, m_pQuestInfo->nID));
		CHECK(true ==  m_pPlayer->GetQuests().IsDoingQuest(m_pQuestInfo->nID));
	}
}