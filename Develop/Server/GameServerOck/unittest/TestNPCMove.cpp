#include "stdafx.h"
#include "GNPCInfo.h"
#include "MockField.h"
#include "GTestForward.h"
#include "GNPCMove.h"
#include "GHateTable.h"

SUITE(NPCMove)
{
	struct FNPCMove
	{
		FNPCMove()
		{
			m_pField = GUTHelper_Field::DefaultMockField();
			m_pNPC = GUTHelper_NPC::SpawnNPC(m_pField);			
		}
		~FNPCMove()
		{
		}

		GEntityNPC* m_pNPC;
		MockField*	m_pField;
		DECLWRAPPER_NPCInfoMgr;
	};

	TEST_FIXTURE(FNPCMove, IsRun)
	{
		// 기본은 달리지 않음
		CHECK_EQUAL(false, m_pNPC->GetNPCMove().IsRun());

		// SetAlways 속성이 있으면 달림
		m_pNPC->GetNPCMove().SetAlwaysRun(true);
		CHECK_EQUAL(true, m_pNPC->GetNPCMove().IsRun());
		m_pNPC->GetNPCMove().SetAlwaysRun(false);
		CHECK_EQUAL(false, m_pNPC->GetNPCMove().IsRun());

		// ReturnToMove 속성이 있으면 달림
		m_pNPC->GetNPCMove().SetReturnToMove(true);
		CHECK_EQUAL(true, m_pNPC->GetNPCMove().IsRun());
		m_pNPC->GetNPCMove().SetReturnToMove(false);
		CHECK_EQUAL(false, m_pNPC->GetNPCMove().IsRun());

		// 전투중이면 달림
		GEntityNPC* pEnemy = GUTHelper_NPC::SpawnNPC(m_pField);
		m_pNPC->doAttack(pEnemy->GetUID());
		CHECK_EQUAL(true, m_pNPC->IsNowCombat());
		CHECK_EQUAL(true, m_pNPC->GetNPCMove().IsRun());
		m_pNPC->doMakePeace();
		m_pNPC->GetNPCMove().SetReturnToMove(false); // BehaviorReturn으로 설정된값 되돌리기
		CHECK_EQUAL(true, m_pNPC->IsNowCombat());
		CHECK_EQUAL(true, m_pNPC->GetNPCMove().IsRun());
	}

	TEST_FIXTURE(FNPCMove, GetRunSpeed)
	{
		m_pNPC->GetNPCInfo()->fRunSpeed = 200.0f;
		m_pNPC->GetChrStatus()->ActorModifier.fMoveSpeed = 2.0f;
		// fRunSpeed * (1 + 2.0f)
		CHECK_EQUAL(600.0f, m_pNPC->GetNPCMove().GetRunSpeed());
	}

	TEST_FIXTURE(FNPCMove, GetFleeSpeed)
	{
		m_pNPC->GetNPCInfo()->fRunSpeed = 200.0f;
		m_pNPC->GetChrStatus()->ActorModifier.fMoveSpeed = 2.0f;
		m_pNPC->GetNPCInfo()->fFleeSpeedAmp = 3.0f;
		// fRunSpeed * (1 + 2.0f) * fFleeSpeedAmp
		CHECK_EQUAL(1800.0f, m_pNPC->GetNPCMove().GetFleeSpeed());
	}

	TEST_FIXTURE(FNPCMove, GetSpeed)
	{
		m_pNPC->GetChrInfo()->fSpeed = 100.0f;
		m_pNPC->GetNPCInfo()->fRunSpeed = 200.0f;
		m_pNPC->GetNPCInfo()->fFleeSpeedAmp = 2.0f;
		// 평상시에는 걷기 속도 반환
		CHECK_EQUAL(100.0f, m_pNPC->GetNPCMove().GetSpeed());
		// 달릴때는 달리기 속도 반환
		m_pNPC->GetNPCMove().SetReturnToMove(true);
		CHECK_EQUAL(200.0f, m_pNPC->GetNPCMove().GetSpeed());
		m_pNPC->GetNPCMove().SetReturnToMove(false);
		CHECK_EQUAL(100.0f, m_pNPC->GetNPCMove().GetSpeed());
		// 도망칠때는 도망치기 속도 반환
		m_pNPC->GetNPCMove().SetRetreat(true);
		CHECK_EQUAL(400.0f, m_pNPC->GetNPCMove().GetSpeed());
		m_pNPC->GetNPCMove().SetRetreat(false);
		CHECK_EQUAL(100.0f, m_pNPC->GetNPCMove().GetSpeed());

	}
}
