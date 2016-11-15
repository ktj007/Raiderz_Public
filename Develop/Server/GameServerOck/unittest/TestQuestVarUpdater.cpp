#include "StdAfx.h"
#include "FBaseGame.h"
#include "GTestForward.h"
#include "GQObjectiveInfo.h"
#include "GQuestInfo.h"
#include "GQuestSystem.h"
#include "GQuestVarUpdater.h"
#include "GPlayerQuests.h"
#include "GPlayerQObjective.h"
#include "GPlayerQuest.h"


SUITE(QuestVarUpdater)
{
	struct Fixture : public FBaseGame
	{
		Fixture()
		{
			m_pQuestInfo = test.quest.NewQuestInfo();
			m_pQObejctiveInfo = test.quest.NewQObjectiveInfo(m_pQuestInfo);
			m_pQObejctiveInfo->nType = QOT_ACT;
			m_pQObejctiveInfo->vecParam1.push_back(5);

			test.quest.GiveNewPlayerQuest(m_pPlayer, m_pQuestInfo->nID);
		}

		GTestMgrWrapper<GQuestInfoMgr>			m_QuestInfoMgr;		
		GQuestInfo*			m_pQuestInfo;
		CSQObjectiveInfo*	m_pQObejctiveInfo;
		GQuestVarUpdater	m_QuestVarUpdater;
	};


	TEST_FIXTURE(Fixture, UpdateQuestVar)
	{			
		CHECK(true == m_QuestVarUpdater.Update(m_pPlayer, m_pQuestInfo->nID, m_pQObejctiveInfo->vecParam1[0] +1));

		GPlayerQuest* pPlayerQuest = m_pPlayer->GetQuests().Get(m_pQuestInfo->nID);
		VALID(pPlayerQuest);
		GPlayerQObjective* pPlayerQObjective = pPlayerQuest->Get(m_pQObejctiveInfo->nID);
		VALID(pPlayerQObjective);

		CHECK_EQUAL(m_pQObejctiveInfo->vecParam1[0] +1, pPlayerQuest->GetQuestVar());
		CHECK(false == pPlayerQObjective->IsComplete());
	}		

	TEST_FIXTURE(Fixture, QuestComplete)
	{
		CHECK(true == m_QuestVarUpdater.Update(m_pPlayer, m_pQuestInfo->nID, m_pQObejctiveInfo->vecParam1[0]));

		GPlayerQuest* pPlayerQuest = m_pPlayer->GetQuests().Get(m_pQuestInfo->nID);
		VALID(pPlayerQuest);
		GPlayerQObjective* pPlayerQObjective = pPlayerQuest->Get(m_pQObejctiveInfo->nID);			
		VALID(pPlayerQObjective);

		CHECK_EQUAL(m_pQObejctiveInfo->vecParam1[0], pPlayerQuest->GetQuestVar());
		CHECK(true == pPlayerQObjective->IsComplete());
	}

	TEST_FIXTURE(Fixture, UpdateQuestPublicVar)
	{
		m_pQuestInfo->bPublicVar = true;
		m_pQObejctiveInfo->nPublicProgress = QPP_PARTY;
		GEntityPlayer* pPartyMember = test.player.NewPlayer(m_pField);
		GParty* pParty = test.party.CreateParty(m_pPlayer, pPartyMember);
		test.quest.GiveNewPlayerQuest(pPartyMember, m_pQuestInfo->nID);

		CHECK(true == m_QuestVarUpdater.Update(pPartyMember, m_pQuestInfo->nID, m_pQObejctiveInfo->vecParam1[0]));

		GPlayerQuest* pPlayerQuest = m_pPlayer->GetQuests().Get(m_pQuestInfo->nID);
		VALID(pPlayerQuest);
		GPlayerQObjective* pPlayerQObjective = pPlayerQuest->Get(m_pQObejctiveInfo->nID);
		VALID(pPlayerQObjective);

		CHECK_EQUAL(m_pQObejctiveInfo->vecParam1[0], pPlayerQuest->GetQuestVar());
		CHECK(true == pPlayerQObjective->IsComplete());
	}

}