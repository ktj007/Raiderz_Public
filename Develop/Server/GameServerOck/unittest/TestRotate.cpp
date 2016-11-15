#include "StdAfx.h"
#include "GTestForward.h"
#include "GEntityPlayer.h"
#include "GModuleCombat.h"
#include "GTask_Move.h"
#include "GServer.h"
#include "GFieldMgr.h"
#include "GJob_Broker.h"
#include "GJob.h"
#include "GModuleAI.h"
#include "GJobMgr.h"
#include "FBasePlayer.h"
#include "GModuleMovable.h"

SUITE(Rotate)
{
	struct FRotate : public FBasePlayer
	{
		FRotate()
		{ 
			m_pField = GUTHelper_Field::DefaultMockField();
			m_pNPC = GUTHelper_NPC::SpawnNPC(m_pField);

			gsys.pServer->FrameCounterClear();			
		}

		~FRotate() 
		{
			m_pField->Destroy();
		}

		MockField*		m_pField;
		GEntityNPC*		m_pNPC;

		DECLWRAPPER_Field;
		DECLWRAPPER_TalentMgr;
		DECLWRAPPER_SimpleCombatCalc;
		DECLWRAPPER_FakeSystem;
	};

	TEST_FIXTURE(FRotate, RouteMoveStopWhenMoveJobExpired)
	{
		GEntityNPC* pTargetNPC = GUTHelper_NPC::SpawnNPC(m_pField);
		pTargetNPC->SetPos(vec3(10000,10000,0));	// 아주 멀리 있음
		GJob* pNewJob = GJobBroker::NewJob_MoveToEntity(m_pNPC->GetModuleAI(), pTargetNPC->GetUID(), 0.0f, false);
		pNewJob->Attr.bClearJobListWhenTimedout = true;
		pNewJob->SetDurationTime(10.0f);

		GJobMgr* pJobMgr = m_pNPC->GetJobMgr();
		pJobMgr->Push(pNewJob);

		GEntityPlayer* pSeer = NewEntityPlayer(m_pField);
		MockLink* m_pLinkTester = test.network.NewLink(pSeer);
		m_pLinkTester->AddIgnoreID(MC_QUESTPVP_TEAMCOUNT);

		for ( int i = 0;  i < 15;  i++)
			m_pField->Update(1.0f);	// 이동 시간 만료
		

		// 패킷 체크
		if ( m_pLinkTester->GetCommandCount() > 0)
		{
			const minet::MCommand& Command = m_pLinkTester->GetCommand( m_pLinkTester->GetCommandCount() - 1);
			CHECK_EQUAL(Command.GetID(), MC_NPC_MOVE_STOP);
			if (Command.GetParameterCount() < 2)						 { CHECK(false); return; }
			UIID nUIID;
			vec3 vPos;
			if (!Command.GetParameter(&nUIID,	0,		MPT_USHORT))		{ CHECK(false); return; }
			if (!Command.GetParameter(&vPos,		1, MPT_VEC))		{ CHECK(false); return; }

			CHECK_EQUAL(nUIID, m_pNPC->GetUIID());
			CHECK_EQUAL(vPos, m_pNPC->GetPos());
		}

		test.network.ClearLinks();
	}

}