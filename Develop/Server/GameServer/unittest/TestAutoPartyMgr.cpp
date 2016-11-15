#include "stdafx.h"
#include "GTestForward.h"
#include "GAutoPartyMgr.h"
#include "GQuestInfo.h"
#include "FBaseGame.h"
#include "GPartySystem.h"
#include "GSharedField.h"
#include "GStandAloneModePartyRouter.h"
#include "GPlayerRole.h"
#include "GPartyManager.h"
#include "GPlayerAFK.h"

SUITE(AutoPartyMgr)
{
	struct Fixture : public FBaseGame
	{
		Fixture()
		{
			FieldID nFieldID = 777;
			m_pField->SetFieldInfo(test.field.NewFieldInfo(nFieldID));
		}

		~Fixture()
		{
			GAutoPartyMgr::ResetStatics();
			gsys.pPartySystem->GetPartyManager()->Clear();
		}

		void MakeHealer(GEntityPlayer* pPlayer)
		{
			// 힐 탤런트를 배움
			GTalentInfo* pNewTalent = test.talent.NewTalentInfo();
			pNewTalent->m_EffectInfo.m_fRadius = 1000;
			pNewTalent->m_nMaxHeal = 1;
			pNewTalent->m_nRank = 1;
			pNewTalent->Cooking();
			test.talent.Train(pPlayer, pNewTalent);
			pNewTalent->m_nTNeedStyleTP = 1;
		}

		void LearnHealTalent(GEntityPlayer* pPlayer, int nTalentNeedLevel)
		{
			GTalentInfo* pNewTalent = test.talent.NewTalentInfo();
			pNewTalent->m_EffectInfo.m_fRadius = 1000;
			pNewTalent->m_nMaxHeal = 1;
			pNewTalent->m_nRank = 1;
			static int sTalentLine = 111;
			pNewTalent->m_nTalentLine = ++sTalentLine;
			pNewTalent->Cooking();
			test.talent.Train(pPlayer, pNewTalent);
			pNewTalent->m_nTNeedStyleTP = nTalentNeedLevel;
		}

		void LeaveParty(GEntityPlayer* pLeaver)
		{
			// 한명이 파티를 탈퇴
			GParty* pParty = GetPartySys()->FindParty(pLeaver->GetPartyUID());
			if (!pParty)
				return;
			pParty->RemoveMember(pLeaver->GetUID());
			pLeaver->SetParty(MUID::Invalid());
		}

		DECLWRAPPER_FieldInfoMgr;
		DECLWRAPPER_QuestInfoMgr;
	};

	TEST_FIXTURE(Fixture, Cooking)
	{
		QuestID nQuestID = 888;
		GQuestInfo* pQuestInfo1 = test.quest.NewQuestInfo(nQuestID);
		pQuestInfo1->nAutoPartyMemberLimit = 1;
		GAutoPartyMgr::Cooking();

		CHECK_EQUAL(1, GAutoPartyMgr::GetPartyMemberLimit(nQuestID));
		CHECK_EQUAL(MAX_PARTY_MEMBER_COUNT, GAutoPartyMgr::GetPartyMemberLimit(nQuestID+1));
	}

	TEST_FIXTURE(Fixture, EnqueueSingle)
	{
		GAutoPartyMgr party_matcher(m_pField);
		GEntityPlayer* pPlayer1 = NewEntityPlayer(m_pField, vec3::ZERO);
		GEntityPlayer* pPlayer2 = NewEntityPlayer(m_pField, vec3::ZERO);

		QuestID nQuestID1 = 888;
		QuestID nQuestID2 = 889;
		GQuestInfo* pQuestInfo1 = test.quest.NewQuestInfo(nQuestID1);
		GQuestInfo* pQuestInfo2 = test.quest.NewQuestInfo(nQuestID2);

		//// 자동파티가 아닌 퀘스트
		//CHECK_EQUAL(false, party_matcher.Enqueue(nQuestID1, pPlayer1));
		//CHECK_EQUAL(0, party_matcher.GetSingleCount(nQuestID1));
		//CHECK_EQUAL(0, party_matcher.GetSingleCountAll());
		//// 자동파티인 퀘스트
		pQuestInfo1->bAutoParty = true;
		//CHECK_EQUAL(false, party_matcher.Enqueue(nQuestID1, pPlayer1));
		//CHECK_EQUAL(0, party_matcher.GetSingleCount(nQuestID1));
		//CHECK_EQUAL(0, party_matcher.GetSingleCountAll());
		// 퀘스트 소유
		test.quest.GiveNewPlayerQuest(pPlayer1, nQuestID1);
		test.quest.GiveNewPlayerQuest(pPlayer1, nQuestID2);
		test.quest.GiveNewPlayerQuest(pPlayer2, nQuestID1);
		test.quest.GiveNewPlayerQuest(pPlayer2, nQuestID2);
		CHECK_EQUAL(true, party_matcher.Enqueue(nQuestID1, pPlayer1));
		CHECK_EQUAL(1, party_matcher.GetSingleCount(nQuestID1));
		CHECK_EQUAL(0, party_matcher.GetSingleCount(nQuestID2));
		CHECK_EQUAL(1, party_matcher.GetSingleCountAll());
		// 다른 플레이어 진입
		CHECK_EQUAL(true, party_matcher.Enqueue(nQuestID1, pPlayer2));
		CHECK_EQUAL(2, party_matcher.GetSingleCount(nQuestID1));
		CHECK_EQUAL(0, party_matcher.GetSingleCount(nQuestID2));
		CHECK_EQUAL(2, party_matcher.GetSingleCountAll());
		// 다른 퀘스트 진입
		pQuestInfo2->bAutoParty = true;
		CHECK_EQUAL(true, party_matcher.Enqueue(nQuestID2, pPlayer2));
		CHECK_EQUAL(2, party_matcher.GetSingleCount(nQuestID1));
		CHECK_EQUAL(1, party_matcher.GetSingleCount(nQuestID2));
		CHECK_EQUAL(3, party_matcher.GetSingleCountAll());
		// 대기 목록 초기화
		party_matcher.Clear();
		CHECK_EQUAL(0, party_matcher.GetSingleCount(nQuestID1));
		CHECK_EQUAL(0, party_matcher.GetSingleCountAll());
	}

	TEST_FIXTURE(Fixture, EnqueueParty)
	{
		GAutoPartyMgr party_matcher(m_pField);
		GEntityPlayer* pPlayer1 = NewEntityPlayer(m_pField, vec3::ZERO);
		GEntityPlayer* pPlayer2 = NewEntityPlayer(m_pField, vec3::ZERO);

		QuestID nQuestID1 = 888;
		QuestID nQuestID2 = 889;
		GQuestInfo* pQuestInfo1 = test.quest.NewQuestInfo(nQuestID1);
		GQuestInfo* pQuestInfo2 = test.quest.NewQuestInfo(nQuestID2);
		
		GParty* pParty = gsys.pPartySystem->GetPartyManager()->NewParty(SUnitTestUtil::NewUID());
		gsys.pPartySystem->GetPartyManager()->AddParty(pParty);
		TVALID(pParty);
		
		CHECK(pParty->GetUID().Value != MUID::Invalid().Value);

		pPlayer1->SetParty(pParty->GetUID());
		pPlayer2->SetParty(pParty->GetUID());
		pParty->AddMember(pPlayer1->GetUID(), L"", 0);
		pParty->AddMember(pPlayer2->GetUID(), L"", 0);
		pParty->SetLeader(pPlayer1->GetUID());

		//// 자동파티가 아닌 퀘스트
		//CHECK_EQUAL(false, party_matcher.Enqueue(nQuestID1, pPlayer1));
		//CHECK_EQUAL(0, party_matcher.GetPartyCount(nQuestID1));
		//CHECK_EQUAL(0, party_matcher.GetPartyCountAll());
		//// 자동파티인 퀘스트
		pQuestInfo1->bAutoParty = true;
		//CHECK_EQUAL(false, party_matcher.Enqueue(nQuestID1, pPlayer1));
		//CHECK_EQUAL(0, party_matcher.GetPartyCount(nQuestID1));
		//CHECK_EQUAL(0, party_matcher.GetPartyCountAll());
		// 퀘스트 소유
		test.quest.GiveNewPlayerQuest(pPlayer1, nQuestID1);
		test.quest.GiveNewPlayerQuest(pPlayer1, nQuestID2);
		test.quest.GiveNewPlayerQuest(pPlayer2, nQuestID1);
		test.quest.GiveNewPlayerQuest(pPlayer2, nQuestID2);
		CHECK_EQUAL(true, party_matcher.Enqueue(nQuestID1, pPlayer1));
		CHECK_EQUAL(1, party_matcher.GetPartyCount(nQuestID1));
		CHECK_EQUAL(0, party_matcher.GetPartyCount(nQuestID2));
		CHECK_EQUAL(1, party_matcher.GetPartyCountAll());
		// 다른 플레이어 진입
		CHECK_EQUAL(false, party_matcher.Enqueue(nQuestID1, pPlayer2)); // 파티장이 아님
		CHECK_EQUAL(1, party_matcher.GetPartyCount(nQuestID1));
		CHECK_EQUAL(0, party_matcher.GetPartyCount(nQuestID2));
		CHECK_EQUAL(1, party_matcher.GetPartyCountAll());
		// 다른 퀘스트 진입
		pQuestInfo2->bAutoParty = true;
		CHECK_EQUAL(true, party_matcher.Enqueue(nQuestID2, pPlayer1));
		CHECK_EQUAL(1, party_matcher.GetPartyCount(nQuestID1));
		CHECK_EQUAL(1, party_matcher.GetPartyCount(nQuestID2));
		CHECK_EQUAL(2, party_matcher.GetPartyCountAll());
		// 대기 목록 초기화
		party_matcher.Clear();
		CHECK_EQUAL(0, party_matcher.GetPartyCount(nQuestID1));
		CHECK_EQUAL(0, party_matcher.GetPartyCountAll());
	}

	//TEST_FIXTURE(Fixture, EnqueueFailed_HaveNotQuest)
	//{
	//	GAutoPartyMgr party_matcher(m_pField);

	//	QuestID nQuestID = 888;
	//	GQuestInfo* pQuestInfo = test.quest.NewQuestInfo(nQuestID);
	//	pQuestInfo->bAutoParty = true;

	//	CHECK_EQUAL(false, party_matcher.Enqueue(nQuestID, m_pPlayer));
	//	test.quest.GiveNewPlayerQuest(m_pPlayer, nQuestID);
	//	CHECK_EQUAL(true, party_matcher.Enqueue(nQuestID, m_pPlayer));
	//}

	TEST_FIXTURE(Fixture, MakingParty)
	{
		GAutoPartyMgr party_matcher(m_pField);
		m_pMasterServerFacade->SetPartyRouter(new GStandAloneModePartyRouter());

		QuestID nQuestID = 888;
		GQuestInfo* pQuestInfo = test.quest.NewQuestInfo(nQuestID);
		pQuestInfo->bAutoParty = true;

		vector<GEntityPlayer*> vecPlayers;
		for (int i=0; i<MAX_PARTY_MEMBER_COUNT; ++i)
		{
			vecPlayers.push_back(NewEntityPlayer(m_pField, vec3::ZERO));
			MakeHealer(vecPlayers.back());
			test.quest.GiveNewPlayerQuest(vecPlayers.back(), nQuestID);
		}

		for each (GEntityPlayer* each in vecPlayers)
		{
			CHECK_EQUAL(true, party_matcher.Enqueue(nQuestID, each));
		}
		
		CHECK_EQUAL(0, party_matcher.GetSingleCountAll());

		for each (GEntityPlayer* each in vecPlayers)
		{
			GParty* pParty = GetPartySys()->FindParty(each->GetPartyUID());
			CHECK_EQUAL(true, pParty->IsAutoParty());
		}
	}

	TEST_FIXTURE(Fixture, MakingParty_NotEnoughEntries)
	{
		GAutoPartyMgr party_matcher(m_pField);
		m_pMasterServerFacade->SetPartyRouter(new GStandAloneModePartyRouter());

		QuestID nQuestID = 888;
		GQuestInfo* pQuestInfo = test.quest.NewQuestInfo(nQuestID);
		pQuestInfo->bAutoParty = true;

		vector<GEntityPlayer*> vecPlayers;
		for (int i=0; i<MAX_PARTY_MEMBER_COUNT-1; ++i)
		{
			vecPlayers.push_back(NewEntityPlayer(m_pField, vec3::ZERO));
			MakeHealer(vecPlayers.back());
			test.quest.GiveNewPlayerQuest(vecPlayers.back(), nQuestID);
		}

		for each (GEntityPlayer* each in vecPlayers)
		{
			CHECK_EQUAL(true, party_matcher.Enqueue(nQuestID, each));
		}

		CHECK_EQUAL(MAX_PARTY_MEMBER_COUNT-1, party_matcher.GetSingleCountAll());
		CHECK(NULL == GetPartySys()->FindParty(vecPlayers.front()->GetPartyUID()));
	}

	TEST_FIXTURE(Fixture, MakingParty_ExceptAFKPlayer)
	{
		GAutoPartyMgr party_matcher(m_pField);
		m_pMasterServerFacade->SetPartyRouter(new GStandAloneModePartyRouter());

		QuestID nQuestID = 888;
		GQuestInfo* pQuestInfo = test.quest.NewQuestInfo(nQuestID);
		pQuestInfo->bAutoParty = true;

		vector<GEntityPlayer*> vecPlayers;
		for (int i=0; i<MAX_PARTY_MEMBER_COUNT; ++i)
		{
			vecPlayers.push_back(NewEntityPlayer(m_pField, vec3::ZERO));
			MakeHealer(vecPlayers.back());
			vecPlayers.back()->Update(GConst::AFK_TIMEOUT); // 자리비움 상태 만들기
			test.quest.GiveNewPlayerQuest(vecPlayers.back(), nQuestID);
		}

		for each (GEntityPlayer* each in vecPlayers)
		{
			CHECK_EQUAL(true, party_matcher.Enqueue(nQuestID, each));
		}

		CHECK_EQUAL(MAX_PARTY_MEMBER_COUNT, party_matcher.GetSingleCountAll());
		CHECK(NULL == GetPartySys()->FindParty(vecPlayers.front()->GetPartyUID()));
	}

	// 힐러 아닌 3인 파티결성
	TEST_FIXTURE(Fixture, MakingParty_WithRoleMatch)
	{
		GAutoPartyMgr party_matcher(m_pField);
		m_pMasterServerFacade->SetPartyRouter(new GStandAloneModePartyRouter());

		QuestID nQuestID = 888;
		GQuestInfo* pQuestInfo = test.quest.NewQuestInfo(nQuestID);
		pQuestInfo->bAutoParty = true;
		pQuestInfo->nAutoPartyMemberLimit = 3;
		GAutoPartyMgr::Cooking();

		vector<GEntityPlayer*> vecPlayers;
		for (int i=0; i<pQuestInfo->nAutoPartyMemberLimit; ++i)
		{
			vecPlayers.push_back(NewEntityPlayer(m_pField, vec3::ZERO));
//			MakeHealer(vecPlayers.back());
			test.quest.GiveNewPlayerQuest(vecPlayers.back(), nQuestID);
		}

		for each (GEntityPlayer* each in vecPlayers)
		{
			CHECK_EQUAL(true, party_matcher.Enqueue(nQuestID, each));
		}

		CHECK_EQUAL(0, party_matcher.GetSingleCountAll());

		for each (GEntityPlayer* each in vecPlayers)
		{
			GParty* pParty = GetPartySys()->FindParty(each->GetPartyUID());
			CHECK_EQUAL(true, pParty->IsAutoParty());
		}
	}

	TEST_FIXTURE(Fixture, RemoveEntry)
	{
		GSharedField* pField = test.field.NewSharedField(test.field.NewFieldInfo());
		GEntityPlayer* pPlayer = NewEntityPlayer(pField, vec3::ZERO);

	 	GAutoPartyMgr* pAPartyMgr = pField->GetAutoPartyMgr();

		QuestID nQuestID = 888;
		GQuestInfo* pQuestInfo = test.quest.NewQuestInfo(nQuestID);
		pQuestInfo->bAutoParty = true;

		test.quest.GiveNewPlayerQuest(pPlayer, nQuestID);
		pAPartyMgr->Enqueue(nQuestID, pPlayer);
		CHECK_EQUAL(1, pAPartyMgr->GetSingleCountAll());
		pPlayer->DeleteMe();
		pField->Update(0.0f);
		CHECK_EQUAL(0, pAPartyMgr->GetSingleCountAll());
	}

	TEST_FIXTURE(Fixture, Packet_Enqueue)
	{
		GSharedField* pField = test.field.NewSharedField(test.field.NewFieldInfo());
		GEntityPlayer* pPlayer = NewEntityPlayer(pField, vec3::ZERO);
		MockLink* pLink = NewLink(pPlayer);

		QuestID nQuestID = 888;
		GQuestInfo* pQuestInfo = test.quest.NewQuestInfo(nQuestID);
		pQuestInfo->bAutoParty = true;

		test.quest.GiveNewPlayerQuest(pPlayer, nQuestID);

		pLink->ResetCommands();
		pLink->OnRecv(MC_AUTOPARTY_ENQUEUE_REQ,		1, NEW_INT(nQuestID));

		CHECK_EQUAL(1, pField->GetAutoPartyMgr()->GetSingleCount(nQuestID));
		CHECK_EQUAL(1, pField->GetAutoPartyMgr()->GetSingleCountAll());
	}

	TEST_FIXTURE(Fixture, Packet_Dequeue)
	{
		GSharedField* pField = test.field.NewSharedField(test.field.NewFieldInfo());
		GEntityPlayer* pPlayer = NewEntityPlayer(pField, vec3::ZERO);
		MockLink* pLink = NewLink(pPlayer);

		QuestID nQuestID = 888;
		GQuestInfo* pQuestInfo = test.quest.NewQuestInfo(nQuestID);
		pQuestInfo->bAutoParty = true;

		test.quest.GiveNewPlayerQuest(pPlayer, nQuestID);

		pLink->ResetCommands();
		pLink->OnRecv(MC_AUTOPARTY_ENQUEUE_REQ,		1, NEW_INT(nQuestID));
		CHECK_EQUAL(1, pField->GetAutoPartyMgr()->GetSingleCount(nQuestID));
		CHECK_EQUAL(1, pField->GetAutoPartyMgr()->GetSingleCountAll());
		pLink->ResetCommands();  
		pLink->OnRecv(MC_AUTOPARTY_DEQUEUE_REQ,		1, NEW_INT(nQuestID));
		CHECK_EQUAL(0, pField->GetAutoPartyMgr()->GetSingleCount(nQuestID));
		CHECK_EQUAL(0, pField->GetAutoPartyMgr()->GetSingleCountAll());
	}

	TEST_FIXTURE(Fixture, IsHealer)
	{
		m_pPlayer->SetLevel(1);
		CHECK_EQUAL(false, m_pPlayer->GetRole().IsHealer());
		CHECK_EQUAL(1, m_pPlayer->GetLevel());

		MakeHealer(m_pPlayer);
		CHECK_EQUAL(true, m_pPlayer->GetRole().IsHealer());
	}

	TEST_FIXTURE(Fixture, NeedHealTalentPoint)
	{
		struct CheckData
		{
			int nPlayerLevel;
			int nExpectedPoint;
		} cases[] = {
			{1, 1},
			{3, 1},
			{5, 1},
			{8, 1},
			{13, 2},
			{18, 2},
			{24, 4},
			{44, 4},
			{48, 4},
			{50, 4},
		};

		for each (CheckData each in cases)
		{
			CHECK_EQUAL(each.nExpectedPoint, m_pPlayer->GetRole().GetNeedHealTalentPoint(each.nPlayerLevel));
		}
	}

	TEST_FIXTURE(Fixture, CalcHealTalentPoint)
	{
		m_pPlayer->SetLevel(10);
		CHECK_EQUAL(0, m_pPlayer->GetRole().CalcHealTalentPoint(m_pPlayer->GetLevel()));

		LearnHealTalent(m_pPlayer, 1);
		CHECK_EQUAL(1, m_pPlayer->GetRole().CalcHealTalentPoint(m_pPlayer->GetLevel()));

		LearnHealTalent(m_pPlayer, 10);
		CHECK_EQUAL(2, m_pPlayer->GetRole().CalcHealTalentPoint(m_pPlayer->GetLevel()));
	}

	TEST_FIXTURE(Fixture, MakingParty_AdditionMember)
	{
		GAutoPartyMgr party_matcher(m_pField);
		m_pMasterServerFacade->SetPartyRouter(new GStandAloneModePartyRouter());

		QuestID nQuestID = 888;
		GQuestInfo* pQuestInfo = test.quest.NewQuestInfo(nQuestID);
		pQuestInfo->bAutoParty = true;

		vector<GEntityPlayer*> vecPlayers;
		for (int i=0; i<MAX_PARTY_MEMBER_COUNT; ++i)
		{
			vecPlayers.push_back(NewEntityPlayer(m_pField, vec3::ZERO));
			MakeHealer(vecPlayers.back());
			test.quest.GiveNewPlayerQuest(vecPlayers.back(), nQuestID);
		}

		for each (GEntityPlayer* each in vecPlayers)
		{
			party_matcher.Enqueue(nQuestID, each);
		}

		// 한명이 파티를 탈퇴
		GEntityPlayer* pLeaver = vecPlayers.back();
		LeaveParty(pLeaver);

		// 파티매처에 재등록 (나간 파티원, 파티장)
		party_matcher.Enqueue(nQuestID, pLeaver);
		party_matcher.Enqueue(nQuestID, vecPlayers.front());
		CHECK_EQUAL(true, pLeaver->HasParty());
	}

	TEST_FIXTURE(Fixture, AdditionMemberOnLeaderAFK)
	{
		GAutoPartyMgr party_matcher(m_pField);
		m_pMasterServerFacade->SetPartyRouter(new GStandAloneModePartyRouter());

		QuestID nQuestID = 888;
		GQuestInfo* pQuestInfo = test.quest.NewQuestInfo(nQuestID);
		pQuestInfo->bAutoParty = true;

		vector<GEntityPlayer*> vecPlayers;
		for (int i=0; i<MAX_PARTY_MEMBER_COUNT; ++i)
		{
			vecPlayers.push_back(NewEntityPlayer(m_pField, vec3::ZERO));
			MakeHealer(vecPlayers.back());
			test.quest.GiveNewPlayerQuest(vecPlayers.back(), nQuestID);
		}

		for each (GEntityPlayer* each in vecPlayers)
		{
			party_matcher.Enqueue(nQuestID, each);
		}

		// 한명이 파티를 탈퇴
		GEntityPlayer* pLeaver = vecPlayers.back();
		LeaveParty(pLeaver);

		// 파티매처에 재등록 (나간 파티원, 자리비움 상태인 파티장)
		GEntityPlayer* pLeader = vecPlayers.front();
		pLeader->GetAFK().Toggle();
		party_matcher.Enqueue(nQuestID, pLeader);
		party_matcher.Enqueue(nQuestID, pLeaver);
		CHECK_EQUAL(false, pLeaver->HasParty());
	}

	TEST_FIXTURE(Fixture, AdditionMemberOnMemberAFK)
	{
		GAutoPartyMgr party_matcher(m_pField);
		m_pMasterServerFacade->SetPartyRouter(new GStandAloneModePartyRouter());

		QuestID nQuestID = 888;
		GQuestInfo* pQuestInfo = test.quest.NewQuestInfo(nQuestID);
		pQuestInfo->bAutoParty = true;

		vector<GEntityPlayer*> vecPlayers;
		for (int i=0; i<MAX_PARTY_MEMBER_COUNT; ++i)
		{
			vecPlayers.push_back(NewEntityPlayer(m_pField, vec3::ZERO));
			MakeHealer(vecPlayers.back());
			test.quest.GiveNewPlayerQuest(vecPlayers.back(), nQuestID);
		}

		for each (GEntityPlayer* each in vecPlayers)
		{
			party_matcher.Enqueue(nQuestID, each);
		}

		// 한명이 파티를 탈퇴
		GEntityPlayer* pLeaver = vecPlayers.back();
		LeaveParty(pLeaver);

		// 파티매처에 재등록 (나간 파티원, 파티장)
		GEntityPlayer* pLeader = vecPlayers.front();
		GEntityPlayer* pMember = vecPlayers[1];
		pMember->GetAFK().Toggle();
		party_matcher.Enqueue(nQuestID, pLeader);
		party_matcher.Enqueue(nQuestID, pLeaver);
		CHECK_EQUAL(true, pLeaver->HasParty());
	}

	TEST_FIXTURE(Fixture, ChangeAutoPartyState)
	{
		GSharedField* pField = test.field.NewSharedField(test.field.NewFieldInfo());
		GAutoPartyMgr party_matcher(pField);
		m_pMasterServerFacade->SetPartyRouter(new GStandAloneModePartyRouter());

		QuestID nQuestID = 888;
		GQuestInfo* pQuestInfo = test.quest.NewQuestInfo(nQuestID);
		pQuestInfo->bAutoParty = true;
		pQuestInfo->nAutoPartyMemberLimit = 2;
		GAutoPartyMgr::Cooking();

		GEntityPlayer* pPlayer1 = NewEntityPlayer(pField, vec3::ZERO);
		GEntityPlayer* pPlayer2 = NewEntityPlayer(pField, vec3::ZERO);
		test.quest.GiveNewPlayerQuest(pPlayer1, nQuestID);
		test.quest.GiveNewPlayerQuest(pPlayer2, nQuestID);
		party_matcher.Enqueue(nQuestID, pPlayer1);
		party_matcher.Enqueue(nQuestID, pPlayer2);
		
		GParty* pParty = GetPartySys()->FindParty(pPlayer1->GetPartyUID());
		TVALID(pParty);
		CHECK_EQUAL(true, pParty->IsAutoParty());

		MockLink* pLink1 = NewLink(pPlayer1);
		MockLink* pLink2 = NewLink(pPlayer2);

		pLink1->OnRecv(MC_AUTOPARTY_CHANGE_STATE_REQ,		2, NEW_INT(nQuestID), NEW_INT(AUTOPARTY_LOOKUP));
		
		ASSERT_EQUAL(pLink1->GetCommandID(0), MC_AUTOPARTY_CHANGE_STATE);
		ASSERT_EQUAL(pLink2->GetCommandID(0), MC_AUTOPARTY_CHANGE_STATE);
		CHECK_EQUAL(pLink2->GetParam<int>(0, 0), nQuestID);
		CHECK_EQUAL(pLink2->GetParam<int>(0, 1), AUTOPARTY_LOOKUP);
	}

	TEST_FIXTURE(Fixture, ReunionForPlayersOnAFK)
	{
		GSharedField* pField = test.field.NewSharedField(test.field.NewFieldInfo());
		GAutoPartyMgr party_matcher(pField);
		m_pMasterServerFacade->SetPartyRouter(new GStandAloneModePartyRouter());

		QuestID nQuestID = 888;
		GQuestInfo* pQuestInfo = test.quest.NewQuestInfo(nQuestID);
		pQuestInfo->bAutoParty = true;
		pQuestInfo->nAutoPartyMemberLimit = 2;
		GAutoPartyMgr::Cooking();

		GEntityPlayer* pPlayer1 = NewEntityPlayer(pField, vec3::ZERO);
		GEntityPlayer* pPlayer2 = NewEntityPlayer(pField, vec3::ZERO);
		test.quest.GiveNewPlayerQuest(pPlayer1, nQuestID);
		test.quest.GiveNewPlayerQuest(pPlayer2, nQuestID);
		party_matcher.Enqueue(nQuestID, pPlayer1);
		party_matcher.Enqueue(nQuestID, pPlayer2);

		// 파티원 둘 다 자리비움
		pPlayer1->GetAFK().Toggle();
		pPlayer2->GetAFK().Toggle();
		
		// 파티 깨기
		LeaveParty(pPlayer1);
		LeaveParty(pPlayer2);
		CHECK_EQUAL(false, pPlayer1->HasParty());
		CHECK_EQUAL(false, pPlayer2->HasParty());

		// 파티신청 후 자리비움
		party_matcher.Enqueue(nQuestID, pPlayer1);
		party_matcher.Enqueue(nQuestID, pPlayer2);
		pPlayer1->GetAFK().Toggle();
		pPlayer2->GetAFK().Toggle();
		party_matcher.Update(GConst::AUTOPARTY_MATCH_PROCESS_INTERVAL);
		CHECK_EQUAL(true, pPlayer1->HasParty());
		CHECK_EQUAL(true, pPlayer2->HasParty());
	}

	// QA 버그 케이스 : 
	/*3. 4인 자동 파티(숲의 여왕:테레시스 처치) 시 클레릭 치유 탤런트가 없는 캐릭터 4명이 모여도 자동 파티가 이루어 집니다.

		테스트 방식
		클레릭 탤런트를 하나도 찍지 않은 캐릭터 4명이 퀘스트를 받고 한자리에 모였을 시 자동 파티가 구성됩니다.*/
	TEST_FIXTURE(Fixture, PartyMatching_4People)
	{
		GAutoPartyMgr party_matcher(m_pField);
		m_pMasterServerFacade->SetPartyRouter(new GStandAloneModePartyRouter());

		QuestID nQuestID = 888;
		GQuestInfo* pQuestInfo = test.quest.NewQuestInfo(nQuestID);
		pQuestInfo->bAutoParty = true;
		pQuestInfo->nAutoPartyMemberLimit = 4;
		GAutoPartyMgr::Cooking();

		vector<GEntityPlayer*> vecPlayers;
		for (int i=0; i<pQuestInfo->nAutoPartyMemberLimit; ++i)
		{
			vecPlayers.push_back(NewEntityPlayer(m_pField, vec3::ZERO));
			test.quest.GiveNewPlayerQuest(vecPlayers.back(), nQuestID);
		}

		for each (GEntityPlayer* each in vecPlayers)
		{
			CHECK_EQUAL(true, party_matcher.Enqueue(nQuestID, each));
		}

		CHECK_EQUAL(4, party_matcher.GetSingleCountAll());

		for each (GEntityPlayer* each in vecPlayers)
		{
			CHECK_EQUAL(false, each->HasParty());
		}
	}
		
	TEST_FIXTURE(Fixture, PartyMatching_4Healer)
	{
		GAutoPartyMgr party_matcher(m_pField);
		m_pMasterServerFacade->SetPartyRouter(new GStandAloneModePartyRouter());

		QuestID nQuestID = 888;
		GQuestInfo* pQuestInfo = test.quest.NewQuestInfo(nQuestID);
		pQuestInfo->bAutoParty = true;
		pQuestInfo->nAutoPartyMemberLimit = 4;
		GAutoPartyMgr::Cooking();

		vector<GEntityPlayer*> vecPlayers;
		for (int i=0; i<pQuestInfo->nAutoPartyMemberLimit; ++i)
		{
			vecPlayers.push_back(NewEntityPlayer(m_pField, vec3::ZERO));
			MakeHealer(vecPlayers.back());
			test.quest.GiveNewPlayerQuest(vecPlayers.back(), nQuestID);
		}

		for each (GEntityPlayer* each in vecPlayers)
		{
			CHECK_EQUAL(true, party_matcher.Enqueue(nQuestID, each));
		}

		CHECK_EQUAL(0, party_matcher.GetSingleCountAll());

		for each (GEntityPlayer* each in vecPlayers)
		{
			CHECK_EQUAL(true, each->HasParty());
		}
	}
}