#include "stdafx.h"
#include "FBaseGame.h"
#include "GTestForward.h"
#include "GQObjInteractor.h"
#include "GPlayerQuests.h"
#include "GQuestInfo.h"
#include "GPlayerInteraction.h"
#include "GPlayerQuest.h"
#include "GQObjectiveInfo.h"
#include "GPlayerQObjective.h"


SUITE(QObjInteractor)
{
	struct Fixture : public FBaseGame
	{
		Fixture()
		{
			m_pLink->IgnoreAll();
			m_pLink->AddIgnoreID(MC_QUEST_UPDATE_OBJECTIVE);
			m_pLink->AddIgnoreID(MC_ACTION_DESPAWN);

			m_pNPC = test.npc.SpawnNPC(m_pField);
			m_pQuestInfo = test.quest.NewQuestInfo();			
			int nInteractCount = 5;
			m_pQObejctiveInfo = test.quest.NewInteractQObjectiveInfo(m_pQuestInfo, m_pNPC->GetNPCInfo()->nID, nInteractCount);

			test.quest.GiveNewPlayerQuest(m_pPlayer, m_pQuestInfo->nID);
		}

		GTestMgrWrapper<GQuestInfoMgr>			m_QuestInfoMgr;		
		GQuestInfo*			m_pQuestInfo;		
		GQObjectiveInfo*	m_pQObejctiveInfo;
		GEntityNPC*			m_pNPC;
		GQObjInteractor		m_QObjInteractor;
	};

	TEST_FIXTURE(Fixture, Prepare_NotDoingQuest)
	{
		m_pPlayer->GetQuests().Cancel(m_pQuestInfo->nID);
		CHECK_EQUAL(false, m_QObjInteractor.Prepare(m_pPlayer, m_pNPC, m_pQuestInfo->nID));
	}

	TEST_FIXTURE(Fixture, Prepare)
	{
		m_pLink->ReleaseIgnoreAll();
		CHECK_EQUAL(true, m_QObjInteractor.Prepare(m_pPlayer, m_pNPC, m_pQuestInfo->nID));

		CHECK_EQUAL(1, m_pLink->GetCommandCount());
		CHECK_EQUAL(MC_QUEST_INTERACT_QOBJECTIVE_BEGIN, m_pLink->GetCommand(0).GetID());
		CHECK_EQUAL(m_pPlayer->GetUID(), m_pLink->GetParam<MUID>(0, 0));
		CHECK_EQUAL(m_pNPC->GetUID(), m_pLink->GetParam<MUID>(0, 1));
	}


	TEST_FIXTURE(Fixture, Cancel_NotDoingQuest)
	{
		m_QObjInteractor.Prepare(m_pPlayer, m_pNPC, m_pQuestInfo->nID);
		m_pPlayer->GetQuests().Cancel(m_pQuestInfo->nID);

		CHECK_EQUAL(false, m_QObjInteractor.Cancel(m_pPlayer, m_pQuestInfo->nID));
	}

	TEST_FIXTURE(Fixture, Cancel)
	{		
		m_QObjInteractor.Prepare(m_pPlayer, m_pNPC, m_pQuestInfo->nID);

		m_pLink->ReleaseIgnoreAll();
		CHECK_EQUAL(true, m_QObjInteractor.Cancel(m_pPlayer, m_pQuestInfo->nID));

		CHECK_EQUAL(false, m_pPlayer->GetInteraction().IsInteractQObjectiveRegulatorActive());
		CHECK_EQUAL(1, m_pLink->GetCommandCount());
		CHECK_EQUAL(MC_QUEST_INTERACT_QOBJECTIVE_END, m_pLink->GetCommand(0).GetID());
		CHECK_EQUAL(m_pPlayer->GetUID(), m_pLink->GetParam<MUID>(0, 0));
	}


	TEST_FIXTURE(Fixture, Act_NotDoingQuest)
	{
		vector<int> vecQObjectiveID;
		vecQObjectiveID.push_back(m_pQObejctiveInfo->nID);

		m_QObjInteractor.Prepare(m_pPlayer, m_pNPC, m_pQuestInfo->nID);
		m_pPlayer->GetQuests().Cancel(m_pQuestInfo->nID);
		CHECK_EQUAL(false, m_QObjInteractor.Act(m_pPlayer, m_pNPC, m_pQuestInfo->nID, vecQObjectiveID));
	}

	TEST_FIXTURE(Fixture, Act_Immortal)
	{
		vector<int> vecQObjectiveID;
		vecQObjectiveID.push_back(m_pQObejctiveInfo->nID);

		m_QObjInteractor.Prepare(m_pPlayer, m_pNPC, m_pQuestInfo->nID);
		int nBeforeProgress = m_pPlayer->GetQuests().Get(m_pQuestInfo->nID)->Get(m_pQObejctiveInfo->nID)->GetProgress();

		m_pLink->ReleaseIgnoreAll();		
		CHECK_EQUAL(true, m_QObjInteractor.Act(m_pPlayer, m_pNPC, m_pQuestInfo->nID, vecQObjectiveID));

		int nAfterProgress = m_pPlayer->GetQuests().Get(m_pQuestInfo->nID)->Get(m_pQObejctiveInfo->nID)->GetProgress();

		CHECK_EQUAL(nBeforeProgress+1, nAfterProgress);
		CHECK_EQUAL(false, m_pNPC->IsDeleteMe());

		CHECK_EQUAL(1, m_pLink->GetCommandCount());
		CHECK_EQUAL(MC_QUEST_INTERACT_QOBJECTIVE_END, m_pLink->GetCommand(0).GetID());
		CHECK_EQUAL(m_pPlayer->GetUID(), m_pLink->GetParam<MUID>(0, 0));
	}

	TEST_FIXTURE(Fixture, Act_Mortal)
	{
		vector<int> vecQObjectiveID;
		vecQObjectiveID.push_back(m_pQObejctiveInfo->nID);

		m_QObjInteractor.Prepare(m_pPlayer, m_pNPC, m_pQuestInfo->nID);
		m_pPlayer->GetInteraction().GetSelectedIElementInfo().m_nType = IT_QUEST_INTERACT_MORTAL;
		int nBeforeProgress = m_pPlayer->GetQuests().Get(m_pQuestInfo->nID)->Get(m_pQObejctiveInfo->nID)->GetProgress();
			
		m_pLink->ReleaseIgnoreAll();
		CHECK_EQUAL(true, m_QObjInteractor.Act(m_pPlayer, m_pNPC, m_pQuestInfo->nID, vecQObjectiveID));

		int nAfterProgress = m_pPlayer->GetQuests().Get(m_pQuestInfo->nID)->Get(m_pQObejctiveInfo->nID)->GetProgress();

		CHECK_EQUAL(nBeforeProgress+1, nAfterProgress);
		CHECK_EQUAL(true, m_pNPC->IsDeleteMe());

		CHECK_EQUAL(1, m_pLink->GetCommandCount());
		CHECK_EQUAL(MC_QUEST_INTERACT_QOBJECTIVE_END, m_pLink->GetCommand(0).GetID());
		CHECK_EQUAL(m_pPlayer->GetUID(), m_pLink->GetParam<MUID>(0, 0));
	}
}