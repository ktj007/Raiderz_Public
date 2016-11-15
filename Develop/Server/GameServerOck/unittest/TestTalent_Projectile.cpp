#include "stdafx.h"
#include "GTestForward.h"
#include "GTalent.h"
#include "GModuleCombat.h"
#include "FBasePlayer.h"
#include "GRelationChecker.h"
#include "CSFactionCalculator.h"
#include "GFactionInfo.h"
#include "GPlayerFactions.h"

SUITE(TalentProjectile)
{
	struct FTalent : public FBasePlayer
	{
		FTalent()
		{
			m_pField = GUTHelper_Field::DefaultMockField();
			m_pNPC = GUTHelper_NPC::SpawnNPC(m_pField);
			m_pNPC->GetNPCStatus().nAA = NAAT_FACTION;
		}
		~FTalent()
		{
			m_pField->Destroy();
			test.network.ClearLinks();
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

		GTalentInfo* MakeGuidedMissileMagicTalent(float fCastingTime, float fDurationTime, float fEventTime, float fProjectileSpeed)
		{
			GTalentInfo* pTalentInfo = test.talent.NewTalentInfo();
			// 탤런트 시간 --------------------------
			pTalentInfo->m_fCastingTime = fCastingTime;
			pTalentInfo->m_fDurationTime = fDurationTime;
			// 발사체 -------------------------------
			pTalentInfo->m_nSkillType = ST_MAGIC;
			pTalentInfo->m_nTiming = TC_HIT_ENEMY;

			// 발사체 이벤트 추가
			CSTalentEventInfo talentEventInfo;
			talentEventInfo.m_nEvent = TE_LAUNCH_PROJECTILE;
			talentEventInfo.m_fTime1 = fEventTime;
			talentEventInfo.m_ProjectileInfo.m_nType = TPT_MISSILE_GUIDED;
			talentEventInfo.m_ProjectileInfo.m_fSpeed = fProjectileSpeed;	// 발사체속도: (x)m/s
			pTalentInfo->m_Events.m_vecActEvents.push_back(talentEventInfo);

			pTalentInfo->m_EffectInfo.m_nPoint = CSEffectInfo::POINT_TARGET;
			pTalentInfo->Cooking();
			return pTalentInfo;
		}

		GTalentInfo* MakeMultipleGuidedMissileMagicTalent(float fCastingTime, float fDurationTime, float fEventTime, float fProjectileSpeed, int nAmount)
		{
			GTalentInfo* pTalentInfo = test.talent.NewTalentInfo();
			// 탤런트 시간 --------------------------
			pTalentInfo->m_fCastingTime = fCastingTime;
			pTalentInfo->m_fDurationTime = fDurationTime;
			// 발사체 -------------------------------
			pTalentInfo->m_nSkillType = ST_MAGIC;
			pTalentInfo->m_nTiming = TC_HIT_ENEMY;

			for (int i=0; i<nAmount; ++i)
			{
				// 발사체 이벤트 추가
				CSTalentEventInfo talentEventInfo;
				talentEventInfo.m_nEvent = TE_LAUNCH_PROJECTILE;
				talentEventInfo.m_fTime1 = fEventTime*(i+1);
				talentEventInfo.m_ProjectileInfo.m_nType = TPT_MISSILE_GUIDED;
				talentEventInfo.m_ProjectileInfo.m_fSpeed = fProjectileSpeed;	// 발사체속도: (x)m/s
				pTalentInfo->m_Events.m_vecActEvents.push_back(talentEventInfo);
			}
			
			pTalentInfo->m_EffectInfo.m_nPoint = CSEffectInfo::POINT_TARGET;
			pTalentInfo->Cooking();
			return pTalentInfo;
		}

		GTalentInfo* MakeMultipleGuidedMissileMagicTalentAtSameTime(float fCastingTime, float fDurationTime, float fEventTime, float fProjectileSpeed, int nAmount)
		{
			GTalentInfo* pTalentInfo = test.talent.NewTalentInfo();
			// 탤런트 시간 --------------------------
			pTalentInfo->m_fCastingTime = fCastingTime;
			pTalentInfo->m_fDurationTime = fDurationTime;
			// 발사체 -------------------------------
			pTalentInfo->m_nSkillType = ST_MAGIC;
			pTalentInfo->m_nTiming = TC_HIT_ENEMY;

			for (int i=0; i<nAmount; ++i)
			{
				// 발사체 이벤트 추가
				CSTalentEventInfo talentEventInfo;
				talentEventInfo.m_nEvent = TE_LAUNCH_PROJECTILE;
				talentEventInfo.m_fTime1 = fEventTime;
				talentEventInfo.m_ProjectileInfo.m_nType = TPT_MISSILE_GUIDED;
				talentEventInfo.m_ProjectileInfo.m_fSpeed = fProjectileSpeed;	// 발사체속도: (x)m/s
				pTalentInfo->m_Events.m_vecActEvents.push_back(talentEventInfo);
			}

			pTalentInfo->m_EffectInfo.m_nPoint = CSEffectInfo::POINT_TARGET;
			pTalentInfo->Cooking();
			return pTalentInfo;
		}

		MockField*		m_pField;
		GEntityNPC*		m_pNPC;
		MockLink*		m_pLinkTester;
		DECLWRAPPER_FieldInfoMgr;
		DECLWRAPPER_FieldMgr;
		DECLWRAPPER_TalentMgr;
	};

	TEST_FIXTURE(FTalent, NoEvent)
	{
		GTalentInfo* pTalentInfo = test.talent.NewTalentInfo();
		// 탤런트 시간 --------------------------
		pTalentInfo->m_fCastingTime = 0.0f;
		pTalentInfo->m_fDurationTime = 20.0f;
		// 발사체 -------------------------------
		pTalentInfo->m_nSkillType = ST_MAGIC;
		pTalentInfo->m_nTiming = TC_HIT_ENEMY;
		pTalentInfo->m_nMinDamage = pTalentInfo->m_nMaxDamage = 10;	// 피해 10입힘
		pTalentInfo->Cooking();

		GEntityPlayer* pTargetPlayer = NewEntityPlayer(m_pField);
		pTargetPlayer->SetHP(100);
		test.talent.UseTalent(m_pNPC, pTalentInfo, pTargetPlayer->GetUID());
		GTalent* pTalent = m_pNPC->GetModuleCombat()->GetTalent();
		TVALID(pTalent);
		CHECK_EQUAL(TALENT_PHASE_ACT, pTalent->GetPhase());
		m_pNPC->GetModuleCombat()->Update(pTalentInfo->m_fDurationTime);
		CHECK_EQUAL(100, pTargetPlayer->GetHP());
		CHECK_EQUAL(TALENT_PHASE_FINISH, pTalent->GetPhase());
	}

	TEST_FIXTURE(FTalent, CastingAndLaunchMissile)
	{
		float fCastingTime = 5.0f;
		float fDurationTime = 10.0f;
		float fEventTime = 3.0f;
		float fProjectileSpeed = 200.0f;
		GTalentInfo* pTalentInfo = MakeMissileMagicTalent(fCastingTime, fDurationTime, fEventTime, fProjectileSpeed);

		GEntityPlayer* pTargetPlayer = NewEntityPlayer(m_pField);
		test.talent.UseTalent(m_pNPC, pTalentInfo, pTargetPlayer->GetUID());
		GTalent* pTalent = m_pNPC->GetModuleCombat()->GetTalent();
		TVALID(pTalent);
 		CHECK_EQUAL(TALENT_PHASE_PREPARE, pTalent->GetPhase());
		pTalent->Update(fCastingTime);	// 캐스팅시간만큼 대기
		pTalent->ActTalent(pTargetPlayer->GetUID());	
		CHECK_EQUAL(TALENT_PHASE_ACT, pTalent->GetPhase());
		pTalent->Update(fEventTime);	// 발사체 발사시간 대기

		vector<GTalentHitter*> vecHitters = m_pNPC->GetModuleCombat()->GetTargetHitter(THT_PROJECTILE);
		TVALID_EQ(1U, vecHitters.size());
		CHECK_EQUAL(1, static_cast<GTalentHitter_Projectile*>(vecHitters.front())->GetEnemtyList().size());	// 하나의 액터가 검사 대상자
	}

	TEST_FIXTURE(FTalent, MissileCanNotBeHitAllys)	// 발사체가 아군에게는 피격되지 않음
	{
		float fCastingTime = 5.0f;
		float fDurationTime = 10.0f;
		float fEventTime = 3.0f;
		float fProjectileSpeed = 200.0f;
		GTalentInfo* pTalentInfo = MakeMissileMagicTalent(fCastingTime, fDurationTime, fEventTime, fProjectileSpeed);

		GEntityPlayer* pTargetPlayer = NewEntityPlayer(m_pField);
		// 우호적인 상태로 만듦
		GFactionInfo* pFactionInfo = test.faction.NewFactionInfo(CSFactionCalculator::GetNormalMaxQuantity());
		m_pNPC->GetNPCInfo()->m_nFactionLossID = pFactionInfo->m_nID;
		m_pNPC->GetNPCStatus().nAttackable = NAT_FACTION;
		pTargetPlayer->GetPlayerFactions().Increase(pFactionInfo->m_nID, CSFactionCalculator::GetMaxQuantity());
		GRelationChecker checker;
		CHECK_EQUAL(false, checker.IsEnemy(pTargetPlayer, m_pNPC));
		// 발사체 발사
		test.talent.UseTalent(m_pNPC, pTalentInfo, pTargetPlayer->GetUID());
		GTalent* pTalent = m_pNPC->GetModuleCombat()->GetTalent();
		TVALID(pTalent);
		pTalent->Update(fCastingTime);	// 캐스팅시간만큼 대기
		pTalent->ActTalent(pTargetPlayer->GetUID());	
		pTalent->Update(fEventTime);	// 발사체 발사시간 대기

		vector<GTalentHitter*> vecHitters = m_pNPC->GetModuleCombat()->GetTargetHitter(THT_PROJECTILE);
		TVALID_EQ(1U, vecHitters.size());
		CHECK_EQUAL(0, static_cast<GTalentHitter_Projectile*>(vecHitters.front())->GetEnemtyList().size());	// 검사 대상자가 없음
	}

	TEST_FIXTURE(FTalent, CastingAndLaunchGuideMissile)
	{
		float fCastingTime = 5.0f;
		float fDurationTime = 10.0f;
		float fEventTime = 3.0f;
		float fProjectileSpeed = 200.0f;
		GTalentInfo* pTalentInfo = MakeGuidedMissileMagicTalent(fCastingTime, fDurationTime, fEventTime, fProjectileSpeed);

		GEntityPlayer* pTargetPlayer = NewEntityPlayer(m_pField);
		test.talent.UseTalent(m_pNPC, pTalentInfo, pTargetPlayer->GetUID());
		GTalent* pTalent = m_pNPC->GetModuleCombat()->GetTalent();
		TVALID(pTalent);
		CHECK_EQUAL(TALENT_PHASE_PREPARE, pTalent->GetPhase());
		pTalent->Update(fCastingTime);	// 캐스팅시간만큼 대기
		pTalent->ActTalent(pTargetPlayer->GetUID());	
		CHECK_EQUAL(TALENT_PHASE_ACT, pTalent->GetPhase());
		pTalent->Update(fEventTime);	// 발사체 발사시간 대기

		vector<GTalentHitter*> vecHitters = m_pNPC->GetModuleCombat()->GetTargetHitter(THT_TIMEDELAY);
		TVALID_EQ(1U, vecHitters.size());

		GTalentHitter_TimeDelay* TalentHitter = static_cast<GTalentHitter_TimeDelay*>(vecHitters.front());
		CHECK_EQUAL(100.0f/fProjectileSpeed, TalentHitter->GetDelayHitChecker().GetEstimateTime());	// 100.0f은 발사체의 z좌표, 어느정도 z로 올려서 발사함
	}

	TEST_FIXTURE(FTalent, GuideMissileHit)
	{
		float fCastingTime = 0.0f;
		float fDurationTime = 10.0f;
		float fEventTime = 3.0f;
		
		float fProjectileSpeed = 200.0f;
		GTalentInfo* pTalentInfo = MakeGuidedMissileMagicTalent(fCastingTime, fDurationTime, fEventTime, fProjectileSpeed);
		pTalentInfo->m_nMinDamage = pTalentInfo->m_nMaxDamage = 10;	// 피해 10입힘

		GEntityPlayer* pTargetPlayer = NewEntityPlayer(m_pField);
		test.talent.UseTalent(m_pNPC, pTalentInfo, pTargetPlayer->GetUID());
		GTalent* pTalent = m_pNPC->GetModuleCombat()->GetTalent();
		TVALID(pTalent);
		CHECK_EQUAL(TALENT_PHASE_ACT, pTalent->GetPhase());
		pTalent->Update(fEventTime);	// 발사체 발사시간 대기

		pTargetPlayer->SetHP(100);
		float fHitTime = 100.0f/fProjectileSpeed;
		m_pNPC->GetModuleCombat()->Update(fHitTime);	// 발사체 충돌시간 대기
		CHECK_EQUAL(90, pTargetPlayer->GetHP());
	}

	TEST_FIXTURE(FTalent, GuidedMissileSeveralHit)
	{
		float fCastingTime = 0.0f;
		float fDurationTime = 10.0f;
		float fEventTime = 3.0f;
		
		float fProjectileSpeed = 200.0f;
		GTalentInfo* pTalentInfo = MakeGuidedMissileMagicTalent(fCastingTime, fDurationTime, fEventTime, fProjectileSpeed);
		pTalentInfo->m_nMinDamage = pTalentInfo->m_nMaxDamage = 10;	// 피해 10입힘

		GEntityPlayer* pTargetPlayer = NewEntityPlayer(m_pField);
		pTargetPlayer->SetHP(100);
		for (int i=0; i<5; i++)
		{
			test.talent.UseTalent(m_pNPC, pTalentInfo, pTargetPlayer->GetUID());
			GTalent* pTalent = m_pNPC->GetModuleCombat()->GetTalent();
			TVALID(pTalent);
			CHECK_EQUAL(TALENT_PHASE_ACT, pTalent->GetPhase());
			pTalent->Update(fEventTime);	// 발사체 발사시간 대기
			
			float fHitTime = 100.0f/fProjectileSpeed;
			m_pNPC->GetModuleCombat()->Update(fHitTime);	// 발사체 충돌시간 대기
			CHECK_EQUAL(100-(i+1)*10, pTargetPlayer->GetHP());
		}
	}
	
	TEST_FIXTURE(FTalent, MuiltiGuidedMissile) // 하나의 탤런트에서 여러 유도발사체 쏘기
	{
		float fCastingTime = 0.0f;
		float fDurationTime = 20.0f;
		float fEventTime = 3.0f;
		float fProjectileSpeed = 200.0f;
		int	  nEventAmount = 5;
		GTalentInfo* pTalentInfo = MakeMultipleGuidedMissileMagicTalent(fCastingTime, fDurationTime, fEventTime, fProjectileSpeed, nEventAmount);
		pTalentInfo->m_nMinDamage = pTalentInfo->m_nMaxDamage = 10;	// 피해 10입힘

		GEntityPlayer* pTargetPlayer = NewEntityPlayer(m_pField);
		pTargetPlayer->SetHP(100);
		test.talent.UseTalent(m_pNPC, pTalentInfo, pTargetPlayer->GetUID());
		GTalent* pTalent = m_pNPC->GetModuleCombat()->GetTalent();
		TVALID(pTalent);
		CHECK_EQUAL(TALENT_PHASE_ACT, pTalent->GetPhase());

		// 발사체 5개 충돌판정
		for (int i=0; i<nEventAmount; i++)
		{
			pTalent->Update(fEventTime);	// 발사체 발사시간 대기
			float fHitTime = 100.0f/fProjectileSpeed;
			m_pNPC->GetModuleCombat()->Update(fHitTime);	// 발사체 충돌시간 대기
			CHECK_EQUAL(100-(i+1)*10, pTargetPlayer->GetHP());
		}
	}

	TEST_FIXTURE(FTalent, TalentEventTimeOrdering) // 탤런트 이벤트 컨테이너의 엘리먼트를 발생시간 기준으로 정렬하기
	{
		float fCastingTime = 0.0f;
		float fDurationTime = 20.0f;
		float fEventTime = 3.0f;
		float fProjectileSpeed = 200.0f;
		int	  nEventAmount = 5;
		GTalentInfo* pTalentInfo = MakeMultipleGuidedMissileMagicTalent(fCastingTime, fDurationTime, fEventTime, fProjectileSpeed, nEventAmount);
		pTalentInfo->m_nMinDamage = pTalentInfo->m_nMaxDamage = 10;	// 피해 10입힘

		// 이벤트 발생시간을 섞기
		pTalentInfo->m_Events.m_vecActEvents[0].m_fTime1 = 7.52f;
		pTalentInfo->m_Events.m_vecActEvents[1].m_fTime1 = 0.94f;
		pTalentInfo->m_Events.m_vecActEvents[2].m_fTime1 = 3.32f;
		pTalentInfo->m_Events.m_vecActEvents[3].m_fTime1 = 5.78f;
		pTalentInfo->m_Events.m_vecActEvents[4].m_fTime1 = 9.11f;
		pTalentInfo->Cooking();

		// 정렬된 시간 체크
		CHECK_EQUAL(0.94f, pTalentInfo->m_Events.m_vecActEvents[0].m_fTime1);
		CHECK_EQUAL(3.32f, pTalentInfo->m_Events.m_vecActEvents[1].m_fTime1);
		CHECK_EQUAL(5.78f, pTalentInfo->m_Events.m_vecActEvents[2].m_fTime1);
		CHECK_EQUAL(7.52f, pTalentInfo->m_Events.m_vecActEvents[3].m_fTime1);
		CHECK_EQUAL(9.11f, pTalentInfo->m_Events.m_vecActEvents[4].m_fTime1);
	}

	TEST_FIXTURE(FTalent, MuiltiGuidedMissileAtSameTime) // 하나의 탤런트에서 동시에 여러 유도발사체를 동시에 쏘기
	{
		// 테스트 설명 --------------
		// - 3초후에 동시에 5개의 발사체 피격
		// - 6초부터 3초 간격으로 5개의 발사체가 순차적으로 피격

		float fCastingTime = 0.0f;
		float fDurationTime = 20.0f;
		float fEventTime = 3.0f;
		float fProjectileSpeed = 200.0f;
		int	  nEventAmount = 5;
		GTalentInfo* pTalentInfo = MakeMultipleGuidedMissileMagicTalentAtSameTime(fCastingTime, fDurationTime, fEventTime, fProjectileSpeed, nEventAmount);
		pTalentInfo->m_nMinDamage = pTalentInfo->m_nMaxDamage = 10;	// 피해 10입힘

		// 추가 발사체 이벤트 추가
		for (int i=0; i<nEventAmount; ++i)
		{
			CSTalentEventInfo talentEventInfo;
			talentEventInfo.m_nEvent = TE_LAUNCH_PROJECTILE;
			talentEventInfo.m_fTime1 = fEventTime*(i+2);
			talentEventInfo.m_ProjectileInfo.m_nType = TPT_MISSILE_GUIDED;
			talentEventInfo.m_ProjectileInfo.m_fSpeed = fProjectileSpeed;	// 발사체속도: (x)m/s
			pTalentInfo->m_Events.m_vecActEvents.push_back(talentEventInfo);
		}
		pTalentInfo->Cooking();
		CHECK_EQUAL(10U, pTalentInfo->m_Events.m_vecActEvents.size());


		GEntityPlayer* pTargetPlayer = NewEntityPlayer(m_pField);
		pTargetPlayer->SetHP(100);
		test.talent.UseTalent(m_pNPC, pTalentInfo, pTargetPlayer->GetUID());
		GTalent* pTalent = m_pNPC->GetModuleCombat()->GetTalent();
		TVALID(pTalent);
		CHECK_EQUAL(TALENT_PHASE_ACT, pTalent->GetPhase());

		// 동시 이벤트, 발사체 5개 충돌판정
		pTalent->Update(fEventTime);	// 발사체 발사시간 대기
		float fHitTime = 100.0f/fProjectileSpeed;
		m_pNPC->GetModuleCombat()->Update(fHitTime);	// 발사체 충돌시간 대기
		CHECK_EQUAL(50, pTargetPlayer->GetHP());

		// 순차 이벤트, 발사체 5개 충돌판정
		for (int i=0; i<nEventAmount; i++)
		{
			pTalent->Update(fEventTime);	// 발사체 발사시간 대기
			float fHitTime = 100.0f/fProjectileSpeed;
			m_pNPC->GetModuleCombat()->Update(fHitTime);	// 발사체 충돌시간 대기
			CHECK_EQUAL(50-(i+1)*10, pTargetPlayer->GetHP());
		}
	}
}