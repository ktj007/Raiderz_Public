#include "StdAfx.h"
#include "GTestForward.h"
#include "GModuleAI.h"
#include "GJobRunner.h"
#include "GTalentFactory.h"
#include "MockEntityNPC.h"
#include "GTalent.h"
#include "GFieldMgr.h"
#include "GServer.h"
#include "GJob.h"
#include "GJobMgr.h"
#include "FBasePlayer.h"
#include "GHateTable.h"
#include "GModuleBuff.h"
#include "GSoulBindingInfo.h"
#include "MockFieldEnteringMgr.h"


SUITE(Talent_ExtraActive)
{
	class FakeFieldMgr : public GFieldMgr
	{
	public:
		FakeFieldMgr()
		{
			SAFE_DELETE(m_pFieldEnteringQueue);
			m_pFieldEnteringQueue = new MockFieldEnteringMgr();
		}
	};

	struct FTalent : public FBasePlayer
	{
		FTalent()
		{
			m_pField = GUTHelper_Field::DefaultMockField();
			m_pPlayer = NewEntityPlayer(m_pField, vec3(100,100,0));
			m_pLinkTester = test.network.NewLink(m_pPlayer);
			swprintf_s(m_pPlayer->GetPlayerInfo()->szName, L"Tester");

			gsys.pServer->FrameCounterClear();
			SetIgnoreCommonCommand(m_pLinkTester);
		}

		~FTalent()
		{
			m_pField->Destroy();
			test.network.ClearLinks();
		}

		void BeginCommandRecord()
		{
			m_pLinkTester->ResetCommands();
		}

		void SetIgnoreCommonCommand(MockLink* pLink)
		{
			pLink->AddIgnoreID(MC_ACTION_USE_TALENT);
			pLink->AddIgnoreID(MC_ACTION_TALENT_HIT);
			pLink->AddIgnoreID(MC_ACTION_TALENT_HIT_NO_MF);
		}

		void GainFocusBuff(GEntityPlayer* pPlayer, int nBuffID)
		{
			GBuffInfo* pBuff=test.buff.NewBuffInfo(nBuffID);
			test.buff.GainBuffSelf(pPlayer, pBuff);
		}

		MockField*		m_pField;
		GEntityPlayer*	m_pPlayer;
		MockLink*		m_pLinkTester;

		DECLWRAPPER_FieldInfoMgr;
		DECLWRAPPER_FieldMgr;
		DECLWRAPPER_TalentMgr;
		DECLWRAPPER_NPCInfoMgr;
		DECLWRAPPER_SimpleCombatCalc;
	};

	TEST(TestExtraActiveTalent_Portal)
	{
		GTestMgrWrapper2<GFieldMgr, FakeFieldMgr>	fakeFieldMgr;

		MockField* pField = GUTHelper_Field::DefaultMockField();
		GEntityPlayer* pPlayer = test.player.NewPlayer(pField);

		GFieldInfo* pTargetFieldInfo = GUTHelper_Field::NewFieldInfo();
		MARKER_INFO* pTargetMarkerInfo = GUTHelper_Field::InsertNewMarkerInfo(pTargetFieldInfo);
		pTargetMarkerInfo->vDir = vec3(7, 7, 7);
		pTargetMarkerInfo->vPoint = vec3(8, 8, 8);
		MockField* pTargetField = GUTHelper_Field::NewMockField(pTargetFieldInfo);

		GUTHelper_Field::NewSharedField(pTargetFieldInfo, 1);

		GTalentInfo* pTalentInfo = test.talent.NewItemTalentInfo();
		pTalentInfo->m_nTiming = TC_USE_TALENT;
		test.talent.SetExtraActiveTalentInfo(pTalentInfo, INVALID_ID, TEAT_PORTAL);
		pTalentInfo->m_nExtraActiveParam1 = pTargetFieldInfo->m_nFieldID;
		pTalentInfo->m_nExtraActiveParam2 = pTargetMarkerInfo->nID;
		pTalentInfo->Cooking();
		
		test.talent.UseTalent(pPlayer, pTalentInfo);

		pField->Update(1.0f);		
		MockLink* pLink = GUTHelper::NewLink(pPlayer);
		pLink->OnRecv(MC_FIELD_LOADING_COMPLETE, 0, NULL);

		CHECK_EQUAL(pTargetMarkerInfo->nFieldID, pPlayer->GetFieldID());
		CHECK_EQUAL(pTargetMarkerInfo->vPoint.x, pPlayer->GetPos().x);
		CHECK_EQUAL(pTargetMarkerInfo->vPoint.y, pPlayer->GetPos().y);
		CHECK_EQUAL(pTargetMarkerInfo->vPoint.z, pPlayer->GetPos().z);

		pField->Destroy();

		pTargetField->Destroy();
		SAFE_DELETE(pTargetField);

		GUTHelper::ClearLinks();
	}

	TEST(TestExtraActiveTalent_Portal_Soulbinding)
	{
		GTestMgrWrapper2<GFieldMgr, FakeFieldMgr>	fakeFieldMgr;

		GUTHelper helper;
		MockField* pField = GUTHelper_Field::DefaultMockField();
		GEntityPlayer* pPlayer = GUTHelper::NewEntityPlayer(pField);

		GFieldInfo* pFieldInfo = GUTHelper_Field::NewFieldInfo();		
		MARKER_INFO* pMarkerInfo = GUTHelper_Field::InsertNewMarkerInfo(pFieldInfo);
		pMarkerInfo->vDir = vec3(7, 7, 7);
		pMarkerInfo->vPoint = vec3(8, 8, 8);
		MockField* pInnField = GUTHelper_Field::NewMockField(pFieldInfo);

		GSoulBindingInfo* pSoulBindingInfo = helper.NewSoulBindingInfo(pMarkerInfo);		
		pPlayer->GetPlayerInfo()->nSoulBindingID = pSoulBindingInfo->m_nID;

		GUTHelper_Field::NewSharedField(pFieldInfo, 1);

		GTalentInfo* pTalentInfo = helper.NewItemTalentInfo();
		pTalentInfo->m_nTiming = TC_USE_TALENT;
		test.talent.SetExtraActiveTalentInfo(pTalentInfo, INVALID_ID, TEAT_PORTAL_SOULBINDING);

		test.talent.UseTalent(pPlayer, pTalentInfo);

		pField->Update(1.0f);		
		MockLink* pLink = GUTHelper::NewLink(pPlayer);
		pLink->OnRecv(MC_FIELD_LOADING_COMPLETE, 0, NULL);

		CHECK_EQUAL(pMarkerInfo->nFieldID, pPlayer->GetFieldID());
		CHECK_EQUAL(pMarkerInfo->vPoint.x, pPlayer->GetPos().x);
		CHECK_EQUAL(pMarkerInfo->vPoint.y, pPlayer->GetPos().y);
		CHECK_EQUAL(pMarkerInfo->vPoint.z, pPlayer->GetPos().z);

		pField->Destroy();

		pInnField->Destroy();
		SAFE_DELETE(pInnField);

		GUTHelper::ClearLinks();
	}

	TEST_FIXTURE(FTalent, TestExtraActiveTalent_Spawn)
	{
		GNPCInfo* pNPCInfo = test.npc.NewNPCInfo();

		GTalentInfo* pTalentInfo = test.talent.NewItemTalentInfo();
		pTalentInfo->m_nTiming = TC_USE_TALENT;
		test.talent.SetExtraActiveTalentInfo(pTalentInfo, INVALID_ID, TEAT_SPAWN);
		pTalentInfo->m_nExtraActiveParam1 = pNPCInfo->nID;
		pTalentInfo->m_nExtraActiveParam2 = 10;
		pTalentInfo->Cooking();

		test.talent.UseTalent(m_pPlayer, pTalentInfo);

		m_pField->Update(1.0f);
		CHECK_EQUAL(1, m_pField->GetNPCQty(pNPCInfo->nID));

		m_pField->GetSpawn()->Update(10.0f);
		m_pField->Update(1.0f);
		CHECK_EQUAL(0, m_pField->GetNPCQty(pNPCInfo->nID));
	}

	TEST_FIXTURE(FTalent, TestExtraActiveTalent_ActPhaseToFinishPhase)
	{
		GTalentFactory talentFactory;
		GEntityNPC* pNPC = GUTHelper_NPC::SpawnNPC(m_pField);

		GTalentInfo talentInfo;
		talentInfo.m_nID = 1;
		talentInfo.m_nCategory = TC_COMMON;
		talentInfo.m_nSkillType = ST_MELEE;
		talentInfo.m_nTiming = TC_USE_TALENT;
		talentInfo.m_EffectInfo.m_nPoint = CSEffectInfo::POINT_SELF;
		talentInfo.m_EffectInfo.m_nRelation = CSEffectInfo::RELATION_ALL;

		const float AI_TIME = 3.0f;
		talentInfo.m_fDurationTime = AI_TIME;
		talentInfo.Cooking();

		GTalent* pTalent = talentFactory.NewTalent(pNPC, &talentInfo, MUID::Invalid(), false);
		pTalent->Start();

		pTalent->Update(0.1f);
		CHECK_EQUAL(TALENT_PHASE_ACT, pTalent->GetPhase());

		pTalent->Update(AI_TIME);
		pTalent->UpdateExpired(AI_TIME);
		CHECK_EQUAL(TALENT_PHASE_FINISH, pTalent->GetPhase());

		SAFE_DELETE(pTalent);
	}

	TEST_FIXTURE(FTalent, TestExtraActiveTalent_ActPhaseToFinishPhaseOnJob)
	{
		GNPCInfo		m_NPCInfo;
		m_NPCInfo.nID = 1;

		//GNPCInfo* pMode1Info = new GNPCInfo();
		//pMode1Info->nID = 1;
		//pMode1Info->ModeCopyFromParent(&m_NPCInfo);

		m_NPCInfo.vecModes.push_back(new GNPCInfo());	// mode1도 있다고 가정


		GEntityNPC* pNewNPC = m_pField->SpawnTestNPC(&m_NPCInfo);

		GTalentInfo* pExtraActiveTalent = test.talent.NewTalentInfo();
		pExtraActiveTalent->m_nSkillType = ST_EXTRA_ACTIVE;
		pExtraActiveTalent->m_nExtraActive = TEAT_DISPEL_BUFF;
		pExtraActiveTalent->m_nTiming = TC_USE_TALENT;
		pExtraActiveTalent->m_EffectInfo.m_nPoint = CSEffectInfo::POINT_SELF;

		pExtraActiveTalent->m_fDurationTime = 3.0f;
		pExtraActiveTalent->m_ChangeModeInfo.m_bChangeMode = true;
		pExtraActiveTalent->m_ChangeModeInfo.m_nModeTo = NPC_MODE_1;
		pExtraActiveTalent->m_setNPCID.insert(1);

		CHECK_EQUAL(NPC_MODE_DEFAULT, pNewNPC->GetMode());

		GJobRunner job_runner;
		job_runner.UseTalent(pNewNPC, NULL, pExtraActiveTalent->m_nID);

		pNewNPC->UpdateAI(0.1f);
		m_pField->Update(0.1f);
		
		GJobMgr* pJobMgr = pNewNPC->GetJobMgr();
		GJob* pCurrJob = pJobMgr->GetCurrJob();
		
		CHECK(pCurrJob != NULL);
		if (pCurrJob != NULL)
		{
			CHECK_EQUAL(GJOB_TALENT, pCurrJob->GetID());
		}


		pNewNPC->UpdateAI(1.0f);
		pNewNPC->UpdateAI(1.0f);
		pNewNPC->UpdateAI(1.0f);
		pNewNPC->UpdateAI(1.0f);

		// 3초가 훨씬 지났으므로 Talent 사용이 끝났다.
		CHECK(pJobMgr->GetCurrJob() == NULL);

		CHECK_EQUAL(1, pNewNPC->GetMode());

		m_pField->Destroy();
	}

	TEST_FIXTURE(FTalent, Taunt)
	{
		DECLWRAPPER_SimpleCombatCalc;

		GEntityNPC* pNPC = GUTHelper_NPC::SpawnNPC(m_pField);
		GEntityPlayer* pOtherPlayer = NewEntityPlayer(m_pField);
		GTalentInfo* pActiveTalent = test.talent.NewTalentInfo();
		test.talent.SetExtraActiveTalentInfo(pActiveTalent, 1, TEAT_TAUNT);
		pActiveTalent->m_nExtraActiveParam1 = 10;	// 10초 동안 고정
		pActiveTalent->m_EffectInfo.m_nPoint = CSEffectInfo::POINT_TARGET;
		pActiveTalent->m_EffectInfo.m_nRelation = CSEffectInfo::RELATION_ALL;		

		int nDamage = 1000;
		float fHateFactor = 0.5f;
		pNPC->GetHateTable().AddPoint_GainDamage(m_pPlayer, nDamage, fHateFactor);
		CHECK_EQUAL(m_pPlayer->GetUID(), pNPC->GetHateTable().GetTarget());
		test.talent.UseTalent(pOtherPlayer, pActiveTalent, pNPC->GetUID());
		CHECK_EQUAL(pOtherPlayer->GetUID(), pNPC->GetHateTable().GetTarget());
		pNPC->GetHateTable().Update((float)pActiveTalent->m_nExtraActiveParam1);	// 시간 만료
		CHECK_EQUAL(m_pPlayer->GetUID(), pNPC->GetHateTable().GetTarget());
	}
}

