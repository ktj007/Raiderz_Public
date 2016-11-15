#include "stdafx.h"
#include "SUnitTestUtil.h"
#include "GUTHelper.h"
#include "MockField.h"
#include "GModuleAI.h"
#include "GBehaviorStateMachine.h"
#include "GAIState.h"
#include "GJobMgr.h"
#include "GTestForward.h"
#include "GJob.h"

SUITE(BehaviorPeace)
{
	struct FBehaviorPeace
	{
		FBehaviorPeace()
		{
			m_pNPCInfo = m_Helper.NewNPCInfo();
			m_pField = GUTHelper_Field::DefaultMockField();
			m_pNPC = m_pField->SpawnTestNPC(m_pNPCInfo);
		}

		virtual ~FBehaviorPeace()
		{
			m_pField->Destroy();
		}

		DECLWRAPPER_NPCInfoMgr;
		GNPCInfo*		m_pNPCInfo;
		GUTHelper		m_Helper;
		MockField*		m_pField;
		GEntityNPC*		m_pNPC;
	};

	TEST_FIXTURE(FBehaviorPeace, DefaultMoveToPeaceState)
	{
		TVALID_EQ(AI_BEHAVIOR_MISSION, m_pNPC->GetModuleAI()->GetBehaviorFSM()->GetCurrentStateID());
		m_pNPC->GetModuleAI()->Update(0.0f);
		TVALID_EQ(AI_BEHAVIOR_PEACE, m_pNPC->GetModuleAI()->GetBehaviorFSM()->GetCurrentStateID());

	}
	TEST_FIXTURE(FBehaviorPeace, RunIdleColtWhenPeaceStand)
	{
		GColtAction action;
		action.nType = CAT_NOTHING;
		action.qParams[0] = L"10";
		m_pNPCInfo->ColtInfo.checkIdleDefault.vecActions.push_back(action);

		m_pNPC->GetModuleAI()->Update(0.0f);
		TVALID_EQ(AI_BEHAVIOR_MISSION, m_pNPC->GetModuleAI()->GetBehaviorFSM()->GetCurrentStateID()); // 잡을 받아 미션상태로 변경
		TVALID_EQ(true, m_pNPC->GetModuleAI()->GetJobMgr()->IsEmpty());
		CHECK_EQUAL(CAT_NOTHING, m_pNPC->GetMissionJobs().front()->GetID());
	}
}