#include "stdafx.h"
#include "GTestWrapper.h"
#include "GTestForward.h"
#include "MockField.h"
#include "GNPCIconSelector.h"
#include "CSConditionInfo.h"
#include "GQuestInfo.h"
#include "GPlayerQuest.h"
#include "GNPCInteraction.h"
#include "GDialogInfo.h"


SUITE(NPCIconSelector)
{
	class TestNPCIconSelector : public GNPCIconSelector
	{
	public:
		using GNPCIconSelector::HasAutoCondition;
		using GNPCIconSelector::SingleSelect;
		virtual ~TestNPCIconSelector() {}
	};

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
			m_pField->Destroy();
		}

		DECLWRAPPER_NPCInfoMgr;
		DECLWRAPPER_QuestInfoMgr;
		TestNPCIconSelector	m_NPCIConSelector;
		MockField*			m_pField;
		GEntityPlayer*		m_pPlayer;
		GEntityNPC*			m_pNPC;

		vec3				m_vBaseSectorPos;
		vec3				m_vNeighborSectorPos;
		vec3				m_vFarSectorPos;

		void CheckHasAutoCondition_Quest(CONDITION_TYPE nConditionType)
		{
			vector<GNPCInteractionElementInfo*> vecNIEInfo;
			GNPCInteractionElementInfo NIEInfo;
			vecNIEInfo.push_back(&NIEInfo);

			NIEInfo.m_nType = IT_QUEST_BEGIN;
			CHECK_EQUAL(true, m_NPCIConSelector.HasAutoCondition(vecNIEInfo, nConditionType));
			NIEInfo.m_nType = IT_QUEST_END;
			CHECK_EQUAL(true, m_NPCIConSelector.HasAutoCondition(vecNIEInfo, nConditionType));
			NIEInfo.m_nType = IT_QUEST_INTERACT_IMMORTAL;
			CHECK_EQUAL(true, m_NPCIConSelector.HasAutoCondition(vecNIEInfo, nConditionType));
		}

		void CheckHasAutoCondition_Gather(CONDITION_TYPE nConditionType)
		{
			vector<GNPCInteractionElementInfo*> vecNIEInfo;
			GNPCInteractionElementInfo NIEInfo;
			vecNIEInfo.push_back(&NIEInfo);

			for (int i = IT_GATHER_1; i <= IT_GATHER_10; ++i)
			{
				NIEInfo.m_nType = (INTERACTION_TYPE)i;
				CHECK_EQUAL(true, m_NPCIConSelector.HasAutoCondition(vecNIEInfo, nConditionType));
			}
		}
	};

	TEST_FIXTURE(Fixture, HasAutoCondition)
	{
		CheckHasAutoCondition_Quest(CT_QUEST);
		CheckHasAutoCondition_Quest(CT_QUEST_VAR);
		CheckHasAutoCondition_Quest(CT_QUEST_OBJECTIVE_COMPLETE);
		CheckHasAutoCondition_Quest(CT_QUEST_OBJECTIVE_INCOMPLETE);

		CheckHasAutoCondition_Gather(CT_TALENT);
		CheckHasAutoCondition_Gather(CT_ITEM);

		vector<GNPCInteractionElementInfo*> vecNIEInfo;
		GNPCInteractionElementInfo NIEInfo;
		vecNIEInfo.push_back(&NIEInfo);
		NIEInfo.m_nType = IT_NPCSHOP;

		CHECK_EQUAL(true, m_NPCIConSelector.HasAutoCondition(vecNIEInfo, CT_FACTION));
	}

	TEST_FIXTURE(Fixture, SingleSelect_NONE)
	{
		m_pNPC->GetNPCStatus().nAttackable = NAT_ALWAYS;
		CHECK_EQUAL(NIT_NONE, m_NPCIConSelector.SingleSelect(m_pPlayer, m_pNPC));

		m_pNPC->GetNPCStatus().nAttackable = NAT_NONE;
		m_pNPC->GetNPCInteraction().Disable();
		CHECK_EQUAL(NIT_NONE, m_NPCIConSelector.SingleSelect(m_pPlayer, m_pNPC));
	}
	
	// 퀘스트 진행중일때
	TEST_FIXTURE(Fixture, SingleSelect_QuestContinue)
	{
		GPlayerQuest* pPlayerQuest = test.quest.GiveNewPlayerQuest(m_pPlayer);
		GNPCInteractionElementInfo* pIElementInfo = test.interaction.GiveNewIElementInfoForClick(m_pNPC->GetNPCInfo(), IT_QUEST_END, pPlayerQuest->GetInfo()->nID);
		pPlayerQuest->GetInfo()->nIncompleteDialogID =  test.dialog.NewDialogInfo()->m_nID;

		test.interaction.GiveNewIElementInfoForClick(m_pNPC->GetNPCInfo(), IT_DIALOG, pPlayerQuest->GetInfo()->nIncompleteDialogID);
		CHECK_EQUAL(NIT_QUEST_CONTINUE, m_NPCIConSelector.SingleSelect(m_pPlayer, m_pNPC));
	}

	// 퀘스트 진행중인것과 퀘스트 완료인게 있을때 퀘스트 완료 아이콘이 선택된다. 
	TEST_FIXTURE(Fixture, SingleSelect_QuestEnd)
	{
		GPlayerQuest* pPlayerContinueQuest = test.quest.GiveNewPlayerQuest(m_pPlayer);
		GPlayerQuest* pPlayerCompleteQuest = test.quest.GiveNewPlayerQuest(m_pPlayer);		
		pPlayerCompleteQuest->Complete();

		GNPCInteractionElementInfo* pContinueIEInfo = test.interaction.GiveNewIElementInfoForClick(m_pNPC->GetNPCInfo(), IT_QUEST_END, pPlayerContinueQuest->GetInfo()->nID);
		GNPCInteractionElementInfo* pCompleteIEInfo = test.interaction.GiveNewIElementInfoForClick(m_pNPC->GetNPCInfo(), IT_QUEST_END, pPlayerCompleteQuest->GetInfo()->nID);
		
		CHECK_EQUAL(NIT_QUEST_END, m_NPCIConSelector.SingleSelect(m_pPlayer, m_pNPC));
	}

	// 퀘스트 진행중인것과 퀘스트 시작인게 있을때 퀘스트 시작 아이콘이 선택된다. 
	TEST_FIXTURE(Fixture, SingleSelect_QuestBegin)
	{
		GPlayerQuest* pPlayerContinueQuest = test.quest.GiveNewPlayerQuest(m_pPlayer);
		GQuestInfo* pBeingQuestInfo = test.quest.NewQuestInfo();

		GNPCInteractionElementInfo* pContinueIEInfo = test.interaction.GiveNewIElementInfoForClick(m_pNPC->GetNPCInfo(), IT_QUEST_END, pPlayerContinueQuest->GetInfo()->nID);
		GNPCInteractionElementInfo* pBeginIEInfo = test.interaction.GiveNewIElementInfoForClick(m_pNPC->GetNPCInfo(), IT_QUEST_BEGIN, pBeingQuestInfo->nID);

		CHECK_EQUAL(NIT_QUEST_BEGIN, m_NPCIConSelector.SingleSelect(m_pPlayer, m_pNPC));
	}


	// 레벨 컨디션이 2레벨 초과로 실패하지 않을때, 취득불가 퀘스트 아이콘이 선택된다.
	TEST_FIXTURE(Fixture, SingleSelect_QuestNotBegin)
	{		
		GQuestInfo* pQuestInfo = test.quest.NewQuestInfo();
		GNPCInteractionElementInfo* pIEInfo = test.interaction.GiveNewIElementInfoForClick(m_pNPC->GetNPCInfo(), IT_QUEST_BEGIN, pQuestInfo->nID);
		pIEInfo->m_pConditionsInfo = test.condition.NewCondI(test.condition.NewCondLForLevel(m_pPlayer->GetLevel() + 2));

		CHECK_EQUAL(NIT_QUEST_NOT_BEGIN_ONLYICON, m_NPCIConSelector.SingleSelect(m_pPlayer, m_pNPC));

		m_pPlayer->SetLevel(m_pPlayer->GetLevel()-1);

		CHECK_EQUAL(NIT_NONE, m_NPCIConSelector.SingleSelect(m_pPlayer, m_pNPC));
	}

	// 아이템을 사용해서 할수 있는 인터랙션인대, 아이템이 없을때
	TEST_FIXTURE(Fixture, SingleSelect_NotExistInteractionItem)
	{
		GItemData* pItemData = test.item.NewItemData();
		GQuestInfo* pQuestInfo = test.quest.NewQuestInfo();
		GNPCInteractionElementInfo* pIEInfo = test.interaction.GiveNewIElementInfoForItem(m_pNPC->GetNPCInfo(), pItemData->m_nID, IT_QUEST_BEGIN, pQuestInfo->nID);

		CHECK_EQUAL(NIT_ITEM_UNUSABLE, m_NPCIConSelector.SingleSelect(m_pPlayer, m_pNPC));
	}

	// 인터랙션 타겟 컨디션이 live 인대 죽어있는 경우
	TEST_FIXTURE(Fixture, SingleSelect_ExpecteLiveButDead)
	{
		GQuestInfo* pQuestInfo = test.quest.NewQuestInfo();
		GNPCInteractionElementInfo* pIEInfo = test.interaction.GiveNewIElementInfoForClick(m_pNPC->GetNPCInfo(), IT_QUEST_BEGIN, pQuestInfo->nID);
		pIEInfo->m_nTargetCondition = ITCT_LIVE;
		m_pNPC->doDie();
		
		CHECK_EQUAL(NIT_NONE, m_NPCIConSelector.SingleSelect(m_pPlayer, m_pNPC));
	}

	// 인터랙션 타겟 컨디션이 dead 인대 살아 있는 경우
	TEST_FIXTURE(Fixture, SingleSelect_ExpecteDeadButLive)
	{
		GQuestInfo* pQuestInfo = test.quest.NewQuestInfo();
		GNPCInteractionElementInfo* pIEInfo = test.interaction.GiveNewIElementInfoForClick(m_pNPC->GetNPCInfo(), IT_QUEST_BEGIN, pQuestInfo->nID);
		pIEInfo->m_nTargetCondition = ITCT_DEAD;

		CHECK_EQUAL(NIT_NONE, m_NPCIConSelector.SingleSelect(m_pPlayer, m_pNPC));
	}

	// 인터랙션 타겟 컨디션이 dead 인대 아이템이 남아 있는 경우
	TEST_FIXTURE(Fixture, SingleSelect_ExpecteDeadButRemainItem)
	{
		GQuestInfo* pQuestInfo = test.quest.NewQuestInfo();
		GNPCInteractionElementInfo* pIEInfo = test.interaction.GiveNewIElementInfoForClick(m_pNPC->GetNPCInfo(), IT_QUEST_BEGIN, pQuestInfo->nID);
		pIEInfo->m_nTargetCondition = ITCT_DEAD;
		test.loot.PrepareDropItem(m_pPlayer, m_pNPC);
		m_pNPC->doDie();

		CHECK_EQUAL(NIT_NONE, m_NPCIConSelector.SingleSelect(m_pPlayer, m_pNPC));
	}
}