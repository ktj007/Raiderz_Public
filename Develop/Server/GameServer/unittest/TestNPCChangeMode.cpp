#include "stdafx.h"
#include "GTalentInfo.h"
#include "GNPCInfo.h"
#include "MockField.h"
#include "GTestForward.h"

SUITE(NPCChangeMode)
{
	struct FChangeMode
	{
		FChangeMode()
		{
			m_pField = GUTHelper_Field::DefaultMockField();

			m_NPCInfo.nID = 1;

			GNPCInfo* pMode1Info = new GNPCInfo();
			pMode1Info->nID = 1;
			pMode1Info->nMode = NPC_MODE_1;
			pMode1Info->ModeCopyFromParent(&m_NPCInfo);

			m_NPCInfo.vecModes.clear();
			m_NPCInfo.vecModes.push_back(pMode1Info);	// mode1도 있음

		}
		~FChangeMode()
		{
			m_pField->Destroy();
		}

		GNPCInfo	m_NPCInfo;
		MockField*		m_pField;
		DECLWRAPPER_TalentMgr;
	};
	TEST(TestTalentInfo_DefaultChangeMode)
	{
		GTalentInfo talentInfo;

		CHECK_EQUAL(false, talentInfo.m_ChangeModeInfo.m_bChangeMode);
	}

	TEST_FIXTURE(FChangeMode, TestTalentInfo_SimpleChangeMode)
	{
		GEntityNPC* pNPC = m_pField->SpawnTestNPC(&m_NPCInfo);

		CHECK_EQUAL(NPC_MODE_DEFAULT, pNPC->GetMode());

		pNPC->ChangeMode(NPC_MODE_1);

		CHECK_EQUAL(NPC_MODE_1, pNPC->GetMode());


		// 2 모드는 없다
		pNPC->ChangeMode(NPC_MODE_2);
		CHECK_EQUAL(NPC_MODE_1, pNPC->GetMode());
	}


	TEST_FIXTURE(FChangeMode, TestTalentInfo_ChangeModeByTalent)
	{
		GTalentInfo* pChangeModeTalent = test.talent.NewMeleeTalentInfo(1);		
		pChangeModeTalent->m_ChangeModeInfo.m_bChangeMode = true;
		pChangeModeTalent->m_ChangeModeInfo.m_nModeTo = NPC_MODE_1;
		pChangeModeTalent->m_fDurationTime = 1.0f;		// 1초 후 탤런트 끝

		GEntityNPC* pNPC = m_pField->SpawnTestNPC(&m_NPCInfo);

		CHECK_EQUAL(NPC_MODE_DEFAULT, pNPC->GetMode());

		pNPC->doUseTalent(pChangeModeTalent);
		m_pField->Update(0.1f);

		// 탤런트가 끝났을 때 모드 변환함
		m_pField->Update(1.0f);
		CHECK_EQUAL(NPC_MODE_1, pNPC->GetMode());
	}

	TEST_FIXTURE(FChangeMode, TestSpawn_ModeChangedNPC)
	{
		m_NPCInfo.nSpawningMode = NPC_MODE_1;

		GEntityNPC* pNPC = m_pField->SpawnTestNPC(&m_NPCInfo);

		CHECK_EQUAL(NPC_MODE_1, pNPC->GetMode());
	}

	TEST_FIXTURE(FChangeMode, TestNPCMode_SpawningMode)
	{
		m_NPCInfo.nSpawningMode = NPC_MODE_1;

		GEntityNPC* pNPC = m_pField->SpawnTestNPC(&m_NPCInfo);

		pNPC->ChangeMode(NPC_MODE_1);
		CHECK_EQUAL(NPC_MODE_1, pNPC->GetMode());
	}

	TEST_FIXTURE(FChangeMode, ApplyInitAttackableAtChangeMode)
	{
		m_NPCInfo.vecModes[0]->nInitAttackable = NAT_NONE;

		GEntityNPC* pNPC = m_pField->SpawnTestNPC(&m_NPCInfo);

		CHECK_EQUAL(false, pNPC->IsNoneAttackable());
		pNPC->ChangeMode(NPC_MODE_1);
		CHECK_EQUAL(true, pNPC->IsNoneAttackable());
	}

	TEST_FIXTURE(FChangeMode, ApplyAAAtChangeMode)
	{
		m_NPCInfo.vecModes[0]->nAA = NAAT_FACTION;

		GEntityNPC* pNPC = m_pField->SpawnTestNPC(&m_NPCInfo);

		CHECK_EQUAL(NAAT_NONE, pNPC->GetAAType());
		pNPC->ChangeMode(NPC_MODE_1);
		CHECK_EQUAL(NAAT_FACTION, pNPC->GetAAType());
	}


	TEST_FIXTURE(FChangeMode, TestNPCMode_SightRange)
	{
		m_NPCInfo.nSightRange = 1000;
		m_NPCInfo.vecModes[0]->nSightRange = 500;

		GEntityNPC* pNPC = m_pField->SpawnTestNPC(&m_NPCInfo);
		int nEnemyLevel = pNPC->GetLevel();

		CHECK_EQUAL(1000, pNPC->GetSightRange(nEnemyLevel));

		pNPC->ChangeMode(NPC_MODE_1);
		CHECK_EQUAL(500, pNPC->GetSightRange(nEnemyLevel));
	}

	TEST_FIXTURE(FChangeMode, TestNPCMode_SightRange_IfSpawningMode)
	{
		m_NPCInfo.nSpawningMode = NPC_MODE_1;
		m_NPCInfo.nSightRange = 1000;
		m_NPCInfo.vecModes[0]->nSightRange = 500;

		GEntityNPC* pNPC = m_pField->SpawnTestNPC(&m_NPCInfo);
		int nEnemyLevel = pNPC->GetLevel();

		CHECK_EQUAL(500, pNPC->GetSightRange(nEnemyLevel));
	}

}
