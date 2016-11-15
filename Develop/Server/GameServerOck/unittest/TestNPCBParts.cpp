#include "stdafx.h"
#include "GNPCInfo.h"
#include "MockField.h"
#include "GTestForward.h"
#include "GNPCBParts.h"
#include "GHateTable.h"

SUITE(BParts)
{
	struct FMinion
	{
		FMinion()
		{
			m_pField = GUTHelper_Field::DefaultMockField();
			m_pNPC = GUTHelper_NPC::SpawnNPC(m_pField, 111);			
		}
		~FMinion()
		{
		}

		GEntityNPC* m_pNPC;
		MockField*	m_pField;
		DECLWRAPPER_NPCInfoMgr;
	};

	TEST_FIXTURE(FMinion, Recovery)
	{
		CHECK_EQUAL(false, m_pNPC->GetNPCBParts()->IsBroken(1));
		m_pNPC->GetNPCBParts()->SetBreak(1);
		CHECK_EQUAL(true, m_pNPC->GetNPCBParts()->IsBroken(1));
		m_pNPC->GetNPCBParts()->Recovery();
		CHECK_EQUAL(false, m_pNPC->GetNPCBParts()->IsBroken(1));
	}

	TEST_FIXTURE(FMinion, Recovery_viaUpdate)
	{
		CHECK_EQUAL(false, m_pNPC->GetNPCBParts()->IsBroken(1));
		m_pNPC->GetNPCBParts()->SetBreak(1);
		CHECK_EQUAL(true, m_pNPC->GetNPCBParts()->IsBroken(1));
		// 피해입음
		m_pNPC->SetHP(1);
		// 전투중
		GEntityNPC* pEnemy = GUTHelper_NPC::SpawnNPC(m_pField);
		m_pNPC->doAttack(pEnemy->GetUID());
		CHECK_EQUAL(true, m_pNPC->IsNowCombat());
		
		// 변경없이 업데이트
		m_pNPC->Update(0.0f);
		CHECK_EQUAL(true, m_pNPC->GetNPCBParts()->IsBroken(1));
		// 전투해제
		m_pNPC->doMakePeace();
		CHECK_EQUAL(true, m_pNPC->IsNowCombat());
		m_pNPC->Update(0.0f);
		CHECK_EQUAL(true, m_pNPC->GetNPCBParts()->IsBroken(1));
		// 피해회복
		m_pNPC->SetHP(m_pNPC->GetMaxHP());
		m_pNPC->Update(0.0f);
		CHECK_EQUAL(true, m_pNPC->GetNPCBParts()->IsBroken(1));
	}

	TEST_FIXTURE(FMinion, SpawnCorpseWhenBreakBPart)
	{
		GNPCInfo infoNPC;
		BPART_REWARD_INFO infoBPartreward;
		infoBPartreward.nLootID = 777;
		vector<BPART_REWARD_INFO> vecBPartInfos;
		vecBPartInfos.push_back(infoBPartreward);
		infoNPC.m_mapBPartRewards.insert(make_pair(1, vecBPartInfos));
		infoNPC.nID = 111;
		m_pNPC->SetNPCInfo(&infoNPC);

		CHECK_EQUAL(1, m_pField->GetEntityQty(ETID_NPC)); // 전투중인 NPC
		GEntityPlayer* pPlayer = test.player.NewPlayer(m_pField);
		m_pNPC->GetNPCBParts()->Break(1, pPlayer->GetUID());
		CHECK_EQUAL(2, m_pField->GetEntityQty(ETID_NPC)); // 전투중인 NPC + 파괴된 파츠 NPC
	}
}
