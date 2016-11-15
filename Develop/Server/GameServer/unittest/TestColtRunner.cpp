#include "stdafx.h"
#include "GJobRunner.h"
#include "GColtRunner.h"
#include "GUTHelper.h"
#include "GEntityPlayer.h"
#include "MockField.h"
#include "GModuleCombat.h"
#include "GTalent.h"
#include "CCommandTable.h"
#include "GModuleAI.h"
#include "MockLink.h"
#include "GJob.h"
#include "GJob_Talent.h"
#include "GNPCScriptBuilder.h"
#include "GEntityNPC.h"
#include "GJobMgr.h"
#include "GBehaviorStateMachine.h"
#include "GBehaviorState.h"
#include "FBaseMockLink.h"
#include "GCombatFlee.h"
#include "GBehaviorCombat.h"
#include "GHateTable.h"
#include "GNPCBParts.h"
#include "GUTHelper_Field.h"
#include "GTestForward.h"
#include "CSFactionCalculator.h"
#include "GFactionInfo.h"
#include "GModuleMovable.h"

SUITE(ColtRunner)
{
	struct FColtRunner : public FBaseMockLink
	{
		FColtRunner()
		{
			m_pJobRunner = new GJobRunner();
			m_pColtRunner = new GColtRunner(m_pJobRunner);

			m_pField = GUTHelper_Field::DefaultMockField();
			m_pNPC = m_Helper.NewEntityNPC(m_pField);
			m_pOpponent = GUTHelper::NewEntityPlayer(m_pField);

			m_pLink = NewLink(m_pOpponent);
		}

		virtual ~FColtRunner()
		{
			SAFE_DELETE(m_pJobRunner);
			SAFE_DELETE(m_pColtRunner);
		}

		GUTHelper		m_Helper;
		GJobRunner*		m_pJobRunner;
		GColtRunner*	m_pColtRunner;

		MockField*		m_pField;
		GEntityNPC*		m_pNPC;
		GEntityPlayer*	m_pOpponent;
		MockLink*		m_pLink;

		void CHECK_MotionFactor(GEntityNPC* pNPC, MockLink* pMockLink, int nExpectHitCommandID)
		{
			CHECK_EQUAL(AS_MF, pNPC->GetActionState());
			CHECK_EQUAL(TALENT_PHASE_CANCELED, pNPC->GetModuleCombat()->GetTalent()->GetPhase());		
			CHECK_EQUAL(2, pMockLink->GetCommandCount());
			CHECK_EQUAL(nExpectHitCommandID, m_pLink->GetCommand(1).GetID());
		}
	};

	TEST_FIXTURE(FColtRunner, MotionFactor)
	{
		GTalentInfo* pTalentInfo = m_Helper.NewMeleeTalentInfo();
		pTalentInfo->m_fCastingTime = 1.0f; // 캐스팅 타임으로 PREPARE 단계를 두고 이때 취소되도록 유도
		m_pNPC->doUseTalent(pTalentInfo, TALENT_TARGET_INFO());
		GColtAction coltAction;
		coltAction.nType = CAT_MOTIONFACTOR;
		coltAction.qParams[0] = L"0";		// 0 = MF_BEATEN
		coltAction.qParams[1] = L"100";		
		m_pColtRunner->RunAction(&coltAction, &GColtActionInst(), m_pNPC, m_pOpponent);

		CHECK_MotionFactor(m_pNPC, m_pLink, MC_ACTION_TALENT_HIT);
	}

	TEST_FIXTURE(FColtRunner, MotionFactor_Knockback)
	{
		GTalentInfo* pTalentInfo = m_Helper.NewMeleeTalentInfo();
		pTalentInfo->m_fCastingTime = 1.0f; // 캐스팅 타임으로 PREPARE 단계를 두고 이때 취소되도록 유도
		m_pNPC->doUseTalent(pTalentInfo, TALENT_TARGET_INFO());
		GColtAction coltAction;
		coltAction.nType = CAT_MOTIONFACTOR;
		coltAction.qParams[0] = L"1";		// 1 = MF_KNOCKBACK
		coltAction.qParams[1] = L"100";		
		m_pColtRunner->RunAction(&coltAction, &GColtActionInst(), m_pNPC, m_pOpponent);

		CHECK_MotionFactor(m_pNPC, m_pLink, MC_ACTION_TALENT_HIT_KNOCKBACK_MF);
	}

	TEST_FIXTURE(FColtRunner, BreakPart)
	{
		GColtAction coltAction;
		coltAction.nType = CAT_BREAKPART;
		coltAction.qParams[0] = L"1";
		m_pColtRunner->RunAction(&coltAction, &GColtActionInst(), m_pNPC, m_pOpponent);

		CHECK_EQUAL(true, m_pNPC->GetNPCBParts()->IsBroken(1));		
	}

	TEST_FIXTURE(FColtRunner, Damage)
	{
		m_pNPC->GetNPCInfo()->nMaxHP = 100;
		m_pNPC->SetHP(100);
		GColtAction coltAction;
		coltAction.nType = CAT_DAMAGE;
		coltAction.qParams[0] = L"1";
		coltAction.qParams[1] = L"1";
		m_pColtRunner->RunAction(&coltAction, &GColtActionInst(), m_pNPC, m_pOpponent);

		CHECK_EQUAL(99, m_pNPC->GetHP());

		coltAction.qParams[1] = L"99";
		m_pColtRunner->RunAction(&coltAction, &GColtActionInst(), m_pNPC, m_pOpponent);

		CHECK_EQUAL(true, m_pNPC->IsDie());
	}

	TEST_FIXTURE(FColtRunner, Flee)
	{
		GColtAction coltAction;
		coltAction.nType = CAT_FLEE;
		coltAction.qParams[0] = L"from_enemy";
		coltAction.qParams[1] = L"10";
		m_pColtRunner->RunAction(&coltAction, &GColtActionInst(), m_pNPC, m_pOpponent);
		m_pNPC->GetModuleAI()->Update(0.0f);

		GBehaviorStateMachine* pFSM = m_pNPC->GetBehaviorFSM();
		CHECK_EQUAL(AI_COMBAT_FLEE, pFSM->GetCurrentCombatStateID());
		GBehaviorCombat* pCombatBehavior = dynamic_cast<GBehaviorCombat*>(pFSM->FindState(AI_BEHAVIOR_COMBAT));
		TVALID(pCombatBehavior);
		GCombatFlee* pCombatFlee = dynamic_cast<GCombatFlee*>(pCombatBehavior->FindCombatState(AI_COMBAT_FLEE));
		TVALID(pCombatFlee);
		CHECK_EQUAL(10, pCombatFlee->GetDurationTime());
		// 절반만 대기
		m_pNPC->GetModuleAI()->Update(5.0f);
		CHECK_EQUAL(AI_BEHAVIOR_COMBAT, pFSM->GetCurrentStateID());
		CHECK_EQUAL(AI_COMBAT_FLEE, pFSM->GetCurrentCombatStateID());
		// FLEE 시간 모두 대기, 적이 없어 전투가 종료됨
		m_pNPC->GetModuleAI()->Update(5.0f);
		CHECK_EQUAL(AI_BEHAVIOR_RETURN, pFSM->GetCurrentStateID());
	}

	TEST_FIXTURE(FColtRunner, Yell)
	{
		USES_CONVERSION_EX;

		GEntityNPC* pNeighborNPC = m_Helper.NewEntityNPC(m_pField, m_pNPC->GetPos(), m_pNPC->GetNPCInfo()->nID);

		GFactionInfo* pFactionInfo1 = test.faction.NewFactionInfo(CSFactionCalculator::GetNormalMinQuantity());	
		GFactionInfo* pFactionInfo2 = test.faction.NewFactionInfo(CSFactionCalculator::GetNormalMinQuantity());	
		GFactionRelationInfo* pFactionRelationInfo = test.faction.NewFactionRelationInfo(pFactionInfo1->m_nID, pFactionInfo2->m_nID, FRT_GOOD);
		m_pNPC->GetNPCInfo()->m_nFactionLossID = pFactionInfo1->m_nID;		
		pNeighborNPC->GetNPCInfo()->m_nFactionLossID = pFactionInfo2->m_nID;

		gsys.pScriptManager->Init();

		char szNPCID[128];
		sprintf_s(szNPCID, "NPC_%d", m_pNPC->GetNPCInfo()->nID);
		m_pNPC->GetNPCInfo()->strNameInLua = A2W_EX(szNPCID, 0);
		const char* pszTestFuncName = "OnYell";
		const char* pszScript = 
			"function %s:%s(NPC, NPC, NeighborNPC)\n"
			"	TEST_VAR = 1;\n"			
			"end\n";
		
		
		GNPCScriptBuilder nsb(m_pNPC->GetNPCInfo());
		nsb.Build();
		if (!m_Helper.RunLuaString(pszScript, W2A_EX(m_pNPC->GetNPCInfo()->strNameInLua.c_str(), 0), pszTestFuncName))	{ CHECK(FALSE && "WRunString Failed.");	 return; }

		
		m_pNPC->doDamage(m_pOpponent->GetUID(), DA_SLASH, 1);		

		GColtAction coltAction;
		coltAction.nType = CAT_YELL;
		coltAction.qParams[0] = L"100";
		m_pColtRunner->RunAction(&coltAction, &GColtActionInst(), m_pNPC, m_pOpponent);
		m_pNPC->GetModuleAI()->Update(0.0f);

		CHECK_EQUAL(WLUA->GetVar<int>("TEST_VAR"), 1);

		gsys.pScriptManager->Fini();
	}

	namespace Adjust
	{
		TEST_FIXTURE(FColtRunner, VictoryTalentID)
		{		
			GColtAction coltAction;
			coltAction.nType = CAT_ADJUST;
			coltAction.qParams[0] = NPC_XML_ATTR_VICTORY;
			coltAction.qParams[1] = L"30";
			m_pColtRunner->RunAction(&coltAction, &GColtActionInst(), m_pNPC, m_pOpponent);

			CHECK_EQUAL(30, m_pNPC->GetNPCInfo()->ColtInfo.attrCombat[m_pNPC->GetMode()].nVictoryTalentID);
		}
	}

	TEST_FIXTURE(FColtRunner, Distance)
	{		
		m_pNPC->SetPos(vec3(1000, 1000, 0));
		m_pOpponent->SetPos(vec3(100, 100, 0));
		m_pNPC->doDamage(m_pOpponent->GetUID(), DA_SLASH, 1);
		GColtAction coltAction;
		coltAction.nType = CAT_DISTANCE;
		coltAction.qParams[0] = L"20";
		coltAction.qParams[1] = L"30";
		m_pColtRunner->RunAction(&coltAction, &GColtActionInst(), m_pNPC, m_pOpponent);
		m_pNPC->GetModuleAI()->Update(0.0f);
		
		m_pNPC->GetModuleMovable()->Update(5.0f);
		int nDistance = (int) m_pNPC->DistanceTo(m_pOpponent);
		
		CHECK(20 <= nDistance);
		CHECK(30 >= nDistance);
	}

	namespace Follow
	{
		TEST_FIXTURE(FColtRunner, Distance)
		{
			m_pNPC->SetPos(vec3(1000, 1000, 0));
			m_pOpponent->SetPos(vec3(100, 100, 0));
			m_pNPC->doDamage(m_pOpponent->GetUID(), DA_SLASH, 1);
			GColtAction coltAction;
			coltAction.nType = CAT_FOLLOW;
			coltAction.qParams[0] = L"50";
			coltAction.bModifyDurationTime = true;
			coltAction.fDurationTime = -1.0f;  	// 시간 제한 없이 접근

			GColtActionInst coltInstInfo(coltAction);
			m_pColtRunner->RunAction(&coltAction, &coltInstInfo, m_pNPC, m_pOpponent);
			m_pNPC->GetModuleAI()->Update( 0.0f);

			for (int i=0; i<20; i++)
			{
				m_pNPC->GetModuleMovable()->Update( 1.0f);
			}

			int nDistance = (int) m_pNPC->DistanceTo(m_pOpponent);

			CHECK( nDistance >= 50  &&  nDistance <= 100);
		}

		// durationTime이 지나면 따라가지 않는다
		TEST_FIXTURE(FColtRunner, DurationTime)
		{
			m_pNPC->SetPos(vec3(1000, 1000, 0));
			m_pOpponent->SetPos(vec3(100, 100, 0));
			m_pNPC->doDamage(m_pOpponent->GetUID(), DA_SLASH, 1);
			GColtAction coltAction;
			coltAction.nType = CAT_FOLLOW;
			coltAction.qParams[0] = L"20";

			GColtActionInst coltInstInfo(coltAction);
			m_pColtRunner->RunAction(&coltAction, &coltInstInfo, m_pNPC, m_pOpponent);
			m_pNPC->GetModuleAI()->Update(0.0f);

			for (int i=0; i<10; i++)				
			{
				m_pNPC->GetModuleAI()->Update(0.1f);
			}
			int nDistanceAtDurationTime = (int) m_pNPC->DistanceTo(m_pOpponent);

			for (int i=0; i<40; i++)				
			{
				m_pNPC->GetModuleAI()->Update(0.1f);
			}
			int nDistanceAtDurationTimeEnd = (int) m_pNPC->DistanceTo(m_pOpponent);

			CHECK_EQUAL(nDistanceAtDurationTime, nDistanceAtDurationTimeEnd);
		}
	}

	TEST_FIXTURE(FColtRunner, Move)
	{
		m_pNPC->SetPos(vec3(200, 100, 0));
		m_pOpponent->SetPos(vec3(100, 100, 0));
		m_pNPC->doDamage(m_pOpponent->GetUID(), DA_SLASH, 1);
		GColtAction coltAction;
		coltAction.nType = CAT_MOVE;
		coltAction.qParams[0] = L"60";
		coltAction.qParams[1] = L"70";

		m_pColtRunner->RunAction(&coltAction, &GColtActionInst(), m_pNPC, m_pOpponent);
		m_pNPC->GetModuleAI()->Update(0.0f);
		m_pNPC->GetModuleMovable()->Update(5.0f);

		CHECK_EQUAL(200.0f, m_pNPC->GetPos().x);
		CHECK((170.0f >= m_pNPC->GetPos().y &&  160.0f <= m_pNPC->GetPos().y) || 
			 (30.0f <= m_pNPC->GetPos().y && 40.0f >= m_pNPC->GetPos().y));
	}

	TEST_FIXTURE(FColtRunner, ChangeHitCapsule)
	{
		// 함수가 구현되어 있지 않음
	}

	namespace Aggro
	{
		// param2는 초로 명시하되 만일 0이라면 영구히 바꾼다. 
		// 유지시간이 모두 경과되면 자동으로 복귀된다.
		TEST_FIXTURE(FColtRunner, Restore)
		{
			GEntityPlayer* pOpponent2 = GUTHelper::NewEntityPlayer(m_pField);
			m_pNPC->doDamage(m_pOpponent->GetUID(), DA_SLASH, 300);
			m_pNPC->doDamage(pOpponent2->GetUID(), DA_SLASH, 200);
			GColtAction coltAction;
			coltAction.nType = CAT_AGGRO;
			coltAction.qParams[0] = NPC_XML_ATTR_AGGRO_FLIP;
			coltAction.qParams[1] = L"1";

			CHECK_EQUAL(m_pOpponent->GetUID(), m_pNPC->GetTargetActor()->GetUID());

			m_pColtRunner->RunAction(&coltAction, &GColtActionInst(), m_pNPC, m_pOpponent);

			CHECK_EQUAL(pOpponent2->GetUID(), m_pNPC->GetTargetActor()->GetUID());

			m_pNPC->GetHateTable().Update(1.0f);

			CHECK_EQUAL(m_pOpponent->GetUID(), m_pNPC->GetTargetActor()->GetUID());
		}

		// 헤이트 테이블의 1번과 2번의 어그로 수치를 교환한다.
		TEST_FIXTURE(FColtRunner, Flip)
		{
			GEntityPlayer* pOpponent2 = GUTHelper::NewEntityPlayer(m_pField);
			m_pNPC->doDamage(m_pOpponent->GetUID(), DA_SLASH, 3);
			m_pNPC->doDamage(pOpponent2->GetUID(), DA_SLASH, 2);
			GColtAction coltAction;
			coltAction.nType = CAT_AGGRO;
			coltAction.qParams[0] = NPC_XML_ATTR_AGGRO_FLIP;
			coltAction.qParams[1] = L"0";

			CHECK_EQUAL(m_pOpponent->GetUID(), m_pNPC->GetTargetActor()->GetUID());

			m_pColtRunner->RunAction(&coltAction, &GColtActionInst(), m_pNPC, m_pOpponent);

			CHECK_EQUAL(pOpponent2->GetUID(), m_pNPC->GetTargetActor()->GetUID());
		}

		// 헤이트 테이블의 1번과 임의의 대상의 값을 바꾼다.
		TEST_FIXTURE(FColtRunner, Random)
		{
			GEntityPlayer* pOpponent2 = GUTHelper::NewEntityPlayer(m_pField);
			m_pNPC->doDamage(m_pOpponent->GetUID(), DA_SLASH, 3);
			m_pNPC->doDamage(pOpponent2->GetUID(), DA_SLASH, 2);
			GColtAction coltAction;
			coltAction.nType = CAT_AGGRO;
			coltAction.qParams[0] = NPC_XML_ATTR_AGGRO_RANDOM;
			coltAction.qParams[1] = L"0";

			CHECK_EQUAL(m_pOpponent->GetUID(), m_pNPC->GetTargetActor()->GetUID());

			m_pColtRunner->RunAction(&coltAction, &GColtActionInst(), m_pNPC, m_pOpponent);

			CHECK(m_pOpponent->GetUID() != m_pNPC->GetTargetActor()->GetUID());
		}

		// 가장 가까운 대상의 헤이트 수치를 1번과 교체
		TEST_FIXTURE(FColtRunner, Near)
		{
			GEntityPlayer* pOpponent2 = GUTHelper::NewEntityPlayer(m_pField);
			GEntityPlayer* pOpponent3 = GUTHelper::NewEntityPlayer(m_pField);
			m_pNPC->SetPos(vec3(100, 100, 0));			
			pOpponent2->SetPos(vec3(200, 200, 0));
			pOpponent3->SetPos(vec3(300, 300, 0));
			m_pNPC->doDamage(m_pOpponent->GetUID(), DA_SLASH, 3);
			m_pNPC->doDamage(pOpponent2->GetUID(), DA_SLASH, 2);
			m_pNPC->doDamage(pOpponent3->GetUID(), DA_SLASH, 1);
			GColtAction coltAction;
			coltAction.nType = CAT_AGGRO;
			coltAction.qParams[0] = NPC_XML_ATTR_AGGRO_NEAR;
			coltAction.qParams[1] = L"0";

			CHECK_EQUAL(m_pOpponent->GetUID(), m_pNPC->GetTargetActor()->GetUID());

			m_pColtRunner->RunAction(&coltAction, &GColtActionInst(), m_pNPC, m_pOpponent);

			CHECK(pOpponent2->GetUID() == m_pNPC->GetTargetActor()->GetUID());
		}

		// 가장 먼 대상의 헤이트 수치를 1번과 교환 
		TEST_FIXTURE(FColtRunner, Far)
		{
			GEntityPlayer* pOpponent2 = GUTHelper::NewEntityPlayer(m_pField);
			GEntityPlayer* pOpponent3 = GUTHelper::NewEntityPlayer(m_pField);
			m_pNPC->SetPos(vec3(100, 100, 0));
			pOpponent2->SetPos(vec3(200, 200, 0));
			pOpponent3->SetPos(vec3(300, 300, 0));
			m_pNPC->doDamage(m_pOpponent->GetUID(), DA_SLASH, 3);
			m_pNPC->doDamage(pOpponent2->GetUID(), DA_SLASH, 2);
			m_pNPC->doDamage(pOpponent3->GetUID(), DA_SLASH, 1);
			GColtAction coltAction;
			coltAction.nType = CAT_AGGRO;
			coltAction.qParams[0] = NPC_XML_ATTR_AGGRO_FAR;
			coltAction.qParams[1] = L"0";

			CHECK_EQUAL(m_pOpponent->GetUID(), m_pNPC->GetTargetActor()->GetUID());

			m_pColtRunner->RunAction(&coltAction, &GColtActionInst(), m_pNPC, m_pOpponent);

			CHECK(pOpponent3->GetUID() == m_pNPC->GetTargetActor()->GetUID());
		}

		// 가장 가까이 있는 대상의 헤이트 수치를 1번과 교환 (거리, 방향 고려)
		TEST_FIXTURE(FColtRunner, Short)
		{
			GEntityPlayer* pOpponent2 = GUTHelper::NewEntityPlayer(m_pField);
			GEntityPlayer* pOpponent3 = GUTHelper::NewEntityPlayer(m_pField);
			m_pNPC->SetPos(vec3(100, 100, 0));
			m_pNPC->SetDir(vec3(0,0,1));
			m_pNPC->GetNPCInfo()->fRotateSpeed = 10.0f;
			m_pOpponent->SetPos(vec3(400, 100, 0));
			pOpponent2->SetPos(vec3(200, 400, 0));
			pOpponent3->SetPos(vec3(200, 300, 0));
			m_pNPC->doDamage(m_pOpponent->GetUID(), DA_SLASH, 3);
			m_pNPC->doDamage(pOpponent2->GetUID(), DA_SLASH, 2);
			m_pNPC->doDamage(pOpponent3->GetUID(), DA_SLASH, 1);
			GColtAction coltAction;
			coltAction.nType = CAT_AGGRO;
			coltAction.qParams[0] = NPC_XML_ATTR_AGGRO_SHORT;
			coltAction.qParams[1] = L"0";

			CHECK_EQUAL(m_pOpponent->GetUID(), m_pNPC->GetTargetActor()->GetUID());

			m_pColtRunner->RunAction(&coltAction, &GColtActionInst(), m_pNPC, m_pOpponent);

			CHECK(pOpponent3->GetUID() == m_pNPC->GetTargetActor()->GetUID());
		}
	}
	
	TEST_FIXTURE(FColtRunner, Talent)
	{
		GTalentInfo* pTalentInfo = m_Helper.NewMeleeTalentInfo();
		pTalentInfo->m_setNPCID.insert(m_pNPC->GetID());
		wchar_t szTalentID[512];
		_itow(pTalentInfo->m_nID, szTalentID, 10);
		GColtAction coltAction;
		coltAction.nType = CAT_TALENT;		
		coltAction.qParams[0] = szTalentID;		
		coltAction.fDurationTime = -1.0f;
		coltAction.bModifyDurationTime = true;
		GColtActionInst coltInstInfo(coltAction);
		m_pColtRunner->RunAction(&coltAction, &coltInstInfo, m_pNPC, m_pOpponent);

		GJob_UseTalent* pJob = (GJob_UseTalent*) m_pNPC->GetMissionJobs()[1]; // 0번은 이동 탤런트
		CHECK_EQUAL(GJOB_TALENT, pJob->GetID());
		CHECK_EQUAL(pTalentInfo->m_nID, pJob->GetTalentInfo()->m_nID);
	}

	TEST_FIXTURE(FColtRunner, Guard)
	{
		GTalentInfo* pTalentInfo = m_Helper.NewGuardTalentInfo();
		pTalentInfo->m_setNPCID.insert(m_pNPC->GetID());
		wchar_t szTalentID[512];
		_itow(pTalentInfo->m_nID, szTalentID, 10);
		GColtAction coltAction;
		coltAction.nType = CAT_GUARD;
		coltAction.qParams[0] = szTalentID;	// 사용할 탤런트 ID, 자세한건 GColtRunner::OnGuard함수 참고
		coltAction.qParams[1] = L"3";		// 가드를 3초 유지 한다는 뜻임, 자세한건 GColtRunner::OnGuard함수 참고
		m_pColtRunner->RunAction(&coltAction, &GColtActionInst(), m_pNPC, m_pOpponent);
		m_pNPC->GetModuleAI()->Update(0.0f);

		GJobMgr* pJobMgr = m_pNPC->GetJobMgr();		
		CHECK_EQUAL(GJOB_GUARD, pJobMgr->GetCurrJob()->GetID());
		
		m_pNPC->GetModuleAI()->Update(2);
		CHECK_EQUAL(GJOB_GUARD, pJobMgr->GetCurrJob()->GetID());

		m_pNPC->GetModuleAI()->Update(1);
		m_pNPC->GetModuleAI()->Update(1);
		CHECK(NULL == pJobMgr->GetCurrJob());
	}

	TEST_FIXTURE(FColtRunner, Nothing)
	{
		GColtAction coltAction;
		coltAction.nType = CAT_NOTHING;
		coltAction.qParams[0] = L"3";
		m_pColtRunner->RunAction(&coltAction, &GColtActionInst(), m_pNPC, m_pOpponent);
		m_pNPC->GetModuleAI()->Update(0.0f);

		GJobMgr* pJobMgr = m_pNPC->GetJobMgr();
		
		CHECK_EQUAL(GJOB_IDLE, pJobMgr->GetCurrJob()->GetID());

		m_pNPC->GetModuleAI()->Update(2);
		CHECK_EQUAL(GJOB_IDLE, pJobMgr->GetCurrJob()->GetID());

		m_pNPC->GetModuleAI()->Update(1);
		CHECK(NULL == pJobMgr->GetCurrJob());
	}

	TEST_FIXTURE(FColtRunner, Say)
	{
		GColtAction coltAction;
		coltAction.nType = CAT_SAY;
		coltAction.qParams[0] = L"hi";
		coltAction.qParams[1] = L"10";
		m_pColtRunner->RunAction(&coltAction, &GColtActionInst(), m_pNPC, m_pOpponent);
		m_pNPC->GetModuleAI()->Update(0.0f);

		GJobMgr* pJobMgr = m_pNPC->GetJobMgr();
		CHECK_EQUAL(GJOB_SPEAK, pJobMgr->GetCurrJob()->GetID());
	}

	TEST_FIXTURE(FColtRunner, Shout)
	{
		GColtAction coltAction;
		coltAction.nType = CAT_SHOUT;
		coltAction.qParams[0] = L"hi";
		coltAction.qParams[1] = L"10";
		m_pColtRunner->RunAction(&coltAction, &GColtActionInst(), m_pNPC, m_pOpponent);
		m_pNPC->GetModuleAI()->Update(0.0f);

		GJobMgr* pJobMgr = m_pNPC->GetJobMgr();
		CHECK_EQUAL(GJOB_SPEAK, pJobMgr->GetCurrJob()->GetID());
	}

	TEST_FIXTURE(FColtRunner, Narration)
	{
		// 더미 액션
		GColtAction coltAction;
		coltAction.nType = CAT_NOTHING;
		coltAction.qParams[0] = L"3";
		m_pColtRunner->RunAction(&coltAction, &GColtActionInst(), m_pNPC, m_pOpponent);

		coltAction.nType = CAT_NARRATION;
		coltAction.qParams[0] = L"hi";
		coltAction.qParams[1] = L"10";
		m_pColtRunner->RunAction(&coltAction, &GColtActionInst(), m_pNPC, m_pOpponent);
		m_pNPC->GetModuleAI()->Update(0.0f);

		TVALID_EQ(2, m_pNPC->GetMissionJobs().size())
		CHECK_EQUAL(GJOB_SPEAK, m_pNPC->GetMissionJobs()[1]->GetID());	// 2번째 액션
	}

	TEST_FIXTURE(FColtRunner, FaceTo)
	{
		m_pNPC->SetPos(vec3(100, 100, 100));
		m_pNPC->SetDir(vec3(0,0,1));
		m_pOpponent->SetPos(vec3(100, 200, 100));

		GColtAction coltAction;
		coltAction.nType = CAT_FACETO;
		m_pColtRunner->RunAction(&coltAction, &GColtActionInst(), m_pNPC, m_pOpponent);
		m_pNPC->GetModuleAI()->Update(0.0f);

		CHECK_EQUAL(vec3(0,1,0), m_pNPC->GetDir());	
	}

	TEST_FIXTURE(FColtRunner, Script_Global)
	{
		gsys.pScriptManager->Init();

		const char* pszScript = 
			"function TestFunction(NPC, Opponent)\n"
			"	TEST_VAR = 111\n"
			"end\n"
			;
		CHECK(m_Helper.RunLuaString(pszScript));
		CHECK(WLUA->GetVar<int>("TEST_VAR") == 0);		

		GColtAction coltAction;
		coltAction.nType = CAT_LUAFUNC;
		coltAction.qParams[0] = L"TestFunction";
		coltAction.qParams[1] = L"";
		m_pColtRunner->RunAction(&coltAction, &GColtActionInst(), m_pNPC, m_pOpponent);
		m_pNPC->GetModuleAI()->Update(0.0f);

		CHECK(WLUA->GetVar<int>("TEST_VAR") == 111);	

		gsys.pScriptManager->Fini();
	}

	TEST_FIXTURE(FColtRunner, Script_MemberCall)
	{
		gsys.pScriptManager->Init();

		const char* pszScript = 
			"TableName = {}\n"
			"function TableName:MemberFunc(NPC, Opponent)\n"
			"	TEST_VAR = 111\n"
			"end\n"
			;
		CHECK(m_Helper.RunLuaString(pszScript));
		CHECK(WLUA->GetVar<int>("TEST_VAR") == 0);		

		GColtAction coltAction;
		coltAction.nType = CAT_LUAFUNC;
		coltAction.qParams[0] = L"TableName";
		coltAction.qParams[1] = L"MemberFunc";
		m_pColtRunner->RunAction(&coltAction, &GColtActionInst(), m_pNPC, m_pOpponent);
		m_pNPC->GetModuleAI()->Update(0.0f);

		CHECK(WLUA->GetVar<int>("TEST_VAR") == 111);	

		gsys.pScriptManager->Fini();
	}
}
