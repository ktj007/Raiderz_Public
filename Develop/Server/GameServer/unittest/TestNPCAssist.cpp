#include "stdafx.h"
#include "GNPCInfo.h"
#include "MockField.h"
#include "GTestForward.h"
#include "GHateTable.h"
#include "CSFactionCalculator.h"
#include "GFactionInfo.h"
#include "GModuleAI.h"
#include "GNPCAssist.h"

SUITE(Assist)
{
	struct Fixture
	{
		Fixture()
		{
			m_pField = GUTHelper_Field::DefaultMockField();
			m_pNPC = GUTHelper_NPC::SpawnNPC(m_pField);	

			// 자동 지원 설정
			m_pNPC->GetNPCInfo()->m_bAutoAssist = true;
			m_pNPC->GetNPCInfo()->m_fAutoAssistRange = 1000.0f;

			m_pFactionInfo = test.faction.NewFactionInfo(CSFactionCalculator::GetNormalMinQuantity());
		}
		~Fixture()
		{
		}

		void SetFactionLossID(GEntityNPC* pNPC, int nID)
		{
			pNPC->GetNPCInfo()->m_nFactionLossID = m_pFactionInfo->m_nID;
		}

		GEntityNPC* SpawnNPC(int nFactionLossID = 0)
		{
			GEntityNPC* pNewNPC = GUTHelper_NPC::SpawnNPC(m_pField);

			if (nFactionLossID != 0)
			{
				SetFactionLossID(pNewNPC, nFactionLossID);
			}

			return pNewNPC;
		}
		void Update(float fDelta)
		{
			m_pField->Update(fDelta);
		}

		GEntityNPC*		m_pNPC;
		MockField*		m_pField;
		GFactionInfo*	m_pFactionInfo;
		DECLWRAPPER_NPCInfoMgr;
	};

	TEST_FIXTURE(Fixture, TestNPCAssistHelper_IsAssistable_Object)
	{
		m_pNPC->GetNPCInfo()->nNpcType = NPC_TYPE_OBJECT;
		CHECK_EQUAL(false, GNPCAssistHelper::IsAssistable(m_pNPC));

		m_pNPC->GetNPCInfo()->nNpcType = NPC_TYPE_MONSTER;
		CHECK_EQUAL(true, GNPCAssistHelper::IsAssistable(m_pNPC));
	}

	TEST_FIXTURE(Fixture, TestNPCAssistHelper_IsAssistable_AutoAssist)
	{
		m_pNPC->GetNPCInfo()->m_bAutoAssist = false;
		CHECK_EQUAL(false, GNPCAssistHelper::IsAssistable(m_pNPC));

		m_pNPC->GetNPCInfo()->m_bAutoAssist = true;
		CHECK_EQUAL(true, GNPCAssistHelper::IsAssistable(m_pNPC));
	}

	TEST_FIXTURE(Fixture, TargetIsInvalid)
	{
		SetFactionLossID(m_pNPC, m_pFactionInfo->m_nID);
		GEntityNPC* pFriend = SpawnNPC(m_pFactionInfo->m_nID);
		GEntityNPC* pEnemy = SpawnNPC();

		CHECK_EQUAL(MUID::Invalid(), m_pNPC->GetTarget());
		CHECK_EQUAL(MUID::Invalid(), pFriend->GetTarget());
	}


	TEST_FIXTURE(Fixture, AutoAssist)
	{
		SetFactionLossID(m_pNPC, m_pFactionInfo->m_nID);
		GEntityNPC* pFriend = SpawnNPC(m_pFactionInfo->m_nID);
		GEntityNPC* pEnemy = SpawnNPC();

		CHECK_EQUAL(MUID::Invalid(), m_pNPC->GetTarget());
		CHECK_EQUAL(MUID::Invalid(), pFriend->GetTarget());

		pFriend->doAttack(pEnemy->GetUID());

		CHECK_EQUAL(MUID::Invalid(), m_pNPC->GetTarget());
		CHECK_EQUAL(pEnemy->GetUID(), pFriend->GetTarget());

		Update(GConst::NPC_ASSIST_INTERVAL);

		CHECK_EQUAL(pEnemy->GetUID(), m_pNPC->GetTarget());
		CHECK_EQUAL(pEnemy->GetUID(), pFriend->GetTarget());
	}

	TEST_FIXTURE(Fixture, AutoAssist_Range)
	{
		GEntityNPC* pFriend = SpawnNPC();
		GEntityNPC* pEnemy = SpawnNPC();
		pFriend->SetPos(vec3(1001.0f, 0.0f, 0.0f)); // 지원범위 밖에 있음
		CHECK_EQUAL(MUID::Invalid(), m_pNPC->GetTarget());
		CHECK_EQUAL(MUID::Invalid(), pFriend->GetTarget());
		pFriend->doAttack(pEnemy->GetUID());
		CHECK_EQUAL(MUID::Invalid(), m_pNPC->GetTarget());
		CHECK_EQUAL(pEnemy->GetUID(), pFriend->GetTarget());

		Update(GConst::NPC_ASSIST_INTERVAL);

		CHECK_EQUAL(MUID::Invalid(), m_pNPC->GetTarget());
		CHECK_EQUAL(pEnemy->GetUID(), pFriend->GetTarget());
	}

	TEST_FIXTURE(Fixture, AutoAssist_YellRespond)
	{
		SetFactionLossID(m_pNPC, m_pFactionInfo->m_nID);
		GEntityNPC* pFriend = SpawnNPC(m_pFactionInfo->m_nID);
		GEntityNPC* pEnemy = SpawnNPC();
		pFriend->SetPos(vec3(1001.0f, 0.0f, 0.0f)); // 지원범위 밖에 있음

		CHECK_EQUAL(MUID::Invalid(), m_pNPC->GetTarget());
		CHECK_EQUAL(MUID::Invalid(), pFriend->GetTarget());

		pFriend->doAttack(pEnemy->GetUID());

		CHECK_EQUAL(MUID::Invalid(), m_pNPC->GetTarget());
		CHECK_EQUAL(pEnemy->GetUID(), pFriend->GetTarget());

		pFriend->doYell(1001);

		CHECK_EQUAL(pEnemy->GetUID(), m_pNPC->GetTarget());
		CHECK_EQUAL(pEnemy->GetUID(), pFriend->GetTarget());
	}

	TEST_FIXTURE(Fixture, Yell_DoNotInReturn)
	{
		GEntityNPC* pFriend = SpawnNPC();
		GEntityNPC* pEnemy = SpawnNPC();
		m_pNPC->SetHomePoint(vec3(10000,10000,0), vec3(0,1,0)); // 귀환지점이 멀리 있음
		CHECK_EQUAL(false, m_pNPC->IsNowReturn());
		m_pNPC->doAttack(pEnemy->GetUID());
		m_pNPC->GetModuleAI()->Message(GMessage(GMSG_AI_EVT_ENDCOMBAT));
		CHECK_EQUAL(true, m_pNPC->IsNowReturn());
		pFriend->doAttack(pEnemy->GetUID());
		CHECK_EQUAL(MUID::Invalid(), m_pNPC->GetTarget());
		CHECK_EQUAL(pEnemy->GetUID(), pFriend->GetTarget());
		pFriend->doYell(1001);
		// 귀환중이라 무시
		CHECK_EQUAL(MUID::Invalid(), m_pNPC->GetTarget());
		CHECK_EQUAL(pEnemy->GetUID(), pFriend->GetTarget());
	}

	TEST_FIXTURE(Fixture, AutoAssist_DoNotInReturn)
	{
		GEntityNPC* pFriend = SpawnNPC();
		GEntityNPC* pEnemy = SpawnNPC();
		m_pNPC->SetHomePoint(vec3(10000,10000,0), vec3(0,1,0)); // 귀환지점이 멀리 있음
		CHECK_EQUAL(false, m_pNPC->IsNowReturn());
		m_pNPC->doAttack(pEnemy->GetUID());
		m_pNPC->GetModuleAI()->Message(GMessage(GMSG_AI_EVT_ENDCOMBAT));
		CHECK_EQUAL(true, m_pNPC->IsNowReturn());
		pFriend->doAttack(pEnemy->GetUID());
		CHECK_EQUAL(MUID::Invalid(), m_pNPC->GetTarget());
		CHECK_EQUAL(pEnemy->GetUID(), pFriend->GetTarget());

		Update(GConst::NPC_ASSIST_INTERVAL);

		
		// 귀환중이라 무시
		CHECK_EQUAL(MUID::Invalid(), m_pNPC->GetTarget());
		CHECK_EQUAL(pEnemy->GetUID(), pFriend->GetTarget());
	}

	TEST_FIXTURE(Fixture, Yell_InvalidObject)
	{
		GEntityNPC* pFriend = SpawnNPC();
		GEntityNPC* pEnemy = SpawnNPC();
		GEntityNPC* pObject = SpawnNPC();
		pObject->GetNPCInfo()->nNpcType = NPC_TYPE_OBJECT;

		CHECK_EQUAL(true, pObject->IsObject());
		pFriend->doAttack(pEnemy->GetUID());
		CHECK_EQUAL(MUID::Invalid(), pObject->GetTarget());
		CHECK_EQUAL(pEnemy->GetUID(), pFriend->GetTarget());
		pFriend->doYell(1001);
		// 귀환중이라 무시
		CHECK_EQUAL(MUID::Invalid(), pObject->GetTarget());
		CHECK_EQUAL(pEnemy->GetUID(), pFriend->GetTarget());
	}

	TEST_FIXTURE(Fixture, AutoAssist_InvalidObject)
	{
		GEntityNPC* pFriend = SpawnNPC();
		GEntityNPC* pEnemy = SpawnNPC();
		GEntityNPC* pObject = SpawnNPC();
		pObject->GetNPCInfo()->nNpcType = NPC_TYPE_OBJECT;

		CHECK_EQUAL(true, pObject->IsObject());
		pFriend->doAttack(pEnemy->GetUID());
		CHECK_EQUAL(MUID::Invalid(), pObject->GetTarget());
		CHECK_EQUAL(pEnemy->GetUID(), pFriend->GetTarget());

		Update(GConst::NPC_ASSIST_INTERVAL);

		// 귀환중이라 무시
		CHECK_EQUAL(MUID::Invalid(), pObject->GetTarget());
		CHECK_EQUAL(pEnemy->GetUID(), pFriend->GetTarget());
	}
}
