#include "stdafx.h"
#include "GFactionInfo.h"
#include "GNPCShopInfo.h"
#include "GEntityPlayer.h"
#include "GPlayerFactions.h"
#include "GTestForward.h"
#include "GNPCShopShower.h"
#include "CSNPCShopCalculator.h"
#include "CSFactionCalculator.h"
#include "AStlUtil.h"

SUITE(NPCShopShower)
{
	struct Fixture
	{
		Fixture()
		{
			m_pField = GUTHelper_Field::DefaultMockField();
			m_pPlayer = test.player.NewPlayer(m_pField);

			m_pLink = test.network.NewLink(m_pPlayer);
			m_pNPCShopInfo = test.npcshop.NewNPCShopInfo();

			m_pLink->ResetCommands();
		}

		~Fixture()
		{
			test.network.ClearLinks();
		}


		MockField*			m_pField;
		GEntityPlayer*		m_pPlayer;
		MockLink*			m_pLink;

		GNPCShopInfo*		m_pNPCShopInfo;

		GNPCShopShower		m_NPCShopShower;		
	};


	TEST_FIXTURE(Fixture, Show_Item)
	{
		GItemData*	pBuyableUsableItemData  = test.npcshop.AddBuyableUsableItem(m_pNPCShopInfo);
		GItemData*	pBuyableUnusableItemData  = test.npcshop.AddBuyableUnusableItem(m_pNPCShopInfo);
		GItemData*	pUnbuyableItemData  = test.npcshop.AddUnbuyableItem(m_pNPCShopInfo);	
		
		CHECK(true == m_NPCShopShower.Show(m_pPlayer, m_pNPCShopInfo->m_nID));

		ASSERT_EQUAL(1, m_pLink->GetCommandCount());
		ASSERT_EQUAL(MC_NPCSHOP_SHOW, m_pLink->GetCommand(0).GetID());

		vector<int> vecBuyableUsableItemID;
		vector<int> vecBuyableUnusableItemID;
		vector<int> vecUnbuyableItemID;
		ASSERT_CHECK(m_pLink->GetCommand(0).GetBlob(vecBuyableUsableItemID, 0));
		CHECK_EQUAL(true, IsExist(vecBuyableUsableItemID, pBuyableUsableItemData->m_nID));
		CHECK_EQUAL(false, IsExist(vecBuyableUsableItemID, pBuyableUnusableItemData->m_nID));
		CHECK_EQUAL(false, IsExist(vecBuyableUsableItemID, pUnbuyableItemData->m_nID));

		ASSERT_CHECK(m_pLink->GetCommand(0).GetBlob(vecBuyableUnusableItemID, 1));
		CHECK_EQUAL(false, IsExist(vecBuyableUnusableItemID, pBuyableUsableItemData->m_nID));
		CHECK_EQUAL(true, IsExist(vecBuyableUnusableItemID, pBuyableUnusableItemData->m_nID));
		CHECK_EQUAL(false, IsExist(vecBuyableUnusableItemID, pUnbuyableItemData->m_nID));

		ASSERT_CHECK(m_pLink->GetCommand(0).GetBlob(vecUnbuyableItemID, 2));
		CHECK_EQUAL(false, IsExist(vecUnbuyableItemID, pBuyableUsableItemData->m_nID));
		CHECK_EQUAL(false, IsExist(vecUnbuyableItemID, pBuyableUnusableItemData->m_nID));
		CHECK_EQUAL(true, IsExist(vecUnbuyableItemID, pUnbuyableItemData->m_nID));
	}

	TEST_FIXTURE(Fixture, Show_Mod)
	{
		m_pNPCShopInfo->m_fBaseBuyMod = 0.5f;
		m_pNPCShopInfo->m_fBaseSellMod = 0.2f;

		CHECK(true == m_NPCShopShower.Show(m_pPlayer, m_pNPCShopInfo->m_nID));

		ASSERT_EQUAL(1, m_pLink->GetCommandCount());
		ASSERT_EQUAL(MC_NPCSHOP_SHOW, m_pLink->GetCommand(0).GetID());

		CHECK_EQUAL(CSNPCShopCalculator::CalcBuyMod(m_pNPCShopInfo->m_fBaseBuyMod, FRT_NORMAL), m_pLink->GetParam<float>(0, 3));
		CHECK_EQUAL(CSNPCShopCalculator::CalcSellMod(m_pNPCShopInfo->m_fBaseSellMod, FRT_NORMAL), m_pLink->GetParam<float>(0, 4));
	}

	TEST_FIXTURE(Fixture, Show_Repairer)
	{
		m_pNPCShopInfo->m_bRepairable = true;

		CHECK(true == m_NPCShopShower.Show(m_pPlayer, m_pNPCShopInfo->m_nID));

		ASSERT_EQUAL(1, m_pLink->GetCommandCount());
		ASSERT_EQUAL(MC_NPCSHOP_SHOW, m_pLink->GetCommand(0).GetID());

		CHECK_EQUAL(m_pNPCShopInfo->m_bRepairable, m_pLink->GetParam<bool>(0, 5));
	}
}