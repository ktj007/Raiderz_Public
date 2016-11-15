#include "stdafx.h"
#include "SUnitTestUtil.h"
#include "MockEntityPlayer.h"
#include "CCommandResultTable.h"
#include "MockField.h"
#include "MockDBManager.h"
#include "GGlobal.h"
#include "GTestWrapper.h"
#include "GItemSystem.h"
#include "CCommandResultTable.h"
#include "TestItemManager.h"
#include "GUTHelper.h"
#include "GTestForward.h"
#include "GItemEquiper.h"
#include "GItemHolder.h"
#include "FBaseGame.h"

SUITE(ItemEquiper_Equip)
{
	struct Fixture : public FBaseGame
	{
		Fixture()
		{ 
			m_pItemHolder = m_pPlayer->GetItemHolder();
			test.talent.TrainEquipUsableAllTalent(m_pPlayer);
		}

		~Fixture() 
		{
		}

		GItemHolder*			m_pItemHolder;
		GItemEquiper			m_ItemEquiper;
	};


	TEST_FIXTURE(Fixture, SingleSlot)
	{
		GItem* p1HS = test.item.GainWeaponItem(m_pPlayer, INVALID_ID, WEAPON_1H_SLASH);

		CHECK_EQUAL(true, m_ItemEquiper.Equip(m_pPlayer, p1HS, ITEMSLOT_RWEAPON));
		CHECK_EQUAL(m_pItemHolder->GetItem(SLOTTYPE_EQUIP, ITEMSLOT_RWEAPON), p1HS);
		CHECK_EQUAL(m_pItemHolder->GetItem(SLOTTYPE_EQUIP, ITEMSLOT_LWEAPON), (GItem*)NULL);		
	}

	TEST_FIXTURE(Fixture, DoubleSlot)
	{
		GItem* p2HD = test.item.GainWeaponItem(m_pPlayer, INVALID_ID, WEAPON_TWO_HANDED);

		CHECK_EQUAL(true, m_ItemEquiper.Equip(m_pPlayer, p2HD, ITEMSLOT_RWEAPON));
		CHECK_EQUAL(m_pItemHolder->GetItem(SLOTTYPE_EQUIP, ITEMSLOT_RWEAPON), p2HD);
		CHECK_EQUAL(m_pItemHolder->GetItem(SLOTTYPE_EQUIP, ITEMSLOT_LWEAPON), (GItem*)NULL);
	}

	TEST_FIXTURE(Fixture, DuelWield)
	{
		GItem* p1HS = test.item.GainWeaponItem(m_pPlayer, INVALID_ID, WEAPON_1H_SLASH);
		GItem* p1HB = test.item.GainWeaponItem(m_pPlayer, INVALID_ID, WEAPON_1H_BLUNT);
		p1HB->m_pItemData->m_bLeftWeaponEquipable = true;

		m_ItemEquiper.Equip(m_pPlayer, p1HS, ITEMSLOT_RWEAPON);
		m_ItemEquiper.Equip(m_pPlayer, p1HB, ITEMSLOT_LWEAPON);

		CHECK_EQUAL(m_pItemHolder->GetItem(SLOTTYPE_EQUIP, ITEMSLOT_RWEAPON), p1HS);
		CHECK_EQUAL(m_pItemHolder->GetItem(SLOTTYPE_EQUIP, ITEMSLOT_LWEAPON), p1HB);
	}

	TEST_FIXTURE(Fixture, SingleSlotAtAlreadyEquipedSingleSlot)
	{
		GItem* p1HS = test.item.GainWeaponItem(m_pPlayer, INVALID_ID, WEAPON_1H_SLASH);
		GItem* p1HS2 = test.item.GainWeaponItem(m_pPlayer, INVALID_ID, WEAPON_1H_SLASH);
		p1HS2->m_pItemData->m_bLeftWeaponEquipable = true;

		m_ItemEquiper.Equip(m_pPlayer, p1HS, ITEMSLOT_RWEAPON);
		m_ItemEquiper.Equip(m_pPlayer, p1HS2, ITEMSLOT_RWEAPON);

		CHECK_EQUAL(m_pItemHolder->GetItem(SLOTTYPE_EQUIP, ITEMSLOT_RWEAPON), p1HS2);
		CHECK_EQUAL(m_pItemHolder->GetItem(SLOTTYPE_EQUIP, ITEMSLOT_LWEAPON), (GItem*)NULL);		
	}

	TEST_FIXTURE(Fixture, DoubleSlotItemAtAlreadyEquipeSingledSlot)
	{
		GItem* p1HS = test.item.GainWeaponItem(m_pPlayer, INVALID_ID, WEAPON_1H_SLASH);
		GItem* p2HD = test.item.GainWeaponItem(m_pPlayer, INVALID_ID, WEAPON_TWO_HANDED);

		m_ItemEquiper.Equip(m_pPlayer, p1HS, ITEMSLOT_RWEAPON);
		m_ItemEquiper.Equip(m_pPlayer, p2HD, ITEMSLOT_RWEAPON);

		CHECK_EQUAL(m_pItemHolder->GetItem(SLOTTYPE_EQUIP, ITEMSLOT_RWEAPON), p2HD);
		CHECK_EQUAL(m_pItemHolder->GetItem(SLOTTYPE_EQUIP, ITEMSLOT_LWEAPON), (GItem*)NULL);
	}
	
	TEST_FIXTURE(Fixture, SingleSlotItemAtAlreadyEquipedDoubleSlot)
	{
		GItem* p2HD = test.item.GainWeaponItem(m_pPlayer, INVALID_ID, WEAPON_TWO_HANDED);
		GItem* p1HS = test.item.GainWeaponItem(m_pPlayer, INVALID_ID, WEAPON_1H_SLASH);
		
		m_ItemEquiper.Equip(m_pPlayer, p2HD, ITEMSLOT_RWEAPON);
		m_ItemEquiper.Equip(m_pPlayer, p1HS, ITEMSLOT_RWEAPON);

		CHECK_EQUAL(m_pItemHolder->GetItem(SLOTTYPE_EQUIP, ITEMSLOT_RWEAPON), p1HS);
		CHECK_EQUAL(m_pItemHolder->GetItem(SLOTTYPE_EQUIP, ITEMSLOT_LWEAPON), (GItem*)NULL);		
	}

	TEST_FIXTURE(Fixture, DoubleSlotItemAtAlreadyEquipedDoubleSlot)
	{
		GItem* p2HD = test.item.GainWeaponItem(m_pPlayer, INVALID_ID, WEAPON_TWO_HANDED);
		GItem* p2HD2 = test.item.GainWeaponItem(m_pPlayer, INVALID_ID, WEAPON_TWO_HANDED);

		m_ItemEquiper.Equip(m_pPlayer, p2HD, ITEMSLOT_RWEAPON);
		m_ItemEquiper.Equip(m_pPlayer, p2HD2, ITEMSLOT_RWEAPON);
		CHECK_EQUAL(m_pItemHolder->GetItem(SLOTTYPE_EQUIP, ITEMSLOT_RWEAPON), p2HD2);
		CHECK_EQUAL(m_pItemHolder->GetItem(SLOTTYPE_EQUIP, ITEMSLOT_LWEAPON), (GItem*)NULL);
	}

	TEST_FIXTURE(Fixture, ArmorItems)
	{
		GItem* pHead = test.item.GainArmorItem(m_pPlayer);
		pHead->m_pItemData->m_nItemSlot = ITEMSLOT_HEAD;
		CHECK_EQUAL(true, m_ItemEquiper.Equip(m_pPlayer, pHead, ITEMSLOT_HEAD));
		CHECK_EQUAL(m_pItemHolder->GetItem(SLOTTYPE_EQUIP, ITEMSLOT_HEAD), pHead);

		GItem* pLFinger = test.item.GainArmorItem(m_pPlayer);
		pLFinger->m_pItemData->m_nItemSlot = ITEMSLOT_LFINGER;
		CHECK_EQUAL(true, m_ItemEquiper.Equip(m_pPlayer, pLFinger, ITEMSLOT_LFINGER));
		CHECK_EQUAL(m_pItemHolder->GetItem(SLOTTYPE_EQUIP, ITEMSLOT_LFINGER),	pLFinger);

		GItem* pLookHead = test.item.GainArmorItem(m_pPlayer);
		pLookHead->m_pItemData->m_nItemSlot = ITEMSLOT_LOOK_HEAD;
		CHECK_EQUAL(true, m_ItemEquiper.Equip(m_pPlayer, pLookHead, ITEMSLOT_LOOK_HEAD));
		CHECK_EQUAL(m_pItemHolder->GetItem(SLOTTYPE_EQUIP, ITEMSLOT_LOOK_HEAD),	pLookHead);
	}
	
	TEST_FIXTURE(Fixture, SecondaryWeapons)
	{
		GItem* p1HS = test.item.GainWeaponItem(m_pPlayer, INVALID_ID, WEAPON_1H_SLASH);
		GItem* pShield = test.item.GainShieldItem(m_pPlayer);

		CHECK_EQUAL(true, m_ItemEquiper.Equip(m_pPlayer, p1HS, ITEMSLOT_RWEAPON2));
		CHECK_EQUAL(true, m_ItemEquiper.Equip(m_pPlayer, pShield, ITEMSLOT_LWEAPON2));
		CHECK_EQUAL(m_pItemHolder->GetItem(SLOTTYPE_EQUIP, ITEMSLOT_RWEAPON2), p1HS);
		CHECK_EQUAL(m_pItemHolder->GetItem(SLOTTYPE_EQUIP, ITEMSLOT_LWEAPON2), pShield);		
	}

	TEST_FIXTURE(Fixture, Bind)
	{
		GItem* pWeapon = test.item.GainWeaponItem(m_pPlayer);
		pWeapon->m_pItemData->m_bClaimRequired = true;

		CHECK_EQUAL(pWeapon->m_bBind, false);
		
		gsys.pItemSystem->GetEquiper().Equip(m_pPlayer, pWeapon, pWeapon->m_pItemData->m_nItemSlot);

		CHECK_EQUAL(pWeapon->m_bBind, true);
	}	

	TEST_FIXTURE(Fixture, ChangeEquipSlot)
	{
		GItem* pWeapon1 = test.item.EquipNewItem(m_pPlayer, INVALID_ID, ITEMSLOT_RWEAPON);
		GItem* pWeapon2 = test.item.EquipNewItem(m_pPlayer, INVALID_ID, ITEMSLOT_RWEAPON2);

		test.item.EquipItem(m_pPlayer, pWeapon1, ITEMSLOT_RWEAPON2);

		CHECK_EQUAL(m_pItemHolder->GetItem(SLOTTYPE_EQUIP, ITEMSLOT_RWEAPON2), pWeapon1);
		CHECK_EQUAL(m_pItemHolder->GetItem(SLOTTYPE_EQUIP, ITEMSLOT_RWEAPON), (GItem*)NULL);		
	}
}
