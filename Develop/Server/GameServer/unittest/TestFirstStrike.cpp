#include "stdafx.h"
#include "SUnitTestUtil.h"
#include "GTestWrapper.h"
#include "MockField.h"
#include "GUTHelper.h"
#include "GNPCInfo.h"
#include "GEntityNPC.h"
#include "GEntityPlayer.h"
#include "GModuleAI.h"
#include "GJob_MoveToPos.h"
#include "GJob_MoveToEntity.h"
#include "MockEntityNPC.h"
#include "GFieldSystem.h"
#include "GConditionInfo.h"
#include "GConditionsInfo.h"
#include "GConditionsInfoMgr.h"
#include "GFieldMgr.h"
#include "GJobMgr.h"
#include "GUTHelper_Field.h"
#include "GTestForward.h"
#include "GNPCFirstStrike.h"

const vec3 vCommonPos = vec3(1000.0f, 1000.0f, 0.0f);
const vec3 vCommonDir = vec3(0,1,0);

SUITE(FirstStrike)
{
	struct Fixture
	{
		Fixture()
		{ 
			m_pFieldInfo = new GFieldInfo;
			m_pFieldInfo->m_strFieldFile = L"mockfield";
			m_pFieldInfo->m_nFieldID = 100;
			m_pFieldInfo->SetFieldMaxXY_ForTest(50000.0f, 50000.0f);
		
			m_pField = GUTHelper_Field::NewMockField();
			m_pField->SetFieldInfo(m_pFieldInfo);
		
			m_pNPCInfo = new GNPCInfo;
			m_pNPCInfo->nID = 1;
			gmgr.pNPCInfoMgr->Insert(m_pNPCInfo);
		}

		~Fixture() 
		{
			m_pField->Destroy();
			SAFE_DELETE(m_pField);
			SAFE_DELETE(m_pFieldInfo);
		}

		GUTHelper		m_Helper;
		MockField*		m_pField;
		GFieldInfo*		m_pFieldInfo;
		GNPCInfo*		m_pNPCInfo;

		GTestMgrWrapper<GNPCInfoMgr>				m_NPCInfoMgr;
	
	};

	TEST_FIXTURE(Fixture, DuringRoam)
	{
		std::auto_ptr<SPAWN_INFO> apSpawninfo(new SPAWN_INFO());
		apSpawninfo->bCheckSpawnPointValidation = false;
		apSpawninfo->vPoint = vCommonPos;
		apSpawninfo->fRoamRadius = 100;
		apSpawninfo->fRoamDepth = 100;
		apSpawninfo->vecNPCInfoList.push_back(SPAWN_NPC_INFO(1));

		GEntityPlayer* pPlayer = GUTHelper::NewEntityPlayer(m_pField, vCommonPos);
		GEntityNPC* pNPC = m_pField->SpawnTestNPC(m_pNPCInfo, vCommonPos, vCommonDir, apSpawninfo.get());
		pNPC->GetChrInfo()->fSpeed = 0.0f;

		pNPC->UpdateAI(1.0f);	// Jobs: [ReturnToHome], PushJobQueue
		pNPC->UpdateAI(1.0f);	// Jobs: [ReturnToHome], JobQueue -> CurJob
		pNPC->UpdateAI(1.0f);	// Load from mission job
		pNPC->UpdateAI(1.0f);	// Jobs: [Roam], JobQueue -> CurJob
		
		GJobMgr* pJobMgr = pNPC->GetJobMgr();
		GJob* pCurJob = pJobMgr->GetCurrJob();

		if (!pCurJob)
		{
			CHECK(FALSE&&"No pushed roam job.");
			return;
		}
		
		CHECK_EQUAL(pCurJob->GetID(), GJOB_ROAM);
		CHECK(pNPC->GetTargetActor() == NULL);

		// enable to first-stirke
		m_pNPCInfo->nInitAttackable = NAT_ALWAYS;
		pNPC->GetNPCStatus().nAA = NAAT_ALWAYS;
		m_pNPCInfo->nSightRange = 100;
		pNPC->UpdateAI(1.0f); // FindEnemy ´ë±â Æ½
		CHECK_EQUAL(pNPC->GetTargetActor(), pPlayer);
	}

	TEST_FIXTURE(Fixture, FirstStrikeDelay)
	{
		GEntityPlayer* pPlayer = GUTHelper::NewEntityPlayer(m_pField, vCommonPos);
		GEntityNPC* pNPC = m_pField->SpawnTestNPC(m_pNPCInfo, vCommonPos, vCommonDir);
		pNPC->GetNPCFirstStrike().SetEnableSpawnDelay(true);

		// enable to first-stirke
		m_pNPCInfo->nInitAttackable = NAT_ALWAYS;
		pNPC->GetNPCStatus().nAA = NAAT_ALWAYS;
		m_pNPCInfo->nSightRange = 50000;
		CHECK_EQUAL(MUID::Invalid(), pNPC->GetTarget());
		m_pField->Update(GConst::NPC_FIRSTSTRIKE_DELAY);
		pNPC->UpdateAI(1.0f); // FindEnemy ´ë±â Æ½
		CHECK_EQUAL(pPlayer->GetUID(), pNPC->GetTarget());
	}
}
