#include "stdafx.h"
#include "FBaseGame.h"
#include "GQObjUpdater.h"
#include "GTestForward.h"
#include "GQuestInfo.h"
#include "GQObjectiveInfo.h"
#include "GPlayerQuests.h"
#include "GPlayerQObjective.h"
#include "GPlayerQuest.h"
#include "GQuestSystem.h"
#include "GHateTable.h"


SUITE(QObjUpdater)
{
	struct Fixture : public FBaseGame
	{
		Fixture()
		{
			m_pQuestInfo = test.quest.NewQuestInfo();						
		}

		GTestMgrWrapper<GQuestInfoMgr>	m_QuestInfoMgr;		
		GQuestInfo*			m_pQuestInfo;
		GQObjUpdater		m_QObjUpdater;
	};

	TEST_FIXTURE(Fixture, IncCollectQObj)
	{
		int nAmount = 5;
		GQObjectiveInfo* pQObejctiveInfo = test.quest.NewCollectQObjectiveInfo(m_pQuestInfo, nAmount);
		int nItemID = pQObejctiveInfo->vecParam1.front();
		test.quest.GiveNewPlayerQuest(m_pPlayer, m_pQuestInfo->nID);

		CHECK(true == m_QObjUpdater.IncCollectQObj(m_pPlayer, nItemID, nAmount));

		GPlayerQuest* pPlayerQuest = m_pPlayer->GetQuests().Get(m_pQuestInfo->nID);
		ASSERT_CHECK(pPlayerQuest);
		GPlayerQObjective* pPlayerQObjective = pPlayerQuest->Get(pQObejctiveInfo->nID);
		ASSERT_CHECK(pPlayerQObjective);

		CHECK_EQUAL(nAmount, pPlayerQObjective->GetProgress());
		CHECK(true == pPlayerQObjective->IsComplete());
	}

	TEST_FIXTURE(Fixture, DecCollectQObj)
	{
		int nAmount = 5;
		GQObjectiveInfo* pQObejctiveInfo = test.quest.NewCollectQObjectiveInfo(m_pQuestInfo, nAmount);
		int nItemID = pQObejctiveInfo->vecParam1.front();
		test.quest.GiveNewPlayerQuest(m_pPlayer, m_pQuestInfo->nID);

		int nIncAmount = 3;
		m_QObjUpdater.IncCollectQObj(m_pPlayer, nItemID, nIncAmount);

		int nDecAmount = 1;
		CHECK(true == m_QObjUpdater.DecCollectQObj(m_pPlayer, nItemID, nDecAmount));

		GPlayerQuest* pPlayerQuest = m_pPlayer->GetQuests().Get(m_pQuestInfo->nID);
		ASSERT_CHECK(pPlayerQuest);
		GPlayerQObjective* pPlayerQObjective = pPlayerQuest->Get(pQObejctiveInfo->nID);
		ASSERT_CHECK(pPlayerQObjective);

		CHECK_EQUAL(nIncAmount - nDecAmount, pPlayerQObjective->GetProgress());
		CHECK(false == pPlayerQObjective->IsComplete());
	}

	TEST_FIXTURE(Fixture, UpdDestroyQObj)
	{
		int nAmount = 5;
		GQObjectiveInfo* pQObejctiveInfo = test.quest.NewDestroyQObjectiveInfo(m_pQuestInfo, nAmount);
		int nNPCID = pQObejctiveInfo->vecParam1.front();
		GEntityNPC* pNPC = test.npc.SpawnNPC(m_pField, nNPCID);
		test.quest.GiveNewPlayerQuest(m_pPlayer, m_pQuestInfo->nID);

		CHECK(true == gsys.pQuestSystem->GetQObjUpdater().UpdDestroyQObj(m_pPlayer, nNPCID, nAmount, pNPC));

		GPlayerQuest* pPlayerQuest = m_pPlayer->GetQuests().Get(m_pQuestInfo->nID);
		ASSERT_CHECK(pPlayerQuest);
		GPlayerQObjective* pPlayerQObjective = pPlayerQuest->Get(pQObejctiveInfo->nID);
		ASSERT_CHECK(pPlayerQObjective);

		CHECK_EQUAL(nAmount, pPlayerQObjective->GetProgress());
		CHECK(true == pPlayerQObjective->IsComplete());
	}

	TEST_FIXTURE(Fixture, UpdScoutQObj)
	{
		int nSensorID = 2;
		GQObjectiveInfo* pQObejctiveInfo = test.quest.NewScoutQObjectiveInfo(m_pQuestInfo, nSensorID);
		int nFieldID = pQObejctiveInfo->vecParam1.front();
		test.quest.GiveNewPlayerQuest(m_pPlayer, m_pQuestInfo->nID);

		CHECK(true == gsys.pQuestSystem->GetQObjUpdater().UpdScoutQObj(m_pPlayer, nFieldID, nSensorID));

		GPlayerQuest* pPlayerQuest = m_pPlayer->GetQuests().Get(m_pQuestInfo->nID);
		ASSERT_CHECK(pPlayerQuest);
		GPlayerQObjective* pPlayerQObjective = pPlayerQuest->Get(pQObejctiveInfo->nID);
		ASSERT_CHECK(pPlayerQObjective);

		CHECK(true == pPlayerQObjective->IsComplete());
	}


	struct FPublicProgress : public Fixture
	{
		FPublicProgress()
		{
			m_nDestroyNPCAmount = 5;
			m_pQObejctiveInfo = test.quest.NewDestroyQObjectiveInfo(m_pQuestInfo, m_nDestroyNPCAmount);
			m_nDestroyNPCID = m_pQObejctiveInfo->vecParam1.front();

			m_pMember = test.player.NewPlayer(m_pField);
			m_pEqualSectorPlayer = test.player.NewPlayer(m_pField);		
			test.party.CreateParty(m_pPlayer, m_pMember);

			m_pNPC = test.npc.SpawnNPC(m_pField, m_nDestroyNPCID);
			
			test.quest.GiveNewPlayerQuest(m_pPlayer, m_pQuestInfo->nID);
			test.quest.GiveNewPlayerQuest(m_pMember, m_pQuestInfo->nID);
			test.quest.GiveNewPlayerQuest(m_pEqualSectorPlayer, m_pQuestInfo->nID);
		}

		GEntityPlayer*		m_pMember;
		GEntityPlayer*		m_pEqualSectorPlayer;
		GEntityNPC*			m_pNPC;
		GQObjectiveInfo*	m_pQObejctiveInfo;
		int					m_nDestroyNPCID;
		int					m_nDestroyNPCAmount;
	};

	TEST_FIXTURE(FPublicProgress, PublicProgress_Self)
	{
		m_pQObejctiveInfo->nPublicProgress = QPP_SELF;

		CHECK(true == gsys.pQuestSystem->GetQObjUpdater().UpdDestroyQObj(m_pPlayer, m_nDestroyNPCID, m_nDestroyNPCAmount, m_pNPC));

		CHECK_EQUAL(true, m_pPlayer->GetQuests().IsCompleteQuest(m_pQuestInfo->nID));
		CHECK_EQUAL(false, m_pMember->GetQuests().IsCompleteQuest(m_pQuestInfo->nID));
		CHECK_EQUAL(false, m_pEqualSectorPlayer->GetQuests().IsCompleteQuest(m_pQuestInfo->nID));
	}

	TEST_FIXTURE(FPublicProgress, PublicProgress_Party)
	{
		m_pQObejctiveInfo->nPublicProgress = QPP_PARTY;

		CHECK(true == gsys.pQuestSystem->GetQObjUpdater().UpdDestroyQObj(m_pPlayer, m_nDestroyNPCID, m_nDestroyNPCAmount, m_pNPC));

		CHECK_EQUAL(true, m_pPlayer->GetQuests().IsCompleteQuest(m_pQuestInfo->nID));
		CHECK_EQUAL(true, m_pMember->GetQuests().IsCompleteQuest(m_pQuestInfo->nID));
		CHECK_EQUAL(false, m_pEqualSectorPlayer->GetQuests().IsCompleteQuest(m_pQuestInfo->nID));
	}

	TEST_FIXTURE(FPublicProgress, PublicProgress_Sector)
	{
		m_pQObejctiveInfo->nPublicProgress = QPP_SECTOR;
		m_pNPC->GetHateTable().AddPoint_FoundEnemy(m_pPlayer);
		m_pNPC->GetHateTable().AddPoint_FoundEnemy(m_pMember);
		m_pNPC->GetHateTable().AddPoint_FoundEnemy(m_pEqualSectorPlayer);

		CHECK(true == gsys.pQuestSystem->GetQObjUpdater().UpdDestroyQObj(m_pPlayer, m_nDestroyNPCID, m_nDestroyNPCAmount, m_pNPC));

		CHECK_EQUAL(true, m_pPlayer->GetQuests().Get(m_pQuestInfo->nID)->IsComplete());
		CHECK_EQUAL(true, m_pMember->GetQuests().Get(m_pQuestInfo->nID)->IsComplete());
		CHECK_EQUAL(true, m_pEqualSectorPlayer->GetQuests().Get(m_pQuestInfo->nID)->IsComplete());
	}
}