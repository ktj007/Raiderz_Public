#include "stdafx.h"
#include "GTestWrapper.h"
#include "GTestForward.h"
#include "MockField.h"
#include "GHateTable.h"
#include "GActorHateTracker.h"

SUITE(ActorHateTracker)
{

	struct Fixture
	{
		Fixture()
		{ 
			m_pField = new MockField(SUnitTestUtil::NewUID());
			m_pField->Create();

			pNPC_A = test.npc.SpawnNPC(m_pField);
			pNPC_B = test.npc.SpawnNPC(m_pField);
			pNPC_C = test.npc.SpawnNPC(m_pField);
			pHT_A = &pNPC_A->GetHateTable();
			pHT_B = &pNPC_B->GetHateTable();
			pHT_C = &pNPC_C->GetHateTable();
		}

		~Fixture() 
		{
			m_pField->Destroy();
			SAFE_DELETE(m_pField);
		}

		DECLWRAPPER_NPCInfoMgr;
		MockField*			m_pField;
		GEntityNPC*			pNPC_A;
		GEntityNPC*			pNPC_B;
		GEntityNPC*			pNPC_C;
		GHateTable*			pHT_A;
		GHateTable*			pHT_B;
		GHateTable*			pHT_C;
	};

	TEST_FIXTURE(Fixture, DropPoint)
	{	
		pHT_A->AddPoint_FoundEnemy(pNPC_B);
		pHT_B->AddPoint_FoundEnemy(pNPC_A);
		pHT_C->AddPoint_FoundEnemy(pNPC_A);
		CHECK_EQUAL(true, pHT_A->IsExist(pNPC_B->GetUID()));
		CHECK_EQUAL(true, pHT_B->IsExist(pNPC_A->GetUID()));
		CHECK_EQUAL(true, pHT_C->IsExist(pNPC_A->GetUID()));
		// 헤이트테이블 트래커 초기화
		pNPC_A->GetActorHateTracker().Clear();
		// 상대편 헤이트테이블에서 지워짐
		CHECK_EQUAL(true, pHT_A->IsExist(pNPC_B->GetUID()));
		CHECK_EQUAL(false, pHT_B->IsExist(pNPC_A->GetUID()));
		CHECK_EQUAL(false, pHT_C->IsExist(pNPC_A->GetUID()));
	}
}