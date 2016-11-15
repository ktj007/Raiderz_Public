#include "stdafx.h"
#include "FBaseScriptCallback.h"
#include "GModuleAI.h"
#include "GModuleCombat.h"
#include "GNPCSwallow.h"
#include "GScriptGroup_NPC.h"
#include "GNPCScriptBuilder.h"
#include "CSFactionInfo.h"
#include "GFactionRelationInfo.h"
#include "GNPCInteraction.h"
#include "GHateTable.h"
#include "GNPCBParts.h"

const vec3 vCommonPos = vec3(1000.0f, 1000.0f, 0.0f);
const vec3 vCommonDir = vec3(0,1,0);

SUITE(ScriptCallback_NPC)
{
	struct Fixture: public FBaseScriptCallback
	{
		GTestMgrWrapper<GFactionRelationInfoMgr>	m_FactionRelationInfoMgr;

		Fixture()			{}				
		virtual ~Fixture()	{}
	};

	TEST_FIXTURE(Fixture, HitCapsule)
	{
		GEntityNPC* pNPC = m_pField->SpawnTestNPC(m_pNPCInfo, vCommonPos, vCommonDir);
		GEntityPlayer* pPlayer = GUTHelper::NewEntityPlayer(m_pField, vCommonPos);

		//공격 탤런트
		const int ATTACK_TALENT_DAMAGE = 100;
		GTalentInfo* pAktTalent = MakeSimpleDmgAtkTalent(ATTACK_TALENT_DAMAGE);

		m_pNPCInfo->m_pMeshInfo = new GActorMeshInfo();
		pNPC->GetMeshInfo()->ReserveHitGroupForTest(1);

		const char* pszScript = 
			"function NPC_1:OnHitCapsule_0_1(this, CombatInfo)\n"
			"	CombatInfo.Damage = CombatInfo.Damage * 2;\n"
			"	CombatInfo.MF:Get(MF_KNOCKDOWN).Value = 50;\n"
			"	CombatInfo.MF:Adjust(MF_KNOCKDOWN, 50, 100);\n"
			"	CombatInfo.Attacker:Warp(vec3(1,2,3));\n"
			"	return CombatInfo;\n"
			"end";

		if (!RunLuaString(pszScript))	{ CHECK(FALSE && "WRunString Failed.");	 return; }
		CHECK_EQUAL(pNPC->GetHP(), 50000);

		GHitInfo infoHit;
		infoHit.nDamage = 100;
		infoHit.pTalentInfo = pAktTalent;
		infoHit.pAttacker = pPlayer;
		infoHit.pVictim = pNPC;
		infoHit.pMotionFactorGroup = &pAktTalent->m_MotionFactorGroup;
		
		// 캡슐 지정없이 일반 공격
		infoHit.nCapsuleGroup = INVALID_ID;
		infoHit.nCapsuleIndex = INVALID_ID;
		pNPC->OnHit(infoHit, false);
		CHECK_EQUAL(pNPC->GetHP(), 50000-(100*1));

		// 캡슐 지정해서 공격 (루아 호출)
		CHECK_EQUAL(pNPC->GetCurrentMotionFactor(), MF_NONE);
		CHECK_EQUAL(pPlayer->GetPos().IsEqual(vCommonPos), true);
		CHECK_EQUAL(pNPC->GetPos().IsEqual(vCommonPos), true);
		pNPC->SetHP(50000);
		infoHit.nCapsuleGroup = 0;
		infoHit.nCapsuleIndex = 1;
		pNPC->OnHit(infoHit, false);	

		CHECK_EQUAL(pNPC->GetHP(), 50000-(100*2));
		CHECK_EQUAL(pNPC->GetCurrentMotionFactor(), MF_KNOCKDOWN);
		CHECK_EQUAL(pPlayer->GetPos().IsEqual(vec3(1,2,3)), true);

		SAFE_DELETE(m_pNPCInfo->m_pMeshInfo);
	}

	TEST_FIXTURE(Fixture, NPC_OnSwallowBegin)
	{
		USES_CONVERSION_EX;

		// 먹는 NPC
		GNPCInfo		SwallowedNPCInfo;
		SwallowedNPCInfo.nID = 1;
		SwallowedNPCInfo.nMaxHP = 50000;
		SwallowedNPCInfo.nEatOwnerBuffID = 560;
		SwallowedNPCInfo.nEatTargetBuffID = 561;
		SwallowedNPCInfo.fEatPeriod = 2.0f;
		char szBuff[128];
		sprintf_s(szBuff, "NPC_%d", SwallowedNPCInfo.nID);
		SwallowedNPCInfo.strNameInLua = A2W_EX(szBuff, 0);

		GEntityNPC* pNPC = m_pField->SpawnTestNPC(&SwallowedNPCInfo, vCommonPos, vCommonDir);
		GEntityPlayer* pPlayer = GUTHelper::NewEntityPlayer(m_pField, vCommonPos);

		// 먹기 탤런트
		GTalentInfo*	pSwallowedTalentInfo;
		pSwallowedTalentInfo = m_Helper.NewTalentInfo();
		pSwallowedTalentInfo->m_nSkillType = ST_EXTRA_ACTIVE;
		pSwallowedTalentInfo->m_nExtraActive = TEAT_NPC_ONLY_EAT;
		pSwallowedTalentInfo->m_nTiming = TC_HIT_TALENT;
		pSwallowedTalentInfo->m_EffectInfo.m_fRadius = 1.0f;
		pSwallowedTalentInfo->m_fDurationTime = 2.0f;		// aitime 2초
		pSwallowedTalentInfo->m_fExtraPhaseTime = 2.0f;		// 잡은 후 2초 걸림
		pSwallowedTalentInfo->m_nForceMF = MF_SWALLOWED;
		pSwallowedTalentInfo->m_nForceMFWeight = 2000;
		pSwallowedTalentInfo->Cooking();
		m_Helper.SetTalentDamage(pSwallowedTalentInfo, 100);		// 데미지 100로 설정
		m_Helper.AddSimpleHitInfo(pSwallowedTalentInfo, 0.0f);	// 바로 판정하는 탤런트임

		const char* pszScript = 
			"function NPC_1:OnSwallowBegin(this, TargetActor)\n"
			"TEST_VAR = 777\n"
			"end";

		if (!RunLuaString(pszScript))	{ CHECK(FALSE && "WRunString Failed.");	 return; }
		CHECK_EQUAL(pNPC->GetHP(), 50000);

		// 먹은 상태에서 공격 (루아 호출)
		TALENT_TARGET_INFO target_info;
		target_info.uidTarget = pPlayer->GetUID();
		CHECK_EQUAL(false, WLUA->IsExistGlobal("TEST_VAR"));
		pNPC->GetModuleCombat()->UseTalent(pSwallowedTalentInfo, target_info, false);
		m_pField->Update(0.0f);
		CHECK_EQUAL(WLUA->GetVar<int>("TEST_VAR"), 777);

		SAFE_DELETE(m_pNPCInfo->m_pMeshInfo);
	}

	TEST_FIXTURE(Fixture, NPC_OnHitSwallowed)
	{
		USES_CONVERSION_EX;

		// 먹는 NPC
		GNPCInfo		SwallowedNPCInfo;
		SwallowedNPCInfo.nID = 1;
		SwallowedNPCInfo.nMaxHP = 50000;
		SwallowedNPCInfo.nEatOwnerBuffID = 560;
		SwallowedNPCInfo.nEatTargetBuffID = 561;
		SwallowedNPCInfo.fEatPeriod = 2.0f;
		char szBuff[128];
		sprintf_s(szBuff, "NPC_%d", SwallowedNPCInfo.nID);
		SwallowedNPCInfo.strNameInLua = A2W_EX(szBuff, 0);
		
		GEntityNPC* pNPC = m_pField->SpawnTestNPC(&SwallowedNPCInfo, vCommonPos, vCommonDir);
		GEntityPlayer* pPlayer = GUTHelper::NewEntityPlayer(m_pField, vCommonPos);

		//공격 탤런트
		const int ATTACK_TALENT_DAMAGE = 100;
		GTalentInfo* pAktTalent = MakeSimpleDmgAtkTalent(ATTACK_TALENT_DAMAGE);

		// 먹기 탤런트
		GTalentInfo*	pSwallowedTalentInfo;
		pSwallowedTalentInfo = m_Helper.NewTalentInfo();
		pSwallowedTalentInfo->m_nSkillType = ST_EXTRA_ACTIVE;
		pSwallowedTalentInfo->m_nExtraActive = TEAT_NPC_ONLY_EAT;
		pSwallowedTalentInfo->m_nTiming = TC_HIT_TALENT;
		pSwallowedTalentInfo->m_fDurationTime = 2.0f;		// aitime 2초
		pSwallowedTalentInfo->m_fExtraPhaseTime = 2.0f;		// 잡은 후 2초 걸림
		pSwallowedTalentInfo->m_nForceMF = MF_SWALLOWED;
		pSwallowedTalentInfo->m_nForceMFWeight = 2000;
		pSwallowedTalentInfo->m_EffectInfo.m_nRelation = CSEffectInfo::RELATION_ENEMY;
		pSwallowedTalentInfo->m_EffectInfo.m_fRadius = 1.0f;
		pSwallowedTalentInfo->Cooking();
		m_Helper.SetTalentDamage(pSwallowedTalentInfo, 100);		// 데미지 100로 설정
		m_Helper.AddSimpleHitInfo(pSwallowedTalentInfo, 0.0f);	// 바로 판정하는 탤런트임

		const char* pszScript = 
			"function NPC_1:OnHitSwallowed(this, CombatInfo)\n"
			"	CombatInfo.Damage = CombatInfo.Damage * 2;\n"
			"	CombatInfo.MF:Get(MF_KNOCKDOWN).Value = 50;\n"
			"	CombatInfo.MF:Adjust(MF_KNOCKDOWN, 50, 100);\n"
			"	return CombatInfo;\n"
			"end";

		if (!RunLuaString(pszScript))	{ CHECK(FALSE && "WRunString Failed.");	 return; }
		CHECK_EQUAL(pNPC->GetHP(), 50000);

		GHitInfo infoHit;
		infoHit.nDamage = 100;
		infoHit.pTalentInfo = pAktTalent;
		infoHit.pAttacker = pPlayer;
		infoHit.pVictim = pNPC;
		infoHit.pMotionFactorGroup = &pAktTalent->m_MotionFactorGroup;

		// 일반 공격
		pNPC->OnHit(infoHit, false);
		CHECK_EQUAL(pNPC->GetHP(), 50000-(100*1));

		// 먹은 상태에서 공격 (루아 호출)
		TALENT_TARGET_INFO target_info;
		target_info.uidTarget = pPlayer->GetUID();
		CHECK_EQUAL(true, pNPC->GetNPCSwallow().IsSwallowable());
		pNPC->GetModuleCombat()->UseTalent(pSwallowedTalentInfo, target_info, false);
		m_pField->Update(0.0f);
		CHECK_EQUAL(false, pNPC->GetNPCSwallow().IsSwallowable());
		pNPC->OnHit(infoHit, false);
		CHECK_EQUAL(pNPC->GetHP(), 50000-(100*3));
		CHECK_EQUAL(pNPC->GetCurrentMotionFactor(), MF_KNOCKDOWN);

		SAFE_DELETE(m_pNPCInfo->m_pMeshInfo);

		// NPC 삭제
		pNPC->DeleteMe();
		m_pField->Update(0.0f);
	}

	TEST_FIXTURE(Fixture, NPC_OnSpawnAndDie)
	{
		// Info: OnSpawn() 콜백은 삭제 예정
		SPAWN_INFO* pSpawninfo = new SPAWN_INFO();
		pSpawninfo->nID = 1122;
		pSpawninfo->strName = "1122";
		pSpawninfo->bCheckSpawnPointValidation = false;
		pSpawninfo->vecNPCInfoList.push_back(SPAWN_NPC_INFO(1));
		pSpawninfo->vPoint = vec3(1234.1f, 5678.2f, 9012.3f);

		string strTableName = MAKE_LUATABLE_FIELD(m_pFieldInfo->m_nFieldID);
		MAKE_TABLE(strTableName.c_str());

		const char* pszScript = 
			"function NPC_1:OnSpawn(this, SpawnInfo)\n"
			"	NPC_VAR = 1;\n"
			"	EXPECT_SpawnID = SpawnInfo.SpawnID;\n"
			"	EXPECT_NPC = SpawnInfo.NPC;\n"
			"	EXPECT_Field = SpawnInfo.Field;\n"
			"end\n"
			"function NPC_1:OnDie(this, DespawnInfo)\n"
			"	NPC_VAR = 2;\n"
			"	EXPECT_SpawnID2 = DespawnInfo.SpawnID;\n"
			"	EXPECT_NPC2 = DespawnInfo.NPC;\n"
			"	EXPECT_Field2 = DespawnInfo.Field;\n"
			"end\n"
			;

		if (!RunLuaString(pszScript))	{ CHECK(FALSE && "WRunString Failed.");	 return; }
				
		GEntityNPC* pNPC = m_pField->SpawnTestNPC(m_pNPCInfo, vCommonPos, vCommonDir, pSpawninfo);

		if (!WLUA->IsExistGlobal("NPC_VAR"))	{ CHECK(FALSE && "npc callback not called\n"); return; }

		CHECK_EQUAL(WLUA->GetVar<int>("NPC_VAR"), 1);
		CHECK_EQUAL(WLUA->GetVar<int>("EXPECT_SpawnID"), 1122);
		CHECK_EQUAL(WLUA->GetVar<GGlueNPC*>("EXPECT_NPC"), pNPC->GetDelegator());
		CHECK_EQUAL(WLUA->GetVar<GGlueField*>("EXPECT_Field"), m_pField->GetDelegator());
		pNPC->doDie();
		CHECK_EQUAL(WLUA->GetVar<int>("NPC_VAR"), 2);
		CHECK_EQUAL(WLUA->GetVar<int>("EXPECT_SpawnID2"), 1122);
		CHECK_EQUAL(WLUA->GetVar<GGlueNPC*>("EXPECT_NPC2"), pNPC->GetDelegator());
		CHECK_EQUAL(WLUA->GetVar<GGlueField*>("EXPECT_Field2"), m_pField->GetDelegator());

		pNPC->Destroy();
		SAFE_DELETE(pNPC);
		SAFE_DELETE(pSpawninfo);
	}

	TEST_FIXTURE(Fixture, NPC_OnUserdataChanged)
	{
		const char* pszScript = 
			"function NPC_1:Init(ID)\n"
			"	NPC_ReserveUserData(ID, 3);\n"
			"end\n"
			"function NPC_1:OnUserdataChanged(NPC, index, data)\n"
			"	NPC:SetUserData(index+1, data+5);\n"
			"end\n"
			;

		if (!RunLuaString(pszScript))	{ CHECK(FALSE && "WRunString Failed.");	 return; }

		GetScriptNPC()->OnInit(m_pNPCInfo);

		GEntityNPC* pNPC = m_pField->SpawnTestNPC(m_pNPCInfo, vCommonPos, vCommonDir);

		pNPC->SetUserdata(1, 6);
		CHECK_EQUAL(pNPC->GetUserdata(1), 6);
		CHECK_EQUAL(pNPC->GetUserdata(1+1), 6+5);

		pNPC->Destroy();
		SAFE_DELETE(pNPC);
	}

	TEST_FIXTURE(Fixture, NPC_Yell)
	{
		// NPC 셋팅
		m_pNPCInfo->m_nFactionLossID = 1;

		// 우후적인 NPC 생성
		GNPCInfo* pNPCHelperInfo = new GNPCInfo();
		pNPCHelperInfo->strNameInLua = L"NPC_Helper";
		pNPCHelperInfo->nID = 2;
		pNPCHelperInfo->m_nFactionLossID = 2;
		gmgr.pNPCInfoMgr->Insert(pNPCHelperInfo);

		GNPCScriptBuilder nsb(pNPCHelperInfo);
		nsb.Build();

		GFactionRelationInfo* pFactionRelationInfo_Excellent = new GFactionRelationInfo(m_pNPCInfo->m_nFactionLossID, pNPCHelperInfo->m_nFactionLossID, FRT_EXCELLENT);
		gmgr.pFactionRelationInfoMgr->Insert(pFactionRelationInfo_Excellent);
		
		GEntityNPC* pRequester = m_pField->SpawnTestNPC(m_pNPCInfo, vCommonPos, vCommonDir);
		GEntityNPC* pHelper = m_pField->SpawnTestNPC(pNPCHelperInfo, vCommonPos, vCommonDir);

		const char* pszScript = 
			"function NPC_Helper:OnYell(NPC, Requester, Target)\n"
			"	TEST_VAR = 1;\n"			
			"end\n";
		if (!RunLuaString(pszScript))	{ CHECK(FALSE && "WRunString Failed.");	 return; }

		// 우호도가 나쁜 적을 생성
		GNPCInfo* pNPCOpponentInfo = new GNPCInfo();
		pNPCOpponentInfo->nID = 3;
		pNPCOpponentInfo->m_nFactionLossID = 3;
		gmgr.pNPCInfoMgr->Insert(pNPCOpponentInfo);

		GFactionRelationInfo* pFactionRelationInfo_Worst = new GFactionRelationInfo(m_pNPCInfo->m_nFactionLossID, pNPCOpponentInfo->m_nFactionLossID, FRT_WORST);
		gmgr.pFactionRelationInfoMgr->Insert(pFactionRelationInfo_Worst);

		GEntityNPC* pOpponent = m_pField->SpawnTestNPC(pNPCOpponentInfo, vCommonPos, vCommonDir);
		pRequester->doDamage(pOpponent->GetUID(), DA_SLASH, 1);		

		// 도움 요청
		int nRange = 1000;
		pRequester->doYell(nRange);
		CHECK_EQUAL(WLUA->GetVar<int>("TEST_VAR"), 1);

		pRequester->Destroy();
		SAFE_DELETE(pRequester);
		pHelper->Destroy();
		SAFE_DELETE(pHelper);
	}

	TEST_FIXTURE(Fixture, NPC_Yell_Boundary_Range)
	{
		// NPC 셋팅
		m_pNPCInfo->m_nFactionLossID = 1;

		// 우후적인 NPC 생성
		GNPCInfo* pNPCHelperInfo = new GNPCInfo();
		pNPCHelperInfo->strNameInLua = L"NPC_Helper";
		pNPCHelperInfo->nID = 2;
		pNPCHelperInfo->m_nFactionLossID = 2;
		gmgr.pNPCInfoMgr->Insert(pNPCHelperInfo);

		GNPCScriptBuilder nsb(pNPCHelperInfo);
		nsb.Build();

		vec3 vHelperPos = vCommonPos;
		vHelperPos.x += 800;

		GFactionRelationInfo* pFactionRelationInfo_Excellent = new GFactionRelationInfo(m_pNPCInfo->m_nFactionLossID, pNPCHelperInfo->m_nFactionLossID, FRT_EXCELLENT);
		gmgr.pFactionRelationInfoMgr->Insert(pFactionRelationInfo_Excellent);

		GEntityNPC* pRequester = m_pField->SpawnTestNPC(m_pNPCInfo, vCommonPos, vCommonDir);
		GEntityNPC* pHelper = m_pField->SpawnTestNPC(pNPCHelperInfo, vHelperPos, vCommonDir);

		const char* pszScript = 
			"function NPC_Helper:OnYell(NPC, Requester, Target)\n"
			"	TEST_VAR = 1;\n"			
			"end\n";
		if (!RunLuaString(pszScript))	{ CHECK(FALSE && "WRunString Failed.");	 return; }

		// 우호도가 나쁜 적을 생성
		GNPCInfo* pNPCOpponentInfo = new GNPCInfo();
		pNPCOpponentInfo->nID = 3;
		pNPCOpponentInfo->m_nFactionLossID = 3;
		gmgr.pNPCInfoMgr->Insert(pNPCOpponentInfo);

		GFactionRelationInfo* pFactionRelationInfo_Worst = new GFactionRelationInfo(m_pNPCInfo->m_nFactionLossID, pNPCOpponentInfo->m_nFactionLossID, FRT_WORST);
		gmgr.pFactionRelationInfoMgr->Insert(pFactionRelationInfo_Worst);

		GEntityNPC* pOpponent = m_pField->SpawnTestNPC(pNPCOpponentInfo, vCommonPos, vCommonDir);
		pRequester->doDamage(pOpponent->GetUID(), DA_SLASH, 1);		

		// 도움 요청
		int nBoundaryRange = 800;
		pRequester->doYell(nBoundaryRange);
		CHECK_EQUAL(WLUA->GetVar<int>("TEST_VAR"), 1);

		pRequester->Destroy();
		SAFE_DELETE(pRequester);
		pHelper->Destroy();
		SAFE_DELETE(pHelper);
	}

	TEST_FIXTURE(Fixture, NPC_Yell_Failure_ShortRange)
	{
		// NPC 셋팅
		m_pNPCInfo->m_nFactionLossID = 1;

		// 우후적인 NPC 생성
		GNPCInfo* pNPCHelperInfo = new GNPCInfo();
		pNPCHelperInfo->strNameInLua = L"NPC_Helper";
		pNPCHelperInfo->nID = 2;
		pNPCHelperInfo->m_nFactionLossID = 2;
		gmgr.pNPCInfoMgr->Insert(pNPCHelperInfo);

		GNPCScriptBuilder nsb(pNPCHelperInfo);
		nsb.Build();

		vec3 vHelperPos = vCommonPos;
		vHelperPos.x += 1000;

		GFactionRelationInfo* pFactionRelationInfo_Excellent = new GFactionRelationInfo(m_pNPCInfo->m_nFactionLossID, pNPCHelperInfo->m_nFactionLossID, FRT_EXCELLENT);
		gmgr.pFactionRelationInfoMgr->Insert(pFactionRelationInfo_Excellent);

		GEntityNPC* pRequester = m_pField->SpawnTestNPC(m_pNPCInfo, vCommonPos, vCommonDir);
		GEntityNPC* pHelper = m_pField->SpawnTestNPC(pNPCHelperInfo, vHelperPos, vCommonDir);

		const char* pszScript = 
			"function NPC_Helper:OnYell(NPC, Requester, Target)\n"
			"	TEST_VAR = 1;\n"			
			"end\n";
		if (!RunLuaString(pszScript))	{ CHECK(FALSE && "WRunString Failed.");	 return; }

		// 우호도가 나쁜 적을 생성
		GNPCInfo* pNPCOpponentInfo = new GNPCInfo();
		pNPCOpponentInfo->nID = 3;
		pNPCOpponentInfo->m_nFactionLossID = 3;
		gmgr.pNPCInfoMgr->Insert(pNPCOpponentInfo);

		GFactionRelationInfo* pFactionRelationInfo_Worst = new GFactionRelationInfo(m_pNPCInfo->m_nFactionLossID, pNPCOpponentInfo->m_nFactionLossID, FRT_WORST);
		gmgr.pFactionRelationInfoMgr->Insert(pFactionRelationInfo_Worst);

		GEntityNPC* pOpponent = m_pField->SpawnTestNPC(pNPCOpponentInfo, vCommonPos, vCommonDir);
		pRequester->doDamage(pOpponent->GetUID(), DA_SLASH, 1);		

		// 도움 요청
		int nRange = 800;
		pRequester->doYell(nRange);
		CHECK_EQUAL(WLUA->GetVar<int>("TEST_VAR"), 0);

		pRequester->Destroy();
		SAFE_DELETE(pRequester);
		pHelper->Destroy();
		SAFE_DELETE(pHelper);
	}

	TEST_FIXTURE(Fixture, NPC_Aggro)
	{
		GEntityNPC* pAttacker = m_pField->SpawnTestNPC(m_pNPCInfo, vCommonPos, vCommonDir);
		GEntityNPC* pDefender = m_pField->SpawnTestNPC(m_pNPCInfo, vCommonPos, vCommonDir);

		const char* pszScript = 
			"function NPC_1:OnAggro(NPC, Target)\n"
			"	TEST_VAR = 1;\n"			
			"end\n";
		if (!RunLuaString(pszScript))	{ CHECK(FALSE && "WRunString Failed.");	 return; }

		// 공격
		pAttacker->doDamage(pDefender->GetUID(), DA_SLASH, 1);		
		pAttacker->GetModuleAI()->Update(0.0f);
		CHECK_EQUAL(WLUA->GetVar<int>("TEST_VAR"), 1);
		
		pAttacker->Destroy();
		SAFE_DELETE(pAttacker);
		pDefender->Destroy();
		SAFE_DELETE(pDefender);
	}

	TEST_FIXTURE(Fixture, NPC_OnTimer)
	{
		const char* pszScript = 
			"function NPC_1:OnTimer(NPC, TimerID)\n"
			"	TEST_VAR = TimerID;\n"
			"end\n"
			;

		if (!RunLuaString(pszScript))	{ CHECK(FALSE && "WRunString Failed.");	 return; }

		m_pNPCInfo->Cooking();

		GetScriptNPC()->OnInit(m_pNPCInfo);

		GEntityNPC* pNPC = m_pField->SpawnTestNPC(m_pNPCInfo, vCommonPos, vCommonDir);
		
		// 일회성 콜백
		pNPC->SetTimer(707, 100.0f, false);
		CHECK(WLUA->GetVar<int>("TEST_VAR") != 707);	// 시간이 안지났다
		pNPC->Update(100.0f);
		CHECK_EQUAL(WLUA->GetVar<int>("TEST_VAR"), 707);	// 시간이 지나서 호출됨
		WRUNSTRING(WLUA, "TEST_VAR = 0");		// 테스트값 초기화
		pNPC->Update(100.0f);
		CHECK(WLUA->GetVar<int>("TEST_VAR") != 707);	// 만료됐다

		// 반복 콜백
		pNPC->SetTimer(707, 100.0f, true);
		m_pField->Update(100.0f);
		CHECK_EQUAL(WLUA->GetVar<int>("TEST_VAR"), 707);	// 시간이 지나서 호출됨
		m_pField->Update(100.0f);
		CHECK_EQUAL(WLUA->GetVar<int>("TEST_VAR"), 707);	// 반복되서 호출됨
		WRUNSTRING(WLUA, "TEST_VAR = 0");		// 테스트값 초기화
		pNPC->KillTimer(707);
		m_pField->Update(100.0f);
		CHECK(WLUA->GetVar<int>("TEST_VAR") != 707);	// 만료됐다

		pNPC->Destroy();
		SAFE_DELETE(pNPC);
	}

	TEST_FIXTURE(Fixture, NPC_OnInteract)
	{
		const char* pszScript = 
			"function NPC_1:OnInteract(Player, NPC)\n"
			"	TEST_VAR = 101;\n"
			"end\n"
			;

		if (!RunLuaString(pszScript))	{ CHECK(FALSE && "WRunString Failed.");	 return; }

		GEntityPlayer* pPlayer = GUTHelper::NewEntityPlayer(m_pField);
		GEntityNPC* pNPC = m_pField->SpawnTestNPC(m_pNPCInfo, vCommonPos, vCommonDir);

		pNPC->GetNPCInteraction().Begin(pPlayer);

		CHECK(WLUA->GetVar<int>("TEST_VAR") == 101);
	}


	TEST_FIXTURE(Fixture, NPC_OnTryHit)
	{
		const char* pszScript = 
			"function NPC_1:OnTryHit(Actor, TalentID)\n"
			"	TEST_VAR = 312;\n"
			"end\n"
			;

		if (!RunLuaString(pszScript))	{ CHECK(FALSE && "WRunString Failed.");	 return; }

		GEntityNPC* pNPC = m_pField->SpawnTestNPC(m_pNPCInfo, vCommonPos, vCommonDir);
		GEntityPlayer* pPlayer = GUTHelper::NewEntityPlayer(m_pField);
		GTalentInfo* pTInfo = m_Helper.NewTalentInfo();

		pNPC->doTryHit(pPlayer, pTInfo, 0, 0, 0, 0);

		CHECK(WLUA->GetVar<int>("TEST_VAR") == 312);
	}

	TEST_FIXTURE(Fixture, NPC_OnChangeMode)
	{
		const char* pszScript = 
			"function NPC_1:OnChangeMode(BeforeMode, Mode)\n"
			"	TEST_VAR = 312;\n"
			"end\n"
			;

		if (!RunLuaString(pszScript))	{ CHECK(FALSE && "WRunString Failed.");	 return; }

		GEntityNPC* pNPC = m_pField->SpawnTestNPC(m_pNPCInfo, vCommonPos, vCommonDir);

		GNPCInfo* pModeNPCInfo = new GNPCInfo();
		pModeNPCInfo->nID = 1000;
		pModeNPCInfo->strNameInLua = pNPC->GetNPCInfo()->strNameInLua;
		m_pNPCInfo->vecModes.push_back(pModeNPCInfo);
		pNPC->ChangeMode(NPC_MODE_1);

		CHECK(WLUA->GetVar<int>("TEST_VAR") == 312);
	}

	TEST_FIXTURE(Fixture, NPC_OnTalentEnd)
	{
		const char* pszScript = 
			"function NPC_1:OnTalentEnd(TalentID)\n"
			"	TEST_VAR = 312;\n"
			"end\n"
			;

		if (!RunLuaString(pszScript))	{ CHECK(FALSE && "WRunString Failed.");	 return; }

		GEntityNPC* pNPC = m_pField->SpawnTestNPC(m_pNPCInfo, vCommonPos, vCommonDir);

		GTalentInfo* pTalentInfo = m_Helper.NewTalentInfo();
		pTalentInfo->m_setNPCID.insert(pNPC->GetID());

		pNPC->UseTalentSelf(pTalentInfo->m_nID);
		pNPC->UpdateAI(10.0f);
		m_pField->Update(10.0f);

		CHECK(WLUA->GetVar<int>("TEST_VAR") == 312);
	}

	TEST_FIXTURE(Fixture, NPC_OnVictory)
	{
		const char* pszScript = 
			"function NPC_1:OnVictory()\n"
			"	TEST_VAR = 312;\n"
			"end\n"
			;

		if (!RunLuaString(pszScript))	{ CHECK(FALSE && "WRunString Failed.");	 return; }

		GEntityNPC* pNPC = m_pField->SpawnTestNPC(m_pNPCInfo, vCommonPos, vCommonDir);
		pNPC->GetHateTable().EnableTimeout(true);

		// 전투를 시작하고 종료
		GModuleAI* pmAI = pNPC->GetModuleAI();
		GEntityPlayer* pEnemy = test.player.NewPlayer(m_pField);
		pNPC->doAttack(pEnemy->GetUID(), 10);
		pEnemy->doDie();
		pNPC->GetHateTable().Update(GConst::HATETABLE_ENTRY_TIMEOUT);
		pmAI->Update(0.0f);
		
		CHECK(WLUA->GetVar<int>("TEST_VAR") == 312);
	}

	TEST_FIXTURE(Fixture, NPC_OnCombatBegin)
	{
		const char* pszScript = 
			"function NPC_1:OnCombatBegin()\n"
			"	TEST_VAR = 312;\n"
			"end\n"
			;

		if (!RunLuaString(pszScript))	{ CHECK(FALSE && "WRunString Failed.");	 return; }

		GEntityNPC* pNPC = m_pField->SpawnTestNPC(m_pNPCInfo, vCommonPos, vCommonDir);
		pNPC->GetHateTable().EnableTimeout(true);

		// 전투를 시작하고 종료
		GModuleAI* pmAI = pNPC->GetModuleAI();
		GEntityPlayer* pEnemy = test.player.NewPlayer(m_pField);
		pNPC->doAttack(pEnemy->GetUID(), 10);

		CHECK(WLUA->GetVar<int>("TEST_VAR") == 312);
	}

	TEST_FIXTURE(Fixture, NPC_OnCombatEnd)
	{
		const char* pszScript = 
			"function NPC_1:OnCombatEnd()\n"
			"	TEST_VAR = 312;\n"
			"end\n"
			;

		if (!RunLuaString(pszScript))	{ CHECK(FALSE && "WRunString Failed.");	 return; }

		GEntityNPC* pNPC = m_pField->SpawnTestNPC(m_pNPCInfo, vCommonPos, vCommonDir);
		pNPC->GetHateTable().EnableTimeout(true);

		// 전투를 시작하고 종료
		GModuleAI* pmAI = pNPC->GetModuleAI();
		GEntityPlayer* pEnemy = test.player.NewPlayer(m_pField);
		pNPC->doAttack(pEnemy->GetUID(), 10);
		pEnemy->doDie();
		pNPC->GetHateTable().Update(GConst::HATETABLE_ENTRY_TIMEOUT);
		pmAI->Update(0.0f);
		pmAI->Update(GConst::VICTORY_DEFAULT_DELAY);
		pmAI->Update(0.0f);

		CHECK(WLUA->GetVar<int>("TEST_VAR") == 312);
	}

	TEST_FIXTURE(Fixture, NPC_OnBPartRecover)
	{
		const char* pszScript = 
			"function NPC_1:OnBPartRecover()\n"
			"	TEST_VAR = 312;\n"
			"end\n"
			;

		if (!RunLuaString(pszScript))	{ CHECK(FALSE && "WRunString Failed.");	 return; }

		GEntityNPC* pNPC = m_pField->SpawnTestNPC(m_pNPCInfo, vCommonPos, vCommonDir);
		pNPC->GetNPCBParts()->Break(0, 0);

		// BPart 를 파괴하고 회복
		pNPC->GetNPCBParts()->Update(0.0f);

		CHECK(WLUA->GetVar<int>("TEST_VAR") == 312);
	}

}


