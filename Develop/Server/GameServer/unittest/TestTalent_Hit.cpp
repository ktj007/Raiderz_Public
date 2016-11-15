#include "stdafx.h"
#include "GUTHelper.h"
#include "GNPCInfo.h"
#include "MockField.h"
#include "GUTHelper.h"
#include "GEntityPlayer.h"
#include "GEntityNPC.h"
#include "GCombatCalculator.h"
#include "GTestWrapper.h"
#include "GModuleCombat.h"
#include "GModuleAI.h"
#include "GModuleBuff.h"
#include "GConst.h"
#include "GPlayerTalent.h"
#include "TestCombatCalculator.h"
#include "CCommandTable.h"
#include "GTalentInfoMgr.h"
#include "GFieldMgr.h"
#include "MockLink.h"
#include "GJobRunner.h"
#include "FBaseMockLink.h"
#include "GTestForward.h"

const float BUFF_EXPIRED_TIME = 30000.0f;
const vec3 vCommonPos = vec3(1000.0f, 1000.0f, 0.0f);
const vec3 vCommonDir = vec3(0,1,0);


SUITE(TalentHit)
{
	struct UseTalentSampleFixture : public FBaseMockLink
	{
		UseTalentSampleFixture()
		{
			m_pField = GUTHelper_Field::DefaultMockField();
			m_NPCInfo.nID = 1;
			m_NPCInfo.nMaxHP = 50000;
			m_NPCInfo.nAP = 100;

			m_bPVPMode = GConst::PVP_MODE;
		}
		~UseTalentSampleFixture()
		{
			m_pField->Update(1.0f);
			m_pField->Destroy();

			GConst::PVP_MODE = m_bPVPMode;
		}

		GTalentInfo* MakeSimpleDmgAtkTalent(const int nDamage, int nTalentID=INVALID_TALENT_ID)
		{
			if (nTalentID == INVALID_TALENT_ID)		nTalentID = INT_MAX-2;

			GTalentInfo* pAktTalent = gmgr.pTalentInfoMgr->Find(nTalentID);
			if (pAktTalent)			
			{
				m_Helper.SetTalentDamage(pAktTalent, nDamage);
				return pAktTalent;
			}

			pAktTalent = m_Helper.NewMeleeTalentInfo(nTalentID);
			m_Helper.SetTalentDamage(pAktTalent, nDamage);
			m_Helper.AddSimpleHitInfo(pAktTalent);

			return pAktTalent;
		}

		GUTHelper		m_Helper;
		MockField*		m_pField;
		GNPCInfo		m_NPCInfo;
		GTestSysWrapper2<GAttackDamageCalculator, SimpleAttackDamageCalculator>	m_AttackDamageCalculator;
		GTestSysWrapper2<GCombatCalculator, SimpleCombatCalculator>				m_CombatCalculator;
		GTestMgrWrapper<GTalentInfoMgr>	m_TalentInfoMgrWrapper;
		bool			m_bPVPMode;
	};

	TEST_FIXTURE(UseTalentSampleFixture, HitMe)
	{
		GEntityPlayer* pPlayer = GUTHelper::NewEntityPlayer(m_pField, vCommonPos);
		GEntityNPC* pNPC = m_pField->SpawnTestNPC(&m_NPCInfo, vCommonPos, vCommonDir);

		//공격 탤런트
		const int ATTACK_TALENT_DAMAGE = 10;
		GTalentInfo* pAktTalent = MakeSimpleDmgAtkTalent(ATTACK_TALENT_DAMAGE);

		GTalentInfo* pPassiveTalentInfo = m_Helper.NewTalentInfo();
		pPassiveTalentInfo->m_nSkillType = ST_PASSIVE;
		pPassiveTalentInfo->m_nTiming = TC_HIT_ME;
		pPassiveTalentInfo->m_EffectInfo.m_nPoint = CSEffectInfo::POINT_TARGET;
		pPassiveTalentInfo->m_ActorModifier.nHPMax.nMod = -20;
		pPassiveTalentInfo->m_bNeedTraining = true;

		pPlayer->GetTalent().DeleteAll();
		pPlayer->GetTalent().Insert(pPassiveTalentInfo);

		int nOldPlayerHP = pPlayer->GetHP();
		int nOldNPCHP = pNPC->GetMaxHP();
		pNPC->doUseTalent(pAktTalent);
		m_pField->Update(1.0f);
		int nPlayerDamage = nOldPlayerHP - pPlayer->GetHP();
		int nNPCDamage = nOldNPCHP - pNPC->GetMaxHP();
		CHECK_EQUAL(nPlayerDamage, 10);		// 탤런트 피해
		CHECK_EQUAL(nNPCDamage, 20);		// 패시브 효과
	}

	TEST_FIXTURE(UseTalentSampleFixture, CriticalHitMe)
	{
		GTestSysWrapper2<GCombatCalculator, CombatCalculator_ForCritical>				m_CombatCalculator;

		GEntityPlayer* pPlayer = GUTHelper::NewEntityPlayer(m_pField, vCommonPos);
		GEntityNPC* pNPC = m_pField->SpawnTestNPC(&m_NPCInfo, vCommonPos, vCommonDir);

		//공격 탤런트
		const int ATTACK_TALENT_DAMAGE = 10;
		GTalentInfo* pAktTalent = MakeSimpleDmgAtkTalent(ATTACK_TALENT_DAMAGE);

		GTalentInfo* pPassiveTalentInfo = m_Helper.NewTalentInfo();
		pPassiveTalentInfo->m_nSkillType = ST_PASSIVE;
		pPassiveTalentInfo->m_nTiming = TC_CRITICAL_HIT_ME;
		pPassiveTalentInfo->m_EffectInfo.m_nPoint = CSEffectInfo::POINT_TARGET;
		pPassiveTalentInfo->m_ActorModifier.nHPMax.nMod = -20;
		pPassiveTalentInfo->m_bNeedTraining = true;

		pPlayer->GetTalent().DeleteAll();
		pPlayer->GetTalent().Insert(pPassiveTalentInfo);

		int nOldPlayerHP = pPlayer->GetHP();
		int nOldNPCHP = pNPC->GetHP();
		pNPC->doUseTalent(pAktTalent);
		m_pField->Update(1.0f);
		int nPlayerDamage = nOldPlayerHP - pPlayer->GetHP();
		int nNPCDamage = nOldNPCHP - pNPC->GetMaxHP();
		CHECK_EQUAL(nNPCDamage, 20);		// 패시브 효과
	}

	TEST_FIXTURE(UseTalentSampleFixture, HitEnemy)
	{
		GEntityPlayer* pPlayer = GUTHelper::NewEntityPlayer(m_pField, vCommonPos);
		GEntityNPC* pNPC = m_pField->SpawnTestNPC(&m_NPCInfo, vCommonPos, vCommonDir);

		//공격 탤런트
		const int ATTACK_TALENT_DAMAGE = 10;
		GTalentInfo* pAktTalent = MakeSimpleDmgAtkTalent(ATTACK_TALENT_DAMAGE);

		GTalentInfo* pPassiveTalentInfo = m_Helper.NewTalentInfo();
		pPassiveTalentInfo->m_nSkillType = ST_PASSIVE;
		pPassiveTalentInfo->m_nTiming = TC_HIT_ENEMY;
		pPassiveTalentInfo->m_EffectInfo.m_nPoint = CSEffectInfo::POINT_SELF;
		pPassiveTalentInfo->m_ActorModifier.nHPMax.nMod = 100;
		pPassiveTalentInfo->m_bNeedTraining = true;
		

		pPlayer->GetTalent().DeleteAll();
		pPlayer->GetTalent().Insert(pPassiveTalentInfo);

		pPlayer->SetHP(1);
		int nOldPlayerHP = pPlayer->GetMaxHP();
		pPlayer->doUseTalent(pAktTalent);
		m_pField->Update(1.0f);
		int nPlayerDamage = nOldPlayerHP - pPlayer->GetMaxHP();
		CHECK_EQUAL(nPlayerDamage, -100);	// 패시브 효과
	}

	// 상대방을 모두 피격시킴
	TEST_FIXTURE(UseTalentSampleFixture, HitEnemyAll)
	{
		GEntityPlayer* pPlayer = GUTHelper::NewEntityPlayer(m_pField, vCommonPos);
		GEntityNPC* pNPC = m_pField->SpawnTestNPC(&m_NPCInfo, vCommonPos, vCommonDir);

		//공격 탤런트
		const int ATTACK_TALENT_DAMAGE = 10;
		GTalentInfo* pAktTalent = MakeSimpleDmgAtkTalent(ATTACK_TALENT_DAMAGE);
		pAktTalent->m_bSeparateHit = true;
		pAktTalent->m_nTiming = TC_HIT_ALL_TALENT;

		GTalentInfo* pPassiveTalentInfo = m_Helper.NewTalentInfo();
		pPassiveTalentInfo->m_nSkillType = ST_PASSIVE;
		pPassiveTalentInfo->m_nTiming = TC_HIT_ALL_TALENT;
		pPassiveTalentInfo->m_EffectInfo.m_nPoint = CSEffectInfo::POINT_SELF;
		pPassiveTalentInfo->m_ActorModifier.nHPMax.nMod = 100;
		pPassiveTalentInfo->m_bNeedTraining = true;

		pPlayer->GetTalent().DeleteAll();
		pPlayer->GetTalent().Insert(pPassiveTalentInfo);

		pPlayer->SetHP(1);
		int nOldPlayerHP = pPlayer->GetMaxHP();
		pPlayer->doUseTalent(pAktTalent);
		m_pField->Update(1.0f);
		int nPlayerDamage = nOldPlayerHP - pPlayer->GetMaxHP();
		CHECK_EQUAL(nPlayerDamage, -100);	// 패시브 효과
	}

	TEST_FIXTURE(UseTalentSampleFixture, CriticalHitEnemy)
	{
		GTestSysWrapper2<GCombatCalculator, CombatCalculator_ForCritical>				m_CombatCalculator;

		GEntityPlayer* pPlayer = GUTHelper::NewEntityPlayer(m_pField, vCommonPos);
		GEntityNPC* pNPC = m_pField->SpawnTestNPC(&m_NPCInfo, vCommonPos, vCommonDir);

		//공격 탤런트
		const int ATTACK_TALENT_DAMAGE = 10;
		GTalentInfo* pAktTalent = MakeSimpleDmgAtkTalent(ATTACK_TALENT_DAMAGE);

		GTalentInfo* pPassiveTalentInfo = m_Helper.NewTalentInfo();
		pPassiveTalentInfo->m_nSkillType = ST_PASSIVE;
		pPassiveTalentInfo->m_nTiming = TC_CRITICAL_HIT_ENEMY;
		pPassiveTalentInfo->m_EffectInfo.m_nPoint = CSEffectInfo::POINT_SELF;	
		pPassiveTalentInfo->m_ActorModifier.nHPMax.nMod = 100;
		pPassiveTalentInfo->m_bNeedTraining = true;

		pPlayer->GetTalent().DeleteAll();
		pPlayer->GetTalent().Insert(pPassiveTalentInfo);

		pPlayer->SetHP(1);
		int nOldPlayerHP = pPlayer->GetMaxHP();
		pPlayer->doUseTalent(pAktTalent);
		m_pField->Update(1.0f);
		int nPlayerDamage = nOldPlayerHP - pPlayer->GetMaxHP();
		CHECK_EQUAL(nPlayerDamage, -100);		// 패시브 효과
	}

	TEST_FIXTURE(UseTalentSampleFixture, AvoidMe)
	{
		GTestSysWrapper2<GCombatCalculator, CombatCalculator_ForMiss>				m_CombatCalculator;

		GEntityPlayer* pPlayer = GUTHelper::NewEntityPlayer(m_pField, vCommonPos);
		GEntityNPC* pNPC = m_pField->SpawnTestNPC(&m_NPCInfo, vCommonPos, vCommonDir);

		//공격 탤런트
		const int ATTACK_TALENT_DAMAGE = 10;
		GTalentInfo* pAktTalent = MakeSimpleDmgAtkTalent(ATTACK_TALENT_DAMAGE);

		GTalentInfo* pPassiveTalentInfo = m_Helper.NewTalentInfo();
		pPassiveTalentInfo->m_nSkillType = ST_PASSIVE;
		pPassiveTalentInfo->m_nTiming = TC_MISS_ME;
		pPassiveTalentInfo->m_EffectInfo.m_nPoint = CSEffectInfo::POINT_TARGET;
		pPassiveTalentInfo->m_EffectInfo.m_nRelation = CSEffectInfo::RELATION_ALL;
		pPassiveTalentInfo->m_ActorModifier.nHPMax.nMod = -20;
		pPassiveTalentInfo->m_bNeedTraining = true;

		pPlayer->GetTalent().DeleteAll();
		pPlayer->GetTalent().Insert(pPassiveTalentInfo);

		int nBeforeMaxHP = pNPC->GetMaxHP();
		pNPC->doUseTalent(pAktTalent);
		m_pField->Update(1.0f);
		int nAfterMaxHP = pNPC->GetMaxHP();
		CHECK_EQUAL(nBeforeMaxHP-20, nAfterMaxHP);		// 패시브 효과
	}

	TEST_FIXTURE(UseTalentSampleFixture, AvoidEnemy)
	{
		GTestSysWrapper2<GCombatCalculator, CombatCalculator_ForMiss>				m_CombatCalculator;

		GEntityPlayer* pPlayer = GUTHelper::NewEntityPlayer(m_pField, vCommonPos);
		GEntityNPC* pNPC = m_pField->SpawnTestNPC(&m_NPCInfo, vCommonPos, vCommonDir);

		//공격 탤런트
		const int ATTACK_TALENT_DAMAGE = 10;
		GTalentInfo* pAktTalent = MakeSimpleDmgAtkTalent(ATTACK_TALENT_DAMAGE);

		GTalentInfo* pPassiveTalentInfo = m_Helper.NewTalentInfo();
		pPassiveTalentInfo->m_nSkillType = ST_PASSIVE;
		pPassiveTalentInfo->m_nTiming = TC_MISS_ENEMY;
		pPassiveTalentInfo->m_EffectInfo.m_nPoint = CSEffectInfo::POINT_SELF;
		pPassiveTalentInfo->m_EffectInfo.m_nRelation = CSEffectInfo::RELATION_ALL;
		pPassiveTalentInfo->m_ActorModifier.nHPMax.nMod = 20;
		pPassiveTalentInfo->m_bNeedTraining = true;

		pPlayer->GetTalent().DeleteAll();
		pPlayer->GetTalent().Insert(pPassiveTalentInfo);

		pPlayer->SetHP(1);
		int nBeforeMaxHP = pPlayer->GetMaxHP();
		pPlayer->doUseTalent(pAktTalent);
		m_pField->Update(1.0f);
		int nAfterMaxHP = pPlayer->GetMaxHP();
		CHECK_EQUAL(nBeforeMaxHP + 20, nAfterMaxHP);		// 패시브 효과
	}

	TEST_FIXTURE(UseTalentSampleFixture, GuardMe)
	{
		GEntityPlayer* pPlayer = GUTHelper::NewEntityPlayer(m_pField, vCommonPos);
		GEntityNPC* pNPC = m_pField->SpawnTestNPC(&m_NPCInfo, vCommonPos, vCommonDir);

		// 방어 탤런트 배우기
		{
			GTalentInfo* pNewTalentInfo = new GTalentInfo();
			pNewTalentInfo->m_nID = 300;
			pNewTalentInfo->m_nCategory = TC_MELEE;
			pNewTalentInfo->m_nTalentType = TT_SKILL;
			pNewTalentInfo->m_nSkillType = ST_GUARD;
			pNewTalentInfo->m_nGuardDirection = GUARD_DIRECTION_ALL;
			gmgr.pTalentInfoMgr->Insert(pNewTalentInfo);

			pPlayer->GetTalent().DeleteAll();
			pPlayer->GetTalent().Insert(pNewTalentInfo);
		}

		//공격 탤런트
		const int ATTACK_TALENT_DAMAGE = 10;
		GTalentInfo* pAktTalent = MakeSimpleDmgAtkTalent(ATTACK_TALENT_DAMAGE);

		GTalentInfo* pPassiveTalentInfo = m_Helper.NewTalentInfo();	
		pPassiveTalentInfo->m_nSkillType = ST_PASSIVE;
		pPassiveTalentInfo->m_nTiming = TC_GUARD_ME;
		pPassiveTalentInfo->m_EffectInfo.m_nPoint = CSEffectInfo::POINT_TARGET;	
		pPassiveTalentInfo->m_ActorModifier.nHPMax.nMod = -20;
		pPassiveTalentInfo->m_bNeedTraining = true;

		pPlayer->GetTalent().Insert(pPassiveTalentInfo);

		int nOldNPCHP = pNPC->GetMaxHP();
		pPlayer->doGuard(300);
		pNPC->doUseTalent(pAktTalent);
		m_pField->Update(1.0f);
		int nNPCDamage = nOldNPCHP - pNPC->GetMaxHP();
		CHECK_EQUAL(nNPCDamage, 20);		// 패시브 효과
	}

	TEST_FIXTURE(UseTalentSampleFixture, GuardEnemy)
	{
		GEntityPlayer* pPlayer = GUTHelper::NewEntityPlayer(m_pField, vCommonPos);
		GEntityNPC* pNPC = m_pField->SpawnTestNPC(&m_NPCInfo, vCommonPos, vCommonDir);

		// 방어 탤런트 배우기
		{
			GTalentInfo* pNewTalentInfo = new GTalentInfo();
			pNewTalentInfo->m_nID = 300;
			pNewTalentInfo->m_nCategory = TC_MELEE;
			pNewTalentInfo->m_nTalentType = TT_SKILL;
			pNewTalentInfo->m_nSkillType = ST_GUARD;
			pNewTalentInfo->m_nGuardDirection = GUARD_DIRECTION_ALL;
			gmgr.pTalentInfoMgr->Insert(pNewTalentInfo);
		}

		//공격 탤런트
		const int ATTACK_TALENT_DAMAGE = 10;
		GTalentInfo* pAktTalent = MakeSimpleDmgAtkTalent(ATTACK_TALENT_DAMAGE);

		GTalentInfo* pPassiveTalentInfo = m_Helper.NewTalentInfo();
		pPassiveTalentInfo->m_nSkillType = ST_PASSIVE;
		pPassiveTalentInfo->m_nTiming = TC_GUARD_ENEMY;
		pPassiveTalentInfo->m_EffectInfo.m_nPoint = CSEffectInfo::POINT_SELF;
		pPassiveTalentInfo->m_ActorModifier.nHPMax.nMod = 20;
		pPassiveTalentInfo->m_bNeedTraining = true;

		pPlayer->GetTalent().Insert(pPassiveTalentInfo);

		pPlayer->SetHP(1);
		int nOldPlayerHP = pPlayer->GetMaxHP();
		pNPC->doGuard(300);
		pPlayer->doUseTalent(pAktTalent);
		m_pField->Update(1.0f);
		int nPlayerDamage = nOldPlayerHP - pPlayer->GetMaxHP();
		CHECK_EQUAL(nPlayerDamage, -20);		// 패시브 효과
	}

	class TestCombatCalculator_ForResist : public GCombatCalculator
	{
	public:
		virtual bool CheckMiss(GEntityActor* pAttacker, GEntityActor* pVictim, const GTalentInfo* pTalentInfo) override
		{	return false;	}
		virtual bool CheckResist(float& foutResistPercent, GEntityActor* pAttacker, GEntityActor* pVictim, const GTalentResist& Resist) override
		{	return true;	}
		virtual bool CheckCritical(GEntityActor* pAttacker, GEntityActor* pVictim, const GTalentInfo* pTalentInfo) override
		{	return false;	}
	};

	TEST_FIXTURE(UseTalentSampleFixture, HitEnemyButResistPerfect)
	{
		GTestSysWrapper2<GCombatCalculator, TestCombatCalculator_ForResist>				m_CombatCalculator;

		GEntityPlayer* pPlayer = GUTHelper::NewEntityPlayer(m_pField, vCommonPos);
		GEntityNPC* pNPC = m_pField->SpawnTestNPC(&m_NPCInfo, vCommonPos, vCommonDir);

		MockLink* pPlayerLink = NewLink(pPlayer);
		pPlayerLink->IgnoreAll();
		pPlayerLink->AddAllowedID(MC_ACTION_TALENT_HIT);

		//공격 탤런트
		const int ATTACK_TALENT_DAMAGE = 10;
		GTalentInfo* pAktTalent = MakeSimpleDmgAtkTalent(ATTACK_TALENT_DAMAGE);
		pAktTalent->m_nCategory = TC_MAGIC;

		pPlayer->SetHP(1);
		int nOldPlayerHP = pPlayer->GetMaxHP();
		pPlayer->doUseTalent(pAktTalent);
		m_pField->Update(1.0f);

		CHECK_EQUAL(1, pPlayer->GetHP());
		CHECK_EQUAL(MC_ACTION_TALENT_HIT, pPlayerLink->GetCommand(0).GetID());

		if (pPlayerLink->GetCommand(0).GetID() == MC_ACTION_TALENT_HIT)
		{
			MCommandParameter* pParam = pPlayerLink->GetCommand(0).GetParameter(0);
			TD_TALENT_HIT* td_talent_hit = static_cast<TD_TALENT_HIT*>(pParam->GetPointer());

			CHECK(CheckBitSet(td_talent_hit->nFlags, CTR_RESIST_PERFECT) > 0);
		}
	}

	TEST_FIXTURE(UseTalentSampleFixture, TestHit_WhileNPCRotating)
	{
		const int NPC_MAX_HP = 10000;
		GActorMeshInfo* pActorMeshInfo = new GActorMeshInfo();

		// 앞,뒤로 판정 범위가 500
		m_Helper.SetSimpleActorMeshInfoHit(pActorMeshInfo, vec3(0.0f, 400.0f, 0.0f), vec3(0.0f, -400.0f, 1000.0f), 100.0f);
		m_NPCInfo.m_pMeshInfo = pActorMeshInfo;
		m_NPCInfo.nMaxHP = NPC_MAX_HP;

		vec3 npc_pos = vec3(0.0f, 1000.0f, 0.0f);
		vec3 npc_dir = vec3(0.0f, 1.0f, 0.0f);

		vec3 pc_pos = vec3(0.0f, 350.0f, 0.0f);
		vec3 pc_dir = npc_dir;

		GEntityPlayer* pPlayer = GUTHelper::NewEntityPlayer(m_pField, pc_pos);
		pPlayer->SetDir(pc_dir);

		GEntityNPC* pNPC = m_pField->SpawnTestNPC(&m_NPCInfo, npc_pos, npc_dir);

		//공격 탤런트
		const int ATTACK_TALENT_DAMAGE = 1000;
		const int TALENT_ID = 1;
		GTalentInfo* pAktTalent = m_Helper.NewMeleeTalentInfo(TALENT_ID);
		m_Helper.SetTalentDamage(pAktTalent, ATTACK_TALENT_DAMAGE);
		m_Helper.AddHitInfo(pAktTalent, vec3(0.0f, -100.0f, 0.0f), vec3(0.0f, -100.0f, 1000.0f), 100.0f, 0.0f);


		// NPC 회전 탤런트
		const int NPC_TALENT_ID = 2;
		GTalentInfo* pRotationtTalent = m_Helper.NewTalentInfo(NPC_TALENT_ID);
		pRotationtTalent->m_nSkillType = ST_MELEE;
		pRotationtTalent->m_setNPCID.insert(m_NPCInfo.nID);
		pRotationtTalent->m_fDurationTime = 0.2f + 0.2f;	// 뒤의 0.2f는 탤런트 지속시간에 대한 틱 보정값 (SAFE_TICK_TIME)
		m_Helper.SetTalentDamage(pRotationtTalent, 0);

		// 탤런트 이동 정보 설정
		{
			CSTalentInfo::_TALENT_MOVE_INFO	new_move_info(vec3::ZERO, 0.0f);
			pRotationtTalent->m_MovingInfoList.push_back(new_move_info);
		}
		{
			// 0.1초 후에는 90도 회전
			CSTalentInfo::_TALENT_MOVE_INFO	new_move_info(vec3::ZERO, GMath::HALFPI);
			pRotationtTalent->m_MovingInfoList.push_back(new_move_info);
		}
		{
			// 0.2초 후에는 180도 회전
			CSTalentInfo::_TALENT_MOVE_INFO	new_move_info(vec3::ZERO, GMath::PI);
			pRotationtTalent->m_MovingInfoList.push_back(new_move_info);
		}

		gmgr.pTalentInfoMgr->Cooking();


		pPlayer->doUseTalent(pAktTalent);
		m_pField->Update(0.1f);

		// 판정됨
		CHECK(NPC_MAX_HP != pNPC->GetHP());


		// 다시 시작 --
		pNPC->SetHP(NPC_MAX_HP);
		pNPC->UpdateAI(10.0f);

		// NPC가 90도 회전되어 있음
		pNPC->SetDir(vec3(1.0f, 0.0f, 0.0f));

		pPlayer->doUseTalent(pAktTalent);
		pNPC->UpdateAI(0.1f);

		// NPC가 90도 회전되어 있으므로 판정안됨
		CHECK_EQUAL(NPC_MAX_HP, pNPC->GetHP());

		// 다시 시작 --

		pNPC->SetHP(NPC_MAX_HP);
		CHECK_EQUAL(NPC_MAX_HP, pNPC->GetHP());

		pNPC->SetDir(npc_dir);

		GJobRunner job_runner;
		job_runner.ClearJob(pNPC->GetModuleAI());
		job_runner.UseTalent(pNPC, NULL, NPC_TALENT_ID);
		pNPC->UpdateAI(0.0f);

		pNPC->UpdateAI(0.1f);
		pNPC->UpdateAI(0.05f);

	pPlayer->doUseTalent(pAktTalent);
	pNPC->UpdateAI(0.03f);

	// 90도 회전되있는 상태이므로 판정 안됨
	CHECK(NPC_MAX_HP == pNPC->GetHP());
}

TEST(HitOverap)
{
	DECLWRAPPER_SimpleCombatCalc;

	GUTHelper		m_Helper;
	MockField* m_pField = GUTHelper_Field::DefaultMockField();
	GEntityPlayer* pPlayer = test.player.NewPlayer(m_pField);
	GEntityNPC* pNPC = GUTHelper_NPC::SpawnNPC(m_pField);

	// 겹치게 위치
	pPlayer->SetPos(vec3(100,100,100));
	pNPC->SetPos(vec3(100,100,100));

	//공격 탤런트
	GTalentInfo*	pTestTalent;
	pTestTalent = m_Helper.NewTalentInfo();
	pTestTalent->m_nSkillType = ST_MELEE;
	pTestTalent->m_nTiming = TC_HIT_ENEMY;
	pTestTalent->m_EffectInfo.m_nRelation = CSEffectInfo::RELATION_ALL;
	m_Helper.SetTalentDamage(pTestTalent, 10);		// 데미지 1로 설정

	// 충돌 캡슐은 충돌할 수 없게 설정
//	m_Helper.AddSimpleHitInfo(pTestTalent, 0.0f);	// 바로 판정하는 탤런트임
	m_Helper.AddHitInfo(pTestTalent, vec3(-10000.f, 0.f, 0.f), vec3(-10000.f, 0.f, 100.f), 1.f, 0.0f);
	
	pPlayer->SetHP(100);
	test.talent.UseTalent(pNPC, pTestTalent, pPlayer->GetUID());
	CHECK_EQUAL(90, pPlayer->GetHP());	// 제대로 피해를 입음

	// 겹치면 맞지 않게 설정
	pTestTalent->m_bHitOverlap = false;
	pPlayer->SetHP(100);
	test.talent.UseTalent(pNPC, pTestTalent, pPlayer->GetUID());
	CHECK_EQUAL(100, pPlayer->GetHP());	// 피해입지 않음
}

} // SUITE
