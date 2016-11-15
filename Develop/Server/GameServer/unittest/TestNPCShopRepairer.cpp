#include "stdafx.h"
#include "GTestForward.h"
#include "GNPCShopRepairer.h"
#include "GNPCShopInfo.h"
#include "CSNPCShopCalculator.h"

SUITE(NPCShopRepairer)
{
	struct Fixture
	{
		Fixture() 
		{
			m_pField = GUTHelper_Field::DefaultMockField();
			m_pPlayer = test.player.NewPlayer(m_pField);
			m_pPlayer->GetPlayerInfo()->SetMoney(MAX_MONEY);

			m_pLink = test.network.NewLink(m_pPlayer);
			m_pNPCShopInfo = test.npcshop.NewNPCShopInfo();	
			m_pNPCShopInfo->m_bRepairable = true;

			m_pRepairItem = test.item.GiveNewItem(m_pPlayer);
			m_pRepairItem->m_pItemData->m_nArmorType = ARMOR_CLOTH;
			m_pRepairItem->m_pItemData->m_nRepairGrade = 3;
			m_pRepairItem->m_pItemData->m_nMaxDurability = 100;
			m_pRepairItem->m_nDurability = 50;			
			//m_pRepairItem->m_nEnchantCount = 2;

			m_pLink->ResetCommands();
		}


		MockField*			m_pField;
		GEntityPlayer*		m_pPlayer;
		MockLink*			m_pLink;

		GNPCShopInfo*		m_pNPCShopInfo;
		GItem*				m_pRepairItem;

		GNPCShopRepairer	m_NPCShopRepairer;		
	};

	// 수리할 수 없는 엔피씨인 경우
	TEST_FIXTURE(Fixture, CheckRepairableNPC)
	{
		m_pNPCShopInfo->m_bRepairable = false;

		CHECK_EQUAL(false, m_NPCShopRepairer.RepairOne(m_pPlayer, m_pNPCShopInfo->m_nID, m_pRepairItem->m_nSlotType, m_pRepairItem->m_nSlotID));
	}

	// 수리 할수 없는 아이템인 경우
	TEST_FIXTURE(Fixture, CheckRepairableItem)
	{
		// 내구도가 없는 아이템
		m_pRepairItem->m_pItemData->m_nMaxDurability = 0;
		CHECK_EQUAL(false, m_NPCShopRepairer.RepairOne(m_pPlayer, m_pNPCShopInfo->m_nID, m_pRepairItem->m_nSlotType, m_pRepairItem->m_nSlotID));

		// 최대 내구도인 아이템
		m_pRepairItem->m_pItemData->m_nMaxDurability = 10;
		m_pRepairItem->m_nDurability = 10;
		CHECK_EQUAL(false, m_NPCShopRepairer.RepairOne(m_pPlayer, m_pNPCShopInfo->m_nID, m_pRepairItem->m_nSlotType, m_pRepairItem->m_nSlotID));

		// 수리 할수 없는 아이템
		m_pRepairItem->m_pItemData->m_nMaxDurability = 10;
		m_pRepairItem->m_nDurability = 0;
		m_pRepairItem->m_pItemData->m_bRepairable = false;
		CHECK_EQUAL(false, m_NPCShopRepairer.RepairOne(m_pPlayer, m_pNPCShopInfo->m_nID, m_pRepairItem->m_nSlotType, m_pRepairItem->m_nSlotID));
	}

	// 수리비가 모자른 경우
	TEST_FIXTURE(Fixture, CheckMoney)
	{
		m_pPlayer->GetPlayerInfo()->SetMoney(0);

		CHECK_EQUAL(false, m_NPCShopRepairer.RepairOne(m_pPlayer, m_pNPCShopInfo->m_nID, m_pRepairItem->m_nSlotType, m_pRepairItem->m_nSlotID));
	}

	// 지정 아이템 한개 수리
	TEST_FIXTURE(Fixture, RepairOne)
	{
		int nBeforeMoney = m_pPlayer->GetPlayerInfo()->GetMoney();
		int nRepairPrice = CSNPCShopCalculator::CalcRepairPrice(m_pRepairItem->GetLostedDurability(), m_pRepairItem->m_pItemData->m_nRepairGrade, m_pRepairItem->GetEnchantCount());

		CHECK_EQUAL(true, m_NPCShopRepairer.RepairOne(m_pPlayer, m_pNPCShopInfo->m_nID, m_pRepairItem->m_nSlotType, m_pRepairItem->m_nSlotID));

		CHECK_EQUAL(nBeforeMoney - nRepairPrice, m_pPlayer->GetPlayerInfo()->GetMoney());
		CHECK_EQUAL(m_pRepairItem->m_pItemData->m_nMaxDurability, m_pRepairItem->m_nDurability);
	}

	// 아이템 전체 수리
	TEST_FIXTURE(Fixture, RepairAll)
	{
		GItem* pRepairItem2 = test.item.GiveNewItem(m_pPlayer);
		pRepairItem2->m_pItemData->m_nArmorType = ARMOR_LIGHT;
		pRepairItem2->m_pItemData->m_nMaxDurability = 100;
		pRepairItem2->m_nDurability = 30;

		int nBeforeMoney = m_pPlayer->GetPlayerInfo()->GetMoney();
		int nRepairPrice1 = CSNPCShopCalculator::CalcRepairPrice(m_pRepairItem->GetLostedDurability(), m_pRepairItem->m_pItemData->m_nRepairGrade, m_pRepairItem->GetEnchantCount());
		int nRepairPrice2 = CSNPCShopCalculator::CalcRepairPrice(pRepairItem2->GetLostedDurability(), pRepairItem2->m_pItemData->m_nRepairGrade, pRepairItem2->GetEnchantCount());
		int nRepairPrice = nRepairPrice1 + nRepairPrice2;

		CHECK_EQUAL(true, m_NPCShopRepairer.RepairAll(m_pPlayer, m_pNPCShopInfo->m_nID));

		CHECK_EQUAL(nBeforeMoney - nRepairPrice, m_pPlayer->GetPlayerInfo()->GetMoney());
		CHECK_EQUAL(m_pRepairItem->m_pItemData->m_nMaxDurability, m_pRepairItem->m_nDurability);
		CHECK_EQUAL(pRepairItem2->m_pItemData->m_nMaxDurability, pRepairItem2->m_nDurability);
	}
}