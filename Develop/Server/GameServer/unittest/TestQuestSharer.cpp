#include "stdafx.h"
#include "FBaseGame.h"
#include "GQuestSharer.h"
#include "GTestForward.h"
#include "GQuestInfo.h"
#include "GQuestSystem.h"
#include "GQuestCanceler.h"
#include "GConditionsInfo.h"


SUITE(QuestSharer)
{
	struct Fixture : public FBaseGame
	{
		Fixture()
		{
			m_pQuestInfo = test.quest.NewQuestInfo();
			m_pQuestInfo->nShareType = QSHAT_MANUAL;
			test.quest.GiveNewPlayerQuest(m_pPlayer, m_pQuestInfo->nID);

			m_pShareTarget = test.player.NewPlayer(m_pField);
			test.party.CreateParty(m_pPlayer, m_pShareTarget);

			m_pLink->ResetCommands();
		}

		GTestMgrWrapper<GQuestInfoMgr>	m_QuestInfoMgr;		
		GQuestInfo*			m_pQuestInfo;
		GQuestSharer		m_QuestSharer;

		GEntityPlayer*		m_pShareTarget;
	};

	TEST_FIXTURE(Fixture, NotDoingQuest)
	{
		gsys.pQuestSystem->GetCanceler().Cancel(m_pPlayer, m_pQuestInfo->nID);

		CHECK(false == gsys.pQuestSystem->GetSharer().Share(m_pPlayer, m_pQuestInfo->nID));
	}

	TEST_FIXTURE(Fixture, SelectShareResult_Distance)
	{
		vec3 vPos = m_pPlayer->GetPos();
		vPos.x += GConst::QUEST_SHARE_DISTANCE + 1;
		m_pShareTarget->SetPos(vPos);
		
		CHECK_EQUAL(QSR_FAIL_DISTANCE, gsys.pQuestSystem->GetSharer().SelectShareResult(m_pPlayer, m_pShareTarget, m_pQuestInfo));
	}

	TEST_FIXTURE(Fixture, SelectShareResult_Condition)
	{
		GConditionsInfo* pConditionsInfo = test.condition.NewCondI(test.condition.NewCondLForLevel(10, 10));
		m_pQuestInfo->nShareConditionsID = pConditionsInfo->m_nID;
		m_pShareTarget->GetPlayerInfo()->nLevel = 3;

		CHECK_EQUAL(QSR_FAIL_CONDITION, gsys.pQuestSystem->GetSharer().SelectShareResult(m_pPlayer, m_pShareTarget, m_pQuestInfo));
	}

	TEST_FIXTURE(Fixture, SelectShareResult_Exist)
	{
		test.quest.GiveNewPlayerQuest(m_pShareTarget, m_pQuestInfo->nID);

		CHECK_EQUAL(QSR_FAIL_EXIST, gsys.pQuestSystem->GetSharer().SelectShareResult(m_pPlayer, m_pShareTarget, m_pQuestInfo));
	}


	TEST_FIXTURE(Fixture, SelectShareResult_Done)
	{
		test.quest.GiveNewPlayerQuest(m_pShareTarget, m_pQuestInfo->nID);
		test.quest.DoneQuest(m_pShareTarget, m_pQuestInfo->nID);

		CHECK_EQUAL(QSR_FAIL_DONE, gsys.pQuestSystem->GetSharer().SelectShareResult(m_pPlayer, m_pShareTarget, m_pQuestInfo));
	}

	TEST_FIXTURE(Fixture, Route)
	{
		MockLink* pShareTargetLink = test.network.NewLink(m_pShareTarget);

		CHECK(true == gsys.pQuestSystem->GetSharer().Share(m_pPlayer, m_pQuestInfo->nID));

		vector<TD_QUEST_SHARE_RESULT> vecResult;
		CHECK_EQUAL(1, m_pLink->GetCommandCount());
		CHECK_EQUAL(MC_QUEST_SHARE, m_pLink->GetCommand(0).GetID());
		m_pLink->GetCommand(0).GetBlob(vecResult, 0);
		CHECK(TD_QUEST_SHARE_RESULT(m_pShareTarget->GetUID(), m_pQuestInfo->nID, QSR_SUCCESS) ==  vecResult.front());

		TD_QUEST_SHARE_RESULT result;
		CHECK_EQUAL(1, pShareTargetLink->GetCommandCount());
		CHECK_EQUAL(MC_QUEST_SHARE_REQ_RELAY, pShareTargetLink->GetCommand(0).GetID());
		pShareTargetLink->GetCommand(0).GetSingleBlob(result, 0);
		CHECK(TD_QUEST_SHARE_RESULT(m_pPlayer->GetUID(), m_pQuestInfo->nID, QSR_SUCCESS) ==  result);
	}
}