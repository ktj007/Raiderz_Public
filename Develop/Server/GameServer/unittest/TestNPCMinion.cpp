#include "stdafx.h"
#include "GNPCInfo.h"
#include "MockField.h"
#include "GTestForward.h"
#include "GNPCMinion.h"
#include "GModuleAI.h"
#include "GMessageID.h"
#include "GPartySystem.h"
#include "GPartyManager.h"
#include "GHateTable.h"

SUITE(Minion)
{
	struct FMinion
	{
		FMinion()
		{
			m_pField = GUTHelper_Field::DefaultMockField();
			m_pNPC = GUTHelper_NPC::SpawnNPC(m_pField);			
			GUTHelper_NPC::NewNPCInfo(111);	// 111 NPC정보 만듬
		}
		~FMinion()
		{
		}

		GEntityNPC* m_pNPC;
		MockField*	m_pField;
		DECLWRAPPER_NPCInfoMgr;
		GTestSysWrapper<GPartySystem>		m_PartySystemWrapper;
	};
	TEST_FIXTURE(FMinion, Summon)
	{
		CHECK_EQUAL(0, m_pNPC->GetNPCMinion().GetMinionCount());
		CHECK_EQUAL(0, m_pField->GetNPCQty(111));
		m_pNPC->GetNPCMinion().Summon(111, vec3(0,0,0));
		CHECK_EQUAL(1, m_pNPC->GetNPCMinion().GetMinionCount());
		CHECK_EQUAL(1, m_pField->GetNPCQty(111));
		m_pNPC->GetNPCMinion().Summon(111, vec3(0,0,0));
		CHECK_EQUAL(2, m_pNPC->GetNPCMinion().GetMinionCount());
		CHECK_EQUAL(2, m_pField->GetNPCQty(111));
		m_pNPC->GetNPCMinion().Summon(111, vec3(0,0,0));
		CHECK_EQUAL(3, m_pNPC->GetNPCMinion().GetMinionCount());
		CHECK_EQUAL(3, m_pField->GetNPCQty(111));
	}

	TEST_FIXTURE(FMinion, UnsummonWhenMasterDie)
	{
		m_pNPC->GetNPCMinion().Summon(111, vec3(0,0,0));
		m_pNPC->GetNPCMinion().Summon(111, vec3(0,0,0));
		m_pNPC->GetNPCMinion().Summon(111, vec3(0,0,0));
		CHECK_EQUAL(3, m_pNPC->GetNPCMinion().GetMinionCount());
		CHECK_EQUAL(3, m_pField->GetNPCQty(111));
		m_pNPC->doDie();
		CHECK_EQUAL(0, m_pNPC->GetNPCMinion().GetMinionCount());
		CHECK_EQUAL(0, m_pField->GetNPCQty(111));
	}

	TEST_FIXTURE(FMinion, MinionDie)
	{
		GEntityNPC* pSpawnNPC1 = m_pNPC->GetNPCMinion().Summon(111, vec3(0,0,0));
		GEntityNPC* pSpawnNPC2 = m_pNPC->GetNPCMinion().Summon(111, vec3(0,0,0));
		GEntityNPC* pSpawnNPC3 = m_pNPC->GetNPCMinion().Summon(111, vec3(0,0,0));
		CHECK_EQUAL(3, m_pField->GetNPCQty(111));
		CHECK_EQUAL(3, m_pNPC->GetNPCMinion().GetMinionCount());
		pSpawnNPC1->doDie();
		CHECK_EQUAL(2, m_pField->GetNPCQty(111));
		CHECK_EQUAL(2, m_pNPC->GetNPCMinion().GetMinionCount());
		pSpawnNPC2->doDie();
		CHECK_EQUAL(1, m_pField->GetNPCQty(111));
		CHECK_EQUAL(1, m_pNPC->GetNPCMinion().GetMinionCount());
		pSpawnNPC3->doDie();
		CHECK_EQUAL(0, m_pField->GetNPCQty(111));
		CHECK_EQUAL(0, m_pNPC->GetNPCMinion().GetMinionCount());
	}

	TEST_FIXTURE(FMinion, UnsummonWhenEndCombat)
	{
		m_pNPC->GetHateTable().EnableTimeout(true);
		m_pNPC->GetNPCMinion().Summon(111, vec3(0,0,0));
		m_pNPC->GetNPCMinion().Summon(111, vec3(0,0,0));
		m_pNPC->GetNPCMinion().Summon(111, vec3(0,0,0));
		CHECK_EQUAL(3, m_pNPC->GetNPCMinion().GetMinionCount());
		CHECK_EQUAL(3, m_pField->GetNPCQty(111));

		// 전투를 시작하고 종료
		GEntityPlayer* pEnemy = test.player.NewPlayer(m_pField);
		m_pNPC->doAttack(pEnemy->GetUID());
		pEnemy->doDie();
		m_pNPC->GetHateTable().Update(GConst::HATETABLE_ENTRY_TIMEOUT);
		m_pNPC->UpdateAI(0.0f);
		m_pField->Update(0.0f);
		CHECK_EQUAL(0, m_pNPC->GetNPCMinion().GetMinionCount());
		CHECK_EQUAL(0, m_pField->GetNPCQty(111));
	}

	TEST_FIXTURE(FMinion, MinionShareMasterSingleTarget)
	{
		GModuleAI* pmAI = m_pNPC->GetModuleAI();
		GEntityPlayer* pEnemy = test.player.NewPlayer(m_pField);
		m_pNPC->doAttack(pEnemy->GetUID());

		GEntityNPC* pSpawnNPC1 = m_pNPC->GetNPCMinion().Summon(111, vec3(0,0,0));

		TVALID(pSpawnNPC1->GetTargetActor());
		CHECK_EQUAL(pEnemy->GetUID(), pSpawnNPC1->GetTargetActor()->GetUID());
	}

	TEST_FIXTURE(FMinion, MinionShareMasterPartyTarget)
	{
		GModuleAI* pmAI = m_pNPC->GetModuleAI();
		GEntityPlayer* pEnemy1 = test.player.NewPlayer(m_pField);
		GEntityPlayer* pEnemy2 = test.player.NewPlayer(m_pField);
		
		GUTHelper_Party::CreateParty(pEnemy1, pEnemy2);

		m_pNPC->doAttack(pEnemy1->GetUID());
		// 2번 적이 더 가까이 있음
		pEnemy1->SetPos(vec3(100,0,0));
		pEnemy2->SetPos(vec3(0,0,0));	
		GEntityNPC* pSpawnNPC1 = m_pNPC->GetNPCMinion().Summon(111, vec3(0,0,0));
		TVALID(pSpawnNPC1->GetTargetActor());
		CHECK_EQUAL(pEnemy2->GetUID(), pSpawnNPC1->GetTargetActor()->GetUID());

		// 1번 적이 더 가까이 있음
		pEnemy1->SetPos(vec3(0,0,0));
		pEnemy2->SetPos(vec3(100,0,0));	
		GEntityNPC* pSpawnNPC2 = m_pNPC->GetNPCMinion().Summon(111, vec3(0,0,0));
		TVALID(pSpawnNPC2->GetTargetActor());
		CHECK_EQUAL(pEnemy1->GetUID(), pSpawnNPC2->GetTargetActor()->GetUID());
	}
}
