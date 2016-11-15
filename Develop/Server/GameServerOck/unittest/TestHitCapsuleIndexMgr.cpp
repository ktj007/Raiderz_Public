#include "StdAfx.h"
#include "GTestForward.h"
#include "GFieldMgr.h"
#include "CSHitCapsuleIndexMgr.h"
#include "GModuleCombat.h"
#include "GTalent.h"
#include "GTalentInfoMgr.h"
#include "GHateTable.h"
#include "GNPCSwallow.h"

SUITE(HitCapsuleIndexMgr)
{
	struct FHitCapsuleIndexMgr
	{
		FHitCapsuleIndexMgr()
		{
			m_pField = GUTHelper_Field::DefaultMockField();

			m_pNPC = GUTHelper_NPC::SpawnNPC(m_pField);

			// 캡슐 정보 추가
			m_pNPCInfo = new GNPCInfo;
			m_pNPCInfo_Mode1 = new GNPCInfo;
			m_pNPCInfo->vecModes.push_back(m_pNPCInfo_Mode1);
			m_pNPCInfo->m_pMeshInfo = new GActorMeshInfo();
			m_pNPCInfo->m_pMeshInfo->ReserveHitGroupForTest(5);
			m_pNPCInfo->nAA = NAAT_ALWAYS;
			m_pNPCInfo->nMode = NPC_MODE_DEFAULT;
			m_pNPCInfo->nHitTestIndex = DEFAULT_HIT_INDEX_MODE0;			// 0번 모드의 기본 히트그룹아이디
			m_pNPCInfo->nEatOwnerBuffID = 560;
			m_pNPCInfo->nEatTargetBuffID = 561;
			m_pNPCInfo->fEatPeriod = 2.0f;
			m_pNPCInfo_Mode1->nMode = NPC_MODE_1;
			m_pNPCInfo_Mode1->nHitTestIndex = 21;	// 1번 모드의 기본 히트그룹아이디

			test.buff.NewBuffInfo(m_pNPCInfo->nEatOwnerBuffID);
			test.buff.NewBuffInfo(m_pNPCInfo->nEatTargetBuffID);

			m_pNPC->SetNPCInfo(m_pNPCInfo);
		}

		~FHitCapsuleIndexMgr()
		{
			m_pField->Destroy();
			SAFE_DELETE(m_pNPCInfo->m_pMeshInfo);
			SAFE_DELETE(m_pNPCInfo);			
		}

		static const int DEFAULT_HIT_INDEX_MODE0 = 77;

		MockField*		m_pField;
		GEntityNPC*		m_pNPC;
		GNPCInfo*		m_pNPCInfo;
		GNPCInfo*		m_pNPCInfo_Mode1;
		DECLWRAPPER_FieldInfoMgr;
		DECLWRAPPER_FieldMgr;
		DECLWRAPPER_TalentMgr;
		DECLWRAPPER_BuffMgr;
	};

	TEST(Default)
	{
		CSHitCapsuleIndexMgr mgrHitIndex;
		CHECK_EQUAL(INVALID_HITCAPSULEGROUP_ID, mgrHitIndex.GetGroupIndex());
	}

	TEST(ModeChange)
	{
		CSHitCapsuleIndexMgr mgrHitIndex;
		mgrHitIndex.InsertNPCModeGroupIndex(0, 77);
		mgrHitIndex.InsertNPCModeGroupIndex(1, 21);
		CHECK_EQUAL(77, (int)mgrHitIndex.GetGroupIndex());
		mgrHitIndex.ChangeNPCMode(1);
		CHECK_EQUAL(21, (int)mgrHitIndex.GetGroupIndex());
	}

	TEST(UseTalent)
	{
		CSHitCapsuleIndexMgr mgrHitIndex;
		mgrHitIndex.InsertNPCModeGroupIndex(0, 77);
		CHECK_EQUAL(77, (int)mgrHitIndex.GetGroupIndex());
		mgrHitIndex.Change_ByTalent(55);							// 탤런트 시작
		CHECK_EQUAL(55, (int)mgrHitIndex.GetGroupIndex());		// 탤런트에 의한 변화가 우선순위가 더 높다
		mgrHitIndex.Change_ByTalentComplete();	// 탤런트 끝
		CHECK_EQUAL(77, (int)mgrHitIndex.GetGroupIndex());
	}

	TEST(UseTalent_Invalid)
	{
		CSHitCapsuleIndexMgr mgrHitIndex;
		mgrHitIndex.InsertNPCModeGroupIndex(0, 77);
		CHECK_EQUAL(77, (int)mgrHitIndex.GetGroupIndex());
		mgrHitIndex.Change_ByTalent(INVALID_HITCAPSULEGROUP_ID);							// 탤런트 시작
		CHECK_EQUAL(INVALID_HITCAPSULEGROUP_ID, (int)mgrHitIndex.GetGroupIndex());		// 탤런트에 의한 변화가 우선순위가 더 높다
		mgrHitIndex.Change_ByTalentComplete();	// 탤런트 끝
		CHECK_EQUAL(77, (int)mgrHitIndex.GetGroupIndex());
	}

	TEST(Trigger)
	{
		CSHitCapsuleIndexMgr mgrHitIndex;
		mgrHitIndex.InsertNPCModeGroupIndex(0, 77);
		mgrHitIndex.InsertNPCModeGroupIndex(1, 21);
		CHECK_EQUAL(77, (int)mgrHitIndex.GetGroupIndex());
		mgrHitIndex.Change_ByTrigger(12);					
		CHECK_EQUAL(12, (int)mgrHitIndex.GetGroupIndex());		// 트리거에 의해 모드가 영구히 바뀜
		mgrHitIndex.ChangeNPCMode(1);	
		CHECK_EQUAL(21, (int)mgrHitIndex.GetGroupIndex());
		mgrHitIndex.ChangeNPCMode(0);	
		CHECK_EQUAL(12, (int)mgrHitIndex.GetGroupIndex());		// 모드가 되돌아가도 변화가 유지되어있음
	}

	TEST_FIXTURE(FHitCapsuleIndexMgr, NPC_ModeChange)
	{
		CHECK_EQUAL(DEFAULT_HIT_INDEX_MODE0, (int)m_pNPC->GetCapsuleGroupIndex());
		m_pNPC->ChangeMode(NPC_MODE_1);
		CHECK_EQUAL(21, (int)m_pNPC->GetCapsuleGroupIndex());
	}

	TEST_FIXTURE(FHitCapsuleIndexMgr, NPC_UseTalent_Change)
	{
		// 초기 캡슐 ID는 0번모드의 것을 사용
		CHECK_EQUAL(DEFAULT_HIT_INDEX_MODE0, (int)m_pNPC->GetCapsuleGroupIndex());

		// 탤런트 사용하기
		GTalentInfo* pTestTalent = test.talent.MakeDamageTalent(1);
		pTestTalent->m_nHitCapsuleGroupIndex = 1;		// 탤런트 사용되는 1번 모드로 바뀜
		TALENT_TARGET_INFO target_info;
		target_info.uidTarget = m_pNPC->GetUID();
		m_pNPC->GetModuleCombat()->UseTalent(pTestTalent, target_info, true);
		GTalent* pTaelnt = m_pNPC->GetModuleCombat()->GetTalent();
		TVALID(pTaelnt);
		CHECK_EQUAL(TALENT_PHASE_ACT, pTaelnt->GetPhase());
		CHECK_EQUAL(1, (int)m_pNPC->GetCapsuleGroupIndex());	// 탤런트 사용중이라 1번 모드
		m_pNPC->GetField()->Update(0.0f);
		CHECK_EQUAL(TALENT_PHASE_FINISH, pTaelnt->GetPhase());
		CHECK_EQUAL(DEFAULT_HIT_INDEX_MODE0, (int)m_pNPC->GetCapsuleGroupIndex());
	}

	TEST_FIXTURE(FHitCapsuleIndexMgr, NPC_UseSwallowTalent_SuccessChange)
	{
		// 초기 캡슐 ID는 0번모드의 것을 사용
		CHECK_EQUAL(DEFAULT_HIT_INDEX_MODE0, (int)m_pNPC->GetCapsuleGroupIndex());

		// 탤런트 정의
		GUTHelper		m_Helper;
		GTalentInfo*	pTestTalent;
		pTestTalent = m_Helper.NewTalentInfo();
		pTestTalent->m_nSkillType = ST_EXTRA_ACTIVE;
		pTestTalent->m_nExtraActive = TEAT_NPC_ONLY_EAT;
		pTestTalent->m_nTiming = TC_HIT_TALENT;
		pTestTalent->m_EffectInfo.m_nRelation = CSEffectInfo::RELATION_ENEMY;
		m_Helper.SetTalentDamage(pTestTalent, 1);		// 데미지 1로 설정
		m_Helper.AddSimpleHitInfo(pTestTalent, 0.0f);	// 바로 판정하는 탤런트임

		// 판정 타입은 AREA
		pTestTalent->m_EffectInfo.m_fRadius = 1.0f;
		pTestTalent->m_fDurationTime = 2.0f;		// aitime 2초
		pTestTalent->m_fExtraPhaseTime = 2.0f;	// 먹는 애니메이션시간
		pTestTalent->m_fExtraPhaseTime2 = 3.0f;	// 뱉는 애니메이션시간
		pTestTalent->m_fExtraPhaseTime3 = 4.0f;	// 뱉는 애니메이션시간 (공격받아 강제로 뱉기)
		pTestTalent->m_nExtraActiveParam1 = 10;  // 먹는 시간

		// 히트캡슐그룹 변경정보
		pTestTalent->m_nHitCapsuleGroupIndex = 1;		// 탤런트 사용되는 1번 모드로 바뀜
		pTestTalent->m_nExtraHitCapsuleGroup = 2;
		pTestTalent->m_nExtraHitCapsuleGroup2 = 3;
		pTestTalent->m_nExtraHitCapsuleGroup3 = 4;

		pTestTalent->Cooking();

		// 적 생성
		GEntityPlayer*	m_pPlayer = GUTHelper::NewEntityPlayer(m_pField);
	

		// 탤런트 사용하기
		TALENT_TARGET_INFO target_info;
		target_info.uidTarget = m_pNPC->GetUID();
		m_pNPC->GetHateTable().AddPoint_FoundEnemy(m_pNPC);
		m_pNPC->GetModuleCombat()->UseTalent(pTestTalent, target_info, true);
		GTalent* pTaelnt = m_pNPC->GetModuleCombat()->GetTalent();
		TVALID(pTaelnt);
		CHECK_EQUAL(TALENT_PHASE_ACT, pTaelnt->GetPhase());
		CHECK_EQUAL(1, (int)m_pNPC->GetCapsuleGroupIndex());	// 탤런트 사용중이라 1번 모드
		m_pNPC->GetField()->Update(0.0f);
		CHECK_EQUAL(TALENT_PHASE_EXTRA, pTaelnt->GetPhase());
		CHECK_EQUAL(2, (int)m_pNPC->GetCapsuleGroupIndex());
		m_pField->Update(pTestTalent->m_fExtraPhaseTime+(float)pTestTalent->m_nExtraActiveParam1);
		CHECK_EQUAL(TALENT_PHASE_EXTRA2, pTaelnt->GetPhase());
		CHECK_EQUAL(3, (int)m_pNPC->GetCapsuleGroupIndex());
		m_pField->Update(pTestTalent->m_fExtraPhaseTime2);
		CHECK_EQUAL(TALENT_PHASE_FINISH, pTaelnt->GetPhase());
		CHECK_EQUAL(DEFAULT_HIT_INDEX_MODE0, (int)m_pNPC->GetCapsuleGroupIndex());
	}

	TEST_FIXTURE(FHitCapsuleIndexMgr, NPC_UseSwallowTalent_FailedChange)
	{
		// 초기 캡슐 ID는 0번모드의 것을 사용
		CHECK_EQUAL(DEFAULT_HIT_INDEX_MODE0, (int)m_pNPC->GetCapsuleGroupIndex());

		// 탤런트 정의
		GUTHelper		m_Helper;
		GTalentInfo*	pTestTalent;
		pTestTalent = m_Helper.NewTalentInfo();
		pTestTalent->m_nSkillType = ST_EXTRA_ACTIVE;
		pTestTalent->m_nExtraActive = TEAT_NPC_ONLY_EAT;
		pTestTalent->m_nTiming = TC_HIT_TALENT;
		pTestTalent->m_EffectInfo.m_nRelation = CSEffectInfo::RELATION_ENEMY;
		m_Helper.SetTalentDamage(pTestTalent, 1);		// 데미지 1로 설정
		m_Helper.AddSimpleHitInfo(pTestTalent, 0.0f);	// 바로 판정하는 탤런트임

		// 판정 타입은 AREA
		pTestTalent->m_EffectInfo.m_fRadius = 1.0f;
		pTestTalent->m_fDurationTime = 2.0f;		// aitime 2초
		pTestTalent->m_fExtraPhaseTime = 2.0f;	// 먹는 애니메이션시간
		pTestTalent->m_fExtraPhaseTime2 = 3.0f;	// 뱉는 애니메이션시간
		pTestTalent->m_fExtraPhaseTime3 = 4.0f;	// 뱉는 애니메이션시간 (공격받아 강제로 뱉기)
		pTestTalent->m_nExtraActiveParam1 = 10;  // 먹는 시간

		// 히트캡슐그룹 변경정보
		pTestTalent->m_nHitCapsuleGroupIndex = 1;		// 탤런트 사용되는 1번 모드로 바뀜
		pTestTalent->m_nExtraHitCapsuleGroup = 2;
		pTestTalent->m_nExtraHitCapsuleGroup2 = 3;
		pTestTalent->m_nExtraHitCapsuleGroup3 = 4;

		pTestTalent->Cooking();

		// 적 생성
		GEntityPlayer*	m_pPlayer = GUTHelper::NewEntityPlayer(m_pField);

		// 탤런트 사용하기
		TALENT_TARGET_INFO target_info;
		target_info.uidTarget = m_pNPC->GetUID();
		m_pNPC->GetHateTable().AddPoint_FoundEnemy(m_pNPC);
		m_pNPC->GetModuleCombat()->UseTalent(pTestTalent, target_info, true);
		GTalent* pTaelnt = m_pNPC->GetModuleCombat()->GetTalent();
		TVALID(pTaelnt);
		CHECK_EQUAL(TALENT_PHASE_ACT, pTaelnt->GetPhase());
		CHECK_EQUAL(1, (int)m_pNPC->GetCapsuleGroupIndex());	// 탤런트 사용중이라 1번 모드
		m_pNPC->GetField()->Update(0.0f);
		CHECK_EQUAL(TALENT_PHASE_EXTRA, pTaelnt->GetPhase());
		CHECK_EQUAL(2, (int)m_pNPC->GetCapsuleGroupIndex());
		m_pField->Update(0.0f);
		m_pNPC->GetNPCSwallow().Spewup(true);	// 공격받아 뱉어냄
		m_pNPC->GetModuleCombat()->Update(0.0f);
		CHECK_EQUAL(TALENT_PHASE_EXTRA3, pTaelnt->GetPhase());
		CHECK_EQUAL(4, (int)m_pNPC->GetCapsuleGroupIndex());
		m_pField->Update(pTestTalent->m_fExtraPhaseTime3);
		CHECK_EQUAL(TALENT_PHASE_FINISH, pTaelnt->GetPhase());
		CHECK_EQUAL(DEFAULT_HIT_INDEX_MODE0, (int)m_pNPC->GetCapsuleGroupIndex());
	}

}