#include "stdafx.h"
#include "GUTHelper.h"
#include "GNPCInfo.h"
#include "GFieldInfoMgr.h"
#include "MockField.h"
#include "GUTHelper.h"
#include "GEntityPlayer.h"
#include "GTestWrapper.h"
#include "GFieldSystem.h"
#include "SUnitTestUtil.h"
#include "GEntityNPC.h"
#include "GMath.h"
#include "GFieldMgr.h"
#include "GUTHelper_Field.h"
#include "GNPCLoot.h"
#include "FBaseScriptCallback.h"
#include "GModuleAI.h"
#include "GJobRunner.h"
#include "GColtRunner.h"
#include "GHateTable.h"
#include "GNPCRewarder.h"

SUITE(NPC)
{
	struct Fixture
	{
		Fixture()
		{
			gsys.pScriptManager->Init();

			m_pNPCInfo = new GNPCInfo;
			m_pNPCInfo->nID = 1;
			gmgr.pNPCInfoMgr->Insert(m_pNPCInfo);

			m_pFieldInfo = new GFieldInfo;
			m_pFieldInfo->m_nFieldID = 100;
			m_pFieldInfo->SetFieldMaxXY_ForTest(50000.0f, 50000.0f);

			m_pField = GUTHelper_Field::NewMockField(m_pFieldInfo);
		}
		~Fixture()
		{
			m_pField->Update(1.0f);
			m_pField->Destroy();
			SAFE_DELETE(m_pField);
			SAFE_DELETE(m_pFieldInfo);

			gsys.pScriptManager->Fini();
		}

		GUTHelper		m_Helper;
		MockField*		m_pField;
		GNPCInfo*		m_pNPCInfo;
		GFieldInfo*		m_pFieldInfo;

		GTestMgrWrapper<GFieldInfoMgr>				m_FieldInfoMgr;
		GTestMgrWrapper<GFieldMgr>					m_FieldMgr;
		GTestMgrWrapper<GNPCInfoMgr>				m_NPCInfoMgr;
	};

	TEST_FIXTURE(Fixture, SetDirForwardToActor)
	{
		GEntityNPC* pNPC1 = m_pField->SpawnTestNPC(m_pNPCInfo);
		GEntityNPC* pNPC2 = m_pField->SpawnTestNPC(m_pNPCInfo);
		pNPC1->SetPos(vec3(0,0,0));
		pNPC2->SetPos(vec3(1000,1000,1000));

		pNPC1->SetDir(vec3(0,1,0));
		pNPC1->SetDirForwardToActor(pNPC2, false);

		vec3 vDir = pNPC2->GetPos() - pNPC1->GetPos();
		GMath::NormalizeZ0(vDir);

		CHECK(vDir.IsEqual(pNPC1->GetDir()) == true);
		CHECK(vDir.IsEqual(pNPC1->GetFacingDir()) == true);

		pNPC1->Destroy();
		pNPC2->Destroy();
		SAFE_DELETE(pNPC1);
		SAFE_DELETE(pNPC2);
	}

	TEST_FIXTURE(Fixture, GainEXPFlag)
	{
		m_pNPCInfo->bGainEXP = false;
		GEntityNPC* pNPC1 = m_pField->SpawnTestNPC(m_pNPCInfo);
		GEntityPlayer* pPlayer = GUTHelper::NewEntityPlayer(m_pField);
		pNPC1->GetHateTable().AddPoint_FoundEnemy(pPlayer);
		CHECK_EQUAL(0, pPlayer->GetPlayerInfo()->nXP);
		pNPC1->GetNPCRewarder().RewardByDie(pNPC1);
		CHECK_EQUAL(0, pPlayer->GetPlayerInfo()->nXP);
		m_pNPCInfo->bGainEXP = true;
		pNPC1->GetNPCRewarder().RewardByDie(pNPC1);
		CHECK(0 < pPlayer->GetPlayerInfo()->nXP);
	}


	TEST_FIXTURE(FBaseScriptCallback, BreakHeapMemory_ClearJobInCallback)
	{
		const vec3 vCommonPos = vec3(1000.0f, 1000.0f, 0.0f);
		const vec3 vCommonDir = vec3(0,1,0);
		GEntityNPC* pNPC = m_pField->SpawnTestNPC(m_pNPCInfo, vCommonPos, vCommonDir);
		GNPCInfo* pMode1Info = new GNPCInfo();
		pMode1Info->nID = m_pNPCInfo->nID;
		pMode1Info->nMode = NPC_MODE_1;
		pMode1Info->ModeCopyFromParent(m_pNPCInfo);
		m_pNPCInfo->vecModes.push_back(pMode1Info);	// mode1도 있음
		gmgr.pNPCInfoMgr->Insert(m_pNPCInfo);

		//공격 탤런트
		const int ATTACK_TALENT_DAMAGE = 100;
		GTalentInfo* pAktTalent = test.talent.NewMagicTalentInfo();
		pAktTalent->m_nTiming = TC_HIT_ENEMY;
		test.talent.SetTalentDamage(pAktTalent, 100);
		
		const char* pszScript = 
			"function NPC_1:OnHitCapsule_1_0(this, CombatInfo)\n"
			"	TEST_VAR = 777\n"
			"	this:ClearJob()"
			"	return CombatInfo;\n"
			"end";

		if (!RunLuaString(pszScript))	{ CHECK(FALSE && "WRunString Failed.");	 return; }

		pNPC->doAttack(pNPC->GetUID(), 1);

		GHitInfo infoHit;
		infoHit.nCapsuleGroup = 1;
		infoHit.nCapsuleIndex = 0;
		infoHit.pTalentInfo = pAktTalent;
		infoHit.pAttacker = pNPC;
		infoHit.pVictim = pNPC;
		pNPC->OnHit(infoHit, false);

		pAktTalent->m_setNPCID.insert(pNPC->GetID());
		wchar_t szTalentID[512];
		_itow(pAktTalent->m_nID, szTalentID, 10);
		GColtAction coltAction;
		coltAction.nType = CAT_TALENT;		
		coltAction.qParams[0] = szTalentID;		
		coltAction.fDurationTime = -1.0f;
		coltAction.bModifyDurationTime = true;
		GColtActionInst coltInstInfo(coltAction);

		GJobRunner JobRunner;
		GColtRunner ColtRunner(&JobRunner);

		ColtRunner.RunAction(&coltAction, &coltInstInfo, pNPC, pNPC);
		pNPC->GetModuleAI()->Update(0.0f);
		pNPC->Update(0.0f);

		CHECK_EQUAL(WLUA->GetVar<int>("TEST_VAR"), 777);

		SAFE_DELETE(m_pNPCInfo->m_pMeshInfo);
	}

} // SUITE
