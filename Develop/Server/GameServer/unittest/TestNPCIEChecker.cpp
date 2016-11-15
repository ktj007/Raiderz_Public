#include "stdafx.h"
#include "GTestForward.h"
#include "GNPCIEChecker.h"
#include "GPlayerQObjective.h"
#include "GPlayerQuest.h"
#include "CSFactionCalculator.h"
#include "GNPCShopInfo.h"
#include "GFactionInfo.h"
#include "GPlayerFactions.h"
#include "GQuestInfo.h"
#include "GQObjectiveInfo.h"


SUITE(NPCIEChecker)
{
	struct Fixture
	{
		Fixture()
		{
			m_pField = GUTHelper_Field::DefaultMockField();
			m_pPlayer = test.player.NewPlayer(m_pField);
			m_pNPC = test.npc.SpawnNPC(m_pField);
			m_pNPC->GetNPCStatus().nAttackable = NAT_NONE;
		}
		~Fixture()
		{

		}

		GNPCIEChecker		m_NPCIEChecker;
		MockField*			m_pField;
		GEntityPlayer*		m_pPlayer;
		GEntityNPC*			m_pNPC;
	};

	TEST_FIXTURE(Fixture, QuestBegin_ExistDoingQuest)
	{
		GQuestInfo* pQuestInfo = test.quest.NewQuestInfo();		
		GNPCInteractionElementInfo* pIElementInfo = test.interaction.GiveNewIElementInfoForClick(m_pNPC->GetNPCInfo(), IT_QUEST_BEGIN, pQuestInfo->nID);

		test.quest.GiveNewPlayerQuest(m_pPlayer, pQuestInfo->nID);
		CHECK_EQUAL(false, m_NPCIEChecker.QuestBegin(m_pPlayer, pIElementInfo));
	}

	TEST_FIXTURE(Fixture, QuestBegin_ExistCompleteQuest)
	{
		GQuestInfo* pQuestInfo = test.quest.NewQuestInfo();		
		GNPCInteractionElementInfo* pIElementInfo = test.interaction.GiveNewIElementInfoForClick(m_pNPC->GetNPCInfo(), IT_QUEST_BEGIN, pQuestInfo->nID);

		test.quest.GiveNewPlayerQuest(m_pPlayer, pQuestInfo->nID);
		test.quest.CompleteQuest(m_pPlayer, pQuestInfo->nID);
		CHECK_EQUAL(false, m_NPCIEChecker.QuestBegin(m_pPlayer, pIElementInfo));
	}

	TEST_FIXTURE(Fixture, QuestBegin_ExistDoneQuest)
	{
		GQuestInfo* pQuestInfo = test.quest.NewQuestInfo();
		pQuestInfo->bRepeatable = false;
		GNPCInteractionElementInfo* pIElementInfo = test.interaction.GiveNewIElementInfoForClick(m_pNPC->GetNPCInfo(), IT_QUEST_BEGIN, pQuestInfo->nID);

		test.quest.GiveNewPlayerQuest(m_pPlayer, pQuestInfo->nID);
		test.quest.DoneQuest(m_pPlayer, pQuestInfo->nID);
		CHECK_EQUAL(false, m_NPCIEChecker.QuestBegin(m_pPlayer, pIElementInfo));
	}

	TEST_FIXTURE(Fixture, QuestBegin)
	{
		GQuestInfo* pQuestInfo = test.quest.NewQuestInfo();		
		pQuestInfo->bRepeatable = false;
		GNPCInteractionElementInfo* pIElementInfo = test.interaction.GiveNewIElementInfoForClick(m_pNPC->GetNPCInfo(), IT_QUEST_BEGIN, pQuestInfo->nID);

		CHECK_EQUAL(true, m_NPCIEChecker.QuestBegin(m_pPlayer, pIElementInfo));
	}

	TEST_FIXTURE(Fixture, QuestEnd_NotExistQuest)
	{
		GQuestInfo* pQuestInfo = test.quest.NewQuestInfo();		
		GNPCInteractionElementInfo* pIElementInfo = test.interaction.GiveNewIElementInfoForClick(m_pNPC->GetNPCInfo(), IT_QUEST_END, pQuestInfo->nID);

		CHECK_EQUAL(false, m_NPCIEChecker.QuestEnd(m_pPlayer, pIElementInfo));	
	}

	TEST_FIXTURE(Fixture, QuestEnd_NotExistIncompleteDialog)
	{
		GQuestInfo* pQuestInfo = test.quest.NewQuestInfo();		
		GNPCInteractionElementInfo* pIElementInfo = test.interaction.GiveNewIElementInfoForClick(m_pNPC->GetNPCInfo(), IT_QUEST_END, pQuestInfo->nID);
		test.quest.GiveNewPlayerQuest(m_pPlayer, pQuestInfo->nID);

		CHECK_EQUAL(false, m_NPCIEChecker.QuestEnd(m_pPlayer, pIElementInfo));
	}

	TEST_FIXTURE(Fixture, QuestEnd)
	{
		GQuestInfo* pQuestInfo = test.quest.NewQuestInfo();		
		GNPCInteractionElementInfo* pIElementInfo = test.interaction.GiveNewIElementInfoForClick(m_pNPC->GetNPCInfo(), IT_QUEST_END, pQuestInfo->nID);

		test.quest.GiveNewPlayerQuest(m_pPlayer, pQuestInfo->nID);
		test.quest.CompleteQuest(m_pPlayer, pQuestInfo->nID);
		CHECK_EQUAL(true, m_NPCIEChecker.QuestEnd(m_pPlayer, pIElementInfo));
	}

	TEST_FIXTURE(Fixture, QuestInteract_NotExistQuest)
	{
		GQuestInfo* pQuestInfo = test.quest.NewQuestInfo();	
		GNPCInteractionElementInfo* pIElementInfo = test.interaction.GiveNewIElementInfoForClick(m_pNPC->GetNPCInfo(), IT_QUEST_INTERACT_IMMORTAL, pQuestInfo->nID);

		CHECK_EQUAL(false, m_NPCIEChecker.QuestInteract(m_pPlayer, m_pNPC, pIElementInfo));
	}

	TEST_FIXTURE(Fixture, QuestInteract_ExistFailQuest)
	{
		GQuestInfo* pQuestInfo = test.quest.NewQuestInfo();			
		GNPCInteractionElementInfo* pIElementInfo = test.interaction.GiveNewIElementInfoForClick(m_pNPC->GetNPCInfo(), IT_QUEST_INTERACT_IMMORTAL, pQuestInfo->nID);

		GPlayerQuest* pPlayerQuest = test.quest.GiveNewPlayerQuest(m_pPlayer, pQuestInfo->nID);	
		test.quest.FailQuest(m_pPlayer, pQuestInfo->nID);
		CHECK_EQUAL(false, m_NPCIEChecker.QuestInteract(m_pPlayer, m_pNPC, pIElementInfo));
	}

	TEST_FIXTURE(Fixture, QuestInteract)
	{
		GQuestInfo* pQuestInfo = test.quest.NewQuestInfo();	
		GQObjectiveInfo* pQObjectiveInfo = test.quest.NewInteractQObjectiveInfo(pQuestInfo, m_pNPC->GetID(), 1);
		GNPCInteractionElementInfo* pIElementInfo = test.interaction.GiveNewIElementInfoForClick(m_pNPC->GetNPCInfo(), IT_QUEST_INTERACT_IMMORTAL, pQuestInfo->nID);
		pIElementInfo->m_vecAct.push_back(pQObjectiveInfo->nID);

		GPlayerQuest* pPlayerQuest = test.quest.GiveNewPlayerQuest(m_pPlayer, pQuestInfo->nID);
		CHECK_EQUAL(true, m_NPCIEChecker.QuestInteract(m_pPlayer, m_pNPC, pIElementInfo));
	}

	TEST_FIXTURE(Fixture, NPCShop_BadFaction)
	{		
		GFactionInfo* pFactionInfo = test.faction.NewFactionInfo(CSFactionCalculator::GetBadMinQuantity());
		GNPCShopInfo* pNPCShopInfo = test.npcshop.NewNPCShopInfo();
		pNPCShopInfo->m_nFactionID = pFactionInfo->m_nID;
		GNPCInteractionElementInfo* pIElementInfo = test.interaction.GiveNewIElementInfoForClick(m_pNPC->GetNPCInfo(), IT_NPCSHOP, pNPCShopInfo->m_nID);

		CHECK_EQUAL(false, m_NPCIEChecker.NPCShop(m_pPlayer, pIElementInfo));
	}

	TEST_FIXTURE(Fixture, NPCShop_GoodFaction)
	{		
		GFactionInfo* pFactionInfo = test.faction.NewFactionInfo(CSFactionCalculator::GetGoodMaxQuantity());
		GNPCShopInfo* pNPCShopInfo = test.npcshop.NewNPCShopInfo();
		pNPCShopInfo->m_nFactionID = pFactionInfo->m_nID;
		GNPCInteractionElementInfo* pIElementInfo = test.interaction.GiveNewIElementInfoForClick(m_pNPC->GetNPCInfo(), IT_NPCSHOP, pNPCShopInfo->m_nID);

		CHECK_EQUAL(true, m_NPCIEChecker.NPCShop(m_pPlayer, pIElementInfo));
	}

	TEST_FIXTURE(Fixture, NPCShop_NotExistFaction)
	{		
		GNPCShopInfo* pNPCShopInfo = test.npcshop.NewNPCShopInfo();
		GNPCInteractionElementInfo* pIElementInfo = test.interaction.GiveNewIElementInfoForClick(m_pNPC->GetNPCInfo(), IT_NPCSHOP, pNPCShopInfo->m_nID);

		CHECK_EQUAL(true, m_NPCIEChecker.NPCShop(m_pPlayer, pIElementInfo));
	}

	TEST_FIXTURE(Fixture, GuildStorage_NotJoinedGuild)
	{		
		GNPCInteractionElementInfo* pIElementInfo = test.interaction.GiveNewIElementInfoForClick(m_pNPC->GetNPCInfo(), IT_GUILD_STORAGE);

		CHECK_EQUAL(false, m_NPCIEChecker.GuildStorage(m_pPlayer));
	}

	TEST_FIXTURE(Fixture, GuildStorage)
	{
		GUTHelper helper;
		helper.NewGuild(m_pPlayer, SUnitTestUtil::NewGuildName().c_str());
		GNPCInteractionElementInfo* pIElementInfo = test.interaction.GiveNewIElementInfoForClick(m_pNPC->GetNPCInfo(), IT_GUILD_STORAGE);

		CHECK_EQUAL(true, m_NPCIEChecker.GuildStorage(m_pPlayer));
	}


	//TEST_FIXTURE(Fixture, NPCIEChecker_Check_Gather)
	//{
	//	채집 조건 체크에 대한 테스트 코드는 TestGatheringChecker.cpp 에서 다룹니다.
	//	SUITE Name = GatheringCheck
	//}

}