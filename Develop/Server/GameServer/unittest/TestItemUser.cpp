#include "stdafx.h"
#include "GEntityPlayer.h"
#include "GTestForward.h"
#include "FBaseGame.h"
#include "GItemUser.h"
#include "GItemHolder.h"
#include "GPlayerTalent.h"
#include "GQuestInfo.h"
#include "GPlayerQuests.h"
#include "GConditionInfo.h"
#include "GConditionsInfo.h"
#include "GEquipmentSlot.h"

SUITE(ItemUser)
{
	struct Fixture : public FBaseGame
	{
		Fixture(){}
		~Fixture(){}

		GItemUser m_ItemUser;
	};

	TEST_FIXTURE(Fixture, Equip)
	{
		GItem* pItem = test.item.GainShieldItem(m_pPlayer);

		CHECK(true == m_ItemUser.Use(m_pPlayer, pItem->m_nSlotID));

		CHECK_EQUAL(true, m_pPlayer->GetItemHolder()->GetEquipment().IsEquipItem(pItem));	
	}

	TEST_FIXTURE(Fixture, TalentUse)
	{
		GItem* pItem = test.item.GiveNewItem(m_pPlayer);

		GTalentInfo* pTalentInfo = test.talent.NewItemTalentInfo();
		pItem->m_pItemData->m_ItemType = ITEMTYPE_USABLE;
		pItem->m_pItemData->m_nUsableType = USABLE_TALENT_USE;
		pItem->m_pItemData->m_vecUsableParam.push_back(pTalentInfo->m_nID);

		int nBeforeItemQuantity = pItem->GetAmount();

		CHECK(true == gsys.pItemSystem->GetUser().Use(m_pPlayer, pItem->m_nSlotID));

		CHECK_EQUAL(true, m_pPlayer->GetTalent().IsEnableUseItemTalent(pTalentInfo->m_nID));
		CHECK_EQUAL(nBeforeItemQuantity, pItem->GetAmount());
		CHECK_EQUAL(MC_ITEM_TALENT, m_pLink->GetCommand(0).GetID());
	}

	TEST_FIXTURE(Fixture, TalentTrain)
	{
		// arrange
		GItem* pItem = test.item.GiveNewItem(m_pPlayer);
		pItem->m_pItemData->m_ItemType = ITEMTYPE_USABLE;
		pItem->m_pItemData->m_nUsableType = USABLE_TALENT_TRAIN;

		vector<GTalentInfo*> vecTalentInfo;

		for(int i = 0; i < 3; i++)
		{
			GTalentInfo* pInfo = test.talent.NewTalentInfo();
			vecTalentInfo.push_back(pInfo);

			pInfo->m_bByLicense = true;
			pInfo->m_bNeedTraining = true;

			m_pPlayer->GetTalent().AddTP(1);
			pItem->m_pItemData->m_vecUsableParam.push_back(vecTalentInfo[i]->m_nID);				
		}

		// act
		CHECK(true == gsys.pItemSystem->GetUser().Use(m_pPlayer, pItem->m_nSlotID));


		// assert
		for(size_t i = 0; i < vecTalentInfo.size(); i++)
		{
			CHECK_EQUAL(true, m_pPlayer->GetTalent().IsTrainedTalent(vecTalentInfo[i]));
		}
	}

	TEST_FIXTURE(Fixture, QuestAdd)
	{
		MockLink* pLink = NewLink(m_pPlayer);
		GQuestInfo* pQuestInfo = test.quest.NewQuestInfo();

		GItem* pItem = test.item.GiveNewItem(m_pPlayer);
		pItem->m_pItemData->m_ItemType = ITEMTYPE_USABLE;
		pItem->m_pItemData->m_nUsableType = USABLE_QUEST_ADD;
		pItem->m_pItemData->m_vecUsableParam.push_back(pQuestInfo->nID);

		CHECK(true == gsys.pItemSystem->GetUser().Use(m_pPlayer, pItem->m_nSlotID));

		pLink->OnRecv(MC_QUEST_GIVE_REQ, 1, NEW_INT(pQuestInfo->nID));	

		CHECK(true == m_pPlayer->GetQuests().IsDoingQuest(pQuestInfo->nID));
	}
	
	TEST_FIXTURE(Fixture, CheckCondition)
	{
		GConditionsInfo* pConditionsInfo = test.condition.NewCondI(test.condition.NewCondLForLevel(10), true);
		GTalentInfo* pTalentInfo = test.talent.NewItemTalentInfo();

		GItem* pItem = test.item.GiveNewItem(m_pPlayer);
		pItem->m_pItemData->m_ItemType = ITEMTYPE_USABLE;
		pItem->m_pItemData->m_nUsableType = USABLE_TALENT_USE;
		pItem->m_pItemData->m_vecUsableParam.push_back(pTalentInfo->m_nID);
		pItem->m_pItemData->m_nConditionsInfoID = pConditionsInfo->m_nID;

		m_pPlayer->GetPlayerInfo()->nLevel = 9;
		CHECK(false == gsys.pItemSystem->GetUser().Use(m_pPlayer, pItem->m_nSlotID));

		m_pPlayer->GetPlayerInfo()->nLevel = 10;
		CHECK(true == gsys.pItemSystem->GetUser().Use(m_pPlayer, pItem->m_nSlotID));
	}

}