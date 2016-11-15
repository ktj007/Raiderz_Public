#include "StdAfx.h"
#include "GTestWrapper.h"
#include "SUnitTestUtil.h"
#include "GUTHelper.h"
#include "GTestForward.h"
#include "FBasePlayer.h"
#include "GChallengerQuestMgr.h"
#include "FBaseMockLink.h"
#include "GPlayerQuests.h"
#include "GQuestInfo.h"
#include "GPlayerInteraction.h"
#include "GQuestSystem.h"
#include "GConditionsInfo.h"
#include "GQuestGiver.h"

class FakeChallengerQuest : public GChallengerQuestMgr
{
public:
	void SetCurrentTime(tm tmFake)
	{
		m_tmFake = tmFake;
	}
private:
	virtual tm GetCurrentTime() override
	{
		return m_tmFake;
	}

private:
	tm m_tmFake;
};

SUITE(ChallengerQuest)
{
	struct FCQ : public FBasePlayer, public FBaseMockLink
	{
		FCQ()
		{ 
			m_pField = GUTHelper_Field::DefaultMockField();
		}

		~FCQ() 
		{
			m_pField->Destroy();
		}

		void AddTier(int nDailyQuestCount, vector<int>& vecQuests)
		{
			GChallengerQuestMgr::Tier tier;
			tier.nDailyQuestCount = nDailyQuestCount;
			for each (int nQuestID in vecQuests)
			{
				tier.vecSourceQuests.push_back(test.quest.NewQuestInfo(nQuestID));
			}
			gmgr.pChallengerQuestMgr->m_vecSourceTiers.push_back(tier);
		}

		vector<const GQuestInfo*> GetDailyQuests()
		{
			return gmgr.pChallengerQuestMgr->m_vecDailyQuests;
		}
		
		void InsertTestTiers()
		{
			{
				vector<int> v;
				v.push_back(111);
				v.push_back(222);
				v.push_back(333);
				v.push_back(444);
				AddTier(2, v);
			}
			{
				vector<int> v;
				v.push_back(555);
				v.push_back(666);
				v.push_back(777);
				AddTier(3, v);
			}

			{
				vector<int> v;
				v.push_back(888);
				AddTier(2, v);
			}
		}

		void OnQuestExpired()
		{
			gmgr.pChallengerQuestMgr->OnQuestExpired();
		}

		void AddDailyQuest(int nQuestID)
		{
			GQuestInfo* pInfo = gmgr.pQuestInfoMgr->Get(nQuestID);
			VALID(pInfo);
			gmgr.pChallengerQuestMgr->m_vecDailyQuests.push_back(pInfo);
		}

		GChallengerQuestMgr::Tier& GetTierData(int nTier)
		{
			return gmgr.pChallengerQuestMgr->m_vecSourceTiers[nTier];
		}

		void QuestInfoCooking()
		{
			gmgr.pChallengerQuestMgr->Cooking();
		}

		MockField*		m_pField;
		GTestMgrWrapper2<GChallengerQuestMgr, FakeChallengerQuest>	__cq_mgr;
		DECLWRAPPER_QuestInfoMgr;
	};

	TEST_FIXTURE(FCQ, ResetDailyQuest)
	{
		InsertTestTiers();

		gmgr.pChallengerQuestMgr->SetRandomSeed(89491);
		gmgr.pChallengerQuestMgr->ResetDailyQuest();
		vector<const GQuestInfo*> d1 = GetDailyQuests();
		gmgr.pChallengerQuestMgr->ResetDailyQuest();
		vector<const GQuestInfo*> d2 = GetDailyQuests();
		gmgr.pChallengerQuestMgr->ResetDailyQuest();
		vector<const GQuestInfo*> d3 = GetDailyQuests();

		CHECK(d1 == d2);
		CHECK(d2 == d3);
		CHECK(d3 == d1);
		TVALID_EQ(6, d1.size());
		CHECK_EQUAL(333, d1[0]->nID);
		CHECK_EQUAL(111, d1[1]->nID);
		CHECK_EQUAL(777, d1[3]->nID);
		CHECK_EQUAL(666, d1[2]->nID);
		CHECK_EQUAL(555, d1[4]->nID);
		CHECK_EQUAL(888, d1[5]->nID);
	}

	TEST_FIXTURE(FCQ, OnQuestExpired)
	{
		InsertTestTiers();

		tm tmCur;
		tmCur.tm_mday = 17;
		tmCur.tm_hour = GConst::CQ_EXPIRED_HOUR;

		gmgr.pChallengerQuestMgr->SetRandomSeed(89491);

		__cq_mgr.Get()->SetCurrentTime(tmCur);
		gmgr.pChallengerQuestMgr->ResetDailyQuest();
		vector<const GQuestInfo*> d1 = GetDailyQuests();

		gmgr.pChallengerQuestMgr->SetRandomSeed(89492);

		tmCur.tm_mday = 18;
		__cq_mgr.Get()->SetCurrentTime(tmCur);
		gmgr.pChallengerQuestMgr->Update(600.1f); // 10분 주기
		vector<const GQuestInfo*> d2 = GetDailyQuests();

		CHECK(d1 != d2);
	}

	TEST_FIXTURE(FCQ, RouteAllCancelQuest)
	{
		GEntityPlayer* pPlayer = NewEntityPlayer(m_pField, vec3::ZERO);
		GEntityPlayer* pPlayer2 = NewEntityPlayer(m_pField, vec3::ZERO);
		
		test.quest.NewQuestInfo(725);
		test.quest.NewQuestInfo(333);
		pPlayer->GetQuests().Insert(0, 725, L"", L""); // 일반퀘스트 얻기
		pPlayer2->GetQuests().Insert(0, 333, L"", L""); // 도전자퀘스트 얻기

		MockLink* pLink = NewLink(pPlayer);
		MockLink* pLink2 = NewLink(pPlayer2);
		
		InsertTestTiers();
		AddDailyQuest(333); // 오늘의 도전자 퀘스트는 333
		OnQuestExpired();

		TVALID_EQ(0, pLink->GetCommandCount());
		TVALID_EQ(1, pLink2->GetCommandCount());
		CHECK_EQUAL(MC_QUEST_FAIL, pLink2->GetCommand(0).GetID());
		TVALID_EQ(1, pLink2->GetCommand(0).GetParameterCount());
		int nQuestID = 0;
		pLink2->GetCommand(0).GetParameter(&nQuestID, 0, MPT_INT);
		CHECK_EQUAL(333, nQuestID);
	}

	TEST_FIXTURE(FCQ, CQ_Cooking_Level)
	{
		InsertTestTiers();
		GetTierData(0).nMinLevel = 12;
		GetTierData(0).nMaxLevel = 27;
		GetTierData(1).nMinLevel = 35;
		GetTierData(1).nMaxLevel = 44;
		
		test.quest.NewQuestInfo(111);
		test.quest.NewQuestInfo(777);
		QuestInfoCooking();
		GQuestInfo* pQuest111 = gmgr.pQuestInfoMgr->Get(111);
		GQuestInfo* pQuest777 = gmgr.pQuestInfoMgr->Get(777);
		TVALID(pQuest111);
		TVALID(pQuest777);
		CHECK_EQUAL(12, pQuest111->nRequiredMinLevelForChallengerQuest);
		CHECK_EQUAL(27, pQuest111->nRequiredMaxLevelForChallengerQuest);
		CHECK_EQUAL(35, pQuest777->nRequiredMinLevelForChallengerQuest);
		CHECK_EQUAL(44, pQuest777->nRequiredMaxLevelForChallengerQuest);
	}

	TEST_FIXTURE(FCQ, CQ_Cooking_FieldFlag)
	{
		const int TEST_FIELD_GROUP_ID = 10000;
		const int TEST_QUEST_ID = 111;

		GFieldInfo* pFieldInfo = test.field.NewFieldInfo();
		FIELD_GROUP_INFO* pFieldGroup = test.field.NewFieldGroupInfo(pFieldInfo, TEST_FIELD_GROUP_ID);
		pFieldGroup->setRunningQuest.insert(TEST_QUEST_ID);

		InsertTestTiers();
		GQuestInfo* pQuest = gmgr.pQuestInfoMgr->Get(TEST_QUEST_ID);
		pQuest->nTrialFieldGroupID = pFieldGroup->nID;
		CHECK_EQUAL(false, pFieldGroup->bChallengerQuest);
		QuestInfoCooking();
		CHECK_EQUAL(true, pFieldGroup->bChallengerQuest);
	}

	TEST_FIXTURE(FCQ, CQ_Cooking_QuestCancelableFlag)
	{
		const int TEST_FIELD_GROUP_ID = 10000;
		const int TEST_QUEST_ID = 111;

		InsertTestTiers();
		GQuestInfo* pQuest = gmgr.pQuestInfoMgr->Get(TEST_QUEST_ID);
		CHECK_EQUAL(true, pQuest->bCancelable);
		QuestInfoCooking();
		CHECK_EQUAL(false, pQuest->bCancelable);
	}

	TEST_FIXTURE(FCQ, GiveQuest_InvalidLevelForChallengeQuest)
	{
		MockField*		m_pCommonField = GUTHelper_Field::DefaultMockField();
		GEntityPlayer*	m_pPlayer = GUTHelper::NewEntityPlayer(m_pCommonField);
		m_pPlayer->GetInteraction().GetSelectedIElementInfo().m_nType = IT_CHALLENGERQUEST;

		GQuestInfo*		m_pQuestInfo = test.quest.NewQuestInfo(789);
		m_pQuestInfo->bChallengerQuest = true;
		m_pQuestInfo->nRequiredMinLevelForChallengerQuest = 11;
		m_pQuestInfo->nRequiredMaxLevelForChallengerQuest = 22;

		AddDailyQuest(m_pQuestInfo->nID);

		m_pPlayer->GetPlayerInfo()->nLevel = 5;
		CHECK_EQUAL(false, gsys.pQuestSystem->GetGiver().Give(m_pPlayer, m_pQuestInfo->nID));
		m_pPlayer->GetPlayerInfo()->nLevel = 15;
		CHECK_EQUAL(true, gsys.pQuestSystem->GetGiver().Give(m_pPlayer, m_pQuestInfo->nID));
		m_pPlayer->GetPlayerInfo()->nLevel = 25;
		CHECK_EQUAL(false, gsys.pQuestSystem->GetGiver().Give(m_pPlayer, m_pQuestInfo->nID));
	}

	TEST_FIXTURE(FCQ, GiveQuest_InvalidConditionIDForChallengeQuest)
	{
		GUTHelper		m_Helper;
		GConditionsInfo* pConditionsInfo = m_Helper.NewConditionsInfoForLevel(11, 22);

		MockField*		m_pCommonField = GUTHelper_Field::DefaultMockField();
		GEntityPlayer*	m_pPlayer = GUTHelper::NewEntityPlayer(m_pCommonField);
		m_pPlayer->GetInteraction().GetSelectedIElementInfo().m_nType = IT_CHALLENGERQUEST;

		GQuestInfo*		m_pQuestInfo = test.quest.NewQuestInfo(789);
		m_pQuestInfo->bChallengerQuest = true;
		m_pQuestInfo->nRequiredConditionIDForChallengerQuest = pConditionsInfo->m_nID;

		AddDailyQuest(m_pQuestInfo->nID);

		m_pPlayer->GetPlayerInfo()->nLevel = 5;
		CHECK_EQUAL(false, gsys.pQuestSystem->GetGiver().Give(m_pPlayer, m_pQuestInfo->nID));
		m_pPlayer->GetPlayerInfo()->nLevel = 15;
		CHECK_EQUAL(true, gsys.pQuestSystem->GetGiver().Give(m_pPlayer, m_pQuestInfo->nID));
		m_pPlayer->GetPlayerInfo()->nLevel = 25;
		CHECK_EQUAL(false, gsys.pQuestSystem->GetGiver().Give(m_pPlayer, m_pQuestInfo->nID));
	}

	TEST_FIXTURE(FCQ, GiveQuest_IsTodayQuest)
	{
		MockField*		m_pCommonField = GUTHelper_Field::DefaultMockField();
		GEntityPlayer*	m_pPlayer = GUTHelper::NewEntityPlayer(m_pCommonField);
		m_pPlayer->GetInteraction().GetSelectedIElementInfo().m_nType = IT_CHALLENGERQUEST;

		GQuestInfo*		m_pQuestInfo = test.quest.NewQuestInfo(789);
		m_pQuestInfo->bChallengerQuest = true;
		
		CHECK_EQUAL(false, gsys.pQuestSystem->GetGiver().Give(m_pPlayer, m_pQuestInfo->nID));
		AddDailyQuest(m_pQuestInfo->nID);
		CHECK_EQUAL(true, gsys.pQuestSystem->GetGiver().Give(m_pPlayer, m_pQuestInfo->nID));
	}

}