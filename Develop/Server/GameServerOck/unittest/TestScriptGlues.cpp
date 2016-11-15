#include "stdafx.h"
#include "FBaseScriptGlue.h"
#include "MockField.h"
#include "GModuleAI.h"
#include "GJobMgr.h"
#include "GJob.h"
#include "GJob_MoveToPos.h"
#include "GJob_Roam.h"
#include "GJob_Patrol.h"
#include "GJob_MoveToEntity.h"
#include "MockEntityNPC.h"
#include "GFieldInfoMgr.h"
#include "GTestForward.h"
#include "GJobRunner.h"


const vec3 vCommonPos = vec3(1000.0f, 1000.0f, 0.0f);
const vec3 vCommonDir = vec3(0,1,0);

SUITE(TestScriptGlues)
{
	struct Fixture : public FBaseScriptGlue
	{
		Fixture()			
		{

		}
		virtual ~Fixture()	{}
	};

	TEST_FIXTURE(Fixture, TestInvalidPointerSafeHandle)
	{
		const char* pszScript = 
			"	local Marker = GetMarker(100)\n"	// 무효한 마커 얻기
			"	Marker.a = 7;\n"			// 없는 변수 쓰기
			"	local ma1 = Marker.a;\n"	// 없는 변수 읽기
			"	Marker.ID = 7;\n"			// 존재하는 변수 쓰기
			"	local ma2 = Marker.ID;\n"	// 존재하는 변수 읽기
			"	Marker:IsValid();\n"		// 멤버 함수 함수 호출
			;

		if (!m_Helper.RunLuaString(pszScript))	{ CHECK(FALSE && "WRunString Failed.");	 return; }
	}

	TEST_FIXTURE(Fixture, TestGlue_GetMarker)
	{
		MARKER_INFO MarkerInfo;
		MarkerInfo.nID = 777;
		MarkerInfo.strName = "Tetetest Mamamarker";
		m_pFieldInfo->InsertMarkerInfo(MarkerInfo);

		const char* pszTestFuncName = "_Test_GetMarker";
		const char* pszScript = 
			"function %s()\n"
			"	local Marker = GetMarker(%d, 777, Marker);\n"
			"	return Marker.Name;\n"
			"end";

		if (!m_Helper.RunLuaString(pszScript, pszTestFuncName, m_pFieldInfo->m_nFieldID))	{ CHECK(FALSE && "WRunString Failed.");	 return; }

		string strRet;
		WCALL(WLUA, pszTestFuncName, &strRet);
		CHECK_EQUAL(MarkerInfo.strName, strRet);
	}

	TEST_FIXTURE(Fixture, TestGlue_NPC_Despawn)
	{
		const char* pszTestFuncName = "_Test_NPC_Despawn";
		const char* pszScript = 
			"function %s(NPC)\n"
			"	NPC:Despawn(false);\n"
			"end";

		if (!m_Helper.RunLuaString(pszScript, pszTestFuncName))	{ CHECK(FALSE && "WRunString Failed.");	 return; }

		//공격 탤런트
		const int ATTACK_TALENT_DAMAGE = 100;
		GTalentInfo* pAtkTalent = MakeSimpleDmgAtkTalent(ATTACK_TALENT_DAMAGE);

		GEntityPlayer* pPlayer = GUTHelper::NewEntityPlayer(m_pField, vCommonPos);
		GEntityNPC* pNPC = m_pField->SpawnTestNPC(m_pNPCInfo, vCommonPos, vCommonDir);

		// NPC를 공격해 보상을 받을 자격을 갖춘다.
		pPlayer->doUseTalent(pAtkTalent);
		m_pField->Update(1.0f);	

		// NPC가 디스폰된다.
		MUID uidNPC = pNPC->GetUID();
		int nOldXP = pPlayer->GetPlayerInfo()->nXP;
		CHECK_EQUAL(pNPC->IsDead(), false);
		CHECK_EQUAL(m_pField->FindNPC(uidNPC), pNPC);
		WCALL1(WLUA, pszTestFuncName, pNPC->GetDelegator(), WNULL);
		pNPC->UpdateAI(1.0f);
		m_pField->Update(1.0f);	
		int nNewXP = pPlayer->GetPlayerInfo()->nXP;
		CHECK_EQUAL(m_pField->FindNPC(uidNPC), (GEntityNPC*)NULL);
		CHECK_EQUAL(nOldXP, nNewXP);
	}

	TEST_FIXTURE(Fixture, TestGlue_NPC_Move)
	{
		const char* pszTestFuncName = "_Test_NPC_Move";
		const char* pszScript = 
			"function %s(NPC)\n"
			"	NPC:Move(vec3(742, 512, 0));\n"
			"end";

		if (!m_Helper.RunLuaString(pszScript, pszTestFuncName))	{ CHECK(FALSE && "WRunString Failed.");	 return; }

		GEntityNPC* pNPC = m_pField->SpawnTestNPC(m_pNPCInfo, vCommonPos, vCommonDir);
		WCALL1(WLUA, pszTestFuncName, pNPC->GetDelegator(), WNULL);
		pNPC->UpdateAI(1.0f);
		m_pField->Update(1.0f);		// 잡이 큐에서 Current로 이동된다.
		GModuleAI* pmAI = pNPC->GetModuleAI();
		if (!pmAI) 	{ CHECK(FALSE && "ModuleAI is null.");	 return; }
		
		GJobMgr* pJobMgr = pmAI->GetJobMgr();
		GJob* pJob = pJobMgr->GetCurrJob();
		if (!pJob) 	{ CHECK(FALSE && "Job is null.");	 return; }
		CHECK_EQUAL(pJob->GetID(), GJOB_MOVE_TO_POS);

		GJob_MoveToPos* pMoveJob = static_cast<GJob_MoveToPos*>(pJob);
		CHECK_EQUAL(pMoveJob->GetTarPos().IsEqual(vec3(742, 512, 0)), true);

		pNPC->Destroy();
		SAFE_DELETE(pNPC);
	}

	TEST_FIXTURE(Fixture, TestGlue_NPC_Roam)
	{
		const char* pszTestFuncName = "_Test_NPC_Roam";
		const char* pszScript = 
			"function %s(NPC)\n"
			"	NPC:Roam(1000, 60);\n"
			"end";

		if (!m_Helper.RunLuaString(pszScript, pszTestFuncName))	{ CHECK(FALSE && "WRunString Failed.");	 return; }

		std::auto_ptr<SPAWN_INFO> apSpawninfo(new SPAWN_INFO());
		apSpawninfo->bCheckSpawnPointValidation = false;
		apSpawninfo->vPoint = vec3(1000.0f, 1000.0f, 0.0f);
		apSpawninfo->fRoamRadius = 0;
		apSpawninfo->fRoamDepth = 100;
		apSpawninfo->vecNPCInfoList.push_back(SPAWN_NPC_INFO(m_pNPCInfo->nID));

		GEntityNPC* pNPC = m_pField->SpawnTestNPC(m_pNPCInfo, vCommonPos, vCommonDir, apSpawninfo.get());
		WCALL1(WLUA, pszTestFuncName, pNPC->GetDelegator(), WNULL);
		pNPC->UpdateAI(1.0f);
		m_pField->Update(1.0f);		// 잡이 큐에서 Current로 이동된다.
		GModuleAI* pmAI = pNPC->GetModuleAI();
		if (!pmAI) 	{ CHECK(FALSE && "ModuleAI is null.");	 return; }
		GJobMgr* pJobMgr = pmAI->GetJobMgr();
		GJob* pJob = pJobMgr->GetCurrJob();
		if (!pJob) 	{ CHECK(FALSE && "Job is null.");	 return; }
		CHECK_EQUAL(pJob->GetID(), GJOB_ROAM);
		//		GTaskBroker::PushTask_MoveStraight(pNPC, vec3(0,0,0));	// MoveJob에서 길찾기 실패로 태스크를 넣을 수 없으므로, 임시로 넣어둔다.

		GJob_Roam* pMoveJob = static_cast<GJob_Roam*>(pJob);
		CHECK_EQUAL(pMoveJob->GetRadius(), 1000.0f);
		pNPC->UpdateAI(60.0f);	// Roam 지속시간만큼 시간이 지났다.

		CHECK_EQUAL(pJobMgr->GetCurrJob(), (GJob*)NULL);

		pNPC->Destroy();
		SAFE_DELETE(pNPC);
	}

	TEST_FIXTURE(Fixture, TestGlue_NPC_Patrol)
	{
		const char* pszTestFuncName = "_Test_NPC_Patrol";
		const char* pszScript = 
			"function %s(NPC)\n"
			"	NPC:Patrol({10, 11, 12, 13}, 2);\n"
			"end";

		if (!m_Helper.RunLuaString(pszScript, pszTestFuncName))	{ CHECK(FALSE && "WRunString Failed.");	 return; }

		{
			MARKER_INFO Marker;
			Marker.nFieldID = 100;
			for (float i=0; i<4; i++)
			{
				Marker.nID = 10+(int)i;	Marker.vPoint = vec3(10+i, 10+i, 10+i);	m_pFieldInfo->InsertMarkerInfo(Marker);
			}
		}

		GEntityNPC* pNPC = m_pField->SpawnTestNPC(m_pNPCInfo, vCommonPos, vCommonDir);
		WCALL1(WLUA, pszTestFuncName, pNPC->GetDelegator(), WNULL);
		pNPC->UpdateAI(1.0f);
		m_pField->Update(1.0f);		// 잡이 큐에서 Current로 이동된다.
		GModuleAI* pmAI = pNPC->GetModuleAI();
		if (!pmAI) 	{ CHECK(FALSE && "ModuleAI is null.");	 return; }
		GJobMgr* pJobMgr = pmAI->GetJobMgr();
		GJob* pJob = pJobMgr->GetCurrJob();
		if (!pJob) 	{ CHECK(FALSE && "Job is null.");	 return; }
		CHECK_EQUAL(pJob->GetID(), GJOB_PATROL);

		GJob_Patrol* pMoveJob = static_cast<GJob_Patrol*>(pJob);
		MARKER_INFO Marker;
		int nMarkerQty = 0;
		for (int i=0; i<10; i++)
		{
			if (!pMoveJob->GetCurrentPatrolMarker(Marker))
				break;

			CHECK_EQUAL(Marker.nID, 10+(int)i%4);

			pMoveJob->RunAdvance();
			nMarkerQty++;
		}

		pNPC->Destroy();
		SAFE_DELETE(pNPC);
	}


	TEST_FIXTURE(Fixture, TestGlue_NPC_Follow)
	{
		const char* pszTestFuncName = "_Test_NPC_Follow";
		const char* pszScript = 
			"function %s(NPC, TargetActor)\n"
			"	NPC:Follow(TargetActor, 123);\n"
			"end";

		if (!m_Helper.RunLuaString(pszScript, pszTestFuncName))	{ CHECK(FALSE && "WRunString Failed.");	 return; }

		vec3 vTargetPos(0);
		vTargetPos.x = vCommonPos.x + 1000;
		vTargetPos.y = vCommonPos.y + 2300;
		GEntityPlayer* pPlayer = GUTHelper::NewEntityPlayer(m_pField, vTargetPos);
		GEntityNPC* pNPC = m_pField->SpawnTestNPC(m_pNPCInfo, vCommonPos, vCommonDir);
		WCALL2(WLUA, pszTestFuncName, pNPC->GetDelegator(), pPlayer->GetDelegator(), WNULL);
		pNPC->UpdateAI(1.0f);
		m_pField->Update(1.0f);		// 잡이 큐에서 Current로 이동된다.
		GModuleAI* pmAI = pNPC->GetModuleAI();
		if (!pmAI) 	{ CHECK(FALSE && "ModuleAI is null.");	 return; }
		GJobMgr* pJobMgr = pmAI->GetJobMgr();
		GJob* pJob = pJobMgr->GetCurrJob();
		if (!pJob) 	{ CHECK(FALSE && "Job is null.");	 return; }
		CHECK_EQUAL(pJob->GetID(), GJOB_MOVE_TO_ENTITY);

		GJob_MoveToEntity* pMoveJob = static_cast<GJob_MoveToEntity*>(pJob);
		vec3 vTarPos(0);
		bool bRet = pMoveJob->GetTargetEntityPos(vTarPos);
		CHECK_EQUAL(true, bRet);
		CHECK_EQUAL(true, vTarPos.IsEqual(vTargetPos));

		CHECK_CLOSE(pMoveJob->GetLeastDistance(), 123.0f + DEFAULT_ENTITY_RADIUS, 0.001f);	// actor의 크기를 더함
	}

	TEST_FIXTURE(Fixture, TestGlue_NPC_ClearJob)
	{
		const char* pszTestFuncName = "_Test_NPC_ClearJob";
		const char* pszScript = 
			"function %s(NPC)\n"
			"	NPC:ClearJob();\n"
			"end";

		if (!m_Helper.RunLuaString(pszScript, pszTestFuncName))	{ CHECK(FALSE && "WRunString Failed.");	 return; }

		GEntityNPC* pNPC = m_pField->SpawnTestNPC(m_pNPCInfo, vCommonPos, vCommonDir);
		GModuleAI* pmAI = pNPC->GetModuleAI();
		if (!pmAI) 	{ CHECK(FALSE && "ModuleAI is null.");	 return; }

		pNPC->Move(vec3(0,0,0));
		pNPC->UpdateAI(1.0f);
		m_pField->Update(1.0f);		// 잡이 큐에서 Current로 이동된다.
		
		GJobMgr* pJobMgr = pmAI->GetJobMgr();
		CHECK(pJobMgr->GetCurrJob() != NULL);
		WCALL1(WLUA, pszTestFuncName, pNPC->GetDelegator(), WNULL);
		pmAI->Update(0.0f);
		
		CHECK_EQUAL(pJobMgr->GetCurrJob(), (GJob*)NULL);

		pNPC->Destroy();
		SAFE_DELETE(pNPC);
	}

	//TEST_FIXTURE(Fixture, TestGlue_NPC_ClearMission)
	//{
	//	const char* pszTestFuncName = "_Test_NPC_ClearMission";
	//	const char* pszScript = 
	//		"function %s(NPC)\n"
	//		"	NPC:ClearMission();\n"
	//		"end";

	//	if (!m_Helper.RunLuaString(pszScript, pszTestFuncName))	{ CHECK(FALSE && "WRunString Failed.");	 return; }

	//	GEntityPlayer* pPlayer = GUTHelper::NewEntityPlayer(m_pField, vCommonPos);
	//	GEntityNPC* pNPC = m_pField->SpawnTestNPC(m_pNPCInfo, vCommonPos, VCommonDir);
	//	GModuleAI* pmAI = pNPC->GetModuleAI();
	//	if (!pmAI) 	{ CHECK(FALSE && "ModuleAI is null.");	 return; }

	//	pNPC->Move(vec3(0,0,0));
	//	CHECK(pmAI->GetCurMissionJob() != NULL);
	//	WCALL1(WLUA, pszTestFuncName, pNPC, WNULL);
	//	CHECK(pmAI->GetCurMissionJob() == NULL);

	//	pNPC->Destroy();
	//	SAFE_DELETE(pNPC);
	//}


	TEST_FIXTURE(Fixture, TestGlue_NPC_UseTalent)
	{
		const char* pszTestFuncName = "_Test_NPC_UseTalent";
		const char* pszScript = 
			"function %s(NPC, Target, TalentID)\n"
			"	NPC:UseTalent(TalentID, Target);\n"
			"end";

		if (!m_Helper.RunLuaString(pszScript, pszTestFuncName))	{ CHECK(FALSE && "WRunString Failed.");	 return; }

		//공격 탤런트
		const int ATTACK_TALENT_DAMAGE = 100;
		GTalentInfo* pAtkTalent = MakeSimpleDmgAtkTalent(ATTACK_TALENT_DAMAGE);
		pAtkTalent->m_setNPCID.insert(1);

		GEntityPlayer* pPlayer = GUTHelper::NewEntityPlayer(m_pField, vCommonPos);
		GEntityNPC* pNPC = m_pField->SpawnTestNPC(m_pNPCInfo, vCommonPos, vCommonDir);
		GModuleAI* pmAI = pNPC->GetModuleAI();
		if (!pmAI) 	{ CHECK(FALSE && "ModuleAI is null.");	 return; }

		WCALL3(WLUA, pszTestFuncName, pNPC->GetDelegator(), pPlayer->GetDelegator(), pAtkTalent->m_nID, WNULL);
		
		CHECK(pNPC->HasMissionJob());		
		CHECK_EQUAL(GJOB_MOVE_TO_ENTITY, pNPC->GetMissionJobs()[0]->GetID());
		CHECK_EQUAL(GJOB_TALENT, pNPC->GetMissionJobs()[1]->GetID());
	}

	TEST_FIXTURE(Fixture, TestGlue_NPC_Say)
	{
		const char* pszTestFuncName = "_Test_NPC_Say";
		const char* pszScript = 
			"function %s(NPC)\n"
			"	NPC:Say(\"Test\");\n"
			"end";

		if (!m_Helper.RunLuaString(pszScript, pszTestFuncName))	{ CHECK(FALSE && "WRunString Failed.");	 return; }

		GEntityNPC* pNPC = m_pField->SpawnTestNPC(m_pNPCInfo);
		GEntityPlayer* pPlayer = GUTHelper::NewEntityPlayer(m_pField);

		MockLink* pLink = NewLink(pPlayer);
		WCALL1(WLUA, pszTestFuncName, pNPC->GetDelegator(), WNULL);
		pNPC->GetModuleAI()->Update(0.0f);	// 큐에 있는 잡을 시작

		CHECK_EQUAL(1, pLink->GetCommandCount());
		CHECK_EQUAL(MC_NPC_SAY, pLink->GetCommand(0).GetID());
		CHECK_EQUAL(pNPC->GetUID(), pLink->GetParam<MUID>(0, 0));
		CHECK_EQUAL(pNPC->GetID(), pLink->GetParam<int>(0, 1));
		CHECK(wstring(L"Test") == pLink->GetParam<const wchar_t*>(0, 2));
		CHECK_EQUAL(GConst::SPEAK_MAINTAIN_TIME,	pLink->GetParam<float>(0, 3));
	}


	TEST_FIXTURE(Fixture, TestGlue_NPC_Attack)
	{
		const char* pszTestFuncName = "_Test_NPC_Attack";
		const char* pszScript = 
			"function %s(NPC, Target)\n"
			"	NPC:Attack(Target);\n"
			"end";

		if (!m_Helper.RunLuaString(pszScript, pszTestFuncName))	{ CHECK(FALSE && "WRunString Failed.");	 return; }

		GEntityPlayer* pPlayer = GUTHelper::NewEntityPlayer(m_pField, vCommonPos);
		GEntityNPC* pNPC = m_pField->SpawnTestNPC(m_pNPCInfo, vCommonPos, vCommonDir);
		CHECK_EQUAL(pNPC->GetTargetActor(), (GEntityActor*)NULL);
		WCALL2(WLUA, pszTestFuncName, pNPC->GetDelegator(), pPlayer->GetDelegator(), WNULL);
		pNPC->GetModuleAI()->Update(0.0f);
		pNPC->Update(0.0f);
		CHECK_EQUAL(pNPC->GetTargetActor(), pPlayer);
	}

	TEST_FIXTURE(Fixture, TestGlue_NPC_Assist)
	{
		const char* pszTestFuncName = "_Test_NPC_Assist";
		const char* pszScript = 
			"function %s(NPC, Helpee)\n"
			"	NPC:Assist(Helpee);\n"
			"end";

		if (!m_Helper.RunLuaString(pszScript, pszTestFuncName))	{ CHECK(FALSE && "WRunString Failed.");	 return; }

		GEntityPlayer* pPlayer = GUTHelper::NewEntityPlayer(m_pField, vCommonPos);
		GEntityNPC* pHelpeeNPC = m_pField->SpawnTestNPC(m_pNPCInfo, vCommonPos, vCommonDir);
		pHelpeeNPC->doAttack(pPlayer->GetUID());
		GEntityNPC* pNPC = m_pField->SpawnTestNPC(m_pNPCInfo, vCommonPos, vCommonDir);
		CHECK_EQUAL(pNPC->GetTargetActor(), (GEntityActor*)NULL);
		WCALL2(WLUA, pszTestFuncName, pNPC->GetDelegator(), pHelpeeNPC->GetDelegator(), WNULL);
		pNPC->GetModuleAI()->Update(0.0f);
		CHECK_EQUAL(pNPC->GetTargetActor(), pPlayer);
	}

	TEST_FIXTURE(Fixture, TestGlue_NPC_Yell)
	{
		const char* pszTestFuncName = "_Test_NPC_Yell";
		const char* pszScript = 
			"function %s(NPC)\n"
			"	NPC:Yell(1000);\n"
			"end";

		if (!m_Helper.RunLuaString(pszScript, pszTestFuncName))	{ CHECK(FALSE && "WRunString Failed.");	 return; }

		GEntityNPC* pNPC = new MockEntityNPC();
		pNPC->Create(MUID(1));

		CHECK_EQUAL(static_cast<MockEntityNPC*>(pNPC)->m_nYellRange, 0);
		WCALL1(WLUA, pszTestFuncName, pNPC->GetDelegator(), WNULL);
		pNPC->GetModuleAI()->Update(0.0f);
		CHECK_EQUAL(static_cast<MockEntityNPC*>(pNPC)->m_nYellRange, 1000);

		pNPC->Destroy();
		SAFE_DELETE(pNPC);
	}

	TEST_FIXTURE(Fixture, TestGlue_ScriptSelf)
	{
		const char* pszTestFuncName = "_Test_ScriptSelf";
		const char* pszScript = 
			"function %s(NPC, strText)\n"
			"	NPC:ScriptSelf(strText)\n"
			"end\n"
			"function DespawnSelf(Self, Opponent)\n"
			"	Self:Despawn(false)\n"
			"end\n"
			;

		if (!m_Helper.RunLuaString(pszScript, pszTestFuncName))	{ CHECK(FALSE && "WRunString Failed.");	 return; }

		GEntityNPC* pNPC = m_pField->SpawnTestNPC(m_pNPCInfo, vCommonPos, vCommonDir);
		CHECK_EQUAL(m_pField->GetEntityQty(ETID_NPC), 1);
		WCALL2(WLUA, pszTestFuncName, pNPC->GetDelegator(), "DespawnSelf", WNULL);
		pNPC->UpdateAI(0.0f);
		pNPC->UpdateAI(0.0f);
		m_pField->Update(0.0f);	// 디스폰이 처리될 틱
		CHECK_EQUAL(m_pField->GetEntityQty(ETID_NPC), 0);
	}

	TEST_FIXTURE(Fixture, TestGlue_IsCooldown)
	{
		// arrange
		const char* pszTestFuncName = "_Test_ScriptSelf";
		const char* pszScript = 
			"function %s(NPC, TalentID)\n"
			"	return NPC:IsCooldown(TalentID)\n"
			"end\n"
			;

		if (!m_Helper.RunLuaString(pszScript, pszTestFuncName))	{ CHECK(FALSE && "WRunString Failed.");	 return; }

		GEntityNPC* pNPC = m_pField->SpawnTestNPC(m_pNPCInfo, vCommonPos, vCommonDir);		
		GTalentInfo* pActiveTalent = test.talent.NewTalentInfo();
		pActiveTalent->m_fCoolTime = 10.0f;		

		// act/assert
		bool isCooldown = false;
		WCALL2(WLUA, pszTestFuncName, pNPC->GetDelegator(), pActiveTalent->m_nID, &isCooldown);
		CHECK(!isCooldown);
		
		// act/assert
		test.talent.UseTalent(pNPC, pActiveTalent, pNPC->GetUID());
		WCALL2(WLUA, pszTestFuncName, pNPC->GetDelegator(), pActiveTalent->m_nID, &isCooldown);
		CHECK(isCooldown);

		// act/assert
		pNPC->Update(10.0f);				
		WCALL2(WLUA, pszTestFuncName, pNPC->GetDelegator(), pActiveTalent->m_nID, &isCooldown);
		CHECK(!isCooldown);
	}

	// Ticket #2194
	TEST_FIXTURE(Fixture, TestGlue_CrashCase_2194)
	{
		const char* pszTestFuncName = "_Test_ScriptSelf";
		const char* pszScript = 
			"function TestFuncSelf(Self, Opponent)\n"
			"	Self:DistanceTo(Opponent)\n"
			"end\n"
			;

		if (!m_Helper.RunLuaString(pszScript, pszTestFuncName))	{ CHECK(FALSE && "WRunString Failed.");	 return; }

		GEntityNPC* pNPC = m_pField->SpawnTestNPC(m_pNPCInfo, vCommonPos, vCommonDir);
		GEntityNPC* pNPC2 = m_pField->SpawnTestNPC(m_pNPCInfo, vCommonPos, vCommonDir);
		GJobRunner job_runner;
		job_runner.Idle(pNPC->GetModuleAI(), 0.0f);
		job_runner.RunScript(pNPC->GetModuleAI(), pNPC2->GetUID(), L"", L"TestFuncSelf");
		pNPC2->DeleteMe();
		m_pField->Update(0.0f);
		m_pField->Update(0.0f);
	}
}
