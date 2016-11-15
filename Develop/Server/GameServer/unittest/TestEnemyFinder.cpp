#include "stdafx.h"
#include "MockField.h"
#include "GEntityPlayer.h"
#include "GEntityNPC.h"
#include "GEnemyFinder.h"
#include "GHateTable.h"
#include "GConfig.h"
#include "GTestForward.h"
#include "MMemPool.h"
#include "GActorInvisibility.h"
#include "GTestForward.h"
#include "GUTHelper_Field.h"

SUITE(EnemyFinder)
{
	struct Fixture
	{
		Fixture()
		{
			m_pField = GUTHelper_Field::DefaultMockField();
			m_pFinderNPC = test.npc.SpawnNPC(m_pField);
			m_pFinderNPC->GetNPCInfo()->nAA = NAAT_ALWAYS;
			m_pPlayer = test.player.NewPlayer(m_pField);
			m_pNPC = test.npc.SpawnNPC(m_pField);
		}
		virtual ~Fixture()
		{
			m_pField->Destroy();
		}

		GUTHelper		m_Helper;
		MockField*		m_pField;
		GEntityNPC*		m_pFinderNPC;
		GEntityPlayer*	m_pPlayer;
		GEntityNPC*		m_pNPC;
		GEnemyFinder	m_EnemyFinder;
	};


	// FinderNPC가 AA 플래그가 켜져있지 않고, 팩션도 가지고 있지 않을때
	TEST_FIXTURE(Fixture, AAIsFalseAndNotHaveFaction)
	{
		m_pFinderNPC->GetNPCInfo()->nAA = NAAT_NONE;

		for (int i=0; i<10; i++)
		{
			MUID nEnemyUID = m_EnemyFinder.Find(m_pFinderNPC);

			CHECK_EQUAL(true, nEnemyUID.IsInvalid());
		}
	}

	// 자동 AITest중인 경우
	TEST_FIXTURE(Fixture, AutoTestAI)
	{
		wstring strOldAutoTestType = GConfig::m_strAutoTestType;
		GConfig::m_strAutoTestType = CONFIG_AUTO_TEST_AI;

		for (int i=0; i<10; i++)
		{			
			MUID nEnemyUID = m_EnemyFinder.Find(m_pFinderNPC);

			if (true == nEnemyUID.IsValid())
			{
				CHECK_EQUAL(m_pNPC->GetUID(), nEnemyUID);
			}
		}

		GConfig::m_strAutoTestType = strOldAutoTestType;
	}

	// FinderNPC가 팩션을 가지고 있지 않은 경우
	TEST_FIXTURE(Fixture, NotHaveFaction)
	{
		for (int i=0; i<10; i++)
		{
			MUID nEnemyUID = m_EnemyFinder.Find(m_pFinderNPC);

			if (true == nEnemyUID.IsValid())
			{
				CHECK_EQUAL(m_pPlayer->GetUID(), nEnemyUID);
			}
		}
	}
	
	// 시야에 보이는 경우
	TEST_FIXTURE(Fixture, CheckSightSuccess)
	{
		m_pFinderNPC->GetNPCInfo()->nSightRange = 100;

		m_pFinderNPC->SetPos(vec3(0, 0, 0));
		m_pNPC->SetPos(vec3(0, 99, 0));
		m_pPlayer->SetPos(vec3(0, 2, 0));

		for (int i=0; i<10; i++)
		{
			MUID nEnemyUID = m_EnemyFinder.Find(m_pFinderNPC);

			if (true == nEnemyUID.IsValid())
			{
				CHECK(	nEnemyUID == m_pNPC->GetUID() || 
						nEnemyUID == m_pPlayer->GetUID());
			}
		}
	}

	// 거리때문에 시야에 안 보이는 경우
	TEST_FIXTURE(Fixture, CheckSightFailByDistance)
	{
		m_pFinderNPC->GetNPCInfo()->nSightRange = 100;

		m_pFinderNPC->SetPos(vec3(0, 0, 0));
		m_pNPC->SetPos(vec3(0, 101, 0));
		m_pPlayer->SetPos(vec3(0, 102, 0));

		for (int i=0; i<10; i++)
		{
			MUID nEnemyUID = m_EnemyFinder.Find(m_pFinderNPC);

			CHECK_EQUAL(true, nEnemyUID.IsInvalid());
		}
	}

	// NPC에게 투명화 버프때문에 적이 안 보이는 경우
	TEST_FIXTURE(Fixture, CheckSightFailByInvisibleToNPCBuff)
	{
		GTestHelpers test;

		m_pFinderNPC->GetNPCInfo()->nSightRange = 100;
		GBuffInfo* pBuff = test.buff.NewBuffInfo();
		pBuff->m_nPassiveExtraAttrib = BUFPEA_INVISIBILITYTONPC;

		test.buff.GainBuffSelf(m_pPlayer, pBuff);
		test.buff.GainBuffSelf(m_pNPC, pBuff);

		m_pFinderNPC->SetPos(vec3(0, 0, 0));
		m_pNPC->SetPos(vec3(0, 99, 0));
		m_pPlayer->SetPos(vec3(0, 2, 0));

		for (int i=0; i<10; i++)
		{
			MUID nEnemyUID = m_EnemyFinder.Find(m_pFinderNPC);

			CHECK_EQUAL(true, nEnemyUID.IsInvalid());
		}
	}

	// 투명화버프때문에 적이 안 보이는 경우
	TEST_FIXTURE(Fixture, CheckSightFailByInvisibleBuff)
	{
		m_pFinderNPC->GetNPCInfo()->nSightRange = 100;
		GBuffInfo* pBuff = test.buff.NewBuffInfo();
		pBuff->m_nPassiveExtraAttrib = BUFPEA_INVISIBILITY;

		MakeLevelFactor(0, m_pFinderNPC, m_pPlayer);
		test.buff.GainBuffSelf(m_pPlayer, pBuff);

		m_pFinderNPC->SetPos(vec3(0, 0, 0));
		m_pPlayer->SetPos(vec3(0, 0, 0));		// 탐지거리 내에 있음
		
		m_pPlayer->GetActorInvisibility().SetDice(new FailOnlyDice());
		m_pFinderNPC->doMakePeace();	// 헤이트 테이블이 비어있지 않다면 EnemyFinder가 작동하지 않음
		CHECK_EQUAL(false, m_EnemyFinder.Find(m_pFinderNPC).IsValid());
		m_pPlayer->GetActorInvisibility().SetDice(new SuccessOnlyDice());
		m_pFinderNPC->doMakePeace();	// 헤이트 테이블이 비어있지 않다면 EnemyFinder가 작동하지 않음
		m_EnemyFinder.Update(GConst::INVISIBILITY_DETECT_TICK);
		CHECK_EQUAL(true, m_EnemyFinder.Find(m_pFinderNPC).IsValid());

		m_pPlayer->SetPos(vec3(0, 2000, 0));	// 탐지거리 밖에 있음
		m_pPlayer->GetActorInvisibility().SetDice(new SuccessOnlyDice());
		m_pFinderNPC->doMakePeace();	// 헤이트 테이블이 비어있지 않다면 EnemyFinder가 작동하지 않음
		CHECK_EQUAL(false, m_EnemyFinder.Find(m_pFinderNPC).IsValid());
	}

	TEST_FIXTURE(Fixture, CheckHeight)
	{
		GFieldInfo* pFieldInfo = GUTHelper_Field::NewFieldInfo();
		pFieldInfo->m_fSectorHeight = 50.0f;

		GField* pField = GUTHelper_Field::NewMockField(pFieldInfo);

		m_pFinderNPC->SetPos(vec3(0.0f, 0.0f, 100.0f));

		m_pPlayer->SetPos(vec3(0.0f, 0.0f, 0.0f));
		CHECK(!m_EnemyFinder.CheckHeight(m_pFinderNPC, m_pPlayer, pField));

		m_pPlayer->SetPos(vec3(0.0f, 0.0f, 50.f));
		CHECK(m_EnemyFinder.CheckHeight(m_pFinderNPC, m_pPlayer, pField));

		m_pPlayer->SetPos(vec3(0.0f, 0.0f, 100.f));
		CHECK(m_EnemyFinder.CheckHeight(m_pFinderNPC, m_pPlayer, pField));

		m_pPlayer->SetPos(vec3(0.0f, 0.0f, 150.f));
		CHECK(m_EnemyFinder.CheckHeight(m_pFinderNPC, m_pPlayer, pField));

		m_pPlayer->SetPos(vec3(0.0f, 0.0f, 200.f));
		CHECK(!m_EnemyFinder.CheckHeight(m_pFinderNPC, m_pPlayer, pField));

		SAFE_DELETE(pField);
	}
}
