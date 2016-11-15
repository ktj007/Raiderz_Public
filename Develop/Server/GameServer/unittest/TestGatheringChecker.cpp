#include "stdafx.h"
#include "SUnitTestUtil.h"
#include "GNPCIEChecker.h"
#include "FBaseNPC.h"
#include "FBaseItem.h"
#include "FBaseTalent.h"
#include "GTestForward.h"
#include "GInteractionSystem.h"
#include "GNPCInteractor.h"

#include "GItemHolder.h"
#include "GPlayerTalent.h"
#include "GInventory.h"


SUITE(GatheringCheck)
{
	struct Fixture : public FBaseNPC, FBaseItem, FBaseTalent
	{
		Fixture()
		{
			InitField_Player();
			InitGatherNPC();

			InitGatherItems();
			InitGatherTalents();
		}
		~Fixture()
		{
			m_pField->Destroy();
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

			m_pIElementInfo = new GNPCInteractionElementInfo();
			m_pIElementInfo->m_nID = SUnitTestUtil::NewID();
			m_pIElementInfo->m_pConditionsInfo = NULL;
			m_pIElementInfo->m_nType = IT_GATHER_1;

			m_pGatherNPCInfo->m_InteractionInfo.m_vecNPCIEInfo.push_back(m_pIElementInfo);
			m_pGatherNPCInfo->m_InteractionInfo.SortByInteractionType();

			m_pTestNPC = NewNPC(m_pField, nGatherNPCInfoID);
		}
		void InitGatherItems()
		{
			for (int i = 0; i < MAX_GATHER_TYPE; ++i)
			{
				GItemData* pItem = m_Helper.item.NewItemData();
				pItem->m_nGatherType = (GATHER_TYPE)i;

				m_vecGatherItems.push_back(pItem->m_nID);
			}
		}
		void InitGatherTalents()
		{
			m_vecGatherTalents.push_back(0);
			for (int i = 1; i <= GATHER_TIER_COUNT; ++i)
			{
				GTalentInfo* pTalent = m_Helper.talent.NewTalentInfo();
				pTalent->m_nTalentType = TT_GATHER;
				pTalent->m_nRank = i;

				m_vecGatherTalents.push_back(pTalent->m_nID);
			}
		}

		void GiveGatherItem(GATHER_TYPE eType)
		{
			int nGatherItem = m_vecGatherItems[eType];
			m_Helper.item.GiveNewItem(m_pTestPlayer, nGatherItem);
		}
		void LearnGatherTalent(int nRank)
		{
			int nGatherTalent = m_vecGatherTalents[nRank];
			GTalentInfo* pGatherTalent = gmgr.pTalentInfoMgr->Find(nGatherTalent);
			m_Helper.talent.Train(m_pTestPlayer, pGatherTalent);
		}

		GTestHelpers	m_Helper;
		MockField*		m_pField;
		GEntityPlayer*	m_pTestPlayer;
		MockLink*		m_pTestPlayerLink;
		GEntityNPC*		m_pTestNPC;
		GNPCInfo*		m_pGatherNPCInfo;
		GNPCInteractionElementInfo* m_pIElementInfo;
		GNPCIEChecker	m_npcIEChecker;

		vector<int>		m_vecGatherItems;
		vector<int>		m_vecGatherTalents;
	};

	namespace Success
	{
		TEST_FIXTURE(Fixture, TestGatheringCheck_Success)
		{
			GiveGatherItem(GATHER_MINE);
			CHECK_EQUAL(1, m_pTestPlayer->GetItemHolder()->GetItemCount(SLOTTYPE_INVENTORY));
			CHECK_EQUAL(true, m_pTestPlayer->GetItemHolder()->GetInventory().HasGatherTool(GATHER_MINE));

			LearnGatherTalent(1);
			CHECK_EQUAL(1, m_pTestPlayer->GetTalent().GetContainer().size());
			CHECK_EQUAL(true, m_pTestPlayer->GetTalent().IsTrainedGatherTalent(1));

			m_pGatherNPCInfo->nGatherType = GATHER_MINE;
			m_pIElementInfo->m_nType = IT_GATHER_1;

			bool bRet = m_npcIEChecker.Gather(m_pTestPlayer, m_pTestNPC, m_pIElementInfo->m_nType);
			CHECK_EQUAL(true, bRet);
		}

		TEST_FIXTURE(Fixture, TestGatheringCheck_Success_Herb_Tier10)
		{
			GiveGatherItem(GATHER_HERB);
			LearnGatherTalent(10);

			m_pGatherNPCInfo->nGatherType = GATHER_HERB;
			m_pIElementInfo->m_nType = IT_GATHER_10;

			bool bRet = m_npcIEChecker.Gather(m_pTestPlayer, m_pTestNPC, m_pIElementInfo->m_nType);
			CHECK_EQUAL(true, bRet);
		}

		TEST_FIXTURE(Fixture, TestGatheringCheck_Success_Herb_Tier5)
		{
			GiveGatherItem(GATHER_HERB);
			LearnGatherTalent(5);
			LearnGatherTalent(3);
			LearnGatherTalent(1);

			m_pGatherNPCInfo->nGatherType = GATHER_HERB;
			m_pIElementInfo->m_nType = IT_GATHER_5;

			bool bRet = m_npcIEChecker.Gather(m_pTestPlayer, m_pTestNPC, m_pIElementInfo->m_nType);
			CHECK_EQUAL(true, bRet);
		}

		TEST_FIXTURE(Fixture, TestGatheringCheck_Success_Lumber_NoTool)
		{
			CHECK_EQUAL(0, m_pTestPlayer->GetItemHolder()->GetItemCount(SLOTTYPE_INVENTORY));
			CHECK_EQUAL(false, m_pTestPlayer->GetItemHolder()->GetInventory().HasGatherTool(GATHER_LUMBER));

			LearnGatherTalent(10);

			m_pGatherNPCInfo->nGatherType = GATHER_LUMBER;
			m_pIElementInfo->m_nType = IT_GATHER_10;

			bool bRet = m_npcIEChecker.Gather(m_pTestPlayer, m_pTestNPC, m_pIElementInfo->m_nType);
			CHECK_EQUAL(true, bRet);
		}
	}
	
	namespace Failure
	{
		TEST_FIXTURE(Fixture, TestGatheringCheck_Failure_Mine_NoTool)
		{
			CHECK_EQUAL(false, m_pTestPlayer->GetItemHolder()->GetInventory().HasGatherTool(GATHER_MINE));
			LearnGatherTalent(10);

			m_pGatherNPCInfo->nGatherType = GATHER_MINE;
			m_pIElementInfo->m_nType = IT_GATHER_10;

			bool bRet = m_npcIEChecker.Gather(m_pTestPlayer, m_pTestNPC, m_pIElementInfo->m_nType);
			CHECK_EQUAL(false, bRet);
		}

		TEST_FIXTURE(Fixture, TestGatheringCheck_Failure_Mine_LowLicense)
		{
			GiveGatherItem(GATHER_MINE);
			LearnGatherTalent(1);

			m_pGatherNPCInfo->nGatherType = GATHER_MINE;
			m_pIElementInfo->m_nType = IT_GATHER_10;

			bool bRet = m_npcIEChecker.Gather(m_pTestPlayer, m_pTestNPC, m_pIElementInfo->m_nType);
			CHECK_EQUAL(false, bRet);
		}
	}

}