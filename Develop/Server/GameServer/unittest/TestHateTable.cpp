#include "stdafx.h"
#include "GHateTable.h"
#include "GEntityNPC.h"
#include "GEntityPlayer.h"
#include "SUnitTestUtil.h"
#include "MockField.h"
#include "GModuleAI.h"
#include "GNPCMgr.h"
#include "GPartySystem.h"
#include "GDropList.h"
#include "GNPCInfoMgr.h"
#include "GGlobal.h"
#include "GTestWrapper.h"
#include "GUTHelper.h"
#include "CCommandTable.h"
#include "GTestForward.h"
#include "GFieldMgr.h"
#include "FBaseMockLink.h"
#include "GActorHateTracker.h"
#include "GModuleCombat.h"
#include "GPlayerObjectManager.h"


const vec3 vCommonPos = vec3(1000.0f, 1000.0f, 0.0f);
const vec3 vCommonDir = vec3(0,1,0);

SUITE(HateTable)
{
	struct Fixture : public FBaseMockLink
	{
		Fixture()
		{ 
			gsys.pScriptManager->Init();

			m_pField = new MockField(SUnitTestUtil::NewUID());
			m_pField->Create();

			m_NPCInfo.nID = 1;
			m_NPCInfo.nMaxHP = 50;
			m_NPCInfo.nSightRange = 1000;
			
			m_pNPC = m_pField->SpawnTestNPC(&m_NPCInfo, vCommonPos, vCommonDir);

			m_pHateTable = &m_pNPC->GetHateTable();
			
			for (int i = 0; i < 3; i++)
			{
				m_pEnemies[i] = new GEntityPlayer();
				m_pEnemies[i]->Create(SUnitTestUtil::NewUID());

				m_pEnemies[i]->GetPlayerInfo()->nMaxHP = 100;
				m_pEnemies[i]->SetHP(100);

				vec3 player_pos = vCommonPos;
				m_pEnemies[i]->EnterToField(m_pField, player_pos);
			}

			// 탤런트 
			GTalentInfo* pNewTalentInfo = new GTalentInfo();
			pNewTalentInfo->m_nID = 1;
			pNewTalentInfo->m_nTiming = TC_HIT_TALENT;
			pNewTalentInfo->m_nTalentType = TT_SKILL;
			pNewTalentInfo->m_nSkillType = ST_MAGIC;
			pNewTalentInfo->m_EffectInfo.m_nPoint = CSEffectInfo::POINT_TARGET;
			pNewTalentInfo->m_EffectInfo.m_nRelation = CSEffectInfo::RELATION_ALL;
			pNewTalentInfo->m_fCastingTime = 0.0f;
			pNewTalentInfo->m_fHitDelay = 0.0f;
			pNewTalentInfo->m_nDodgeDifficulty = 255; // 회피 불가
			pNewTalentInfo->m_nMinDamage = pNewTalentInfo->m_nMaxDamage = 8;		// hp를 8 빼줌
			pNewTalentInfo->m_bNeedTraining = false;

			//pNewTalentInfo->m_ActorModifier.nHP.nMod = 0;
			pNewTalentInfo->Cooking();

			gmgr.pTalentInfoMgr->insert(GTalentInfoMgr::value_type(pNewTalentInfo->m_nID, pNewTalentInfo));

			pNewTalentInfo = new GTalentInfo();
			pNewTalentInfo->m_nID = 2;
			pNewTalentInfo->m_nTalentType = TT_SKILL;
			pNewTalentInfo->m_nSkillType = ST_MAGIC;
			pNewTalentInfo->m_EffectInfo.m_nPoint = CSEffectInfo::POINT_TARGET;
			pNewTalentInfo->m_EffectInfo.m_nRelation = CSEffectInfo::RELATION_ALL;
			pNewTalentInfo->m_fCastingTime = 0.0f;
			pNewTalentInfo->m_fHitDelay = 0.0f;
			pNewTalentInfo->m_nMinDamage = pNewTalentInfo->m_nMaxDamage = 1000;
			pNewTalentInfo->m_bNeedTraining = false;

			//pNewTalentInfo->m_ActorModifier.nHP.nMod = 0;
			pNewTalentInfo->Cooking();

			gmgr.pTalentInfoMgr->insert(GTalentInfoMgr::value_type(pNewTalentInfo->m_nID, pNewTalentInfo));
		}

		~Fixture() 
		{
			for (int i =0; i < 3; i++)
			{
				m_pEnemies[i]->Destroy();
				SAFE_DELETE(m_pEnemies[i]);
			}
			m_pField->Destroy();
			SAFE_DELETE(m_pField);

			gsys.pScriptManager->Fini();
		}

		MockField*			m_pField;
		GNPCInfo			m_NPCInfo;
		GEntityNPC*			m_pNPC;
		GEntityNPC*			m_pNPC2;
		GHateTable*	m_pHateTable;
		GEntityPlayer*		m_pEnemies[3];
		GEntityPlayer*		m_pEnemy2;

		GTestMgrWrapper<GTalentInfoMgr>		m_TalentInfoMgrWrapper;
		GTestMgrWrapper<GNPCInfoMgr>		m_NPCInfoMgrWrapper;
		GTestMgrWrapper<GNPCMgr>			m_NPCMgr;
		GUTHelper		m_Helper;
		DECLWRAPPER_SimpleCombatCalc;
	};

	TEST_FIXTURE(Fixture, TestTargetSelector_SimpleAddPoint)
	{	
		CHECK(m_pHateTable->GetTarget() == MUID::ZERO);

		m_pHateTable->AddPoint_FoundEnemy(m_pEnemies[0]);

		CHECK_EQUAL(false, m_pHateTable->IsEmpty());
		CHECK(m_pHateTable->GetTarget() == m_pEnemies[0]->GetUID());

		vector<MUID> vecTargets;
		m_pHateTable->GetHighestTargetsUID(2, vecTargets);
		CHECK_EQUAL(vecTargets.size(), 1);
		if (vecTargets.size() >= 1)
		{
			CHECK(vecTargets[0] == m_pEnemies[0]->GetUID());
		}

		m_pHateTable->AddPoint_GainDamage(m_pEnemies[1], 1000, 1.0f);
		m_pHateTable->AddPoint_GainDamage(m_pEnemies[1], 2000, 1.0f);
		CHECK(m_pHateTable->GetTarget() == m_pEnemies[1]->GetUID());

		vecTargets.clear();
		m_pHateTable->GetHighestTargetsUID(2, vecTargets);
		CHECK_EQUAL(vecTargets.size(), 2);
		if (vecTargets.size() >= 2)
		{
			CHECK(vecTargets[0] == m_pEnemies[1]->GetUID());
			CHECK(vecTargets[1] == m_pEnemies[0]->GetUID());
		}
	}

	TEST_FIXTURE(Fixture, TestTargetSelector_SimpleAddPointNoDamage)
	{	
		m_pHateTable->AddPoint_GainDamage(m_pEnemies[0], 0, 1.0f);

		CHECK_EQUAL(m_pHateTable->GetQty(), 0);
	}

	TEST_FIXTURE(Fixture, TestTargetSelector_AddPoint)
	{	
		MUID uidTarget = m_pNPC->GetUID();
		m_pEnemies[0]->doUseTalent(1, uidTarget);

		float fDeltaTime = 0.1f;
		m_pField->Update(fDeltaTime);		

		CHECK_EQUAL(m_pHateTable->GetQty(), 1);
		CHECK(m_pHateTable->GetTarget() == m_pEnemies[0]->GetUID());
	}

	TEST_FIXTURE(Fixture, EntryTimeout)
	{	
		m_pHateTable->EnableTimeout(true);

		CHECK_EQUAL(0, m_pNPC->GetHateTable().GetQty());
		m_pNPC->GetHateTable().AddPoint_FoundEnemy(m_pEnemies[0]);
		CHECK_EQUAL(1, m_pNPC->GetHateTable().GetQty());
		CHECK_EQUAL(m_pNPC->GetHateTable().m_mapTimeout.size(), m_pNPC->GetHateTable().GetQty());
		m_pNPC->GetHateTable().Update(GConst::HATETABLE_ENTRY_TIMEOUT/2);
		CHECK_EQUAL(1, m_pNPC->GetHateTable().GetQty());
		m_pNPC->GetHateTable().Update(GConst::HATETABLE_ENTRY_TIMEOUT/2+1);
		CHECK_EQUAL(0, m_pNPC->GetHateTable().GetQty());
	}

	TEST_FIXTURE(Fixture, EntryTimeout_NPCSpecificSetting)
	{	
		// 기본값
		CHECK_EQUAL(GConst::HATETABLE_ENTRY_TIMEOUT, m_pNPC->GetHateTable().GetExpiredTime());

		// NPC별 특정 수치
		GNPCInfo NPCInfo;
		NPCInfo.nID = 2;
		NPCInfo.m_fHateExpiredTime = 10.0f;
		GEntityNPC* pNPC2 = m_pField->SpawnTestNPC(&NPCInfo, vCommonPos, vCommonDir);
		CHECK_EQUAL(10.0f, pNPC2->GetHateTable().GetExpiredTime());
	}

	TEST_FIXTURE(Fixture, TestTargetSelector_Flip)
	{	
		m_pHateTable->AddPoint_GainDamage(m_pEnemies[0], 10);
		m_pHateTable->AddPoint_GainDamage(m_pEnemies[1], 40);
		m_pHateTable->AddPoint_GainDamage(m_pEnemies[2], 20);
		
		CHECK(m_pHateTable->GetTarget() == m_pEnemies[1]->GetUID());
		CHECK(m_pHateTable->ApplyEvent(HATESWAP_FLIP, 10.0f) == true);
		CHECK(m_pHateTable->GetTarget() == m_pEnemies[2]->GetUID());
		m_pHateTable->Update(10.0f);
		CHECK(m_pHateTable->GetTarget() == m_pEnemies[1]->GetUID());

		CHECK(m_pHateTable->ApplyEvent(HATESWAP_FLIP, 0.0f) == true);	// 영구히 변환
		CHECK(m_pHateTable->GetTarget() == m_pEnemies[2]->GetUID());
		m_pHateTable->Update(100.0f);
		CHECK(m_pHateTable->GetTarget() == m_pEnemies[2]->GetUID());
	}

	TEST_FIXTURE(Fixture, TestTargetSelector_Random)
	{	
		m_pHateTable->AddPoint_GainDamage(m_pEnemies[0], 10);
		m_pHateTable->AddPoint_GainDamage(m_pEnemies[1], 40);
		m_pHateTable->AddPoint_GainDamage(m_pEnemies[2], 20);

		CHECK(m_pHateTable->GetTarget() == m_pEnemies[1]->GetUID());
		CHECK(m_pHateTable->ApplyEvent(HATESWAP_RANDOM, 10.0f) == true);
		CHECK(m_pHateTable->GetTarget() != m_pEnemies[1]->GetUID());
		m_pHateTable->Update(10.0f);
		CHECK(m_pHateTable->GetTarget() == m_pEnemies[1]->GetUID());

		CHECK(m_pHateTable->ApplyEvent(HATESWAP_RANDOM, 0.0f) == true);	// 영구히 변환
		CHECK(m_pHateTable->GetTarget() != m_pEnemies[1]->GetUID());
		m_pHateTable->Update(100.0f);
		CHECK(m_pHateTable->GetTarget() != m_pEnemies[1]->GetUID());
	}

	TEST_FIXTURE(Fixture, TestTargetSelector_Near)
	{	
		vec3 pos0 = vec3(1000.0f, 1000.0f, 0.0f);
		vec3 pos1 = vec3(1100.0f, 1000.0f, 0.0f);
		vec3 pos2 = vec3(1200.0f, 1000.0f, 0.0f);

		m_pEnemies[0]->SetPos(pos0);
		m_pEnemies[1]->SetPos(pos1);
		m_pEnemies[2]->SetPos(pos2);

		m_pHateTable->AddPoint_GainDamage(m_pEnemies[0], 1000);
		m_pHateTable->AddPoint_GainDamage(m_pEnemies[1], 4000);
		m_pHateTable->AddPoint_GainDamage(m_pEnemies[2], 2000);

		CHECK(m_pHateTable->GetTarget() == m_pEnemies[1]->GetUID());
		CHECK(m_pHateTable->ApplyEvent(HATESWAP_NEAR, 10.0f) == true);
		CHECK(m_pHateTable->GetTarget() == m_pEnemies[0]->GetUID());
		m_pHateTable->Update(10.0f);
		CHECK(m_pHateTable->GetTarget() == m_pEnemies[1]->GetUID());

		CHECK(m_pHateTable->ApplyEvent(HATESWAP_NEAR, 0.0f) == true);	// 영구히 변환
		CHECK(m_pHateTable->GetTarget() == m_pEnemies[0]->GetUID());
		m_pHateTable->Update(100.0f);
		CHECK(m_pHateTable->GetTarget() == m_pEnemies[0]->GetUID());
	}

	TEST_FIXTURE(Fixture, TestTargetSelector_Far)
	{	
		vec3 pos0 = vec3(1100.0f, 1000.0f, 0.0f);
		vec3 pos1 = vec3(1200.0f, 1000.0f, 0.0f);
		vec3 pos2 = vec3(1300.0f, 1000.0f, 0.0f);

		m_pEnemies[0]->SetPos(pos0);
		m_pEnemies[1]->SetPos(pos1);
		m_pEnemies[2]->SetPos(pos2);

		m_pHateTable->AddPoint_GainDamage(m_pEnemies[0], 10);
		m_pHateTable->AddPoint_GainDamage(m_pEnemies[1], 40);
		m_pHateTable->AddPoint_GainDamage(m_pEnemies[2], 20);

		CHECK(m_pHateTable->GetTarget() == m_pEnemies[1]->GetUID());
		CHECK(m_pHateTable->ApplyEvent(HATESWAP_FAR, 10.0f) == true);
		CHECK(m_pHateTable->GetTarget() == m_pEnemies[2]->GetUID());
		m_pHateTable->Update(10.0f);
		CHECK(m_pHateTable->GetTarget() == m_pEnemies[1]->GetUID());

		CHECK(m_pHateTable->ApplyEvent(HATESWAP_FAR, 0.0f) == true);	// 영구히 변환
		CHECK(m_pHateTable->GetTarget() == m_pEnemies[2]->GetUID());
		m_pHateTable->Update(100.0f);
		CHECK(m_pHateTable->GetTarget() == m_pEnemies[2]->GetUID());
	}

	TEST_FIXTURE(Fixture, Callback_TargetChanged)
	{	
		GEntityNPC* pNPC = m_pField->SpawnTestNPC(&m_NPCInfo, vCommonPos);
		GEntityPlayer* pPlayer1 = GUTHelper::NewEntityPlayer(m_pField, vCommonPos);
		MockLink* pLink = NewLink(pPlayer1);
		MUID uidTarget = pNPC->GetUID();

		pLink->AddIgnoreID(MC_CHAR_ENEMY_INFO);
		pLink->AddIgnoreID(MC_ACTION_USE_TALENT);
		pLink->AddIgnoreID(MC_ACTION_ACT_TALENT);
		pLink->AddIgnoreID(MC_ACTION_TALENT_HIT);
		pLink->AddIgnoreID(MC_ACTION_TALENT_HIT_NO_MF);
		pLink->AddIgnoreID(MC_CHAR_ADD_ENEMYNPC);
		pLink->AddIgnoreID(MC_CHAR_DEL_ENEMYNPC);
		pLink->AddIgnoreID(MC_NPCINTERACTION_ICON);

		pPlayer1->doUseTalent(1, uidTarget);
		m_pField->Update(0.1f);		
		pNPC->UpdateAI(0.1f);
		pNPC->UpdateAI(0.1f);

		CHECK_EQUAL(pLink->GetCommand(0).GetID(), MC_NPC_CHANGE_COMBAT_TARGET);
		MUID uidNPC			= pLink->GetParam<MUID>(0, 0);
		MUID uidTargetActor = pLink->GetParam<MUID>(0, 1);
		CHECK_EQUAL(uidNPC, pNPC->GetUID());	
		CHECK_EQUAL(uidTargetActor, pPlayer1->GetUID());	

		// change other target
		GEntityPlayer* pPlayer2 = GUTHelper::NewEntityPlayer(m_pField, vCommonPos);

		pLink->ResetCommands();	// clear packet history
		pPlayer2->doUseTalent(1, uidTarget);
		m_pField->Update(0.1f);	
		pNPC->UpdateAI(0.1f);
		pPlayer2->doUseTalent(1, uidTarget);	// deal twice damage
		m_pField->Update(0.1f);		
		pNPC->UpdateAI(0.1f);

		CHECK_EQUAL(pLink->GetCommand(0).GetID(), MC_NPC_CHANGE_COMBAT_TARGET);
		uidNPC			= pLink->GetParam<MUID>(0, 0);
		uidTargetActor = pLink->GetParam<MUID>(0, 1);
		CHECK_EQUAL(uidNPC, pNPC->GetUID());	
		CHECK_EQUAL(uidTargetActor, pPlayer2->GetUID());
	}

	TEST_FIXTURE(Fixture, AddPointByFoundEnemy)
	{
		GEntityNPC* pNPC = m_Helper.NewEntityNPC(m_pField);

		GEntityPlayer* pPlayer = GUTHelper::NewEntityPlayer(m_pField);
		pNPC->GetChrInfo()->nMaxHP = 1000;
		pNPC->SetHP(1000);

		pNPC->GetHateTable().AddPoint_FoundEnemy(pPlayer);

		CHECK_EQUAL(pNPC->GetHP()/10, pNPC->GetHateTable().GetHate(pPlayer->GetUID()));
	}

	TEST_FIXTURE(Fixture, AddPointByGainDamage)
	{
		GEntityNPC* pNPC = m_Helper.NewEntityNPC(m_pField);
		GEntityPlayer* pPlayer = GUTHelper::NewEntityPlayer(m_pField);

		int nDamage = 1000;
		float fHateFactor = 0.5f;
		pNPC->GetHateTable().AddPoint_GainDamage(pPlayer, nDamage, fHateFactor);

		CHECK_EQUAL(static_cast<long>(nDamage*fHateFactor), pNPC->GetHateTable().GetHate(pPlayer->GetUID()));
	}

	TEST_FIXTURE(Fixture, AddPointByEnemyUseTalent)
	{
		GEntityNPC* pNPC = m_Helper.NewEntityNPC(m_pField);
		GEntityPlayer* pPlayer = GUTHelper::NewEntityPlayer(m_pField);
		
		int nHateAdj = 100;
		pNPC->GetHateTable().AddPoint_EnemyApplyEffect(pPlayer, nHateAdj);

		CHECK_EQUAL(nHateAdj, pNPC->GetHateTable().GetHate(pPlayer->GetUID()));
	}

	TEST_FIXTURE(Fixture, AddPointByEnemyRecoverHP)
	{
		GEntityNPC* pNPC = m_Helper.NewEntityNPC(m_pField);
		GEntityPlayer* pPlayer = GUTHelper::NewEntityPlayer(m_pField);

		int nRecoveredHP = 100;
		float fHateFactor = 0.5f;
		pNPC->GetHateTable().AddPoint_EnemyRecoverHP(pPlayer, nRecoveredHP, fHateFactor);

		CHECK_EQUAL(nRecoveredHP * fHateFactor * 0.5f, pNPC->GetHateTable().GetHate(pPlayer->GetUID()));
	}

	TEST_FIXTURE(Fixture, TargetLock)
	{
		GEntityNPC* pNPC = m_Helper.NewEntityNPC(m_pField);
		GEntityPlayer* pPlayer = GUTHelper::NewEntityPlayer(m_pField);
		GEntityPlayer* pPlayer2 = GUTHelper::NewEntityPlayer(m_pField);

		int nDamage = 1000;
		float fHateFactor = 0.5f;
		float fLockDurationTime = 10.0f;
		pNPC->GetHateTable().AddPoint_GainDamage(pPlayer, nDamage, fHateFactor);

		CHECK_EQUAL(pPlayer->GetUID(), pNPC->GetHateTable().GetTarget());
		pNPC->GetHateTable().Lock(pPlayer2->GetUID(), fLockDurationTime);
		CHECK_EQUAL(pPlayer2->GetUID(), pNPC->GetHateTable().GetTarget());
		pNPC->GetHateTable().Update(fLockDurationTime);	// 시간 만료
		CHECK_EQUAL(pPlayer->GetUID(), pNPC->GetHateTable().GetTarget());
	}

	TEST_FIXTURE(Fixture, NPCAttr_Tauntable)
	{
		GNPCInfo* pNPCInfo = new GNPCInfo();
		pNPCInfo->nID = 777;
		pNPCInfo->bTauntable = false;
		gmgr.pNPCInfoMgr->Insert(pNPCInfo);

		CHECK_EQUAL(true, m_pNPC->GetHateTable().IsTauntable());	// 기본 설정은 타운트 가능
		GEntityNPC* pNPC = m_Helper.NewEntityNPC(m_pField, vec3::ZERO, 777);
		CHECK_EQUAL(false, pNPC->GetHateTable().IsTauntable());	// 타운트 안되게 설정
	}

	TEST_FIXTURE(Fixture, TargetSelectionBehavior)
	{
		GEntityNPC* pNPC = m_Helper.NewEntityNPC(m_pField);
		GEntityPlayer* pPlayer = GUTHelper::NewEntityPlayer(m_pField);
		GEntityPlayer* pPlayer2 = GUTHelper::NewEntityPlayer(m_pField);

		pNPC->SetPos(vec3(0, 0, 0));
		pPlayer->SetPos(vec3(20, 20, 20));
		pPlayer2->SetPos(vec3(10, 10, 10));

		int nDamage = 1000;
		float fHateFactor = 0.5f;
		float fEventDurationTime = 10.0f;
		pNPC->GetHateTable().AddPoint_GainDamage(pPlayer, nDamage, fHateFactor);

		nDamage = 500;
		fHateFactor = 0.5f;
		fEventDurationTime = 10.0f;
		pNPC->GetHateTable().AddPoint_GainDamage(pPlayer2, nDamage, fHateFactor);

		CHECK_EQUAL(pPlayer->GetUID(), pNPC->GetHateTable().GetTarget());
		pNPC->GetHateTable().ApplyEvent(HATESWAP_NEAR, fEventDurationTime);
		CHECK_EQUAL(pPlayer2->GetUID(), pNPC->GetHateTable().GetTarget());
		pNPC->GetHateTable().Update(fEventDurationTime);	// 시간 만료
		CHECK_EQUAL(pPlayer->GetUID(), pNPC->GetHateTable().GetTarget());
	}

	TEST_FIXTURE(Fixture, TargetSelectionBehavior_DieEventTarget)
	{
		GEntityNPC* pNPC = m_Helper.NewEntityNPC(m_pField);
		GEntityPlayer* pPlayer = GUTHelper::NewEntityPlayer(m_pField);
		GEntityPlayer* pPlayer2 = GUTHelper::NewEntityPlayer(m_pField);

		pNPC->SetPos(vec3(0, 0, 0));
		pPlayer->SetPos(vec3(20, 20, 20));
		pPlayer2->SetPos(vec3(10, 10, 10));

		int nDamage = 1000;
		float fHateFactor = 0.5f;
		float fEventDurationTime = 10.0f;
		pNPC->GetHateTable().AddPoint_GainDamage(pPlayer, nDamage, fHateFactor);

		nDamage = 500;
		fHateFactor = 0.5f;
		fEventDurationTime = 10.0f;
		pNPC->GetHateTable().AddPoint_GainDamage(pPlayer2, nDamage, fHateFactor);

		CHECK_EQUAL(pPlayer->GetUID(), pNPC->GetHateTable().GetTarget());
		pNPC->GetHateTable().ApplyEvent(HATESWAP_NEAR, fEventDurationTime);
		CHECK_EQUAL(pPlayer2->GetUID(), pNPC->GetHateTable().GetTarget());
		pPlayer2->doDie(); // 대상 죽음
		pNPC->GetHateTable().Update(1.0f);
		CHECK_EQUAL(pPlayer->GetUID(), pNPC->GetHateTable().GetTarget());
	}

	TEST_FIXTURE(Fixture, ResetHatetimout)
	{
		GEntityNPC* pNPC = m_Helper.NewEntityNPC(m_pField);
		GEntityPlayer* pPlayer = GUTHelper::NewEntityPlayer(m_pField);

		CHECK_EQUAL(-1.0f, pNPC->GetHateTable().GetTimeoutTime(pPlayer->GetUID())); // 제거됨
		pNPC->GetHateTable().AddPoint_GainDamage(pPlayer, 100, 1.0f);
		CHECK_EQUAL(0.0f, pNPC->GetHateTable().GetTimeoutTime(pPlayer->GetUID()));
		pNPC->GetHateTable().Update(3.0f);
		CHECK_EQUAL(3.0f, pNPC->GetHateTable().GetTimeoutTime(pPlayer->GetUID()));
		pNPC->GetHateTable().AddPoint_GainDamage(pPlayer, 100, 1.0f);
		CHECK_EQUAL(0.0f, pNPC->GetHateTable().GetTimeoutTime(pPlayer->GetUID()));
	}

	TEST_FIXTURE(Fixture, Reset)
	{
		GEntityPlayer* pPlayer = GUTHelper::NewEntityPlayer(m_pField);

		CHECK_EQUAL(true, m_pHateTable->IsEmpty());
		m_pHateTable->AddPoint_GainDamage(pPlayer, 100, 1.0f);
		CHECK_EQUAL(false, m_pHateTable->IsEmpty());
		m_pHateTable->Reset();
		CHECK_EQUAL(true, m_pHateTable->IsEmpty());
	}

	TEST_FIXTURE(Fixture, ResetWhenDie)
	{
		GEntityPlayer* pPlayer = GUTHelper::NewEntityPlayer(m_pField);

		CHECK_EQUAL(true, m_pHateTable->IsEmpty());
		CHECK_EQUAL(true, pPlayer->GetActorHateTracker().IsEmpty());
		m_pHateTable->AddPoint_GainDamage(pPlayer, 100, 1.0f);
		CHECK_EQUAL(false, m_pHateTable->IsEmpty());
		CHECK_EQUAL(false, pPlayer->GetActorHateTracker().IsEmpty());
		m_pNPC->doDie();
		CHECK_EQUAL(true, m_pHateTable->IsEmpty());
		CHECK_EQUAL(true, pPlayer->GetActorHateTracker().IsEmpty());
	}

	TEST_FIXTURE(Fixture, ResetWhenDespawn)
	{
		GEntityPlayer* pPlayer = GUTHelper::NewEntityPlayer(m_pField);

		CHECK_EQUAL(true, m_pHateTable->IsEmpty());
		CHECK_EQUAL(true, pPlayer->GetActorHateTracker().IsEmpty());
		m_pHateTable->AddPoint_GainDamage(pPlayer, 100, 1.0f);
		CHECK_EQUAL(false, m_pHateTable->IsEmpty());
		CHECK_EQUAL(false, pPlayer->GetActorHateTracker().IsEmpty());
		m_pNPC->doDespawn();
		CHECK_EQUAL(true, m_pHateTable->IsEmpty());
		CHECK_EQUAL(true, pPlayer->GetActorHateTracker().IsEmpty());
	}

	TEST_FIXTURE(Fixture, ResetWhenDestroy)
	{
		GEntityPlayer* pPlayer = GUTHelper::NewEntityPlayer(m_pField);

		CHECK_EQUAL(true, m_pHateTable->IsEmpty());
		CHECK_EQUAL(true, pPlayer->GetActorHateTracker().IsEmpty());
		m_pHateTable->AddPoint_GainDamage(pPlayer, 100, 1.0f);
		CHECK_EQUAL(false, m_pHateTable->IsEmpty());
		CHECK_EQUAL(false, pPlayer->GetActorHateTracker().IsEmpty());
		m_pNPC->DeleteMe();
		m_pField->Update(0.0f);
		CHECK_EQUAL(true, pPlayer->GetActorHateTracker().IsEmpty());
	}

	TEST_FIXTURE(Fixture, PacketEnemyNPCAddDel)
	{	
		GEntityNPC* pNPC = m_pField->SpawnTestNPC(&m_NPCInfo, vCommonPos);
		GEntityPlayer* pPlayer = GUTHelper::NewEntityPlayer(m_pField, vCommonPos);
		MockLink* pLink = NewLink(pPlayer);

		pLink->IgnoreAll();
		pLink->AddAllowedID(MC_CHAR_ADD_ENEMYNPC);
		pLink->AddAllowedID(MC_CHAR_DEL_ENEMYNPC);

		// 어그로 얻음
		pNPC->GetHateTable().AddPoint_GainDamage(pPlayer, 100, 1.0f);
		CHECK_EQUAL(1, pLink->GetCommandCount());
		CHECK_EQUAL(pLink->GetCommand(0).GetID(), MC_CHAR_ADD_ENEMYNPC);
		MUID uidNPC			= pLink->GetParam<MUID>(0, 0);
		CHECK_EQUAL(uidNPC, pNPC->GetUID());	
		pLink->ResetCommands();

		// 어그로 잃음
		pNPC->doDie();
		CHECK_EQUAL(1, pLink->GetCommandCount());
		CHECK_EQUAL(pLink->GetCommand(0).GetID(), MC_CHAR_DEL_ENEMYNPC);
		uidNPC			= pLink->GetParam<MUID>(0, 0);
		CHECK_EQUAL(uidNPC, pNPC->GetUID());	
	}

	TEST_FIXTURE(Fixture, PacketEnemyNPCAddDel_Timeout)
	{	
		GEntityNPC* pNPC = m_pField->SpawnTestNPC(&m_NPCInfo, vCommonPos);
		pNPC->GetHateTable().EnableTimeout(true);
		GEntityPlayer* pPlayer = GUTHelper::NewEntityPlayer(m_pField, vCommonPos);
		MockLink* pLink = NewLink(pPlayer);

		pLink->IgnoreAll();
		pLink->AddAllowedID(MC_CHAR_ADD_ENEMYNPC);
		pLink->AddAllowedID(MC_CHAR_DEL_ENEMYNPC);

		// 어그로 얻음
		pNPC->GetHateTable().AddPoint_GainDamage(pPlayer, 100, 1.0f);
		CHECK_EQUAL(1, pLink->GetCommandCount());
		CHECK_EQUAL(pLink->GetCommand(0).GetID(), MC_CHAR_ADD_ENEMYNPC);
		MUID uidNPC			= pLink->GetParam<MUID>(0, 0);
		CHECK_EQUAL(uidNPC, pNPC->GetUID());	
		pLink->ResetCommands();

		// 어그로 잃음
		pNPC->Update(9999.0f);
		CHECK_EQUAL(1, pLink->GetCommandCount());
		CHECK_EQUAL(pLink->GetCommand(0).GetID(), MC_CHAR_DEL_ENEMYNPC);
		uidNPC			= pLink->GetParam<MUID>(0, 0);
		CHECK_EQUAL(uidNPC, pNPC->GetUID());	
	}

	TEST_FIXTURE(Fixture, DontDecreaseWhenUsingTalent)
	{	
		m_pHateTable->EnableTimeout(true);

		GEntityPlayer* pPlayer = GUTHelper::NewEntityPlayer(m_pField, vCommonPos);
		
		// 어그로 얻음
		m_pNPC->GetHateTable().AddPoint_GainDamage(pPlayer, 100, 1.0f);
		CHECK_EQUAL(1, m_pNPC->GetHateTable().GetQty());

		// 탤런트 사용중에는 헤이트 테이블 줄어들지 않음
		GTalentInfo* pTalentInfo = test.talent.MakeDamageTalent(10);
		pTalentInfo->m_fCastingTime = 100.0f;
		pTalentInfo->Cooking();
		test.talent.UseTalent(m_pNPC, pTalentInfo, pPlayer->GetUID());
		for (int i=0; i<10; i++)
		{
			m_pNPC->GetHateTable().Update(GConst::HATETABLE_ENTRY_TIMEOUT);
		}
		CHECK_EQUAL(1, m_pNPC->GetHateTable().GetQty());

		// 탤런트 사용후에는 원래대로 달기
		m_pNPC->GetModuleCombat()->Update(100.0f); // 캐스팅 종료
		m_pNPC->GetModuleCombat()->Update(0.0f); // 액트 페이즈 넘어가기
		for (int i=0; i<10; i++)
			m_pNPC->GetHateTable().Update(GConst::HATETABLE_ENTRY_TIMEOUT);
		CHECK_EQUAL(0, m_pNPC->GetHateTable().GetQty());
	}

	TEST_FIXTURE(Fixture, AddHateTableNPCEntry)
	{
		GEntityNPC* pNPC = m_Helper.NewEntityNPC(m_pField);
		GEntityNPC* pNPCAttacker = m_Helper.NewEntityNPC(m_pField);

		int nDamage = 1000;
		float fHateFactor = 0.5f;
		pNPC->GetHateTable().AddPoint_GainDamage(pNPCAttacker, nDamage, fHateFactor);

		CHECK_EQUAL(static_cast<long>(nDamage*fHateFactor) * 0.5f, pNPC->GetHateTable().GetHate(pNPCAttacker->GetUID()));
	}

	TEST_FIXTURE(Fixture, AddPointByGainReverseDamage)
	{
		GEntityNPC* pNPC = m_Helper.NewEntityNPC(m_pField);
		GEntityPlayer* pPlayer = GUTHelper::NewEntityPlayer(m_pField);

		int nDamage = -1000;
		float fHateFactor = 0.5f;
		pNPC->GetHateTable().AddPoint_GainDamage(pPlayer, nDamage, fHateFactor);

		CHECK_EQUAL(1, pNPC->GetHateTable().GetHate(pPlayer->GetUID()));
	}

	struct Fixture_Party : public Fixture
	{
		Fixture_Party()
		{
			m_pField2 = new MockField(SUnitTestUtil::NewUID());
			m_pField2->Create();

			m_pPlayer0_Leader				= m_Helper.NewEntityPlayer(m_pField, vCommonPos);
			m_pPlayer1_Member				= m_Helper.NewEntityPlayer(m_pField, vCommonPos);
			m_pPlayer2_AnotherFieldMember	= m_Helper.NewEntityPlayer(m_pField2, vCommonPos);
			m_pPlayer3_NoParty				= m_Helper.NewEntityPlayer(m_pField, vCommonPos);

			m_pParty = GUTHelper_Party::CreateParty(m_pPlayer0_Leader, m_pPlayer1_Member);
			GUTHelper_Party::AddMember(m_pParty, m_pPlayer2_AnotherFieldMember);
		}
		virtual ~Fixture_Party()
		{
			m_pField2->Destroy();
			SAFE_DELETE(m_pField2);
		}

		MockField*		m_pField2;
		GParty*			m_pParty;
		GEntityPlayer*	m_pPlayer0_Leader;
		GEntityPlayer*	m_pPlayer1_Member;
		GEntityPlayer*	m_pPlayer2_AnotherFieldMember;
		GEntityPlayer*	m_pPlayer3_NoParty;
		GTestMgrWrapper<GPlayerObjectManager>	m_pPlayerObjectManager;
		GTestSysWrapper<GPartySystem>			m_pPartySystem;
	};

	TEST_FIXTURE(Fixture_Party, TestSimpleAddEntry_WithParty)
	{
		CHECK_EQUAL(0, m_pHateTable->GetQty());

		CHECK_EQUAL(m_pPlayer0_Leader->GetFieldUID(),	m_pPlayer1_Member->GetFieldUID());
		CHECK_EQUAL(m_pPlayer0_Leader->GetPartyUID(),	m_pPlayer1_Member->GetPartyUID());
		CHECK_EQUAL(m_pPlayer0_Leader->GetPos(),		m_pPlayer1_Member->GetPos());

		CHECK(m_pPlayer0_Leader->GetFieldUID() !=		m_pPlayer2_AnotherFieldMember->GetFieldUID());
		CHECK_EQUAL(m_pPlayer0_Leader->GetPartyUID(),	m_pPlayer2_AnotherFieldMember->GetPartyUID());
		CHECK_EQUAL(m_pPlayer0_Leader->GetPos(),		m_pPlayer2_AnotherFieldMember->GetPos());

		CHECK_EQUAL(false, m_pParty->IsExistMember(m_pPlayer3_NoParty->GetUID()));

		m_pHateTable->AddPoint_FoundEnemy(m_pPlayer0_Leader);

		CHECK_EQUAL(2, m_pHateTable->GetQty());

		CHECK_EQUAL(true, m_pHateTable->IsExist(m_pPlayer0_Leader->GetUID()));
		CHECK_EQUAL(true, m_pHateTable->IsExist(m_pPlayer1_Member->GetUID()));
		CHECK_EQUAL(false, m_pHateTable->IsExist(m_pPlayer2_AnotherFieldMember->GetUID()));
		CHECK_EQUAL(false, m_pHateTable->IsExist(m_pPlayer3_NoParty->GetUID()));
	}

	TEST_FIXTURE(Fixture_Party, TestSimpleAddEntry_WithParty_OneIsFar_MAX_DISTANCE)
	{
		vec3 vNewPos = vCommonPos;
		vNewPos.x += GConst::HATETABLE_ENTRY_PARTY_SCAN_DISTANCE;
		m_pPlayer1_Member->SetPos(vNewPos);
		
		m_pHateTable->AddPoint_FoundEnemy(m_pPlayer0_Leader);

		CHECK_EQUAL(2, m_pHateTable->GetQty());

		CHECK_EQUAL(true, m_pHateTable->IsExist(m_pPlayer0_Leader->GetUID()));
		CHECK_EQUAL(true, m_pHateTable->IsExist(m_pPlayer1_Member->GetUID()));
	}

	TEST_FIXTURE(Fixture_Party, TestSimpleAddEntry_WithParty_OneIsFar_Over_MAX_DISTANCE)
	{
		vec3 vNewPos = vCommonPos;
		vNewPos.x += GConst::HATETABLE_ENTRY_PARTY_SCAN_DISTANCE+1;
		m_pPlayer1_Member->SetPos(vNewPos);

		m_pHateTable->AddPoint_FoundEnemy(m_pPlayer0_Leader);

		CHECK_EQUAL(1, m_pHateTable->GetQty());

		CHECK_EQUAL(true, m_pHateTable->IsExist(m_pPlayer0_Leader->GetUID()));
		CHECK_EQUAL(false, m_pHateTable->IsExist(m_pPlayer1_Member->GetUID()));
	}

}


