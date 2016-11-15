#include "stdafx.h"
#include "MockField.h"
#include "GTestForward.h"
#include "GJobMgr.h"
#include "GFieldNPCSession.h"
#include "GJobRunner.h"
#include "GJob_Idle.h"
#include "GFieldNPCSession.h"
#include "GFieldNPCSessionMgr.h"
#include "GGlueActor.h"
#include "GGlueNPC.h"
#include "GNullGlueActor.h"
#include "GModuleAI.h"
#include "GBehaviorStateMachine.h"

SUITE(NPCSession)
{
	struct FNPCSession
	{
		FNPCSession()
		{
			m_pField = GUTHelper_Field::DefaultMockField();
			m_pNPC = GUTHelper_NPC::SpawnNPC(m_pField);			
			m_pJobMgr = m_pNPC->GetJobMgr();
		}
		~FNPCSession()
		{
			m_pField->GetSession()->Clear();
		}

		void PushIdleJob(float fDuration=10.0f)
		{
			GJobRunner job_runner;
			job_runner.Idle(m_pNPC->GetModuleAI(), fDuration);
		}

		void UpdateAI(float fDelta, GEntityNPC* n1, GEntityNPC* n2=NULL)
		{
			n1->GetModuleAI()->Update(fDelta);

			if (n2)
			{
				n2->GetModuleAI()->Update(fDelta);
			}
		}

		GFieldNPCSession* NewSession(string strName = "", bool bAutoAdd=true)
		{
			if (strName == "")
			{
				int n = GMath::RandomNumber(1, 100000);
				char szName[128];
				_itoa(n, szName, 10);
				strName = string("sesssion_") + szName;
			}
			GFieldNPCSession* pSession = new GFieldNPCSession(SUnitTestUtil::NewUID(), m_pField->GetUID(), strName.c_str());
			if (bAutoAdd)
			{
				m_pField->GetSession()->Add(pSession);
			}
			return pSession;
		}

		GEntityNPC* m_pNPC;
		GJobMgr*	m_pJobMgr;
		MockField*	m_pField;
		DECLWRAPPER_NPCInfoMgr;
	};

	TEST_FIXTURE(FNPCSession, HasSession)
	{
		CHECK_EQUAL(false, m_pNPC->HasSession());
		GFieldNPCSession* session = NewSession();
		m_pField->GetSession()->Add(session);
		session->AddNPC(m_pNPC->GetUID());
		session->Update(0.0f);
		CHECK_EQUAL(true, m_pNPC->HasSession());
	}

	TEST_FIXTURE(FNPCSession, SessionName)
	{
		string strName = "test";
		GFieldNPCSession* session = NewSession(strName);
		CHECK_EQUAL(strName, session->GetName());
	}

	TEST_FIXTURE(FNPCSession, FindByName)
	{
		string strName = "test";
		GFieldNPCSession* session = NewSession(strName);
		m_pField->GetSession()->Add(session);
		CHECK(NULL == m_pField->GetSession()->Find("noname"));
		CHECK(session == m_pField->GetSession()->Find(strName.c_str()));
	}

	TEST_FIXTURE(FNPCSession, PushJob)
	{
		GFieldNPCSession* session = NewSession();
		m_pField->GetSession()->Add(session);
		session->AddNPC(m_pNPC->GetUID());
		session->Update(0.0f);
		CHECK_EQUAL(0, session->GetJobQty()); 
		PushIdleJob();
		CHECK_EQUAL(1, session->GetJobQty()); 
	}

	TEST_FIXTURE(FNPCSession, UsingSessionJob)
	{
		GFieldNPCSession* session = NewSession();
		m_pField->GetSession()->Add(session);
		session->AddNPC(m_pNPC->GetUID());
		session->Update(0.0f);
		PushIdleJob();
		CHECK_EQUAL(1, session->GetJobQty());
		CHECK(!m_pJobMgr->GetCurrJob());
		session->Update(0.0f);
		m_pJobMgr->Run(0.0f);
		CHECK_EQUAL(0, session->GetJobQty());
	}

	//TEST_FIXTURE(FNPCSession, NotUsingSessionJobDuringCombating)
	//{
	//	GFieldNPCSession* session = NewSession();
	//	m_pField->GetSession()->Add(session);
	//	session->AddNPC(m_pNPC->GetUID());
	//	session->Update(0.0f);
	//	PushIdleJob();
	//	test.npc.MakeCombat(m_pNPC); // BehaviorMode로 사용하기 위함
	//	CHECK_EQUAL(1, session->GetJobQty());
	//	CHECK(!m_pJobMgr->GetCurrJob());
	//	session->Update(0.0f);
	//	m_pJobMgr->Run(0.0f);
	//	CHECK_EQUAL(1, session->GetJobQty());
	//	CHECK_EQUAL(0, m_pJobMgr->GetCount());
	//	CHECK(!m_pJobMgr->GetCurrJob());
	//}

	TEST_FIXTURE(FNPCSession, SessionJobCooperate)
	{
		GJobRunner job_runner;
		GEntityNPC* m_pNPC2 = GUTHelper_NPC::SpawnNPC(m_pField);			

		GFieldNPCSession* session = NewSession();
		m_pField->GetSession()->Add(session);
		session->AddNPC(m_pNPC->GetUID());
		session->AddNPC(m_pNPC2->GetUID());
		session->Update(0.0f);

		float fJobDuration = 10.0f;
		job_runner.Idle(m_pNPC2->GetModuleAI(), fJobDuration);
		job_runner.Idle(m_pNPC->GetModuleAI(), fJobDuration);
		
		session->Update(0.0f);
		UpdateAI(0.0f, m_pNPC, m_pNPC2);
		UpdateAI(1.0f, m_pNPC, m_pNPC2);
		CHECK_EQUAL(GJOB_IDLE, m_pNPC2->GetJobMgr()->GetCurrJobID()); // NPC2 행동중
		CHECK(!m_pNPC->GetJobMgr()->GetCurrJob()); // NPC2의 행동 대기중
		m_pJobMgr->Run(fJobDuration);
		UpdateAI(fJobDuration, m_pNPC, m_pNPC2);
		session->Update(0.0f);
		UpdateAI(0.0f, m_pNPC, m_pNPC2);
		CHECK_EQUAL(GJOB_IDLE, m_pNPC->GetJobMgr()->GetCurrJobID()); // NPC1 행동중
		CHECK(!m_pNPC2->GetJobMgr()->GetCurrJob()); // NPC1의 행동 대기중
	}

	TEST_FIXTURE(FNPCSession, ClearJobWhenJoinSession)
	{
		test.npc.MakeCombat(m_pNPC); // BehaviorMode로 사용하기 위함

		PushIdleJob();
		CHECK_EQUAL(1, m_pJobMgr->GetCount());
		GFieldNPCSession* session = NewSession();
		m_pField->GetSession()->Add(session);
		session->AddNPC(m_pNPC->GetUID());
		session->Update(0.0f);
		CHECK_EQUAL(0, m_pJobMgr->GetCount());
	}

	TEST_FIXTURE(FNPCSession, DontRegisterSameNames)
	{
		GFieldNPCSession* session = NewSession("same_name", false);
		CHECK_EQUAL(true, m_pField->GetSession()->Add(session));

		GFieldNPCSession* session2 = NewSession("same_name", false);
		CHECK_EQUAL(false, m_pField->GetSession()->Add(session));
		delete session2;
	}

	TEST_FIXTURE(FNPCSession, Blocking)
	{
		GJobRunner job_runner;
		GEntityNPC* m_pNPC2 = GUTHelper_NPC::SpawnNPC(m_pField);			

		GFieldNPCSession* session = NewSession();
		m_pField->GetSession()->Add(session);
		session->AddNPC(m_pNPC->GetUID());
		session->AddNPC(m_pNPC2->GetUID());
		session->Update(0.0f);

		session->NonBlocking();
		session->Blocking();
		
		float fJobDuration = 10.0f;
		job_runner.Idle(m_pNPC2->GetModuleAI(), fJobDuration);
		job_runner.Idle(m_pNPC->GetModuleAI(), fJobDuration);

		session->Update(0.0f);
		UpdateAI(0.0f, m_pNPC, m_pNPC2);
		UpdateAI(1.0f, m_pNPC, m_pNPC2); // LOD로 인한 틱시간
		CHECK_EQUAL(GJOB_IDLE, m_pNPC2->GetJobMgr()->GetCurrJobID()); // NPC2 행동중
		CHECK(!m_pNPC->GetJobMgr()->GetCurrJob()); // NPC2의 행동 대기중
		m_pJobMgr->Run(fJobDuration);
		UpdateAI(fJobDuration, m_pNPC, m_pNPC2);
		session->Update(0.0f);
		UpdateAI(0.0f, m_pNPC, m_pNPC2);
		CHECK_EQUAL(GJOB_IDLE, m_pNPC->GetJobMgr()->GetCurrJobID()); // NPC1 행동중
		CHECK(!m_pNPC2->GetJobMgr()->GetCurrJob()); // NPC1의 행동 대기중
	}

	TEST_FIXTURE(FNPCSession, NonBlocking)
	{
		GJobRunner job_runner;
		GEntityNPC* m_pNPC2 = GUTHelper_NPC::SpawnNPC(m_pField);			

		GFieldNPCSession* session = NewSession();
		m_pField->GetSession()->Add(session);
		session->AddNPC(m_pNPC->GetUID());
		session->AddNPC(m_pNPC2->GetUID());
		session->Update(0.0f);
		session->NonBlocking();

		float fJobDuration = 10.0f;
		job_runner.Idle(m_pNPC2->GetModuleAI(), fJobDuration);
		job_runner.Idle(m_pNPC->GetModuleAI(), fJobDuration);

		session->Update(0.0f);
		UpdateAI(0.0f, m_pNPC, m_pNPC2);
		UpdateAI(1.0f, m_pNPC, m_pNPC2); // LOD로 인한 틱시간
		CHECK_EQUAL(GJOB_IDLE, m_pNPC->GetJobMgr()->GetCurrJobID()); // NPC1 행동중
		CHECK_EQUAL(GJOB_IDLE, m_pNPC2->GetJobMgr()->GetCurrJobID()); // NPC2 행동중
		UpdateAI(fJobDuration, m_pNPC, m_pNPC2);
		UpdateAI(1.0f, m_pNPC, m_pNPC2); // LOD로 인한 틱시간
		CHECK(!m_pNPC->GetJobMgr()->GetCurrJob()); 
		CHECK(!m_pNPC2->GetJobMgr()->GetCurrJob()); 
	}

	//TEST_FIXTURE(FNPCSession, DeleteSessionWhenEmptyJob)
	//{
	//	m_pField->GetSession()->Clear();
	//	CHECK_EQUAL(0, m_pField->GetSession()->GetQty());
	//	m_pField->GetSession()->Add(NewSession());
	//	CHECK_EQUAL(1, m_pField->GetSession()->GetQty());
	//	m_pField->GetSession()->Update(0.0f);
	//	CHECK_EQUAL(0, m_pField->GetSession()->GetQty());
	//}

	TEST_FIXTURE(FNPCSession, ResetWhenDeleteSession)
	{
		GFieldNPCSession* session = NewSession();
		CHECK_EQUAL(false, m_pNPC->HasSession());
		session->AddNPC(m_pNPC->GetUID());
		session->Update(0.0f);
		CHECK_EQUAL(true, m_pNPC->HasSession());
		m_pField->GetSession()->Add(session);
		session->EndSession();
		session->Update(0.0f);
		m_pField->GetSession()->Update(0.0f);
		CHECK_EQUAL(false, m_pNPC->HasSession());
	}

	TEST_FIXTURE(FNPCSession, JoinMultiSessions)
	{
		GFieldNPCSession* session = NewSession();
		GFieldNPCSession* session2 = NewSession();
		m_pField->GetSession()->Add(session);
		m_pField->GetSession()->Add(session2);
		CHECK_EQUAL(0, session->GetNPCQty());
		CHECK_EQUAL(0, session2->GetNPCQty());
		session->AddNPC(m_pNPC->GetUID());
		session->Update(0.0f);
		CHECK(session == m_pNPC->GetSession());
		CHECK_EQUAL(1, session->GetNPCQty());
		CHECK_EQUAL(0, session2->GetNPCQty());
		session2->AddNPC(m_pNPC->GetUID());
		session2->Update(0.0f);
		CHECK(session2 == m_pNPC->GetSession());
		CHECK_EQUAL(0, session->GetNPCQty());
		CHECK_EQUAL(1, session2->GetNPCQty());
	}

	TEST_FIXTURE(FNPCSession, SessionWait)
	{
		GFieldNPCSession* session = NewSession();
		m_pField->GetSession()->Add(session);
		session->AddNPC(m_pNPC->GetUID());
		session->Update(0.0f);
		float fWaitTime = 10.0f;
		session->Wait(fWaitTime);
		PushIdleJob();
		CHECK_EQUAL(2, session->GetJobQty()); // SESSION_WAIT, IDLE
		session->Update(0.0f);
		UpdateAI(0.0f, m_pNPC);
		session->Update(0.0f);
		CHECK_EQUAL(1, session->GetJobQty()); // IDLE
		session->Update(0.0f);
		UpdateAI(0.0f, m_pNPC);
		session->Update(fWaitTime/2);
		CHECK_EQUAL(1, session->GetJobQty()); // IDLE
		session->Update(0.0f);
		UpdateAI(0.0f, m_pNPC);
		session->Update(fWaitTime/2);
		session->Update(0.0f);
		UpdateAI(0.0f, m_pNPC);
		session->Update(0.0f);
		CHECK_EQUAL(0, session->GetJobQty());
	}


	TEST_FIXTURE(FNPCSession, AddNPCAfterSessionBegan)
	{
		GFieldNPCSession* session = NewSession();
		m_pField->GetSession()->Add(session);
		float fWaitTime = 10.0f;
		session->Wait(fWaitTime);
		session->AddNPC(m_pNPC->GetUID());
		PushIdleJob();
		session->Update(0.0f);
		m_pNPC->GetModuleAI()->Update(0.0f);
		m_pNPC->GetJobMgr()->Run(0.0f);
		// 세선대기중이므로 잡이 들어가지 않아야 함
		CHECK(NULL == m_pNPC->GetJobMgr()->GetCurrJob());
		CHECK_EQUAL(0, m_pNPC->GetJobMgr()->GetCount());
		// 세션대기후에 잡이 다시 제대로 들어감
		session->Update(fWaitTime);
		m_pNPC->GetJobMgr()->Run(0.0f);
		m_pNPC->GetModuleAI()->Update(1.0f);
		CHECK(m_pNPC->GetJobMgr()->GetCurrJob());
		CHECK_EQUAL(0, m_pNPC->GetJobMgr()->GetCount());
	}

	TEST_FIXTURE(FNPCSession, RemoveNPCSessionWhenEndSession)
	{
		GFieldNPCSession* session = NewSession();
		m_pField->GetSession()->Add(session);
		session->AddNPC(m_pNPC->GetUID());
		session->Update(0.0f);
		CHECK_EQUAL(true, m_pNPC->HasSession());
		session->EndSession();
		session->Update(0.0f);
		m_pField->GetSession()->Update(0.0f);
		CHECK_EQUAL(false, m_pNPC->HasSession());
	}

	TEST_FIXTURE(FNPCSession, InitalStateIsBegin)
	{
		GFieldNPCSession* session = NewSession();
		session->AddNPC(m_pNPC->GetUID());
		session->Update(0.0f);
		PushIdleJob();
		session->Update(0.0f);
		CHECK_EQUAL("Begin", session->GetSceneName());
	}

	//TEST_FIXTURE(FNPCSession, FinalStateIsFinish)
	//{
	//	GFieldNPCSession* session = NewSession();
	//	session->Update(0.0f);
	//	CHECK_EQUAL("Finish", session->GetSceneName());
	//}

	TEST_FIXTURE(FNPCSession, ChangeRule)
	{
		GFieldNPCSession* session = NewSession();
		session->AddNPC(m_pNPC->GetUID());
		session->Update(0.0f);
		PushIdleJob();

		GFieldNPCSession::Scene::Condition condition;
		condition.uidNPC = m_pNPC->GetUID();
		condition.strNextStateName = "FutureState";
		condition.strConditionType = "hp";
		condition.vecParams.push_back(0);
		condition.vecParams.push_back(40);
		session->AddChangeSceneRule(condition);
		
		const float kUpdateTick = 1.0f;
		// 조건에 충족되지 않아 상태 유지
		CHECK_EQUAL("Begin", session->GetSceneName());
		session->Update(kUpdateTick);
		CHECK_EQUAL("Begin", session->GetSceneName());
		// 조건에 충족되어 상태 변경
		m_pNPC->SetHP(1);
		PushIdleJob();
		session->Update(kUpdateTick);
		CHECK_EQUAL("FutureState", session->GetSceneName());
	}

	TEST_FIXTURE(FNPCSession, ResetJobWhenChangeState)
	{
		GFieldNPCSession* session = NewSession();
		session->AddNPC(m_pNPC->GetUID());
		session->Update(0.0f);
		PushIdleJob();
		CHECK_EQUAL(1, session->GetJobQty());
		session->ChangeScene("another_state");
		CHECK_EQUAL(0, session->GetJobQty());
	}
	
	TEST_FIXTURE(FNPCSession, ChangeCombatState)
	{
		GFieldNPCSession* session = NewSession();
		session->AddNPC(m_pNPC->GetUID());
		session->Update(0.0f);
		PushIdleJob();
		CHECK_EQUAL((int)GFieldNPCSession::COMBATSTATE_NONE, session->GetCombatState());
		session->ChangeCombatState(GFieldNPCSession::COMBATSTATE_ALL);
		CHECK_EQUAL((int)GFieldNPCSession::COMBATSTATE_ALL, session->GetCombatState());
	}

	//TEST_FIXTURE(FNPCSession, IsThinkable)
	//{
	//	CHECK_EQUAL(true, m_pNPC->IsThinkable());
	//	GFieldNPCSession* session = NewSession();
	//	session->AddNPC(m_pNPC->GetUID());
	//	session->Update(0.0f);
	//	PushIdleJob();
	//	CHECK_EQUAL(false, m_pNPC->IsThinkable());
	//	session->ChangeCombatState(GFieldNPCSession::COMBATSTATE_ALL);
	//	CHECK_EQUAL(true, m_pNPC->IsThinkable());
	//}

	TEST_FIXTURE(FNPCSession, NoneCombatStateWhenChangeScene)
	{
		GFieldNPCSession* session = NewSession();
		session->AddNPC(m_pNPC->GetUID());
		session->Update(0.0f);
		PushIdleJob();
		session->ChangeCombatState(GFieldNPCSession::COMBATSTATE_ALL);
		CHECK_EQUAL((int)GFieldNPCSession::COMBATSTATE_ALL, session->GetCombatState());
		session->ChangeScene("NextScene");
		CHECK_EQUAL((int)GFieldNPCSession::COMBATSTATE_NONE, session->GetCombatState());
	}

	TEST_FIXTURE(FNPCSession, IsHittable_None)
	{
		GTalentInfo* pTalent = test.talent.MakeDamageTalent(1);
		GEntityNPC* m_pNPC2 = GUTHelper_NPC::SpawnNPC(m_pField);			
		CHECK_EQUAL(true, m_pNPC->IsHittable(m_pNPC2, pTalent));
		GFieldNPCSession* session = NewSession();
		session->AddNPC(m_pNPC->GetUID());
		session->Update(0.0f);
		PushIdleJob();
		session->ChangeCombatState(GFieldNPCSession::COMBATSTATE_NONE);
		CHECK_EQUAL(false, m_pNPC->IsHittable(m_pNPC2, pTalent));
	}

	TEST_FIXTURE(FNPCSession, IsHittable_All)
	{
		GTalentInfo* pTalent = test.talent.MakeDamageTalent(1);
		GEntityNPC* m_pNPC2 = GUTHelper_NPC::SpawnNPC(m_pField);			
		CHECK_EQUAL(true, m_pNPC->IsHittable(m_pNPC2, pTalent));
		GFieldNPCSession* session = NewSession();
		session->AddNPC(m_pNPC->GetUID());
		session->Update(0.0f);
		PushIdleJob();
		session->ChangeCombatState(GFieldNPCSession::COMBATSTATE_ALL);
		CHECK_EQUAL(true, m_pNPC->IsHittable(m_pNPC2, pTalent));
	}

	TEST_FIXTURE(FNPCSession, IsHittable_Session)
	{
		GTalentInfo* pTalent = test.talent.MakeDamageTalent(1);
		GEntityNPC* m_pNPC2 = GUTHelper_NPC::SpawnNPC(m_pField);			
		CHECK_EQUAL(true, m_pNPC->IsHittable(m_pNPC2, pTalent));
		GFieldNPCSession* session = NewSession();
		session->AddNPC(m_pNPC->GetUID());
		session->AddNPC(m_pNPC2->GetUID());
		session->Update(0.0f);
		PushIdleJob();
		session->ChangeCombatState(GFieldNPCSession::COMBATSTATE_SESSION);
		CHECK_EQUAL(true, m_pNPC->IsHittable(m_pNPC2, pTalent));
	}

	TEST_FIXTURE(FNPCSession, Bookmark)
	{
		GNullGlueActor::Init();
		gmgr.pFieldMgr->AddField(m_pField);

		GFieldNPCSession* session = NewSession();
		CHECK_EQUAL(true, session->FindBookmark("Test")->IsNull());
		session->AddBookmark("Test", m_pNPC->GetDelegator());
		CHECK_EQUAL(false, session->FindBookmark("Test")->IsNull());
		CHECK(m_pNPC == session->FindBookmark("Test")->GetOwnerActor());

		gmgr.pFieldMgr->EraseField(m_pField);
		GNullGlueActor::FIni();
	}

	TEST_FIXTURE(FNPCSession, ChangeSceneViaRule)
	{
		GFieldNPCSession::Scene::Condition condition;
		condition.strNextStateName = "NextScenen";
		condition.uidNPC = m_pNPC->GetUID();
		condition.strConditionType = "dead";

		GFieldNPCSession* session = NewSession();
		session->AddNPC(m_pNPC->GetUID());
		session->Update(0.0f);
		session->AddChangeSceneRule(condition);

		CHECK_EQUAL("Begin", session->GetSceneName());
		session->Update(1.0f);
		CHECK_EQUAL("Begin", session->GetSceneName());
		m_pNPC->doDie();
		session->Update(1.0f);
		CHECK_EQUAL(condition.strNextStateName, session->GetSceneName());
	}

	TEST_FIXTURE(FNPCSession, ChagneSessionStateWhenJoinSession)
	{
		GFieldNPCSession* session = NewSession();
		m_pField->GetSession()->Add(session);
		CHECK_EQUAL(false, m_pNPC->GetModuleAI()->HasNPCSession()); 
		session->AddNPC(m_pNPC->GetUID());
		CHECK_EQUAL(true, m_pNPC->GetModuleAI()->HasNPCSession()); 
		session->Update(0.0f);
		CHECK_EQUAL(true, m_pNPC->GetModuleAI()->HasNPCSession()); 
	}

	TEST_FIXTURE(FNPCSession, ResetSessionStateWhenChangeScene)
	{
		GFieldNPCSession* session = NewSession();
		session->AddNPC(m_pNPC->GetUID());
		session->Update(0.0f);
		PushIdleJob();
		session->Update(0.0f);
		GJob* pCurSessionJob = NULL;
		pCurSessionJob = m_pNPC->GetModuleAI()->GetBehaviorFSM()->GetCurSessionJob();
		TVALID(NULL != pCurSessionJob);
		CHECK_EQUAL(GJOB_IDLE, pCurSessionJob->GetID());
		session->ChangeScene("NextScene");
		pCurSessionJob = m_pNPC->GetModuleAI()->GetBehaviorFSM()->GetCurSessionJob();
		TVALID(NULL == pCurSessionJob);
	}

}
