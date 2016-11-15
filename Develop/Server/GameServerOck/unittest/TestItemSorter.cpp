#include "stdafx.h"
#include "FBaseGame.h"
#include "GItemSorter.h"
#include "GTestForward.h"
#include "GItemHolder.h"

SUITE(ItemSorter)
{
	struct Fixture : public FBaseGame
	{
		Fixture()
		{
			m_pEquipItemData = test.item.NewWeaponItemData(INVALID_ID, WEAPON_1H_SLASH);
			m_pUsableItemData = test.item.NewItemData();
			m_pUsableItemData->m_ItemType = ITEMTYPE_USABLE;
			m_pQuestItemData = test.item.NewItemData();
			m_pQuestItemData->m_bQuestRelated = true;
			m_pMaterialItemData = test.item.NewItemData();
			m_pMaterialItemData->m_bRecipeMaterial = true;
			m_pEtcItemData = test.item.NewItemData();
		}

		GItemSorter	m_ItemSorter;
		GItemData*	m_pEquipItemData;
		GItemData*	m_pUsableItemData;
		GItemData*	m_pQuestItemData;
		GItemData*	m_pMaterialItemData;
		GItemData*	m_pEtcItemData;
	};
	
	TEST_FIXTURE(Fixture, Sort)
	{
		test.item.GiveNewItem(m_pPlayer, m_pMaterialItemData->m_nID);
		test.item.GiveNewItem(m_pPlayer, m_pQuestItemData->m_nID);
		test.item.GiveNewItem(m_pPlayer, m_pUsableItemData->m_nID);
		test.item.GiveNewItem(m_pPlayer, m_pEquipItemData->m_nID);
		test.item.GiveNewItem(m_pPlayer, m_pEtcItemData->m_nID);
		
		m_ItemSorter.Sort(m_pPlayer);

		GItem* pEquipItem = m_pPlayer->GetItemHolder()->GetItem(SLOTTYPE_INVENTORY, 0);		
		GItem* pUsableItem = m_pPlayer->GetItemHolder()->GetItem(SLOTTYPE_INVENTORY, 1);		
		GItem* pQuestItem = m_pPlayer->GetItemHolder()->GetItem(SLOTTYPE_INVENTORY, 2);		
		GItem* pMaterialItem = m_pPlayer->GetItemHolder()->GetItem(SLOTTYPE_INVENTORY, 3);		
		GItem* pEtcItem = m_pPlayer->GetItemHolder()->GetItem(SLOTTYPE_INVENTORY, 4);

		ASSERT_CHECK(pEquipItem);
		ASSERT_CHECK(pUsableItem);
		ASSERT_CHECK(pQuestItem);
		ASSERT_CHECK(pMaterialItem);
		ASSERT_CHECK(pEtcItem);

		CHECK_EQUAL(m_pEquipItemData, pEquipItem->m_pItemData);
		CHECK_EQUAL(m_pUsableItemData, pUsableItem->m_pItemData);
		CHECK_EQUAL(m_pQuestItemData, pQuestItem->m_pItemData);
		CHECK_EQUAL(m_pMaterialItemData, pMaterialItem->m_pItemData);
		CHECK_EQUAL(m_pEtcItemData, pEtcItem->m_pItemData);
	}
};