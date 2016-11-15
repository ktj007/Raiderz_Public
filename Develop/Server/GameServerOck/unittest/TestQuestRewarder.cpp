#include "stdafx.h"
#include "FBaseGame.h"
#include "GQuestRewarder.h"
#include "GTestForward.h"
#include "GItemAdder.h"
#include "GQObjectiveInfo.h"
#include "GQuestInfo.h"
#include "GPlayerQuests.h"
#include "GQuestSystem.h"
#include "GPlayerQuest.h"
#include "GItemHolder.h"
#include "CSFactionCalculator.h"
#include "GFactionInfo.h"
#include "GPlayerFactions.h"


SUITE(QuestRewarder)
{
	struct Fixture : public FBaseGame
	{
		Fixture()
		{			
			m_pQuestInfo = test.quest.NewQuestInfo();
			m_pQuestInfo->bRepeatable = false;
			m_pQObejctive = test.quest.NewCollectQObjectiveInfo(m_pQuestInfo, 5);

			test.quest.GiveNewPlayerQuest(m_pPlayer, m_pQuestInfo->nID);
			test.item.GiveNewItem(m_pPlayer, m_pQObejctive->vecParam1[0], 5);
		}		

		GTestMgrWrapper<GQuestInfoMgr>			m_QuestInfoMgr;		
		GQuestInfo*			m_pQuestInfo;
		GQObjectiveInfo*	m_pQObejctive;
		GQuestRewarder		m_QuestRewarder;
	};
	

	TEST_FIXTURE(Fixture, NotHaveQuest)
	{
		m_pPlayer->GetQuests().Cancel(m_pQuestInfo->nID);
		CHECK_EQUAL(false, gsys.pQuestSystem->GetRewarder().Reward(m_pPlayer, m_pQuestInfo->nID, -1));
	}

	TEST_FIXTURE(Fixture, FailQuest)
	{
		m_pPlayer->GetQuests().Get(m_pQuestInfo->nID)->Fail();
		CHECK_EQUAL(false, gsys.pQuestSystem->GetRewarder().Reward(m_pPlayer, m_pQuestInfo->nID, -1));
	}

	TEST_FIXTURE(Fixture, NotSelectableReward)
	{				
		GItemData* pItemData = test.item.NewItemData();
		int nAmount = 1;
		test.quest.AddSelectableReward(m_pQuestInfo, QUEST_REWARD(QRT_ITEM, pItemData->m_nID, nAmount));
		int nNotExistSelectableReward = 1;
		
		CHECK_EQUAL(false, gsys.pQuestSystem->GetRewarder().Reward(m_pPlayer, m_pQuestInfo->nID, nNotExistSelectableReward));
	}

	TEST_FIXTURE(Fixture, RewardMoney)
	{
		GItemHolder* pItemHolder = m_pPlayer->GetItemHolder();		
		QUEST_REWARD reward = QUEST_REWARD(QRT_MONEY, 100);
		test.quest.AddReward(m_pQuestInfo, reward);

		int nBeforeMoney = m_pPlayer->GetPlayerInfo()->GetMoney();
		int nBeforeItemCount = pItemHolder->GetItemCount(SLOTTYPE_INVENTORY, m_pQObejctive->vecParam1[0]);

		CHECK_EQUAL(true, gsys.pQuestSystem->GetRewarder().Reward(m_pPlayer, m_pQuestInfo->nID));

		CHECK_EQUAL(nBeforeMoney + reward.nParam1, m_pPlayer->GetPlayerInfo()->GetMoney());
		CHECK_EQUAL(nBeforeItemCount - m_pQObejctive->nParam2, pItemHolder->GetItemCount(SLOTTYPE_INVENTORY, m_pQObejctive->vecParam1[0]));
	}

	TEST_FIXTURE(Fixture, RewardXP)
	{
		GItemHolder* pItemHolder = m_pPlayer->GetItemHolder();
		m_pPlayer->GetPlayerInfo()->nFatigueQuantity = 3000;
		QUEST_REWARD reward = QUEST_REWARD(QRT_XP, 100);
		test.quest.AddReward(m_pQuestInfo, reward);

		int nBeforeXP = m_pPlayer->GetPlayerInfo()->nXP;
		int nBeforeItemCount = pItemHolder->GetItemCount(SLOTTYPE_INVENTORY, m_pQObejctive->vecParam1[0]);
		
		CHECK_EQUAL(true, gsys.pQuestSystem->GetRewarder().Reward(m_pPlayer, m_pQuestInfo->nID));

		CHECK_EQUAL(nBeforeXP + reward.nParam1, m_pPlayer->GetPlayerInfo()->nXP);
		CHECK_EQUAL(nBeforeItemCount - m_pQObejctive->nParam2, pItemHolder->GetItemCount(SLOTTYPE_INVENTORY, m_pQObejctive->vecParam1[0]));
	}

	TEST_FIXTURE(Fixture, RewardFactionGain)
	{
		GFactionInfo* pGainFactionInfo =  test.faction.NewFactionInfo(CSFactionCalculator::GetNormalMaxQuantity());
		QUEST_REWARD reward = QUEST_REWARD(QRT_FACTION, pGainFactionInfo->m_nID, 100);
		test.quest.AddReward(m_pQuestInfo, reward);

		uint16 nBeforeGainQuantity = m_pPlayer->GetPlayerFactions().GetQuantity(pGainFactionInfo->m_nID);

		CHECK_EQUAL(true, gsys.pQuestSystem->GetRewarder().Reward(m_pPlayer, m_pQuestInfo->nID, -1));
		CHECK_EQUAL(nBeforeGainQuantity + reward.nParam2, m_pPlayer->GetPlayerFactions().GetQuantity(pGainFactionInfo->m_nID));
	}

	TEST_FIXTURE(Fixture, RewardFactionLoss)
	{
		GFactionInfo* pLossFactionInfo = test.faction.NewFactionInfo(CSFactionCalculator::GetNormalMaxQuantity());
		QUEST_REWARD reward = QUEST_REWARD(QRT_FACTION, pLossFactionInfo->m_nID, -100);
		test.quest.AddReward(m_pQuestInfo, reward);

		uint16 nBeforeLossQuantity = m_pPlayer->GetPlayerFactions().GetQuantity(pLossFactionInfo->m_nID);

		CHECK_EQUAL(true, gsys.pQuestSystem->GetRewarder().Reward(m_pPlayer, m_pQuestInfo->nID, -1));
		CHECK_EQUAL(nBeforeLossQuantity + reward.nParam2, m_pPlayer->GetPlayerFactions().GetQuantity(pLossFactionInfo->m_nID));
	}
	
	TEST_FIXTURE(Fixture, RewardItem)
	{					
		GItemData* pItemData = test.item.NewItemData();
		int nAmount = 3;
		test.quest.AddReward(m_pQuestInfo, QUEST_REWARD(QRT_ITEM, pItemData->m_nID, nAmount));

		CHECK_EQUAL(true, gsys.pQuestSystem->GetRewarder().Reward(m_pPlayer, m_pQuestInfo->nID, -1));

		GItemHolder* pItemHolder = m_pPlayer->GetItemHolder();
		CHECK_EQUAL(nAmount, pItemHolder->GetItemCount(SLOTTYPE_INVENTORY, pItemData->m_nID));
	}

	TEST_FIXTURE(Fixture, RewardSelectableItem)
	{
		GItemData* pNotSelectedItemData = test.item.NewItemData();
		int nNotSelectedItemAmount = 3;
		test.quest.AddSelectableReward(m_pQuestInfo, QUEST_REWARD(QRT_ITEM, pNotSelectedItemData->m_nID, nNotSelectedItemAmount));

		GItemData* pSelectedItemData = test.item.NewItemData();
		int nSelectedItemAmount = 2;
		test.quest.AddSelectableReward(m_pQuestInfo, QUEST_REWARD(QRT_ITEM, pSelectedItemData->m_nID, nSelectedItemAmount));

		CHECK_EQUAL(true, gsys.pQuestSystem->GetRewarder().Reward(m_pPlayer, m_pQuestInfo->nID, 1));

		GItemHolder* pItemHolder = m_pPlayer->GetItemHolder();
		CHECK_EQUAL(0, pItemHolder->GetItemCount(SLOTTYPE_INVENTORY, pNotSelectedItemData->m_nID));
		CHECK_EQUAL(nSelectedItemAmount, pItemHolder->GetItemCount(SLOTTYPE_INVENTORY, pSelectedItemData->m_nID));
	}

	TEST_FIXTURE(Fixture, RemoveQuestItem)
	{		
		GItemData* pItemData = test.item.NewItemData();
		m_pQuestInfo->vecDestroyItemID.push_back(pItemData->m_nID);
		test.item.GiveNewItem(m_pPlayer, pItemData->m_nID, 10);

		CHECK_EQUAL(true, gsys.pQuestSystem->GetRewarder().Reward(m_pPlayer, m_pQuestInfo->nID, -1));

		GItemHolder* pItemHolder = m_pPlayer->GetItemHolder();
		CHECK_EQUAL(0, pItemHolder->GetItemCount(SLOTTYPE_INVENTORY, pItemData->m_nID));
	}

	TEST_FIXTURE(Fixture, DoneQuest)
	{		
		CHECK_EQUAL(true, gsys.pQuestSystem->GetRewarder().Reward(m_pPlayer, m_pQuestInfo->nID, -1));
		CHECK_EQUAL(true, m_pPlayer->GetQuests().IsDoneQuest(m_pQuestInfo->nID));
		CHECK_EQUAL(false, m_pPlayer->GetQuests().IsDoingQuest(m_pQuestInfo->nID));
	}
}