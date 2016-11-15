#include "stdafx.h"
#include "GUTHelper.h"
#include "MockField.h"
#include "GMessageID.h"
#include "GModuleAI.h"
#include "GJob_Broker.h"
#include "GJob_Talent.h"
#include "GTestForward.h"
#include "GJobMgr.h"
#include "GBehaviorStateMachine.h"
#include "GBehaviorState.h"
#include "GHateTable.h"
#include "FBaseMockLink.h"

SUITE(CombatCombat)
{
	struct FCombatCombat : public FBaseMockLink
	{
		FCombatCombat()
		{
			m_pField = GUTHelper_Field::DefaultMockField();
			m_pNPC = m_Helper.NewEntityNPC(m_pField);
			m_pPlayer = GUTHelper::NewEntityPlayer(m_pField);
		}

		virtual ~FCombatCombat()
		{
			m_pField->Destroy();
		}

		GUTHelper		m_Helper;
		MockField*		m_pField;
		GEntityNPC*		m_pNPC;
		GEntityPlayer*	m_pPlayer;
	};

	// job이 다 끝날때까지, combatcombat 스테이트에서 다른 스테이트로 바뀌지 않는다.
	// ex : NPC가 PC를 탤런트로 공격하던중 PC가 죽어도 사용하던 탤런트를 마저 다 사용하고 승리포즈를 취한다.
	TEST_FIXTURE(FCombatCombat, NoChangeStateWhenExistJob)
	{
		GTalentInfo* pTalentInfo = m_Helper.NewTalentInfo();
		pTalentInfo->m_nSkillType = ST_MELEE;
		MUID uidEnemy = m_pPlayer->GetUID();
		GMessage message(GMSG_AI_EVT_STARTCOMBAT, &uidEnemy);

		// 전투 시작
		m_pNPC->GetModuleAI()->Message(message);
		GBehaviorStateMachine* pFSM = m_pNPC->GetBehaviorFSM();
		CHECK_EQUAL(AI_COMBAT_COMBAT, pFSM->GetCurrentCombatStateID());

		// job이 다 끝나지 않아서 combatcomabt 스테이트 유지
		GJob* pJob =  new GJob_UseTalent(m_pNPC->GetModuleAI(), pTalentInfo, m_pNPC->GetUID());

		GJobMgr* pJobMgr = m_pNPC->GetJobMgr();		

		pJobMgr->Push(pJob);
		m_pNPC->GetModuleAI()->Update(0.0f);

		// 전투 종료
		m_pNPC->GetHateTable().Reset();

		// job이 다 끝나지 않아서 combatcomabt 스테이트 유지
		m_pNPC->GetModuleAI()->Update(0.1f);		
		CHECK_EQUAL(AI_COMBAT_COMBAT, pFSM->GetCurrentCombatStateID());

		// job이 다 끝나서 스테이트 변경
		pJobMgr->Clear();
		m_pNPC->GetModuleAI()->Update(0.0f);
		m_pNPC->GetModuleAI()->Update(0.0f);
		CHECK_EQUAL(AI_COMBAT_VICTORY, pFSM->GetCurrentCombatStateID());
	}

	TEST_FIXTURE(FCombatCombat, MissWhenAttackInvincibleTarget)
	{
		GTalentInfo* pTalentInfo = test.talent.MakeDamageTalent(1);
		pTalentInfo->m_nSkillType = ST_MELEE;
		pTalentInfo->m_fDurationTime = 10.0f;
		pTalentInfo->m_bUseInvincibleTime = true;
		pTalentInfo->m_fInvincibleTimeStart = 0.0f;
		pTalentInfo->m_fInvincibleTimeEnd = 5.0f;	// 5초동안 무적
		
		test.talent.UseTalent(m_pNPC, pTalentInfo, m_pPlayer->GetUID());	// 탤런트 사용해서 무적이 됨 (5초)

		MockLink* m_pLinkTester = test.network.NewLink(m_pPlayer);
		m_pLinkTester->AddIgnoreID(MC_ACTION_USE_TALENT);
		m_pLinkTester->AddIgnoreID(MC_NPCINTERACTION_ICON);
		m_pLinkTester->AddIgnoreID(MC_CHAR_ENEMY_INFO);
		m_pLinkTester->AddIgnoreID(MC_CHAR_ADD_ENEMYNPC);
				
		test.talent.UseTalent(m_pPlayer, pTalentInfo, m_pNPC->GetUID());
		CHECK_EQUAL(m_pNPC->GetHP(), m_pNPC->GetHP());	// 피해 안받음

		// 패킷 체크
		const minet::MCommand& Command = m_pLinkTester->GetCommand(0);
		CHECK_EQUAL(Command.GetID(), MC_ACTION_TALENT_HIT);
		if (Command.GetParameterCount() < 1)						 { CHECK(false); return; }
		MCommandParameter* pParam = Command.GetParameter(0);
		if(pParam->GetType()!=MPT_SINGLE_BLOB)								{ CHECK(false); return; }
		TD_TALENT_HIT* pTD = (TD_TALENT_HIT*)pParam->GetPointer();
		CHECK(CheckBitSet(pTD->nFlags,CTR_IMMUNE));

		test.network.ClearLinks();
	}

	TEST_FIXTURE(FCombatCombat, CheckCombatRadius)
	{
		GTalentInfo* pTalentInfo = m_Helper.NewTalentInfo();
		pTalentInfo->m_nSkillType = ST_MELEE;
		MUID uidEnemy = m_pPlayer->GetUID();
		GMessage message(GMSG_AI_EVT_STARTCOMBAT, &uidEnemy);

		// 전투 시작
		m_pNPC->GetModuleAI()->Message(message);
		GBehaviorStateMachine* pFSM = m_pNPC->GetBehaviorFSM();
		CHECK_EQUAL(AI_BEHAVIOR_COMBAT, pFSM->GetCurrentStateID());

		// 전투 범위 넘어섬
		CHECK_EQUAL(GConst::NPC_DEFAULT_COMBAT_RADIUS, m_pNPC->GetCombatRadius());
		m_pNPC->SetPos(m_pNPC->GetPos() + vec3(GConst::NPC_DEFAULT_COMBAT_RADIUS+1, 0, 0));
		pFSM->Update(0.0f);

		// 전투 종료 확인
		CHECK_EQUAL(AI_BEHAVIOR_RETURN, pFSM->GetCurrentStateID());
	}

	TEST_FIXTURE(FCombatCombat, InvincibleWhenReturnState)
	{
		GTalentInfo* pTalentInfo = m_Helper.NewTalentInfo();
		pTalentInfo->m_nSkillType = ST_MELEE;
		MUID uidEnemy = m_pPlayer->GetUID();
		GMessage message(GMSG_AI_EVT_STARTCOMBAT, &uidEnemy);

		// 전투 시작
		m_pNPC->GetModuleAI()->Message(message);
		GBehaviorStateMachine* pFSM = m_pNPC->GetBehaviorFSM();
		CHECK_EQUAL(AI_BEHAVIOR_COMBAT, pFSM->GetCurrentStateID());

		// 전투 종료 확인
		CHECK_EQUAL(false, m_pNPC->IsNowInvincibility());
		m_pNPC->SetPos(m_pNPC->GetPos() + vec3(GConst::NPC_DEFAULT_COMBAT_RADIUS+1, 0, 0));
		pFSM->Update(0.0f);
		CHECK_EQUAL(AI_BEHAVIOR_RETURN, pFSM->GetCurrentStateID());
		CHECK_EQUAL(true, m_pNPC->IsNowInvincibility());
		GJobMgr* pJobMgr = m_pNPC->GetJobMgr();		
		pJobMgr->Clear();
		m_pNPC->GetModuleAI()->Update(0.0f);
		m_pNPC->GetModuleAI()->Update(0.0f);
		CHECK_EQUAL(AI_BEHAVIOR_PEACE, pFSM->GetCurrentStateID());
		CHECK_EQUAL(false, m_pNPC->IsNowInvincibility());

	}

}