#include "stdafx.h"
#include "GTalentInfo.h"
#include "MockField.h"
#include "GEntityPlayer.h"
#include "GEntityNPC.h"
#include "SUnitTestUtil.h"
#include "GNPCInfo.h"
#include "GNPCInfoMgr.h"
#include "GGlobal.h"
#include "GTestWrapper.h"
#include "GUTHelper.h"
#include "GModuleAI.h"
#include "GTalentInfoMgr.h"
#include "GBehaviorStateMachine.h"
#include "GHateTable.h"
#include "GUTHelper_Field.h"

SUITE(AI)
{
	TEST(NonMonsterDontAddAggroPoint)
	{
		GTestMgrWrapper<GTalentInfoMgr>	m_TalentInfoMgrWrapper;
		GTestMgrWrapper<GNPCInfoMgr>	m_NPCInfoMgrWrapper;

		GUTHelper helper;

		GTalentInfo* pTalentInfo = helper.NewMeleeTalentInfo(1);
		pTalentInfo->m_EffectInfo.m_nRelation = CSEffectInfo::RELATION_ENEMY;

		helper.AddSimpleHitInfo(pTalentInfo);
		helper.SetTalentDamage(pTalentInfo, 10);

		GNPCInfo* pNotAttackableNPCInfo = helper.NewNPCInfo(1);
		pNotAttackableNPCInfo->nMaxHP = 100;
		pNotAttackableNPCInfo->nInitAttackable = NAT_NONE;
		
		MockField* pField = GUTHelper_Field::DefaultMockField();
		vec3 pos = vec3(1000.0f, 1000.0f, 0.0f);
		GEntityPlayer* pAttackerPlayer = GUTHelper::NewEntityPlayer(pField, pos);

		GEntityNPC* pVictimNPC = pField->SpawnTestNPC(pNotAttackableNPCInfo, pos, vec3(0,1,0));
		if (pVictimNPC == NULL) return;


		CHECK_EQUAL(pVictimNPC->GetHP(), 100);

		pAttackerPlayer->doUseTalent(1);
		pField->Update(0.5f);

		CHECK_EQUAL(pVictimNPC->GetHP(), 100);		
		GBehaviorStateMachine* pFSM = pVictimNPC->GetBehaviorFSM();
		CHECK(!pFSM->IsNowCombat());

		const GHateTable& hate_table = pVictimNPC->GetHateTable();
		CHECK(hate_table.IsEmpty());

		CHECK(hate_table.GetTarget().IsInvalid() == true);

		CHECK(pAttackerPlayer->IsNowCombat() == false);

		pField->Destroy();
	}

} // SUITE(AI)
