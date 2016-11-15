#include "stdafx.h"
#include "GTestForward.h"
#include "FBaseGame.h"
#include "GDropItems.h"
#include "GEntityNPC.h"
#include "GNPCLoot.h"
#include "GDropList.h"
#include "GLootInfo.h"
#include "GConditionsSystem.h"
#include "GNPCBParts.h"
#include "GPlayerQuest.h"
#include "GPlayerQObjective.h"
#include "GParty.h"
#include "GDropItem.h"
#include "GItemManager.h"
#include "GQObjectiveInfo.h"
#include "GQuestInfo.h"

SUITE(DropItems)
{
	struct Fixture : public FBaseGame
	{
		Fixture()
		{
			m_pPlayer2 = test.player.NewPlayer(m_pField);
			m_pParty = test.party.CreateParty(m_pPlayer, m_pPlayer2);
			m_pNPC = test.npc.SpawnNPC(m_pField);
		}

		GDropItems& GetDropItems()
		{
			return m_pNPC->GetNPCLoot().GetDropList().GetDropItems();
		}
	
		GUTHelper		m_Helper;
		GEntityPlayer*	m_pPlayer2;
		GParty*			m_pParty;
		GEntityNPC*		m_pNPC;	
	};

	// LOOT_ITEM_INFO 1개당 최대, 최소 드랍량
	TEST_FIXTURE(Fixture, MinMax)
	{
		GLootInfo* pLootInfo = test.loot.NewLootInfo();
		LOOT_ITEM_INFO* pLootItemInfo = test.loot.NewLootItemInfo(pLootInfo, 100, 1, 10);
		m_pNPC->GetNPCInfo()->m_pLootInfo = pLootInfo;

		GetDropItems().DropByKill(pLootInfo, m_pPlayer->GetCID(), m_pParty->GetUID(), LOOTING_RULE_DATA());

		int nQuantity = GetDropItems().GetSumOfAmount();
		CHECK(pLootItemInfo->m_nMin <= nQuantity && 
			pLootItemInfo->m_nMax >= nQuantity);
	}
	
	// 브레이크 파츠가 부셔져야 드랍
	TEST_FIXTURE(Fixture, CheckBreakPart)
	{
		GLootInfo* pLootInfo = test.loot.NewLootInfo();
		LOOT_ITEM_INFO* pLootItemInfo = test.loot.NewLootItemInfo(pLootInfo, 100, 1, 10);
		pLootItemInfo->m_nBreakPart = 1;
		m_pNPC->GetNPCInfo()->m_pLootInfo = pLootInfo;

		GetDropItems().DropByKill(pLootInfo, m_pPlayer->GetCID(), m_pParty->GetUID(), LOOTING_RULE_DATA());
		CHECK_EQUAL(true, GetDropItems().IsEmpty());

		GetDropItems().Clear();

		m_pNPC->GetNPCBParts()->SetBreak(pLootItemInfo->m_nBreakPart);
		GetDropItems().DropByKill(pLootInfo, m_pPlayer->GetCID(), m_pParty->GetUID(), LOOTING_RULE_DATA());
		CHECK_EQUAL(false, GetDropItems().IsEmpty());
	}
	
	// bpart와 quest를 제외한 아이템은 최대 드랍 제한 개수를 넘게 드랍될 수 없다.
	TEST_FIXTURE(Fixture, LimitDropQuantityForNoBreakPartAndNoQuest)
	{
		int nOverLimitDropQuantity = 11;
		GQuestInfo* pQuestInfo = test.quest.NewQuestInfo();		
		GQObjectiveInfo* pQObjectiveInfo = test.quest.NewCollectQObjectiveInfo(pQuestInfo, nOverLimitDropQuantity);
		test.quest.GiveNewPlayerQuest(m_pPlayer, pQuestInfo->nID);

		GLootInfo* pLootInfo = test.loot.NewLootInfo();
		pLootInfo->m_nLimitDropQuantity = nOverLimitDropQuantity - 1;
		LOOT_ITEM_INFO* pLootNomalItemInfo = test.loot.NewLootItemInfo(pLootInfo, 100, nOverLimitDropQuantity, nOverLimitDropQuantity);
		LOOT_ITEM_INFO* pLootQuestItemInfo = test.loot.NewLootItemInfo(pLootInfo, 100, nOverLimitDropQuantity, nOverLimitDropQuantity, pQObjectiveInfo->vecParam1[0]);
		pLootQuestItemInfo->m_vecQuestID.push_back(pQuestInfo->nID);
		LOOT_ITEM_INFO* pLootBreakPartItemInfo = test.loot.NewLootItemInfo(pLootInfo, 100, nOverLimitDropQuantity, nOverLimitDropQuantity);						
		pLootBreakPartItemInfo->m_nBreakPart = 1;

		m_pNPC->GetNPCInfo()->m_pLootInfo = pLootInfo;
		m_pNPC->GetNPCBParts()->SetBreak(pLootBreakPartItemInfo->m_nBreakPart);

		GetDropItems().DropByKill(pLootInfo, m_pPlayer->GetCID(), m_pParty->GetUID(), LOOTING_RULE_DATA());

		int nNormalItemQuantity = 0;
		int nQuestItemQuantity = 0;
		int nBreakPartItemQuantity = 0;

		const map<MUID, GDropItem*>& mapDropItem = GetDropItems().GetContainer();
		for each (map<MUID, GDropItem*>::value_type val in mapDropItem)
		{
			GDropItem* pDropItem = val.second;

			if (pLootNomalItemInfo->m_nID == pDropItem->GetID())
			{
				nNormalItemQuantity += pDropItem->GetAmount();
			}
			if (pLootQuestItemInfo->m_nID == pDropItem->GetID())
			{
				nQuestItemQuantity += pDropItem->GetAmount();
			}
			if (pLootBreakPartItemInfo->m_nID == pDropItem->GetID())
			{
				nBreakPartItemQuantity += pDropItem->GetAmount();
			}
		}

		CHECK(pLootInfo->m_nLimitDropQuantity >= nNormalItemQuantity);
		CHECK(pLootInfo->m_nLimitDropQuantity < nQuestItemQuantity);
		CHECK(pLootInfo->m_nLimitDropQuantity < nBreakPartItemQuantity);
	}

	// 루팅룰이 자유롭게일때 죽여서 드랍된 일반 아이템
	TEST_FIXTURE(Fixture, Normaltem_FreeForAll)
	{
		GLootInfo* pLootInfo = test.loot.NewLootInfo();
		LOOT_ITEM_INFO* pLootItemInfo = test.loot.NewLootItemInfo(pLootInfo);
		m_pNPC->GetNPCInfo()->m_pLootInfo = pLootInfo;

		vector<int> vecBeneficiaryCID;
		vecBeneficiaryCID.push_back(m_pPlayer->GetCID());
		vecBeneficiaryCID.push_back(m_pPlayer2->GetCID());

		LOOTING_RULE_DATA lootingRuleData;
		lootingRuleData.m_nLRC = LRC_FREE_FOR_ALL;
		GetDropItems().DropByKill(pLootInfo, vecBeneficiaryCID, m_pParty->GetUID(), lootingRuleData);

		const map<MUID, GDropItem*>& mapDropItem = GetDropItems().GetContainer();
		for each (map<MUID, GDropItem*>::value_type val in mapDropItem)
		{
			GDropItem* pDropItem = val.second;
			
			CHECK_EQUAL(true, pDropItem->IsGettable());
			CHECK_EQUAL(true, pDropItem->IsAuthorizedCID(m_pPlayer->GetCID()));
			CHECK_EQUAL(true, pDropItem->IsAuthorizedCID(m_pPlayer2->GetCID()));
		}
	}

	// 루팅룰이 차례대로일때 죽여서 드랍된 일반 아이템
	// - Gettable
	// - 해당 차례의 보상자만 볼수 있음
	TEST_FIXTURE(Fixture, Normaltem_RoundRobin)
	{
		m_pParty->SetRoundRobinLastOrderCID(m_pPlayer2->GetCID());

		GLootInfo* pLootInfo = test.loot.NewLootInfo();
		LOOT_ITEM_INFO* pLootItemInfo1 = test.loot.NewLootItemInfo(pLootInfo);
		LOOT_ITEM_INFO* pLootItemInfo2 = test.loot.NewLootItemInfo(pLootInfo);
		m_pNPC->GetNPCInfo()->m_pLootInfo = pLootInfo;

		vector<int> vecBeneficiaryCID;
		vecBeneficiaryCID.push_back(m_pPlayer->GetCID());
		vecBeneficiaryCID.push_back(m_pPlayer2->GetCID());

		LOOTING_RULE_DATA lootingRuleData;
		lootingRuleData.m_nLRC = LRC_ROUND_ROBIN;

		GetDropItems().DropByKill(pLootInfo, vecBeneficiaryCID, m_pParty->GetUID(), lootingRuleData);

		const map<MUID, GDropItem*>& mapDropItem = GetDropItems().GetContainer();

		vector<GDropItem*> vecDropItem1 = GetDropItems().CollectDropItemByCID(m_pPlayer->GetCID());
		vector<GDropItem*> vecDropItem2 = GetDropItems().CollectDropItemByCID(m_pPlayer2->GetCID());

		CHECK_EQUAL(false, vecDropItem1.empty());
		CHECK_EQUAL(false, vecDropItem2.empty());

		for each (GDropItem * pDropItem1 in vecDropItem1)
		{
			CHECK_EQUAL(false, pDropItem1->IsAuthorizedCID(m_pPlayer2->GetCID()));
		}

		for each (GDropItem * pDropItem2 in vecDropItem2)
		{
			CHECK_EQUAL(false, pDropItem2->IsAuthorizedCID(m_pPlayer->GetCID()));
		}		
	}

	// 루팅룰이 주사위 굴림일때 죽여서 드랍된 일반아이템
	// - Rollable
	// - 보상자는 모두 볼수 있음
	TEST_FIXTURE(Fixture, Normaltem_RollDice)
	{
		GLootInfo* pLootInfo = test.loot.NewLootInfo();
		LOOT_ITEM_INFO* pLootItemInfo = test.loot.NewLootItemInfo(pLootInfo);
		m_pNPC->GetNPCInfo()->m_pLootInfo = pLootInfo;
		GItemData* pItemData = gmgr.pItemManager->GetItemData(pLootItemInfo->m_nID);
		pItemData->m_nTier = ITEM_TIER_TREASURE;

		vector<int> vecBeneficiaryCID;
		vecBeneficiaryCID.push_back(m_pPlayer->GetCID());
		vecBeneficiaryCID.push_back(m_pPlayer2->GetCID());

		LOOTING_RULE_DATA lootingRuleData;
		lootingRuleData.m_nLRR = LRR_ROLL_DICE;
		GetDropItems().DropByKill(pLootInfo, vecBeneficiaryCID, m_pParty->GetUID(), lootingRuleData);

		const map<MUID, GDropItem*>& mapDropItem = GetDropItems().GetContainer();
		for each (map<MUID, GDropItem*>::value_type val in mapDropItem)
		{
			GDropItem* pDropItem = val.second;

			CHECK_EQUAL(true, pDropItem->IsRollable());
			CHECK_EQUAL(true, pDropItem->IsAuthorizedCID(m_pPlayer->GetCID()));
			CHECK_EQUAL(true, pDropItem->IsAuthorizedCID(m_pPlayer2->GetCID()));
		}
	}

	// 루팅룰이 파티리더일때 죽여서 드랍된 일반아이템
	// - MasterLootable
	// - 보상자는 모두 볼수 있음
	TEST_FIXTURE(Fixture, Normaltem_MasterLoot)
	{
		GLootInfo* pLootInfo = test.loot.NewLootInfo();
		LOOT_ITEM_INFO* pLootItemInfo = test.loot.NewLootItemInfo(pLootInfo);
		m_pNPC->GetNPCInfo()->m_pLootInfo = pLootInfo;

		vector<int> vecBeneficiaryCID;
		vecBeneficiaryCID.push_back(m_pPlayer->GetCID());
		vecBeneficiaryCID.push_back(m_pPlayer2->GetCID());

		LOOTING_RULE_DATA lootingRuleData;
		lootingRuleData.m_nLRC = LRC_MASTER_LOOT;
		GetDropItems().DropByKill(pLootInfo, vecBeneficiaryCID, m_pParty->GetUID(), lootingRuleData);

		const map<MUID, GDropItem*>& mapDropItem = GetDropItems().GetContainer();
		for each (map<MUID, GDropItem*>::value_type val in mapDropItem)
		{
			GDropItem* pDropItem = val.second;

			CHECK_EQUAL(true, pDropItem->IsMasterLootable());
			CHECK_EQUAL(true, pDropItem->IsAuthorizedCID(m_pPlayer->GetCID()));
			CHECK_EQUAL(true, pDropItem->IsAuthorizedCID(m_pPlayer2->GetCID()));
		}
	}

	// 퀘스트아이템
	// - Gettable
	// - 퀘스트를 가진 사람만 볼수 있음
	TEST_FIXTURE(Fixture, QuestItem)
	{
		GQuestInfo* pQuestInfo = test.quest.NewQuestInfo();	
		CSQObjectiveInfo* pQObjectiveInfo = test.quest.NewCollectQObjectiveInfo(pQuestInfo);		
		test.quest.GiveNewPlayerQuest(m_pPlayer2, pQuestInfo->nID);		

		GLootInfo* pLootInfo = test.loot.NewLootInfo();
		LOOT_ITEM_INFO* pLootItemInfo = test.loot.NewLootItemInfo(pLootInfo);
		pLootItemInfo->m_vecQuestID.push_back(pQuestInfo->nID);
		m_pNPC->GetNPCInfo()->m_pLootInfo = pLootInfo;

		vector<int> vecBeneficiaryCID;
		vecBeneficiaryCID.push_back(m_pPlayer->GetCID());
		vecBeneficiaryCID.push_back(m_pPlayer2->GetCID());

		GetDropItems().DropByKill(pLootInfo, vecBeneficiaryCID, m_pParty->GetUID(), LOOTING_RULE_DATA());
		const map<MUID, GDropItem*>& mapDropItem = GetDropItems().GetContainer();
		for each (map<MUID, GDropItem*>::value_type val in mapDropItem)
		{
			GDropItem* pDropItem = val.second;

			CHECK_EQUAL(true, pDropItem->IsGettable());
			CHECK_EQUAL(false, pDropItem->IsViewableCID(m_pPlayer->GetCID()));
			CHECK_EQUAL(true, pDropItem->IsViewableCID(m_pPlayer2->GetCID()));
		}
	}

	// 퀘스트아이템은 퀘스트에 필요한양 이상 드랍되지 않는다.
	TEST_FIXTURE(Fixture, QuestItem_LimitQuestNeedQuantity)
	{
		int nNeedItemQuantity = 10;
		GQuestInfo* pQuestInfo = test.quest.NewQuestInfo();	
		CSQObjectiveInfo* pQObjectiveInfo = test.quest.NewCollectQObjectiveInfo(pQuestInfo, nNeedItemQuantity);

		GPlayerQuest* pPlayer1Quest = test.quest.GiveNewPlayerQuest(m_pPlayer, pQuestInfo->nID);
		pPlayer1Quest->Get(pQObjectiveInfo->nID)->UpdateProgress(nNeedItemQuantity);
		GPlayerQuest* pPlayer2Quest = test.quest.GiveNewPlayerQuest(m_pPlayer2, pQuestInfo->nID);
		pPlayer2Quest->Get(pQObjectiveInfo->nID)->UpdateProgress(3);

		GLootInfo* pLootInfo = test.loot.NewLootInfo();
		LOOT_ITEM_INFO* pLootItemInfo = test.loot.NewLootItemInfo(pLootInfo, 100, 100, 100, pQObjectiveInfo->vecParam1[0]);
		pLootItemInfo->m_vecQuestID.push_back(pQuestInfo->nID);
		m_pNPC->GetNPCInfo()->m_pLootInfo = pLootInfo;

		vector<int> vecBeneficiaryCID;
		vecBeneficiaryCID.push_back(m_pPlayer->GetCID());
		vecBeneficiaryCID.push_back(m_pPlayer2->GetCID());

		GetDropItems().DropByKill(pLootInfo, vecBeneficiaryCID, m_pParty->GetUID(), LOOTING_RULE_DATA());

		int nQuantity1 = 0;
		int nQuantity2= 0;

		const map<MUID, GDropItem*>& mapDropItem = GetDropItems().GetContainer();
		for each (map<MUID, GDropItem*>::value_type val in mapDropItem)
		{
			GDropItem* pDropItem = val.second;

			if (true == pDropItem->IsViewableCID(m_pPlayer->GetCID()) &&
				pQObjectiveInfo->vecParam1[0] == pDropItem->GetID())
			{
				nQuantity1 += pDropItem->GetAmount();
			}

			if (true == pDropItem->IsViewableCID(m_pPlayer2->GetCID()) &&
				pQObjectiveInfo->vecParam1[0] == pDropItem->GetID())
			{
				nQuantity2 += pDropItem->GetAmount();
			}
		}		

		CHECK_EQUAL(0, nQuantity1);
		CHECK_EQUAL(nNeedItemQuantity - 3, nQuantity2);
	}

	// 인터랙션으로 드랍된 아이템
	TEST_FIXTURE(Fixture, InteractItem_Authority)
	{
		GLootInfo* pLootInfo = test.loot.NewLootInfo();
		LOOT_ITEM_INFO* pLootItemInfo = test.loot.NewLootItemInfo(pLootInfo);
		m_pNPC->GetNPCInfo()->m_pLootInfo = pLootInfo;

		GetDropItems().DropByInteract(m_pPlayer->GetCID(), pLootInfo);

		const map<MUID, GDropItem*>& mapDropItem = GetDropItems().GetContainer();
		for each (map<MUID, GDropItem*>::value_type val in mapDropItem)
		{
			GDropItem* pDropItem = val.second;

			CHECK_EQUAL(true, pDropItem->IsGettable());
			CHECK_EQUAL(true, pDropItem->IsViewableCID(m_pPlayer->GetCID()));
			CHECK_EQUAL(false, pDropItem->IsViewableCID(m_pPlayer2->GetCID()));
		}
	}	
}
