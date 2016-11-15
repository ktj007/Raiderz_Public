#include "stdafx.h"
#include "GTestForward.h"
#include "GDialogSystem.h"
#include "GNPCIEApplier.h"
#include "GPlayerQuest.h"
#include "GDialogInfo.h"
#include "GQuestInfo.h"

using testing::Return;

SUITE(NPCIEApplier)
{
	class MockDialogSystem : public GDialogSystem
	{
	public:			
		MOCK_METHOD2(Start, bool(GEntityPlayer* pPlayer, int nDialogID));
	};

	struct Fixture
	{
		Fixture()
		{
			m_pField = GUTHelper_Field::DefaultMockField();
			m_pPlayer = test.player.NewPlayer(m_pField);
			m_pNPC = test.npc.SpawnNPC(m_pField);
			m_pQuestInfo = test.quest.NewQuestInfo();
			m_pIEInfo = test.interaction.GiveNewIElementInfoForClick(m_pNPC->GetNPCInfo(), IT_QUEST_END, m_pQuestInfo->nID);
		}
		~Fixture()
		{

		}

		GNPCIEApplier		m_NPCIEApplier;
		MockField*			m_pField;
		GEntityPlayer*		m_pPlayer;
		GEntityNPC*			m_pNPC;
		GNPCInteractionElementInfo* m_pIEInfo;
		GQuestInfo*			m_pQuestInfo;
	};

	// 퀘스트를 완료했는대, 퀘스트 보상 다이얼로그 정보가 없을때
	TEST_FIXTURE(Fixture, QuestEnd_RewardDialogInfoIsNotExist)
	{
		GPlayerQuest* pPlayerQuest = test.quest.GiveNewPlayerQuest(m_pPlayer, m_pQuestInfo->nID);
		pPlayerQuest->Complete();
		pPlayerQuest->GetInfo()->nRewardDialogID = 0;		

		CHECK_EQUAL(false, m_NPCIEApplier.Apply(m_pPlayer, m_pIEInfo));
	}

	// 퀘스트를 완료했는대, 퀘스트 보상 다이얼로그 정보가 있을때
	TEST_FIXTURE(Fixture, QuestEnd_RewardDialogInfoIsExist)
	{
		GTestSysWrapper2<GDialogSystem, MockDialogSystem> mockDialogSystem;		

		GPlayerQuest* pPlayerQuest = test.quest.GiveNewPlayerQuest(m_pPlayer, m_pQuestInfo->nID);
		pPlayerQuest->Complete();
		pPlayerQuest->GetInfo()->nRewardDialogID = test.dialog.NewDialogInfo()->m_nID;

		EXPECT_CALL(*mockDialogSystem.Get(), Start(m_pPlayer, pPlayerQuest->GetInfo()->nRewardDialogID))
			.WillRepeatedly(Return(true));

		m_NPCIEApplier.Apply(m_pPlayer, m_pIEInfo);	
	}

	// 퀘스트를 미완료했는대, 미완료 다이얼로그 정보가 없을때
	TEST_FIXTURE(Fixture, QuestEnd_IncompleteDialogInfoIsNotExist)
	{
		GPlayerQuest* pPlayerQuest = test.quest.GiveNewPlayerQuest(m_pPlayer, m_pQuestInfo->nID);
		pPlayerQuest->GetInfo()->nIncompleteDialogID = 0;

		CHECK_EQUAL(false, m_NPCIEApplier.Apply(m_pPlayer, m_pIEInfo));
	}

	// 퀘스트를 미완료했는대, 미완료 다이얼로그 정보가 있을때
	TEST_FIXTURE(Fixture, QuestEnd_IncompleteDialogInfoIsExist)
	{
		GTestSysWrapper2<GDialogSystem, MockDialogSystem> mockDialogSystem;

		GPlayerQuest* pPlayerQuest = test.quest.GiveNewPlayerQuest(m_pPlayer, m_pQuestInfo->nID);
		pPlayerQuest->GetInfo()->nIncompleteDialogID = test.dialog.NewDialogInfo()->m_nID;

		EXPECT_CALL(*mockDialogSystem.Get(), Start(m_pPlayer, pPlayerQuest->GetInfo()->nIncompleteDialogID))
			.WillRepeatedly(Return(true));

		m_NPCIEApplier.Apply(m_pPlayer, m_pIEInfo);
	}
}
