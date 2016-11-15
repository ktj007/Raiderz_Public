#include "stdafx.h"
#include "FBaseItem.h"
#include "GUTHelper_Item.h"
#include "SUnitTestUtil.h"
#include "GEntityPlayer.h"
#include "GItemManager.h"
#include "GTestWrapper.h"
#include "GTestForward.h"
#include "GItemHolder.h"
#include "GEquipmentSlot.h"
#include "GTrainingSystem.h"

SUITE(EquipmentSlotTest)
{
	class FEquipmentSlot : FBaseItem
	{
	public:
		FEquipmentSlot()
		{
			m_pField = GUTHelper_Field::DefaultMockField();
			m_pEntityPlayer = m_TestHelpers.player.NewPlayer(m_pField);

			InitItemData();
		}
		virtual ~FEquipmentSlot() 
		{
			m_pField->Destroy();
		}

		void InitItemData()
		{
			m_pItemDataHead = GUTHelper_Item::NewItemData(1, ITEMSLOT_HEAD);
			m_pItemDataBody = GUTHelper_Item::NewItemData(2, ITEMSLOT_BODY);
			m_pItemDataLeg = GUTHelper_Item::NewItemData(3, ITEMSLOT_LEG);
			m_pItemDataHands = GUTHelper_Item::NewItemData(4, ITEMSLOT_HANDS);
			m_pItemDataFeet = GUTHelper_Item::NewItemData(5, ITEMSLOT_FEET);
			m_pItemDataWeapon = GUTHelper_Item::NewWeaponItemData(6, WEAPON_1H_PIERCE);
			m_pItemDataWeaponLeft = GUTHelper_Item::NewWeaponItemData(7, WEAPON_1H_PIERCE);
			m_pItemDataWeaponLeft->m_bLeftWeaponEquipable = true;
		}

	public:
		GItemData* m_pItemDataHead;
		GItemData* m_pItemDataBody;
		GItemData* m_pItemDataLeg;
		GItemData* m_pItemDataHands;
		GItemData* m_pItemDataFeet;

		GItemData* m_pItemDataWeapon;
		GItemData* m_pItemDataWeaponLeft;

		GTestHelpers m_TestHelpers;		
		MockField*	m_pField;		
		GEntityPlayer* m_pEntityPlayer;
	};

	TEST_FIXTURE(FEquipmentSlot, EquipmentSlot_Material)
	{
		GItemHolder* pItemHolder = m_pEntityPlayer->GetItemHolder();

		m_pItemDataHead->m_nArmorType = ARMOR_CLOTH;
		m_pItemDataBody->m_nArmorType = ARMOR_MEDIUM;
		m_pItemDataLeg->m_nArmorType = ARMOR_MEDIUM;
		m_pItemDataHands->m_nArmorType = ARMOR_MEDIUM;
		m_pItemDataFeet->m_nArmorType = ARMOR_HEAVY;

		GUTHelper_Item::EquipNewItem(m_pEntityPlayer, 1, ITEMSLOT_HEAD);
		CHECK_EQUAL(pItemHolder->GetEquipment().GetSlotMaterial(ITEMSLOT_HEAD), ARMOR_CLOTH);

		GUTHelper_Item::EquipNewItem(m_pEntityPlayer, 2, ITEMSLOT_BODY);
		CHECK_EQUAL(pItemHolder->GetEquipment().GetSlotMaterial(ITEMSLOT_BODY), ARMOR_MEDIUM);

		GItem* pLegItem = GUTHelper_Item::EquipNewItem(m_pEntityPlayer, 3, ITEMSLOT_LEG);
		CHECK_EQUAL(pItemHolder->GetEquipment().GetSlotMaterial(ITEMSLOT_LEG), ARMOR_MEDIUM);
		
		GUTHelper_Item::EquipNewItem(m_pEntityPlayer, 4, ITEMSLOT_HANDS);
		CHECK_EQUAL(pItemHolder->GetEquipment().GetSlotMaterial(ITEMSLOT_HANDS), ARMOR_MEDIUM);

		GUTHelper_Item::UnequipItem(m_pEntityPlayer, pLegItem);
		CHECK_EQUAL(pItemHolder->GetEquipment().GetSlotMaterial(ITEMSLOT_LEG), ARMOR_NONE);
	}

	TEST_FIXTURE(FEquipmentSlot, EquipmentSlot_FundSubSlotItem)
	{
		GItemHolder* pItemHolder = m_pEntityPlayer->GetItemHolder();
		CHECK_EQUAL(pItemHolder->GetEquipment().GetSubSlot(ITEMSLOT_HEAD), ITEMSLOT_NONE);	
	}

	TEST_FIXTURE(FEquipmentSlot, EraseModifierByUntrainAll)
	{
		m_pItemDataHead->m_nArmorType = ARMOR_MEDIUM;
		m_pItemDataHead->m_PlayerModifier.nSTR.nMod = 1;
		test.item.EquipNewItem(m_pEntityPlayer, m_pItemDataHead->m_nID);

		int nBeforeSTR = m_pEntityPlayer->GetSTR();

		test.talent.UntrainAll(m_pEntityPlayer);

		CHECK_EQUAL(nBeforeSTR - m_pItemDataHead->m_PlayerModifier.nSTR.nMod, m_pEntityPlayer->GetSTR());
	}

	TEST_FIXTURE(FEquipmentSlot, AddModifierByTrain)
	{
		m_pItemDataHead->m_nArmorType = ARMOR_MEDIUM;
		m_pItemDataHead->m_PlayerModifier.nSTR.nMod = 1;
		test.item.EquipNewItem(m_pEntityPlayer, m_pItemDataHead->m_nID);
		test.talent.UntrainAll(m_pEntityPlayer);		

		int nBeforeSTR = m_pEntityPlayer->GetSTR();

		test.talent.LearnExtraPassiveTalent(m_pEntityPlayer, TEPT_BASE_ARMOR_MEDUIM);

		CHECK_EQUAL(nBeforeSTR + m_pItemDataHead->m_PlayerModifier.nSTR.nMod, m_pEntityPlayer->GetSTR());
	}
}
