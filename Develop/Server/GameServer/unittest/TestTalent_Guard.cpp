#include "StdAfx.h"
#include "GTestForward.h"
#include "GModuleCombat.h"
#include "GModuleBuff.h"
#include "GTalent.h"
#include "GItemManager.h"
#include "GActorObserver.h"
#include "GServer.h"
#include "GFieldMgr.h"
#include "FBasePlayer.h"
#include "GuardEffector.h"
#include "GDefenseFactorCalculator.h"

class MockActorObserver_ForEvent2 : public GActorObserver, public TestCallCounter
{
public:
	// 방어 이벤트
	virtual void OnGuard(GEntityActor* pOwner, GEntityActor* pAttacker, GTalentInfo* pAttackTalentInfo) override { 
		__super::OnGuard(pOwner, pAttacker, pAttackTalentInfo);
		INCREASE_CALLCOUNTER; 
	}
	// 완전 방어 이벤트
	virtual void OnPerfectGuard(GEntityActor* pOwner, GEntityActor* pAttacker, GTalentInfo* pAttackTalentInfo) override  { 
		__super::OnPerfectGuard(pOwner, pAttacker, pAttackTalentInfo);
		INCREASE_CALLCOUNTER; 
	}

	// 부분 방어 이벤트
	virtual void OnPartialGuard(GEntityActor* pOwner, GEntityActor* pAttacker, GTalentInfo* pAttackTalentInfo) override  { 
		__super::OnPartialGuard(pOwner, pAttacker, pAttackTalentInfo);
		INCREASE_CALLCOUNTER; 
	}

	// 부분 방어 이벤트
	virtual void OnAbsoluteGuard(GEntityActor* pOwner, GEntityActor* pAttacker, GTalentInfo* pAttackTalentInfo) override  { 
		__super::OnAbsoluteGuard(pOwner, pAttacker, pAttackTalentInfo);
		INCREASE_CALLCOUNTER; 
	}
};


