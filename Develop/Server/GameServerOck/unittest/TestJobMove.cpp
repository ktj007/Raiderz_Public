#include "StdAfx.h"
#include "GTestForward.h"
#include "GEntityNPC.h"
#include "GServer.h"
#include "GFieldMgr.h"

SUITE(JobMove)
{
	struct FJobMove
	{
		FJobMove()
		{ 
			m_pField = GUTHelper_Field::DefaultMockField();
			m_pNPC = test.npc.SpawnNPC(m_pField);
		}

		~FJobMove() 
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

	TEST_FIXTURE(FJobMove, ReturnToHomePoint)
	{
		m_pNPC->SetPos(vec3(0,0,0));
		m_pNPC->SetDir(vec3(0,1,0));
		m_pNPC->SetHomePoint(vec3(1000,1000,0), vec3(0.3f,0.0f,0.7f));

		CHECK_EQUAL(vec3(0,0,0), m_pNPC->GetPos());
		CHECK_EQUAL(vec3(0,1,0), m_pNPC->GetDir());

		// 홈포인트로 이동
		m_pNPC->ReturnToHomePoint();
		m_pNPC->UpdateAI(0.0f);

		for (int i=0; i<100; ++i)
			m_pNPC->UpdateMovable(100.0f);
		
		m_pNPC->UpdateAI(0.0f);
		CHECK_EQUAL(vec3(1000,1000,0), m_pNPC->GetPos());
		CHECK_EQUAL(vec3(0.3f,0.0f,0.7f), m_pNPC->GetDir());
	}
}