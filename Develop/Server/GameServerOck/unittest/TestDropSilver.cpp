#include "stdafx.h"
#include "GTestForward.h"
#include "FBaseGame.h"
#include "GDropSilver.h"
#include "GEntityNPC.h"
#include "GNPCLoot.h"
#include "GDropList.h"
#include "GLootInfo.h"

SUITE(DropMoney)
{
	struct Fixture : public FBaseGame
	{
		Fixture()
		{
			m_pNPC = test.npc.SpawnNPC(m_pField);			
		}

		GDropMoney& GetDropMoney()
		{
			return m_pNPC->GetNPCLoot().GetDropList().GetDropMoney();
		}

		GUTHelper		m_Helper;
		GEntityNPC*		m_pNPC;	
	};


	// 최대, 최소 드랍량
	TEST_FIXTURE(Fixture, MinMax)
	{
		GLootInfo* pLootInfo = test.loot.NewLootInfo();
		LOOT_MONEY_INFO* pLootMoneyInfo = test.loot.NewLootMoneyInfo(pLootInfo, 100, 1, 10);
		m_pNPC->GetNPCInfo()->m_pLootInfo = pLootInfo;

		GetDropMoney().Drop(m_pPlayer->GetCID(), pLootInfo);

		int nQuantity = GetDropMoney().GetQuantity();

		CHECK(pLootMoneyInfo->m_nMin <= nQuantity);
		CHECK(pLootMoneyInfo->m_nMax >= nQuantity);
	}

	// 공헌자만 권한을 갖는다
	TEST_FIXTURE(Fixture, Authority)
	{
		GEntityPlayer* pPlayer2 = test.player.NewPlayer(m_pField);

		GLootInfo* pLootInfo = test.loot.NewLootInfo();
		LOOT_MONEY_INFO* pLootMoneyInfo = test.loot.NewLootMoneyInfo(pLootInfo, 100, 1, 10);
		m_pNPC->GetNPCInfo()->m_pLootInfo = pLootInfo;

		GetDropMoney().Drop(m_pPlayer->GetCID(), pLootInfo);

		int nQuantity = GetDropMoney().GetQuantity();

		CHECK_EQUAL(true, GetDropMoney().IsAuthorizedCID(m_pPlayer->GetCID()));
		CHECK_EQUAL(false, GetDropMoney().IsAuthorizedCID(pPlayer2->GetCID()));
	}
}