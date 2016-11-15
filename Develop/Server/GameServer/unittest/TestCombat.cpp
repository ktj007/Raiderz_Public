#include "stdafx.h"
#include "GColtPicker.h"
#include "GNPCInfo.h"
#include "GTestWrapper.h"
#include "SUnitTestUtil.h"
#include "GUTHelper.h"
#include "GEntityNPC.h"
#include "MockField.h"
#include "GTestForward.h"
#include "GModuleAI.h"
#include "GModuleCombat.h"

SUITE(Combat)
{
	struct FCombat
	{
		FCombat()
		{
			m_pField = GUTHelper_Field::DefaultMockField();
			m_pPlayer = test.player.NewPlayer(m_pField);
			m_pNPC = GUTHelper_NPC::SpawnNPC(m_pField);
		}
		~FCombat()
		{
			m_pField->Destroy();
		}

		void MakeCombat(GEntityActor* pAttacker, GEntityActor* pVictim)
		{
			GTalentInfo* pAttackTalent = test.talent.MakeDamageTalent(10);
			test.talent.UseTalent(pAttacker, pAttackTalent, pVictim->GetUID());
		}

		MockField*		m_pField;
		GEntityPlayer*	m_pPlayer;
		GEntityNPC*		m_pNPC;
		
		DECLWRAPPER_FieldInfoMgr;
		DECLWRAPPER_FieldMgr;
		DECLWRAPPER_BuffMgr;
		DECLWRAPPER_TalentMgr;
	};

	TEST_FIXTURE(FCombat, MakePeaceForPlayer)
	{
		// PC가 숨은 경우
		MakeCombat(m_pPlayer, m_pNPC);
		CHECK_EQUAL(true, m_pPlayer->IsNowCombat());
		CHECK_EQUAL(true, m_pNPC->IsNowCombat());
		m_pPlayer->doMakePeace();
		m_pNPC->GetModuleAI()->Update(0.0f); // FSM 변경에 필요한 틱
		m_pNPC->GetModuleAI()->Update(GConst::VICTORY_DEFAULT_DELAY); // 승리포즈 대기에 필요한 틱
		m_pNPC->GetModuleAI()->Update(0.0f); // FSM 변경에 필요한 틱
		m_pNPC->ClearJob();
		m_pNPC->GetModuleAI()->Update(0.0f); // FSM 변경에 필요한 틱
		CHECK_EQUAL(false, m_pPlayer->IsNowCombat());
		CHECK_EQUAL(false, m_pNPC->IsNowCombat());

		// NPC가 숨은 경우
		MakeCombat(m_pPlayer, m_pNPC);
		CHECK_EQUAL(true, m_pPlayer->IsNowCombat());
		CHECK_EQUAL(true, m_pNPC->IsNowCombat());
		m_pNPC->doMakePeace();
		m_pNPC->GetModuleAI()->Update(0.0f); // FSM 변경에 필요한 틱
		m_pNPC->GetModuleAI()->Update(GConst::VICTORY_DEFAULT_DELAY); // 승리포즈 대기에 필요한 틱
		m_pNPC->GetModuleAI()->Update(0.0f); // FSM 변경에 필요한 틱
		m_pNPC->ClearJob();
		m_pNPC->GetModuleAI()->Update(0.0f); // FSM 변경에 필요한 틱
		CHECK_EQUAL(false, m_pPlayer->IsNowCombat());
		CHECK_EQUAL(false, m_pNPC->IsNowCombat());
	}

	TEST_FIXTURE(FCombat, MakePeaceForNPC)
	{
		MakeCombat(m_pPlayer, m_pNPC);
		CHECK_EQUAL(true, m_pPlayer->IsNowCombat());
		CHECK_EQUAL(true, m_pNPC->IsNowCombat());
		m_pNPC->doMakePeace();
		m_pNPC->GetModuleAI()->Update(0.0f); // FSM 변경에 필요한 틱
		m_pNPC->GetModuleAI()->Update(GConst::VICTORY_DEFAULT_DELAY); // 승리포즈 대기에 필요한 틱
		m_pNPC->GetModuleAI()->Update(0.0f); // FSM 변경에 필요한 틱
		CHECK_EQUAL(false, m_pPlayer->IsNowCombat());
		CHECK_EQUAL(false, m_pNPC->IsNowCombat());
	}

	TEST_FIXTURE(FCombat, GetSightRange)
	{
		struct Case
		{
			const int nLevelFactor;
			const int nSightRange;
		} cases[] = {
			{10, 850}
			,{9, 880}
			,{8, 910}
			,{7, 940}
			,{6, 970}
			,{5, 1000}
			,{4, 1030}
			,{3, 1060}
			,{2, 1090}
			,{1, 1120}
			,{0, 1150}
		};

		m_pNPC->GetNPCInfo()->nSightRange = 1000;
		m_pPlayer->SetLevel(10);

		for (int i=0; i<ArraySize(cases); ++i) 
		{
			MakeLevelFactor(cases[i].nLevelFactor, m_pNPC, m_pPlayer);
			CHECK_EQUAL(cases[i].nSightRange, m_pNPC->GetSightRange(m_pPlayer->GetLevel()));
		}
	}

	TEST_FIXTURE(FCombat, AvoidTime)
	{
	 	GTalentInfo* pActionTalent = test.talent.NewMagicTalentInfo(); 
		pActionTalent->m_fDurationTime = 10.0f;
		pActionTalent->m_bUseAvoidTime = true;
		pActionTalent->m_fAvoidTimeStart = 5.0f;
		pActionTalent->m_fAvoidTimeEnd = 8.0f;
		pActionTalent->Cooking();
		CHECK_EQUAL(false, m_pPlayer->IsNowAvoidTime());
		test.talent.UseTalent(m_pPlayer, pActionTalent, m_pPlayer->GetUID());
		CHECK_EQUAL(false, m_pPlayer->IsNowAvoidTime());
		m_pPlayer->GetModuleCombat()->Update(5.0f);
		CHECK_EQUAL(true, m_pPlayer->IsNowAvoidTime()); // 5~8초간 유효
		m_pPlayer->GetModuleCombat()->Update(9.0f);
		CHECK_EQUAL(false, m_pPlayer->IsNowAvoidTime());
	}

	TEST_FIXTURE(FCombat, Avoidable)
	{
		GTalentInfo* pActionTalent = test.talent.NewMagicTalentInfo(); 
		pActionTalent->m_fDurationTime = 10.0f;
		pActionTalent->m_bUseAvoidTime = true;
		pActionTalent->m_fAvoidTimeStart = 5.0f;
		pActionTalent->m_fAvoidTimeEnd = 8.0f;
		pActionTalent->Cooking();
		test.talent.UseTalent(m_pPlayer, pActionTalent, m_pPlayer->GetUID());
		m_pPlayer->GetModuleCombat()->Update(5.0f);
		CHECK_EQUAL(true, m_pPlayer->IsNowAvoidTime()); // 5~8초간 유효

		GTalentInfo* pAttackTalent = test.talent.MakeDamageTalent(10);
		pActionTalent->m_bAvoidable = true;
		m_pPlayer->SetHP(100);
		test.talent.UseTalent(m_pNPC, pActionTalent, m_pPlayer->GetUID());
		CHECK_EQUAL(100, m_pPlayer->GetHP());

		pAttackTalent->m_bAvoidable = false;
		test.talent.UseTalent(m_pNPC, pAttackTalent, m_pPlayer->GetUID());
		CHECK_EQUAL(90, m_pPlayer->GetHP());
	}
}