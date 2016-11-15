#include "stdafx.h"
#include "GTalentHitter.h"
#include "GEntityNPC.h"
#include "MockField.h"
#include "MockEntityPlayer.h"
#include "SUnitTestUtil.h"
#include "GNPCMgr.h"
#include "GNPCInfoMgr.h"
#include "GPathFinder.h"
#include "GGlobal.h"
#include "GConst.h"
#include "GTalentEffector.h"
#include "GFieldMgr.h"
#include "GUTHelper.h"

SUITE(TalentHitter)
{
	struct FTalentHitter
	{
		FTalentHitter()
		{
			m_pField = new MockField(SUnitTestUtil::NewUID());
			m_pField->Create();

			m_NPCInfo.nID = 1;
			m_pEntityNPC = m_pField->SpawnTestNPC(&m_NPCInfo, vec3(100,100,0), vec3(1,0,0));

			m_pEntityPlayer = GUTHelper::NewEntityPlayer();

			vec3 pos = vec3(1.0f, 100.0f, 0.0f);
			vec3 dir = vec3(0.0f,1.0f,0.0f);
			m_pEntityPlayer->EnterToField(m_pField, pos, dir);
		}
		~FTalentHitter()
		{
			m_pField->Destroy();
			SAFE_DELETE(m_pField);
		}

		GNPCInfo			m_NPCInfo;
		MockField*			m_pField;
		GEntityNPC*			m_pEntityNPC;
		GEntityPlayer*		m_pEntityPlayer;
		GTalentHitterList	m_TalentHitterList;
		GTalentEffector TalentEffector;
	};

	//TEST_FIXTURE(FTalentHitter, TestTalentHitter_Projectile)
	//{
	//	GTalentHitter_Projectile* pNewHitter1 = GTalentHitter::NewTalentHitter< GTalentHitter_Projectile >();
	//	m_TalentHitterList.Add(pNewHitter1);

	//	MUID uidOwner = SUnitTestUtil::NewUID();

	//	GTalentInfo talent_info1;
	//	talent_info1.m_nID = 1;
	//	talent_info1.m_nHitTestType = SHT_PROJECTILE;
	//	talent_info1.m_nProjectileType = SWT_MISSILE;

	//	pNewHitter1->Setup(uidOwner, vec3(0,0,0), vec3(1.0f, 0.0f, 0.0f), &talent_info1);

	//	// timeout 체크
	//	CHECK(pNewHitter1->Update(0.5f, m_pField) == false);
	//	CHECK(pNewHitter1->Update((float)GTalentHitter::TALENT_HITTER_TIMEOUT, m_pField) == true);


	//	GTalentHitter_Projectile* pNewHitter2 = GTalentHitter::NewTalentHitter< GTalentHitter_Projectile >();
	//	m_TalentHitterList.Add(pNewHitter2);

	//	uidOwner = SUnitTestUtil::NewUID();

	//	GTalentInfo talent_info2;
	//	talent_info2.m_nID = 2;
	//	talent_info2.m_nHitTestType = SHT_PROJECTILE;
	//	talent_info2.m_fProjectileRange = 10.0f;
	//	talent_info2.m_fProjectileHitRadius = 1.0f;

	//	pNewHitter2->Setup(uidOwner, vec3(100.0f, 0.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f), &talent_info2);

	//	// 사거리 체크
	//	CHECK_EQUAL(pNewHitter2->Update(5.0f, m_pField), false);
	//	CHECK_EQUAL(pNewHitter2->Update(6.0f, m_pField), true);

	//	GTalentHitter_Projectile* pNewHitter3 = GTalentHitter::NewTalentHitter< GTalentHitter_Projectile >();
	//	m_TalentHitterList.Add(pNewHitter3);

	//	GTalentInfo talent_info3;
	//	talent_info3.m_nID = 3;
	//	talent_info3.m_nHitTestType = SHT_PROJECTILE;
	//	talent_info3.m_fProjectileRange = 1000.0f;

	//	pNewHitter3->Setup(m_pEntityNPC->GetUID(), vec3(100.0f, 100.0f, 100.0f), vec3(-1.0f, 0.0f, 0.0f), &talent_info3);

	//	// 판정 체크
	//	CHECK_EQUAL(pNewHitter3->Update(1.0f, m_pField), false);
	//	CHECK_EQUAL(pNewHitter3->Update(100.0f, m_pField), true);

	//}

	TEST_FIXTURE(FTalentHitter, TestTalentHitter_TimeDelay)
	{
		
		GTalentHitter_TimeDelay* pNewHitter = GTalentHitter::NewTalentHitter< GTalentHitter_TimeDelay >();
		m_TalentHitterList.Add(pNewHitter);

		GTalentInfo talent_info1;
		talent_info1.m_nID = 1;
		talent_info1.m_nSkillType = ST_MAGIC;
		talent_info1.m_EffectInfo.m_nPoint = CSEffectInfo::POINT_TARGET;
		talent_info1.m_EffectInfo.m_nRelation = CSEffectInfo::RELATION_ALL;

		pNewHitter->Setup(m_pEntityNPC->GetUID(), m_pEntityPlayer->GetUID(), 0, 0, &talent_info1, talent_info1.m_fHitDelay, TalentEffector);		

		// 즉시 발동
		CHECK_EQUAL(pNewHitter->Update(1.0f, m_pField), true);


		// 지연
		GTalentHitter_TimeDelay* pNewHitter2 = GTalentHitter::NewTalentHitter< GTalentHitter_TimeDelay >();
		m_TalentHitterList.Add(pNewHitter2);

		GTalentInfo talent_info2;
		talent_info2.m_nID = 2;
		talent_info2.m_nSkillType = ST_MAGIC;
		talent_info2.m_EffectInfo.m_nPoint = CSEffectInfo::POINT_TARGET;
		talent_info2.m_EffectInfo.m_nRelation = CSEffectInfo::RELATION_ALL;
		talent_info2.m_fHitDelay = 10.0f;

		pNewHitter2->Setup(m_pEntityNPC->GetUID(), m_pEntityPlayer->GetUID(), 0, 0, &talent_info2, talent_info2.m_fHitDelay, TalentEffector);

		// 판정 체크
		CHECK_EQUAL(pNewHitter2->Update(1.0f, m_pField), false);
		CHECK_EQUAL(pNewHitter2->Update(10.0f, m_pField), true);
	}

	TEST(NPCMeshHitInitTest)
	{
		GNPCInfoMgr NPCInfoMgr;

		GNPCInfo* pNewNPCInfo = new GNPCInfo();
		pNewNPCInfo->nID = 1;

		const int nTalentID = 10;
		pNewNPCInfo->m_Talents.push_back(nTalentID);

		NPCInfoMgr.insert(GNPCInfoMgr::value_type(pNewNPCInfo->nID, pNewNPCInfo));

		GNPCInfo* pAnyInfo = NPCInfoMgr.Find(1);
		CHECK(pAnyInfo != NULL);

//		if (!pAnyInfo->m_HitInfo.m_bSimpleCapsule)
		{
			//		CSMeshHitInfoSegment* pHitInfoSeg = pAnyInfo->m_HitInfo.GetMeshHitInfoSegment(nTalentID);
			//		CHECK(pHitInfoSeg != NULL);

		}

		NPCInfoMgr.Clear();
	}

	TEST(TestNPCHittable)
	{
		MUID uidField = SUnitTestUtil::NewUID();
		MockField* pField = new MockField(uidField);
		pField->Create();

		GNPCInfo flying_npc_info;
		flying_npc_info.nID = 1;
		flying_npc_info.nAltitude = AT_SKY;

		GTalentInfo attack_talent_info;
		attack_talent_info.m_nID = 1;
		attack_talent_info.m_EffectInfo.m_nRelation = CSEffectInfo::RELATION_ENEMY;

		vec3 pos = vec3(1000.0f, 1000.0f, 0.0f);
		vec3 dir = vec3(0.0f,1.0f,0.0f);

		GEntityNPC* pNewNPC = pField->SpawnTestNPC(&flying_npc_info, pos, dir);
		if (pNewNPC == NULL) 
		{
			pField->Destroy();
			SAFE_DELETE(pField);
			return;
		}


		GEntityPlayer* pAttackerPlayer = GUTHelper::NewEntityPlayer();

		vec3 player_pos = pos;
		pAttackerPlayer->EnterToField(pField, player_pos, dir);
		
		flying_npc_info.nAltitude = AT_SKY;
		attack_talent_info.m_nSkillType = ST_MELEE;
		CHECK(pNewNPC->IsHittable(pAttackerPlayer, &attack_talent_info) == false);


		flying_npc_info.nAltitude = AT_NORMAL;
		attack_talent_info.m_nSkillType = ST_EXTRA_ACTIVE;
		CHECK(pNewNPC->IsHittable(pAttackerPlayer, &attack_talent_info) == true);


		pField->Destroy();
		SAFE_DELETE(pField);
	}

	TEST(TestNPCHittable_ChangeMode)
	{
		MUID uidField = SUnitTestUtil::NewUID();
		MockField* pField = new MockField(uidField);
		pField->Create();

		GNPCInfo flying_npc_info;
		flying_npc_info.nID = 1;
		flying_npc_info.nAltitude = AT_NORMAL;

		// 날 수 있는 NPC 모드 추가
		GNPCInfo* flying_npc_info_mode1 = new GNPCInfo();
		flying_npc_info_mode1->nID = 1;
		flying_npc_info_mode1->nAltitude = AT_SKY;

		flying_npc_info.vecModes.push_back(flying_npc_info_mode1);


		GTalentInfo attack_talent_info;
		attack_talent_info.m_nID = 1;
		attack_talent_info.m_EffectInfo.m_nRelation = CSEffectInfo::RELATION_ENEMY;
		attack_talent_info.m_nSkillType = ST_MELEE;

		vec3 pos = vec3(1000.0f, 1000.0f, 0.0f);
		vec3 dir = vec3(0.0f,1.0f,0.0f);

		GEntityNPC* pNewNPC = pField->SpawnTestNPC(&flying_npc_info, pos, dir);
		if (pNewNPC == NULL) 
		{
			pField->Destroy();
			SAFE_DELETE(pField);
			return;
		}


		GEntityPlayer* pAttackerPlayer = GUTHelper::NewEntityPlayer();

		vec3 player_pos = pos;
		pAttackerPlayer->EnterToField(pField, player_pos, dir);

		CHECK(pNewNPC->IsHittable(pAttackerPlayer, &attack_talent_info) == true);


		pNewNPC->ChangeMode(NPC_MODE_1);
		CHECK(pNewNPC->IsHittable(pAttackerPlayer, &attack_talent_info) == false);


		pField->Destroy();
		SAFE_DELETE(pField);
	}

	TEST(TestPCHittable)
	{
		MUID uidField = SUnitTestUtil::NewUID();
		MockField* pField = new MockField(uidField);
		pField->Create();

		GTalentInfo attack_talent_info;
		attack_talent_info.m_nID = 1;

		vec3 pos = vec3(1000.0f, 1000.0f, 0.0f);
		vec3 dir = vec3(0.0f,1.0f,0.0f);

		GEntityPlayer* pAttackerPlayer = GUTHelper::NewEntityPlayer();

		vec3 player_pos = pos;
		pAttackerPlayer->EnterToField(pField, player_pos, dir);

		GEntityPlayer* pVictimPlayer = GUTHelper::NewEntityPlayer();

		pVictimPlayer->EnterToField(pField, player_pos, dir);

		bool last_pvp_mode = GConst::PVP_MODE;

		{
			GConst::PVP_MODE = false;

			attack_talent_info.m_nSkillType = ST_MELEE;
			attack_talent_info.m_EffectInfo.m_nRelation = CSEffectInfo::RELATION_ENEMY;
			CHECK(pVictimPlayer->IsHittable(pAttackerPlayer, &attack_talent_info) == false);

			attack_talent_info.m_nSkillType = ST_MELEE;
			attack_talent_info.m_EffectInfo.m_nRelation = CSEffectInfo::RELATION_ALL;
			CHECK(pVictimPlayer->IsHittable(pAttackerPlayer, &attack_talent_info) == true);

			attack_talent_info.m_nSkillType = ST_MELEE;
			attack_talent_info.m_EffectInfo.m_nRelation = CSEffectInfo::RELATION_ALLIED;
			CHECK(pVictimPlayer->IsHittable(pAttackerPlayer, &attack_talent_info) == true);

			GConst::PVP_MODE = last_pvp_mode;
		}


		pField->Destroy();
		SAFE_DELETE(pField);
	}

}