SUITE(Talent_Guard)
{
	struct FTalent : public FBasePlayer
	{
		FTalent()
		{
			m_pField = GUTHelper_Field::DefaultMockField();
			m_pTester = NewEntityPlayer(m_pField, vec3(100,100,0));
			m_pLinkTester = test.network.NewLink(m_pTester);
			swprintf_s(m_pTester->GetPlayerInfo()->szName, L"Tester");

			gsys.pServer->FrameCounterClear();
			SetIgnoreCommonCommand(m_pLinkTester);

			m_pTester->AttachObserver(&m_ActorObserver);
		}

		~FTalent()
		{
			m_pTester->DetachObserver(&m_ActorObserver);
			m_pField->Destroy();
			test.network.ClearLinks();
		}

		void BeginCommandRecord()
		{
			m_pLinkTester->ResetCommands();
		}

		void SetIgnoreCommonCommand(MockLink* pLink)
		{
			pLink->AddIgnoreID(MC_ACTION_USE_TALENT);
			pLink->AddIgnoreID(MC_ACTION_TALENT_HIT);
			pLink->AddIgnoreID(MC_ACTION_TALENT_HIT_NO_MF);
		}

		void GainAbsoluteGuardBuff(GEntityActor* pEffect)
		{
			DCHECK(pEffect);

			// 절대방어 버프 얻기
			int nBuffID = 123;
			GBuffInfo* pBuff=test.buff.NewBuffInfo(nBuffID);
			pBuff->m_nPassiveExtraAttrib = BUFPEA_ABSOLUTE_GUARD;
			CSBuffEnchantInfo infoEnchant;
			infoEnchant.nID = nBuffID;
			infoEnchant.EffectInfo.m_nPoint = CSEffectInfo::POINT_SELF;
			infoEnchant.EffectInfo.m_nRelation = CSEffectInfo::RELATION_PARTY;
			test.buff.GainBuffSelf(pEffect, pBuff, &infoEnchant);
		}

		GTalentInfo* MakeMissileMagicTalent(float fCastingTime, float fDurationTime, float fEventTime, float fProjectileSpeed)
		{
			GTalentInfo* pTalentInfo = test.talent.NewTalentInfo();
			// 탤런트 시간 --------------------------
			pTalentInfo->m_fCastingTime = fCastingTime;
			pTalentInfo->m_fDurationTime = fDurationTime;
			// 발사체 -------------------------------
			pTalentInfo->m_nSkillType = ST_MAGIC;
			pTalentInfo->m_nTiming = TC_HIT_ENEMY;
			pTalentInfo->m_EffectInfo.m_nPoint = CSEffectInfo::POINT_TARGET;

			// 발사체 이벤트 추가
			CSTalentEventInfo talentEventInfo;
			talentEventInfo.m_nEvent = TE_LAUNCH_PROJECTILE;
			talentEventInfo.m_fTime1 = fEventTime;
			talentEventInfo.m_ProjectileInfo.m_nType = TPT_MISSILE;
			talentEventInfo.m_ProjectileInfo.m_fSpeed = fProjectileSpeed;	// 발사체속도: (x)m/s
			pTalentInfo->m_Events.m_vecActEvents.push_back(talentEventInfo);

			pTalentInfo->Cooking();
			return pTalentInfo;
		}


		MockField*		m_pField;
		GEntityPlayer*	m_pTester;
		MockLink*		m_pLinkTester;
		MockActorObserver_ForEvent2	m_ActorObserver;
		
		DECLWRAPPER_FieldInfoMgr;
		DECLWRAPPER_FieldMgr;
		DECLWRAPPER_TalentMgr;
		DECLWRAPPER_ItemMgr;
		DECLWRAPPER_SimpleCombatCalc;
	};

	TEST_FIXTURE(FTalent, GuardFail_SetWrongDirection)
	{
		GTestMgrWrapper<GNPCInfoMgr>();

		test.buff.NewBuffInfo(BUFF_FOCUS_ID[TFT_COUNTER]);
		test.focus.GainItemForCounter(m_pTester);

		// 공격자가 방어자 왼쪽에 위치
		GEntityPlayer* pAttacker = 
			NewEntityPlayer(m_pField, m_pTester->GetPos()-vec3(-10,0,0));

		// 방어자가 정면을 바라봄
		m_pTester->SetDir(vec3(0,1,0));
		int nGuardTalentID = SUnitTestUtil::NewID();
		GTalentInfo* pGuardTalent = test.talent.NewGuardTalentInfo(nGuardTalentID);
		pGuardTalent->m_nGuardDirection = GUARD_DIRECTION_FRONT;
		m_pTester->doGuard(nGuardTalentID);

		// 공격자가 공격
		m_pTester->SetHP(m_pTester->GetMaxHP());
		GTalentInfo* pAtkTalent = test.talent.MakeDamageTalent(100);	// 100의 피해를 주는 탤런트
		test.talent.UseTalent(pAttacker, pAtkTalent, m_pTester->GetUID());

		// 피해 확인
		CHECK_EQUAL(m_pTester->GetHP(), m_pTester->GetMaxHP()-100);	// 피해 입음
	}

	TEST_FIXTURE(FTalent, PerfectGuard)
	{
		test.item.EquipNewShield(m_pTester);

		// 공격자가 방어자 왼쪽에 위치
		GEntityPlayer* pAttacker = 
			NewEntityPlayer(m_pField, m_pTester->GetPos()-vec3(-10,0,0));

		// 공격자/방어자 레벨이 동일 (레벨팩터=5), 가드확률=100%, 피해보정률=0.0
		m_pTester->SetLevel(100);
		pAttacker->SetLevel(1);

		// 방어자가 공격자를 바라보며 방어
		m_pTester->SetDirForwardToActor(pAttacker);
		int nGuardTalentID = SUnitTestUtil::NewID();
		GTalentInfo* pGuardTalent = test.talent.NewGuardTalentInfo(nGuardTalentID);
		pGuardTalent->m_nGuardDirection = GUARD_DIRECTION_FRONT;
		m_pTester->doGuard(nGuardTalentID);

		// 공격자가 공격
		m_pTester->SetHP(m_pTester->GetMaxHP());
		CHECK_EQUAL(0, m_ActorObserver.GetCallCounter(L"MockActorObserver_ForEvent2::OnPerfectGuard"));
		GTalentInfo* pAtkTalent = test.talent.MakeDamageTalent(100);	// 100의 피해를 주는 탤런트
		test.talent.UseTalent(pAttacker, pAtkTalent, m_pTester->GetUID());

		// 피해 확인
		CHECK_EQUAL(m_pTester->GetHP(), m_pTester->GetMaxHP());	// 피해 입지 않음
		CHECK_EQUAL(1, m_ActorObserver.GetCallCounter(L"MockActorObserver_ForEvent2::OnPerfectGuard"));
	}


	TEST_FIXTURE(FTalent, PerfectGuardBuff)
	{
		test.item.EquipNewShield(m_pTester);		

		GEntityPlayer* pAttacker = 
			NewEntityPlayer(m_pField, m_pTester->GetPos()-vec3(-10,0,0));

		int nMeleeAttackTalentID = SUnitTestUtil::NewID();
		GTalentInfo* pAttackTalentInfo = test.talent.NewMeleeTalentInfo(nMeleeAttackTalentID);

		pAttackTalentInfo-> m_nTiming = TC_HIT_TALENT;
		pAttackTalentInfo->m_Buff1.nID = 40005;

		pAttacker->OnGuardEnemy(m_pTester, pAttackTalentInfo);
		CHECK(!m_pTester->GetModuleBuff()->IsGained(40005));
	}


	TEST_FIXTURE(FTalent, PartialGuard)
	{
		// 공격자가 방어자 왼쪽에 위치
		GEntityPlayer* pAttacker = 
			NewEntityPlayer(m_pField, m_pTester->GetPos()-vec3(-10,0,0));

		// 방어자가 공격자 레벨보다 2높음 (레벨팩터=4), 가드확률=100%, 피해보정률=0.1
		m_pTester->SetLevel(15);
		pAttacker->SetLevel(13);
		m_pTester->SetHP(m_pTester->GetMaxHP());

		// 방어자가 공격자를 바라보며 방어
		test.item.EquipNewWeapon(m_pTester, WEAPON_1H_SLASH);
		m_pTester->SetDirForwardToActor(pAttacker);
		int nGuardTalentID = SUnitTestUtil::NewID();
		GTalentInfo* pGuardTalent = test.talent.NewGuardTalentInfo(nGuardTalentID);
		pGuardTalent->m_nGuardDirection = GUARD_DIRECTION_FRONT;
		m_pTester->doGuard(nGuardTalentID);

		// 공격자가 공격
		CHECK_EQUAL(0, m_ActorObserver.GetCallCounter(L"MockActorObserver_ForEvent2::OnPartialGuard"));
		GTalentInfo* pAtkTalent = test.talent.MakeDamageTalent(100);	// 100의 피해를 주는 탤런트
		test.talent.UseTalent(pAttacker, pAtkTalent, m_pTester->GetUID());

		GDefenseFactorCalculator defenseFactorCalc;
		float fDefenseFactor = defenseFactorCalc.CalcDefenseFactor(pAttacker, m_pTester, pAtkTalent);

		// 50%피해만 입음
		int nDamage = (int)((100 / 2) * fDefenseFactor);

		// 피해 확인
		CHECK_EQUAL(m_pTester->GetHP(), m_pTester->GetMaxHP()-nDamage);	// 50%의 피해만 입음 (한손검으로 방어시 50%임)
		CHECK_EQUAL(1, m_ActorObserver.GetCallCounter(L"MockActorObserver_ForEvent2::OnPartialGuard"));
	}

	TEST_FIXTURE(FTalent, PartialGuard_WhenPerfectGuardFailed)
	{
		test.item.EquipNewShield(m_pTester);

		// 공격자가 방어자 왼쪽에 위치
		GEntityNPC* pAttacker =
			GUTHelper_NPC::SpawnNPC(m_pField, INVALID_ID, m_pTester->GetPos()-vec3(-10,0,0));

		// 공격자/방어자 레벨이 동일 (레벨팩터=10), 가드확률=0%, 피해보정률=0.4
		m_pTester->SetLevel(1);
		pAttacker->SetLevel(99);
		int8 nOldGrade = pAttacker->GetNPCInfo()->nGrade;
		pAttacker->GetNPCInfo()->nGrade = 100; // 최대 등급

		// 방어자가 공격자를 바라보며 방어
		m_pTester->SetDirForwardToActor(pAttacker);
		int nGuardTalentID = SUnitTestUtil::NewID();
		GTalentInfo* pGuardTalent = test.talent.NewGuardTalentInfo(nGuardTalentID);
		pGuardTalent->m_nGuardDirection = GUARD_DIRECTION_FRONT;
		m_pTester->doGuard(nGuardTalentID);

		// 공격자가 공격
		GTalentInfo* pAtkTalent = test.talent.MakeDamageTalent(100);	// 100의 피해를 주는 탤런트
		test.talent.UseTalent(pAttacker, pAtkTalent, m_pTester->GetUID());

		GuardEffector guard_effector;
		CHECK(GT_PARTIAL == guard_effector.GetGuardLevel(m_pTester, pAttacker, pAtkTalent));

		pAttacker->GetNPCInfo()->nGrade = nOldGrade;
	}

	TEST_FIXTURE(FTalent, NPCGuardType_Partial)
	{
		// 공격자가 방어자 오른쪽에 위치
		GEntityNPC* pDefender =
			GUTHelper_NPC::SpawnNPC(m_pField);

		pDefender->SetPos(m_pTester->GetPos()-vec3(10,0,0));
		pDefender->GetNPCInfo()->nDefaultGuardType = GT_PARTIAL; // 부분 방어만 함
		MockActorObserver_ForEvent2	ActorObserver;
		pDefender->AttachObserver(&ActorObserver);

		// 방어자가 공격자를 바라보며 방어
		pDefender->SetDirForwardToActor(m_pTester);
		int nGuardTalentID = SUnitTestUtil::NewID();
		GTalentInfo* pGuardTalent = test.talent.NewGuardTalentInfo(nGuardTalentID);
		pGuardTalent->m_nGuardDirection = GUARD_DIRECTION_FRONT;
		pDefender->doGuard(nGuardTalentID);

		// 공격자가 공격
		CHECK_EQUAL(0, ActorObserver.GetCallCounter(L"MockActorObserver_ForEvent2::OnPartialGuard"));
		GTalentInfo* pAtkTalent = test.talent.MakeDamageTalent(100);	// 100의 피해를 주는 탤런트
		test.talent.UseTalent(m_pTester, pAtkTalent, pDefender->GetUID());

		// 피해 확인
		CHECK_EQUAL(1, ActorObserver.GetCallCounter(L"MockActorObserver_ForEvent2::OnPartialGuard"));
		pDefender->DetachObserver(&ActorObserver);
	}

	TEST_FIXTURE(FTalent, NPCGuardType_Perfect)
	{
		// 공격자가 방어자 오른쪽에 위치
		GEntityNPC* pDefender =
			GUTHelper_NPC::SpawnNPC(m_pField);
		pDefender->SetPos(m_pTester->GetPos()-vec3(10,0,0));
		pDefender->GetNPCInfo()->nDefaultGuardType = GT_PERFECT; // 완전 방어만 함
		MockActorObserver_ForEvent2	ActorObserver;
		pDefender->AttachObserver(&ActorObserver);

		// 방어자가 공격자를 바라보며 방어
		pDefender->SetDirForwardToActor(m_pTester);
		int nGuardTalentID = SUnitTestUtil::NewID();
		GTalentInfo* pGuardTalent = test.talent.NewGuardTalentInfo(nGuardTalentID);
		pGuardTalent->m_nGuardDirection = GUARD_DIRECTION_FRONT;
		pDefender->doGuard(nGuardTalentID);

		// 공격자/방어자 레벨이 동일 (레벨팩터=5), 가드확률=100%, 피해보정률=0.0
		pDefender->SetLevel(100);
		m_pTester->SetLevel(1);

		// 공격자가 공격
		CHECK_EQUAL(0, ActorObserver.GetCallCounter(L"MockActorObserver_ForEvent2::OnPerfectGuard"));
		GTalentInfo* pAtkTalent = test.talent.MakeDamageTalent(100);	// 100의 피해를 주는 탤런트
		test.talent.UseTalent(m_pTester, pAtkTalent, pDefender->GetUID());

		// 피해 확인
		CHECK_EQUAL(1, ActorObserver.GetCallCounter(L"MockActorObserver_ForEvent2::OnPerfectGuard"));
		pDefender->DetachObserver(&ActorObserver);
	}

	TEST_FIXTURE(FTalent, GuardAttr_PerfectGuardOnly)
	{
		// 공격자가 방어자 오른쪽에 위치
		GEntityNPC* pDefender =
			GUTHelper_NPC::SpawnNPC(m_pField);
		pDefender->SetPos(m_pTester->GetPos()-vec3(10,0,0));
		pDefender->GetNPCInfo()->nDefaultGuardType = GT_PARTIAL; // 부분 방어만 함
		MockActorObserver_ForEvent2	ActorObserver;
		pDefender->AttachObserver(&ActorObserver);

		// 방어자가 공격자를 바라보며 방어
		pDefender->SetDirForwardToActor(m_pTester);
		int nGuardTalentID = SUnitTestUtil::NewID();
		GTalentInfo* pGuardTalent = test.talent.NewGuardTalentInfo(nGuardTalentID);
		pGuardTalent->m_nGuardDirection = GUARD_DIRECTION_FRONT;
		pDefender->doGuard(nGuardTalentID);

		// 공격자가 공격
		CHECK_EQUAL(0, ActorObserver.GetCallCounter(L"MockActorObserver_ForEvent2::OnPerfectGuard"));
		CHECK_EQUAL(0, ActorObserver.GetCallCounter(L"MockActorObserver_ForEvent2::OnPartialGuard"));
		GTalentInfo* pAtkTalent = test.talent.MakeDamageTalent(10);	// 10의 피해를 주는 탤런트
		pAtkTalent->m_bPerfectGuardOnly = true;
		test.talent.UseTalent(m_pTester, pAtkTalent, pDefender->GetUID());
		CHECK_EQUAL(0, ActorObserver.GetCallCounter(L"MockActorObserver_ForEvent2::OnPerfectGuard"));
		CHECK_EQUAL(0, ActorObserver.GetCallCounter(L"MockActorObserver_ForEvent2::OnPartialGuard"));

		pDefender->DetachObserver(&ActorObserver);
	}

	TEST_FIXTURE(FTalent, GuardAttr_PartaialGuardHit)
	{
		// 공격자가 방어자 오른쪽에 위치
		GEntityNPC* pDefender =
			GUTHelper_NPC::SpawnNPC(m_pField);
		pDefender->SetPos(m_pTester->GetPos()-vec3(10,0,0));
		pDefender->GetNPCInfo()->nDefaultGuardType = GT_PERFECT; // 완전 방어만 함
		MockActorObserver_ForEvent2	ActorObserver;
		pDefender->AttachObserver(&ActorObserver);

		// 방어자가 공격자를 바라보며 방어
		pDefender->SetDirForwardToActor(m_pTester);
		int nGuardTalentID = SUnitTestUtil::NewID();
		GTalentInfo* pGuardTalent = test.talent.NewGuardTalentInfo(nGuardTalentID);
		pGuardTalent->m_nGuardDirection = GUARD_DIRECTION_FRONT;
		pDefender->doGuard(nGuardTalentID);

		// 공격자가 공격, 부분 방어로 처리됨
		CHECK_EQUAL(0, ActorObserver.GetCallCounter(L"MockActorObserver_ForEvent2::OnPerfectGuard"));
		CHECK_EQUAL(0, ActorObserver.GetCallCounter(L"MockActorObserver_ForEvent2::OnPartialGuard"));
		GTalentInfo* pAtkTalent = test.talent.MakeDamageTalent(10);	// 10의 피해를 주는 탤런트
		pAtkTalent->m_bPartialGuardHit = true;
		test.talent.UseTalent(m_pTester, pAtkTalent, pDefender->GetUID());
		CHECK_EQUAL(0, ActorObserver.GetCallCounter(L"MockActorObserver_ForEvent2::OnPerfectGuard"));
		CHECK_EQUAL(1, ActorObserver.GetCallCounter(L"MockActorObserver_ForEvent2::OnPartialGuard"));

		pDefender->DetachObserver(&ActorObserver);
	}

	TEST_FIXTURE(FTalent, AbsoluteGuard)
	{
		test.item.EquipNewShield(m_pTester);

		// 절대방어 버프 얻기
		GainAbsoluteGuardBuff(m_pTester);

		// 공격자가 방어자 왼쪽에 위치
		GEntityPlayer* pAttacker = 
			NewEntityPlayer(m_pField, m_pTester->GetPos()-vec3(-10,0,0));

		// 공격자/방어자 레벨이 동일 (레벨팩터=5), 가드확률=100%, 피해보정률=0.0
		m_pTester->SetLevel(1);
		pAttacker->SetLevel(100);

		// 방어자가 공격자를 바라보며 방어
		m_pTester->SetDirForwardToActor(pAttacker);
		int nGuardTalentID = SUnitTestUtil::NewID();
		GTalentInfo* pGuardTalent = test.talent.NewGuardTalentInfo(nGuardTalentID);
		pGuardTalent->m_nGuardDirection = GUARD_DIRECTION_FRONT;
		m_pTester->doGuard(nGuardTalentID);

		// 공격자가 공격
		m_pTester->SetHP(m_pTester->GetMaxHP());
		CHECK_EQUAL(0, m_ActorObserver.GetCallCounter(L"MockActorObserver_ForEvent2::OnAbsoluteGuard"));
		GTalentInfo* pAtkTalent = test.talent.MakeDamageTalent(100);	// 100의 피해를 주는 탤런트
		test.talent.UseTalent(pAttacker, pAtkTalent, m_pTester->GetUID());

		// 피해 확인
		CHECK_EQUAL(m_pTester->GetHP(), m_pTester->GetMaxHP());	// 피해 입지 않음
		CHECK_EQUAL(1, m_ActorObserver.GetCallCounter(L"MockActorObserver_ForEvent2::OnAbsoluteGuard"));
	}

	TEST_FIXTURE(FTalent, AbsoluteGuard_NeedShield)
	{
		// 절대방어 버프 얻기
		GainAbsoluteGuardBuff(m_pTester);

		// 공격자가 방어자 왼쪽에 위치
		GEntityPlayer* pAttacker = 
			NewEntityPlayer(m_pField, m_pTester->GetPos()-vec3(-10,0,0));

		// 공격자/방어자 레벨이 동일 (레벨팩터=5), 가드확률=100%, 피해보정률=0.0
		m_pTester->SetLevel(1);
		pAttacker->SetLevel(100);

		// 방어자가 공격자를 바라보며 방어
		m_pTester->SetDirForwardToActor(pAttacker);
		int nGuardTalentID = SUnitTestUtil::NewID();
		GTalentInfo* pGuardTalent = test.talent.NewGuardTalentInfo(nGuardTalentID);
		pGuardTalent->m_nGuardDirection = GUARD_DIRECTION_FRONT;
		m_pTester->doGuard(nGuardTalentID);

		// 공격자가 공격
		CHECK_EQUAL(0, m_ActorObserver.GetCallCounter(L"MockActorObserver_ForEvent2::OnAbsoluteGuard"));
		GTalentInfo* pAtkTalent = test.talent.MakeDamageTalent(100);	// 100의 피해를 주는 탤런트
		test.talent.UseTalent(pAttacker, pAtkTalent, m_pTester->GetUID());

		// 피해 확인
		CHECK(m_pTester->GetHP() != m_pTester->GetMaxHP());	// 피해 입음
		CHECK_EQUAL(0, m_ActorObserver.GetCallCounter(L"MockActorObserver_ForEvent2::OnAbsoluteGuard"));
	}

	TEST_FIXTURE(FTalent, AbsoluteGuard_GuardMagic)
	{
		test.item.EquipNewShield(m_pTester);

		// 절대방어 버프 얻기
		GainAbsoluteGuardBuff(m_pTester);

		// 공격자가 방어자 왼쪽에 위치
		GEntityNPC* pAttacker = GUTHelper_NPC::SpawnNPC(m_pField, 777, m_pTester->GetPos()-vec3(-10,0,0));
		pAttacker->GetNPCStatus().nAA = NAAT_FACTION;

		// 공격자/방어자 레벨이 동일 (레벨팩터=5), 가드확률=100%, 피해보정률=0.0
		m_pTester->SetLevel(1);
		pAttacker->SetLevel(100);

		// 방어자가 공격자를 바라보며 방어
		m_pTester->SetDirForwardToActor(pAttacker);
		int nGuardTalentID = SUnitTestUtil::NewID();
		GTalentInfo* pGuardTalent = test.talent.NewGuardTalentInfo(nGuardTalentID);
		pGuardTalent->m_nGuardDirection = GUARD_DIRECTION_FRONT;
		m_pTester->doGuard(nGuardTalentID);

		// 공격자가 공격
		m_pTester->SetHP(m_pTester->GetMaxHP());
		float fCastingTime = 5.0f;
		float fDurationTime = 10.0f;
		float fEventTime = 3.0f;
		float fProjectileSpeed = 200.0f;
		GTalentInfo* pTalentInfo = MakeMissileMagicTalent(fCastingTime, fDurationTime, fEventTime, fProjectileSpeed);
		test.talent.SetTalentDamage(pTalentInfo, 100);
		test.talent.UseTalent(pAttacker, pTalentInfo, m_pTester->GetUID());
		GTalent* pTalent = pAttacker->GetModuleCombat()->GetTalent();
		TVALID(pTalent);
		CHECK_EQUAL(TALENT_PHASE_PREPARE, pTalent->GetPhase());
		pTalent->Update(fCastingTime);	// 캐스팅시간만큼 대기
		pTalent->ActTalent(m_pTester->GetUID());	
		CHECK_EQUAL(TALENT_PHASE_ACT, pTalent->GetPhase());
		pTalent->Update(fEventTime);	// 발사체 발사시간 대기
		pAttacker->GetModuleCombat()->Update(10.0f);	// 발사체 충돌시간 대기

		// 피해 확인
		CHECK_EQUAL(m_pTester->GetHP(), m_pTester->GetMaxHP());	// 피해 입지 않음
	}
}

