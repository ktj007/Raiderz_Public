#include "stdafx.h"
#include "GTestForward.h"
#include "GTalent.h"
#include "GModuleCombat.h"
#include "FBasePlayer.h"

SUITE(TalentCasting)
{
	struct FTalent : public FBasePlayer
	{
		FTalent()
		{
			m_pField = GUTHelper_Field::DefaultMockField();
			m_pPlayer = NewEntityPlayer(m_pField);
			m_pLinkTester = test.network.NewLink(m_pPlayer);
			swprintf_s(m_pPlayer->GetPlayerInfo()->szName, L"Tester");

		}
		~FTalent()
		{
			m_pField->Destroy();
			test.network.ClearLinks();
		}

		MockField*		m_pField;
		GEntityPlayer*	m_pPlayer;
		MockLink*		m_pLinkTester;
		DECLWRAPPER_FieldInfoMgr;
		DECLWRAPPER_FieldMgr;
		DECLWRAPPER_TalentMgr;
	};

	TEST_FIXTURE(FTalent, InstantActWithoutCastingTime)
	{
		GTalentInfo* pTalentInfo = test.talent.NewTalentInfo();
		pTalentInfo->m_fDurationTime = 10.0f;
		pTalentInfo->Cooking();

		test.talent.UseTalent(m_pPlayer, pTalentInfo, m_pPlayer->GetUID());
		GTalent* pTalent = m_pPlayer->GetModuleCombat()->GetTalent();
		TVALID(pTalent);
 		CHECK_EQUAL(TALENT_PHASE_ACT, pTalent->GetPhase());
	}

	TEST_FIXTURE(FTalent, MaintainPreparePhaseInCastingTime)
	{
		GTalentInfo* pTalentInfo = test.talent.NewTalentInfo();
		pTalentInfo->m_fCastingTime = 5.0f;
		pTalentInfo->m_fDurationTime = 10.0f;
		pTalentInfo->Cooking();

		test.talent.UseTalent(m_pPlayer, pTalentInfo, m_pPlayer->GetUID());
		GTalent* pTalent = m_pPlayer->GetModuleCombat()->GetTalent();
		TVALID(pTalent);
		CHECK_EQUAL(TALENT_PHASE_PREPARE, pTalent->GetPhase());
		pTalent->Update(5.0f);	// 캐스팅시간만큼 대기
		CHECK_EQUAL(TALENT_PHASE_PREPARE, pTalent->GetPhase());	// ACT 요청이 들어올때까지 대기
		pTalent->Update(5.0f);	// 캐스팅시간만큼 대기
		CHECK_EQUAL(TALENT_PHASE_PREPARE, pTalent->GetPhase());	// ACT 요청이 들어올때까지 대기
	}

	TEST_FIXTURE(FTalent, ProceedActPhaseByUserRequest)	// 사용자가 ACT 요청을 해서 ACT 페이즈로 넘어가기
	{
		GTalentInfo* pTalentInfo = test.talent.NewTalentInfo();
		pTalentInfo->m_fCastingTime = 5.0f;
		pTalentInfo->m_fDurationTime = 10.0f;
		pTalentInfo->Cooking();

		test.talent.UseTalent(m_pPlayer, pTalentInfo, m_pPlayer->GetUID());
		GTalent* pTalent = m_pPlayer->GetModuleCombat()->GetTalent();
		TVALID(pTalent);
		CHECK_EQUAL(TALENT_PHASE_PREPARE, pTalent->GetPhase());
		pTalent->Update(5.0f);	// 캐스팅시간만큼 대기
		CHECK_EQUAL(TALENT_PHASE_PREPARE, pTalent->GetPhase());	// ACT 요청이 들어올때까지 대기
		pTalent->ActTalent(m_pPlayer->GetUID());	
		CHECK_EQUAL(TALENT_PHASE_ACT, pTalent->GetPhase());	
	}

	TEST_FIXTURE(FTalent, AutoDelayedByEalryUserRequest)	// 사용자가 ACT 요청을 캐스팅 완료되기 전에 보내면 대기했다가 처리하기
	{
		GTalentInfo* pTalentInfo = test.talent.NewTalentInfo();
		pTalentInfo->m_fCastingTime = 5.0f;
		pTalentInfo->m_fDurationTime = 10.0f;
		pTalentInfo->Cooking();

		test.talent.UseTalent(m_pPlayer, pTalentInfo, m_pPlayer->GetUID());
		GTalent* pTalent = m_pPlayer->GetModuleCombat()->GetTalent();
		TVALID(pTalent);
		pTalent->Update(2.0f);	// 캐스팅시간보다 적게 대기
		pTalent->ActTalent(m_pPlayer->GetUID());	
		CHECK_EQUAL(TALENT_PHASE_PREPARE, pTalent->GetPhase());	
		pTalent->Update(3.0f);	// 캐스팅시간 마저 대기
		CHECK_EQUAL(TALENT_PHASE_ACT, pTalent->GetPhase());	
	}

	TEST_FIXTURE(FTalent, ShouldBeSamePlaceAtActTalent)	// 사용자가 ACT 요청할때의 위치와 USE 요청할때의 위치가 같아야한다.
	{
		GTalentInfo* pTalentInfo = test.talent.NewTalentInfo();
		pTalentInfo->m_fCastingTime = 5.0f;
		pTalentInfo->m_fDurationTime = 10.0f;
		pTalentInfo->Cooking();

		test.talent.UseTalent(m_pPlayer, pTalentInfo, m_pPlayer->GetUID());
		GTalent* pTalent = m_pPlayer->GetModuleCombat()->GetTalent();
		TVALID(pTalent);
		pTalent->Update(5.0f);	// 캐스팅시간만큼 대기
		m_pPlayer->SetPos(vec3(123,456,789));	// 위치를 변경함
		pTalent->ActTalent(m_pPlayer->GetUID());	
		CHECK_EQUAL(TALENT_PHASE_CANCELED, pTalent->GetPhase());
	}

	TEST_FIXTURE(FTalent, ActTalentTargetCheck)	// 사용자가 ACT 요청할때 보낸 타겟으로 제대로 변경되었는지 확인
	{
		GEntityPlayer* pTargetPlayer = NewEntityPlayer(m_pField);

		GTalentInfo* pTalentInfo = test.talent.NewTalentInfo();
		pTalentInfo->m_fCastingTime = 5.0f;
		pTalentInfo->m_fDurationTime = 10.0f;
		pTalentInfo->Cooking();

		test.talent.UseTalent(m_pPlayer, pTalentInfo, m_pPlayer->GetUID());
		GTalent* pTalent = m_pPlayer->GetModuleCombat()->GetTalent();
		TVALID(pTalent);
		CHECK_EQUAL(m_pPlayer->GetUID(), pTalent->GetTarget().uidTarget);
		pTalent->Update(5.0f);	// 캐스팅시간만큼 대기
		pTalent->ActTalent(pTargetPlayer->GetUID());	
		CHECK_EQUAL(pTargetPlayer->GetUID(), pTalent->GetTarget().uidTarget);	// 대상이 ActTalent의 요청 대상으로 바뀜
	}

	TEST_FIXTURE(FTalent, CastingUsingRealPacket)	// 실제 패킷으로 테스트
	{
		GEntityPlayer* pTargetPlayer = NewEntityPlayer(m_pField);

		GTalentInfo* pTalentInfo = test.talent.NewTalentInfo(777);
		pTalentInfo->m_fCastingTime = 5.0f;
		pTalentInfo->m_fDurationTime = 10.0f;
		pTalentInfo->Cooking();

		// UseTalent
		m_pLinkTester->OnRecv(MC_ACTION_USE_TALENT_REQ,		3, NEW_INT(777), NEW_SVEC(m_pPlayer->GetDir()), NEW_VEC(m_pPlayer->GetPos()));

		GTalent* pTalent = m_pPlayer->GetModuleCombat()->GetTalent();
		TVALID(pTalent);
		CHECK_EQUAL(TALENT_PHASE_PREPARE, pTalent->GetPhase());
		pTalent->Update(5.0f);	// 캐스팅시간만큼 대기
		CHECK_EQUAL(TALENT_PHASE_PREPARE, pTalent->GetPhase());

		// ActTalent
		TD_TALENT_TARGET_DETAIL infoTarget;
		infoTarget.uidTarget = pTargetPlayer->GetUID();
		m_pLinkTester->OnRecv(MC_ACTION_ACT_TALENT_WITH_HITCAPSULE_REQ,		2, NEW_SVEC(m_pPlayer->GetDir()), NEW_SINGLE_BLOB(&infoTarget, sizeof(TD_TALENT_TARGET_DETAIL)));

		CHECK_EQUAL(pTargetPlayer->GetUID(), pTalent->GetTarget().uidTarget);	// 대상이 ActTalent의 요청 대상으로 바뀜
		CHECK_EQUAL(TALENT_PHASE_ACT, pTalent->GetPhase());	
	}

	TEST_FIXTURE(FTalent, NPCAdvancePhaseWithoutCastingtime)	// NPC는 ACT없이 prepare페이즈를 넘김
	{
		GEntityNPC* pNPCCaster = GUTHelper_NPC::SpawnNPC(m_pField);

		GTalentInfo* pTalentInfo = test.talent.NewTalentInfo();
		pTalentInfo->m_fCastingTime = 5.0f;
		pTalentInfo->m_fDurationTime = 10.0f;
		pTalentInfo->Cooking();

		test.talent.UseTalent(pNPCCaster, pTalentInfo, pNPCCaster->GetUID());
		GTalent* pTalent = pNPCCaster->GetModuleCombat()->GetTalent();
		TVALID(pTalent);
		CHECK_EQUAL(TALENT_PHASE_PREPARE, pTalent->GetPhase());
		pTalent->Update(5.0f);	// 캐스팅시간만큼 대기
		CHECK_EQUAL(TALENT_PHASE_ACT, pTalent->GetPhase());
	}

	TEST_FIXTURE(FTalent, ActMethodCanUseOnlyPreparePhase)	// ACT 메쏘드는 prepare페이즈에서만 쓸 수 있음
	{
		GTalentInfo* pTalentInfo = test.talent.NewTalentInfo();
		pTalentInfo->m_fCastingTime = 5.0f;
		pTalentInfo->m_fDurationTime = 10.0f;
		pTalentInfo->Cooking();

		test.talent.UseTalent(m_pPlayer, pTalentInfo, m_pPlayer->GetUID());
		GTalent* pTalent = m_pPlayer->GetModuleCombat()->GetTalent();
		TVALID(pTalent);
		pTalent->Update(5.0f);	// 캐스팅시간만큼 대기
		pTalent->ActTalent(m_pPlayer->GetUID());	
		CHECK_EQUAL(TALENT_PHASE_ACT, pTalent->GetPhase());	
		pTalent->Update(10.0f);	// 만료시간까지 대기
		pTalent->UpdateExpired(1.0f);
		CHECK_EQUAL(TALENT_PHASE_FINISH, pTalent->GetPhase());	

		// 만료된 탤런트에서 ACT시켜도 작동되지 않아야함
		pTalent->ActTalent(m_pPlayer->GetUID());	
		CHECK_EQUAL(TALENT_PHASE_FINISH, pTalent->GetPhase());	
	}

	TEST_FIXTURE(FTalent, ActTime)	
	{
		GTalentInfo* pTalentInfo = test.talent.NewTalentInfo();
		pTalentInfo->m_fCastingTime = 5.0f;
		pTalentInfo->m_fDurationTime = 10.0f;
		pTalentInfo->Cooking();

		test.talent.UseTalent(m_pPlayer, pTalentInfo, m_pPlayer->GetUID());
		GTalent* pTalent = m_pPlayer->GetModuleCombat()->GetTalent();
		TVALID(pTalent);
		CHECK_EQUAL(0.0f, pTalent->GetActElapsedTime());	// 시작시 0으로 셋팅
		pTalent->Update(5.0f);	// 캐스팅시간만큼 대기
		CHECK_EQUAL(0.0f, pTalent->GetActElapsedTime());	// 액트 페이즈로 넘어가지 않음
		pTalent->ActTalent(m_pPlayer->GetUID());		
		CHECK_EQUAL(0.0f, pTalent->GetActElapsedTime());	// 액트 페이즈로 넘어가지 않음
		CHECK_EQUAL(TALENT_PHASE_ACT, pTalent->GetPhase());	
		pTalent->Update(3.0f);
		CHECK_EQUAL(3.0f, pTalent->GetActElapsedTime());
		pTalent->Update(2.0f);
		CHECK_EQUAL(5.0f, pTalent->GetActElapsedTime());
	}

	TEST_FIXTURE(FTalent, NotPayCostWhenSelfCanceled)		// 탤런트가 캐스팅중에 스스로 취소되면 탤런트 비용을 지불않함
	{
		GTalentInfo* pTalentInfo = test.talent.NewTalentInfo();
		pTalentInfo->m_fCastingTime = 5.0f;
		pTalentInfo->m_fDurationTime = 10.0f;
		pTalentInfo->m_nENCost = 10; // 에너지를 10만큼 소모
		pTalentInfo->Cooking();

		m_pPlayer->SetEN(10);
		CHECK_EQUAL(10, m_pPlayer->GetEN());

		test.talent.UseTalent(m_pPlayer, pTalentInfo, m_pPlayer->GetUID());
		GTalent* pTalent = m_pPlayer->GetModuleCombat()->GetTalent();
		TVALID(pTalent);
		CHECK_EQUAL(0.0f, pTalent->GetActElapsedTime());	// 시작시 0으로 셋팅
		pTalent->Update(1.0f);	// 캐스팅시간에 모자르게 대기
		CHECK_EQUAL(10, m_pPlayer->GetEN());

		// 스스로 취소
		m_pPlayer->doCancelTalentForce();

		// 탤런트 취소됐는지 확인
		CHECK_EQUAL(TALENT_PHASE_CANCELED, m_pPlayer->GetModuleCombat()->GetTalent()->GetPhase());
		CHECK_EQUAL(10, m_pPlayer->GetEN());	// 탤런트 발동 비용 소비 안되고 유지
	}

	TEST_FIXTURE(FTalent, PayCostWhenHitCanceled)		// 탤런트가 캐스팅중에 피격으로 취소되면 탤런트 비용을 지불함
	{
		GTalentInfo* pTalentInfo = test.talent.NewTalentInfo();
		pTalentInfo->m_fCastingTime = 5.0f;
		pTalentInfo->m_fDurationTime = 10.0f;
		pTalentInfo->m_nENCost = 10; // 에너지를 10만큼 소모
		pTalentInfo->Cooking();

		m_pPlayer->SetEN(10);
		CHECK_EQUAL(10, m_pPlayer->GetEN());

		test.talent.UseTalent(m_pPlayer, pTalentInfo, m_pPlayer->GetUID());
		GTalent* pTalent = m_pPlayer->GetModuleCombat()->GetTalent();
		TVALID(pTalent);
		CHECK_EQUAL(0.0f, pTalent->GetActElapsedTime());	// 시작시 0으로 셋팅
		pTalent->Update(1.0f);	// 캐스팅시간에 모자르게 대기
		CHECK_EQUAL(10, m_pPlayer->GetEN());

		// 공격 받음
		GEntityPlayer* pAttacker = NewEntityPlayer(m_pField);
		GTalentInfo* pAtkTalent = test.talent.MakeDamageTalent(1);
		pAtkTalent->m_MotionFactorGroup.Adjust(MF_BEATEN, 100, 1000);	// 비튼에 걸림
		test.talent.UseTalent(pAttacker, pAtkTalent, m_pPlayer->GetUID());
		m_pField->Update(.0f);

		// 탤런트 취소됐는지 확인
		if (NULL != m_pPlayer->GetModuleCombat()->GetTalent())
		{
			CHECK_EQUAL(TALENT_PHASE_CANCELED, m_pPlayer->GetModuleCombat()->GetTalent()->GetPhase());
		}
		CHECK_EQUAL(0, m_pPlayer->GetEN());	// 탤런트 발동 비용 소비 안되고 유지
	}
}