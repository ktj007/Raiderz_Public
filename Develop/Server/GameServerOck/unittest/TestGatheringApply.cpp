#include "stdafx.h"
#include "SUnitTestUtil.h"
#include "GNPCIEApplier.h"
#include "FBaseNPC.h"
#include "FBaseItem.h"
#include "FBaseTalent.h"
#include "GTestForward.h"
#include "GInteractionSystem.h"
#include "GLootInfo.h"
#include "GLootInfoMgr.h"
#include "GNPCInteractor.h"


SUITE(GatheringApply)
{
	struct Fixture : public FBaseNPC, FBaseItem, FBaseTalent
	{
		Fixture()
		{
			InitGatheringLootInfo();
			InitField_Player();
			InitGatherNPC();
			InitGatherTimeConst();

			InitPlayerGatherItem();
			InitPlayerGatherTalent();
		}
		~Fixture()
		{
			m_pField->Destroy();
		}

		void InitGatheringLootInfo()
		{
			m_pNewLootInfo = new GLootInfo();
			m_pNewLootInfo->m_nID = SUnitTestUtil::NewID();
			gmgr.pLootInfoMgr->Insert(m_pNewLootInfo);
		}
		void InitField_Player()
		{
			m_pField = GUTHelper_Field::DefaultMockField();
			m_pTestPlayer = m_Helper.player.NewPlayer(m_pField);

			m_pTestPlayerLink = GUTHelper::NewLink(m_pTestPlayer);
		}
		void InitGatherNPC()
		{
			int nGatherNPCInfoID = SUnitTestUtil::NewID();
			m_pGatherNPCInfo = NewNPCInfo(nGatherNPCInfoID);
			m_pGatherNPCInfo->nNpcType = NPC_TYPE_GATHER;
			m_pGatherNPCInfo->nGatherType = GATHER_MINE;

			m_pNewIElementInfo = new GNPCInteractionElementInfo();
			m_pNewIElementInfo->m_nID = SUnitTestUtil::NewID();
			m_pNewIElementInfo->m_pConditionsInfo = NULL;
			m_pNewIElementInfo->m_nType = IT_GATHER_1;
			m_pNewIElementInfo->m_vecAct.push_back(m_pNewLootInfo->m_nID);

			m_pGatherNPCInfo->m_InteractionInfo.m_vecNPCIEInfo.push_back(m_pNewIElementInfo);
			m_pGatherNPCInfo->m_InteractionInfo.SortByInteractionType();

			m_pTestNPC = NewNPC(m_pField, nGatherNPCInfoID);
			m_pTestNPC->GetNPCStatus().nAttackable = NAT_NONE;
		}
		void InitGatherTimeConst()
		{
			//GConst::GATHER_TIME[ TYPE ][ TIER ]
			GConst::GATHER_TIME[GATHER_MINE][1] = 1.0f;
			GConst::GATHER_TIME[GATHER_MINE][2] = 2.0f;
			GConst::GATHER_TIME[GATHER_MINE][3] = 3.0f;
			GConst::GATHER_TIME[GATHER_MINE][4] = 4.0f;
			GConst::GATHER_TIME[GATHER_MINE][5] = 5.0f;
			GConst::GATHER_TIME[GATHER_MINE][6] = 6.0f;
			GConst::GATHER_TIME[GATHER_MINE][7] = 7.0f;
			GConst::GATHER_TIME[GATHER_MINE][8] = 8.0f;
			GConst::GATHER_TIME[GATHER_MINE][9] = 9.0f;
			GConst::GATHER_TIME[GATHER_MINE][10] = 10.0f;

			GConst::GATHER_TIME[GATHER_HERB][1] = 21.0f;
			GConst::GATHER_TIME[GATHER_HERB][2] = 22.0f;
			GConst::GATHER_TIME[GATHER_HERB][3] = 23.0f;
			GConst::GATHER_TIME[GATHER_HERB][4] = 24.0f;
			GConst::GATHER_TIME[GATHER_HERB][5] = 25.0f;
			GConst::GATHER_TIME[GATHER_HERB][6] = 26.0f;
			GConst::GATHER_TIME[GATHER_HERB][7] = 27.0f;
			GConst::GATHER_TIME[GATHER_HERB][8] = 28.0f;
			GConst::GATHER_TIME[GATHER_HERB][9] = 29.0f;
			GConst::GATHER_TIME[GATHER_HERB][10] = 30.0f;
		}
		void InitPlayerGatherItem()
		{
			for (int i = 0; i < MAX_GATHER_TYPE; ++i)
			{
				GItemData* pItem = m_Helper.item.NewItemData();
				pItem->m_nGatherType = (GATHER_TYPE)i;

				m_Helper.item.GiveNewItem(m_pTestPlayer, pItem->m_nID);
			}
		}
		void InitPlayerGatherTalent()
		{
			for (int i = 1; i <= GATHER_TIER_COUNT; ++i)
			{
				GTalentInfo* pTalent = m_Helper.talent.NewTalentInfo();
				pTalent->m_nTalentType = TT_GATHER;
				pTalent->m_nRank = i;

				m_Helper.talent.Train(m_pTestPlayer, pTalent);
			}
		}

		void Call_InteractionElement()
		{
			CHECK(gsys.pInteractionSystem->Interaction(m_pTestPlayer, m_pTestNPC->GetUID()));
			m_pTestPlayerLink->ResetCommands();
			CHECK(gsys.pInteractionSystem->GetNPCInteractor().InteractionElement(m_pTestPlayer, m_pTestNPC, m_pNewIElementInfo->m_nID));
		}

		GTestHelpers	m_Helper;
		GLootInfo*		m_pNewLootInfo;
		MockField*		m_pField;
		GEntityPlayer*	m_pTestPlayer;
		MockLink*		m_pTestPlayerLink;
		GEntityNPC*		m_pTestNPC;
		GNPCInfo*		m_pGatherNPCInfo;
		GNPCInteractionElementInfo* m_pNewIElementInfo;
		GNPCIEApplier	m_npcIEApplier;

		GTestMgrWrapper<GLootInfoMgr>	m_LootInfoMgr;
	};

	TEST_FIXTURE(Fixture, TestGatheringApply)
	{
		bool bRet = false;

		bRet = gsys.pInteractionSystem->Interaction(m_pTestPlayer, m_pTestNPC->GetUID());
		CHECK_EQUAL(true, bRet);

		bRet = gsys.pInteractionSystem->GetNPCInteractor().InteractionElement(m_pTestPlayer, m_pTestNPC, m_pNewIElementInfo->m_nID);
		CHECK_EQUAL(true, bRet);
	}

	TEST_FIXTURE(Fixture, TestGatheringApply_Command)
	{
		gsys.pInteractionSystem->Interaction(m_pTestPlayer, m_pTestNPC->GetUID());
		m_pTestPlayerLink->ResetCommands();

		gsys.pInteractionSystem->GetNPCInteractor().InteractionElement(m_pTestPlayer, m_pTestNPC, m_pNewIElementInfo->m_nID);

		CHECK_EQUAL(2, m_pTestPlayerLink->GetCommandCount());

		CHECK_EQUAL(MC_NPCINTERACTION_END,		m_pTestPlayerLink->GetCommand(0).GetID());
		CHECK_EQUAL(MC_LOOT_BEGIN,				m_pTestPlayerLink->GetCommand(1).GetID());
		CHECK_EQUAL(m_pTestPlayer->GetUID(),	m_pTestPlayerLink->GetParam<MUID>(1, 0));
		CHECK_EQUAL(m_pTestNPC->GetUID(),		m_pTestPlayerLink->GetParam<MUID>(1, 1));
		CHECK_EQUAL(1.f,						m_pTestPlayerLink->GetParam<float>(1, 2));
	}

	namespace GatherTime
	{
		TEST_FIXTURE(Fixture, GatherTIme_Mine_Tier1)
		{
			m_pGatherNPCInfo->nGatherType = GATHER_MINE;
			m_pNewIElementInfo->m_nType = IT_GATHER_1;

			Call_InteractionElement();
			CHECK_EQUAL(GConst::GATHER_TIME[GATHER_MINE][1],	m_pTestPlayerLink->GetParam<float>(1, 2));
		}

		TEST_FIXTURE(Fixture, GatherTIme_Mine_Tier5)
		{
			m_pGatherNPCInfo->nGatherType = GATHER_MINE;
			m_pNewIElementInfo->m_nType = IT_GATHER_5;

			Call_InteractionElement();
			CHECK_EQUAL(GConst::GATHER_TIME[GATHER_MINE][5],	m_pTestPlayerLink->GetParam<float>(1, 2));
		}

		TEST_FIXTURE(Fixture, GatherTIme_Mine_Tier10)
		{
			m_pGatherNPCInfo->nGatherType = GATHER_MINE;
			m_pNewIElementInfo->m_nType = IT_GATHER_10;

			Call_InteractionElement();
			CHECK_EQUAL(GConst::GATHER_TIME[GATHER_MINE][10],	m_pTestPlayerLink->GetParam<float>(1, 2));
		}

		TEST_FIXTURE(Fixture, GatherTIme_Herb_Tier7)
		{
			m_pGatherNPCInfo->nGatherType = GATHER_HERB;
			m_pNewIElementInfo->m_nType = IT_GATHER_7;

			Call_InteractionElement();
			CHECK_EQUAL(GConst::GATHER_TIME[GATHER_HERB][7],	m_pTestPlayerLink->GetParam<float>(1, 2));
		}
	}

}