#include "stdafx.h"
#include "GTestForward.h"
#include "FBaseGame.h"
#include "GDropSouls.h"
#include "GEntityNPC.h"
#include "GNPCLoot.h"
#include "GDropList.h"
#include "GLootInfo.h"
#include "GNPCBParts.h"

SUITE(DropSouls)
{
	struct Fixture : public FBaseGame
	{
		Fixture()
		{
			m_pNPC = test.npc.SpawnNPC(m_pField);			
		}

		GDropSouls& GetDropSouls()
		{
			return m_pNPC->GetNPCLoot().GetDropList().GetDropSouls();
		}

		GUTHelper		m_Helper;
		GEntityNPC*		m_pNPC;	
	};


	// 최대, 최소 드랍량
	TEST_FIXTURE(Fixture, MinMax)
	{
		GLootInfo* pLootInfo = test.loot.NewLootInfo();
		LOOT_SOUL_INFO* pLootSoulInfo = test.loot.NewLootSoulInfo(pLootInfo, SOULT_A, 100, 1, 10);
		m_pNPC->GetNPCInfo()->m_pLootInfo = pLootInfo;

		GetDropSouls().Drop(m_pPlayer->GetCID(), pLootInfo);

		int nQuantity = GetDropSouls().GetQuantity(m_pPlayer->GetCID(), pLootSoulInfo->m_nType);

		CHECK(pLootSoulInfo->m_nMin <= nQuantity);
		CHECK(pLootSoulInfo->m_nMax >= nQuantity);
	}

	// 공헌자만 권한을 갖는다
	TEST_FIXTURE(Fixture, Authority)
	{
		GEntityPlayer* pPlayer2 = test.player.NewPlayer(m_pField);

		GLootInfo* pLootInfo = test.loot.NewLootInfo();
		LOOT_SOUL_INFO* pLootSoulInfo = test.loot.NewLootSoulInfo(pLootInfo, SOULT_A, 100, 1, 10);
		m_pNPC->GetNPCInfo()->m_pLootInfo = pLootInfo;

		GetDropSouls().Drop(m_pPlayer->GetCID(), pLootInfo);

		CHECK_EQUAL(true, GetDropSouls().IsAuthorizedCID(m_pPlayer->GetCID()));
		CHECK_EQUAL(false, GetDropSouls().IsAuthorizedCID(pPlayer2->GetCID()));
	}

	// 브레이크 파트에 따른 드랍 확률
	TEST_FIXTURE(Fixture, BreakPartRate)
	{
		GLootInfo* pLootInfo = test.loot.NewLootInfo();
		LOOT_SOUL_INFO* pLootSoulInfo = test.loot.NewLootSoulInfo(pLootInfo, SOULT_A, 0);
		pLootSoulInfo->m_fBPart1Rate = 25.0f;
		pLootSoulInfo->m_fBPart2Rate = 25.0f;
		pLootSoulInfo->m_fBPart3Rate = 25.0f;
		pLootSoulInfo->m_fBPart4Rate = 25.0f;
		m_pNPC->GetNPCInfo()->m_pLootInfo = pLootInfo;

		// 부위 파괴가 되지 않아서 확률 0%
		GetDropSouls().Drop(m_pPlayer->GetCID(), pLootInfo);
		CHECK_EQUAL(0, GetDropSouls().GetQuantity(m_pPlayer->GetCID(), pLootSoulInfo->m_nType));

		GetDropSouls().Clear();

		// 부위 파괴가 4부분이 되어서 확률 100(25*4)%
		m_pNPC->GetNPCBParts()->Break(1, m_pPlayer->GetUID());
		m_pNPC->GetNPCBParts()->Break(2, m_pPlayer->GetUID());
		m_pNPC->GetNPCBParts()->Break(3, m_pPlayer->GetUID());
		m_pNPC->GetNPCBParts()->Break(4, m_pPlayer->GetUID());
		GetDropSouls().Drop(m_pPlayer->GetCID(), pLootInfo);
		CHECK(0 < GetDropSouls().GetQuantity(m_pPlayer->GetCID(), pLootSoulInfo->m_nType));
	}
}