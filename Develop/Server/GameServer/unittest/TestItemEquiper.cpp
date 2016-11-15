#include "stdafx.h"
#include "FBaseItem.h"
#include "FBaseMockLink.h"
#include "GItemEquiper.h"
#include "GUTHelper_Item.h"
#include "GTestForward.h"
#include "GItemHolder.h"

SUITE(ItemEquiper)
{
	struct Fixture : public FBaseItem, public FBaseMockLink
	{
	public:
		Fixture()
		{
			m_pField = test.field.DefaultMockField();
			m_pPlayer = test.player.NewPlayer(m_pField);
		}

		~Fixture()
		{			
			m_pField->Destroy();
		}

		MockField*		m_pField;
		GEntityPlayer*	m_pPlayer;
		GItemEquiper	m_ItemEquiper;
	};

	TEST_FIXTURE(Fixture, CheckSlot)
	{
		// 인벤이나, 장비창 외에 다른곳에 있는 아이템은 장비할수 없다.
		GItem* pItemInMail = test.item.GainWeaponItem(m_pPlayer, INVALID_ID, WEAPON_1H_SLASH);
		pItemInMail->m_nSlotType = SLOTTYPE_MAIL;		
		CHECK_EQUAL(false, m_ItemEquiper.CheckSlot(m_pPlayer, pItemInMail, ITEMSLOT_RWEAPON));

		// 장비창에 있는 아이템을 같은 슬롯에 장비할수 없다.
		GItem* pEquipedItem = test.item.EquipNewItem(m_pPlayer);
		CHECK_EQUAL(false, m_ItemEquiper.CheckSlot(m_pPlayer, pEquipedItem, static_cast<SH_ITEM_SLOT>(pEquipedItem->m_nSlotID)));

		// 방패가 아닌 무기아이템은 오른속에 아이템이 착용되지 않은 상태에서, 왼손에 착용할수 없다.
		GItem* pWeapon = test.item.GainWeaponItem(m_pPlayer, INVALID_ID, WEAPON_1H_SLASH);
		CHECK_EQUAL(false, m_ItemEquiper.CheckSlot(m_pPlayer, pWeapon, ITEMSLOT_LWEAPON));
		GItem* pShield = test.item.GainShieldItem(m_pPlayer);
		CHECK_EQUAL(true, m_ItemEquiper.CheckSlot(m_pPlayer, pShield, ITEMSLOT_LWEAPON));

		// 나머지 경우 테스트는 TestItemHelper의 IsEquipableSlot에서 하고 있다.
	}	

	TEST_FIXTURE(Fixture, CheckReservedDummySlot)
	{
		CHECK_EQUAL(true, m_ItemEquiper.CheckReservedDummySlot(m_pPlayer, ITEMSLOT_RWEAPON, ITEMSLOT_NONE));

		m_pPlayer->GetItemHolder()->ReserveDummySlot(SLOTTYPE_EQUIP, ITEMSLOT_RWEAPON);

		CHECK_EQUAL(false, m_ItemEquiper.CheckReservedDummySlot(m_pPlayer, ITEMSLOT_RWEAPON, ITEMSLOT_NONE));
	}

	TEST_FIXTURE(Fixture, CheckHasPassiveTalent_Weapon)
	{		
		GItem* pWeapon = test.item.GainWeaponItem(m_pPlayer, INVALID_ID, WEAPON_1H_SLASH);

		pWeapon->m_pItemData->m_nWeaponType = WEAPON_1H_SLASH;
		CHECK_EQUAL(false, m_ItemEquiper.CheckHasPassiveTalent(m_pPlayer, pWeapon));
		test.talent.LearnExtraPassiveTalent(m_pPlayer, TEPT_BASE_WEAPON_1H_SLASH);
		CHECK_EQUAL(true, m_ItemEquiper.CheckHasPassiveTalent(m_pPlayer, pWeapon));

		pWeapon->m_pItemData->m_nWeaponType = WEAPON_1H_BLUNT;
		CHECK_EQUAL(false, m_ItemEquiper.CheckHasPassiveTalent(m_pPlayer, pWeapon));
		test.talent.LearnExtraPassiveTalent(m_pPlayer, TEPT_BASE_WEAPON_1H_BLUNT);
		CHECK_EQUAL(true, m_ItemEquiper.CheckHasPassiveTalent(m_pPlayer, pWeapon));

		pWeapon->m_pItemData->m_nWeaponType = WEAPON_1H_PIERCE;
		CHECK_EQUAL(false, m_ItemEquiper.CheckHasPassiveTalent(m_pPlayer, pWeapon));
		test.talent.LearnExtraPassiveTalent(m_pPlayer, TEPT_BASE_WEAPON_1H_PIERCE);
		CHECK_EQUAL(true, m_ItemEquiper.CheckHasPassiveTalent(m_pPlayer, pWeapon));

		pWeapon->m_pItemData->m_nWeaponType = WEAPON_TWO_HANDED;
		CHECK_EQUAL(false, m_ItemEquiper.CheckHasPassiveTalent(m_pPlayer, pWeapon));
		test.talent.LearnExtraPassiveTalent(m_pPlayer, TEPT_BASE_WEAPON_TWO_HANDED);
		CHECK_EQUAL(true, m_ItemEquiper.CheckHasPassiveTalent(m_pPlayer, pWeapon));

		pWeapon->m_pItemData->m_nWeaponType = WEAPON_STAFF;
		CHECK_EQUAL(false, m_ItemEquiper.CheckHasPassiveTalent(m_pPlayer, pWeapon));
		test.talent.LearnExtraPassiveTalent(m_pPlayer, TEPT_BASE_WEAPON_STAFF);
		CHECK_EQUAL(true, m_ItemEquiper.CheckHasPassiveTalent(m_pPlayer, pWeapon));

		pWeapon->m_pItemData->m_nWeaponType = WEAPON_ARCHERY;
		CHECK_EQUAL(false, m_ItemEquiper.CheckHasPassiveTalent(m_pPlayer, pWeapon));
		test.talent.LearnExtraPassiveTalent(m_pPlayer, TEPT_BASE_WEAPON_ARCHERY);
		CHECK_EQUAL(true, m_ItemEquiper.CheckHasPassiveTalent(m_pPlayer, pWeapon));

		pWeapon->m_pItemData->m_nWeaponType = WEAPON_2H_BLUNT;
		CHECK_EQUAL(false, m_ItemEquiper.CheckHasPassiveTalent(m_pPlayer, pWeapon));
		test.talent.LearnExtraPassiveTalent(m_pPlayer, TEPT_BASE_WEAPON_2H_BLUNT);
		CHECK_EQUAL(true, m_ItemEquiper.CheckHasPassiveTalent(m_pPlayer, pWeapon));

		pWeapon->m_pItemData->m_nWeaponType = WEAPON_DUAL_PIERCE;
		CHECK_EQUAL(false, m_ItemEquiper.CheckHasPassiveTalent(m_pPlayer, pWeapon));
		test.talent.LearnExtraPassiveTalent(m_pPlayer, TEPT_BASE_WEAPON_DUAL_PIERCE);
		CHECK_EQUAL(true, m_ItemEquiper.CheckHasPassiveTalent(m_pPlayer, pWeapon));
	}

	TEST_FIXTURE(Fixture, CheckHasPassiveTalent_Armor)
	{
		GItem* pArmor = test.item.GiveNewItem(m_pPlayer);
		pArmor->m_pItemData->m_ItemType = ITEMTYPE_ARMOR;

		pArmor->m_pItemData->m_nArmorType = ARMOR_CLOTH;
		CHECK_EQUAL(true, m_ItemEquiper.CheckHasPassiveTalent(m_pPlayer, pArmor));

		pArmor->m_pItemData->m_nArmorType = ARMOR_LIGHT;
		CHECK_EQUAL(false, m_ItemEquiper.CheckHasPassiveTalent(m_pPlayer, pArmor));
		test.talent.LearnExtraPassiveTalent(m_pPlayer, TEPT_BASE_ARMOR_LIGHT);
		CHECK_EQUAL(true, m_ItemEquiper.CheckHasPassiveTalent(m_pPlayer, pArmor));

		pArmor->m_pItemData->m_nArmorType = ARMOR_MEDIUM;
		CHECK_EQUAL(false, m_ItemEquiper.CheckHasPassiveTalent(m_pPlayer, pArmor));
		test.talent.LearnExtraPassiveTalent(m_pPlayer, TEPT_BASE_ARMOR_MEDUIM);
		CHECK_EQUAL(true, m_ItemEquiper.CheckHasPassiveTalent(m_pPlayer, pArmor));

		pArmor->m_pItemData->m_nArmorType = ARMOR_HEAVY;
		CHECK_EQUAL(false, m_ItemEquiper.CheckHasPassiveTalent(m_pPlayer, pArmor));
		test.talent.LearnExtraPassiveTalent(m_pPlayer, TEPT_BASE_ARMOR_HEAVY);
		CHECK_EQUAL(true, m_ItemEquiper.CheckHasPassiveTalent(m_pPlayer, pArmor));
	}

	TEST_FIXTURE(Fixture, SelectUnequipItem)
	{
		GItem* p1HS = test.item.GainWeaponItem(m_pPlayer, INVALID_ID, WEAPON_1H_SLASH);
		GItem* p1HS2 = test.item.GainWeaponItem(m_pPlayer, INVALID_ID, WEAPON_1H_SLASH);
		GItem* p2HD = test.item.GainWeaponItem(m_pPlayer, INVALID_ID, WEAPON_TWO_HANDED);
		GItem* p2HD2 = test.item.GainWeaponItem(m_pPlayer, INVALID_ID, WEAPON_TWO_HANDED);
		GItem* pShield = test.item.GainShieldItem(m_pPlayer);

		GItem* pUnequipItem1 = NULL;
		GItem* pUnequipItem2 = NULL;

		// 장착할 메인 슬롯을 메인슬롯으로 차지하고 있는 아이템 찾기
		test.item.EquipItem(m_pPlayer, p1HS, ITEMSLOT_RWEAPON);
		m_ItemEquiper.SelectUnequipItem(m_pPlayer, p1HS2, ITEMSLOT_RWEAPON, ITEMSLOT_NONE, pUnequipItem1, pUnequipItem2);
		CHECK_EQUAL(pUnequipItem1, p1HS);

		// 장착할 메인슬롯을 서브슬롯으로 사용하고 있는 아이템 찾기
		test.item.EquipItem(m_pPlayer, p2HD, ITEMSLOT_RWEAPON);
		m_ItemEquiper.SelectUnequipItem(m_pPlayer, pShield, ITEMSLOT_LWEAPON, ITEMSLOT_NONE, pUnequipItem1, pUnequipItem2);
		CHECK_EQUAL(pUnequipItem1, p2HD);

		// 장착할 서브슬롯을 메인슬롯으로 차지하고 있는 아이템 찾기
		test.item.EquipItem(m_pPlayer, pShield, ITEMSLOT_LWEAPON);
		m_ItemEquiper.SelectUnequipItem(m_pPlayer, p2HD, ITEMSLOT_RWEAPON, ITEMSLOT_NONE, pUnequipItem1, pUnequipItem2);
		CHECK_EQUAL(pUnequipItem1, pShield);

		// 장착할 서브슬롯을 서브슬롯으로 차지하고 있는 아이템 찾기
		test.item.EquipItem(m_pPlayer, p2HD, ITEMSLOT_RWEAPON);
		m_ItemEquiper.SelectUnequipItem(m_pPlayer, p2HD2, ITEMSLOT_RWEAPON, ITEMSLOT_NONE, pUnequipItem1, pUnequipItem2);
		CHECK_EQUAL(pUnequipItem1, p2HD);

		// 같이 착용할수 없는 무기 찾는 테스트는 TestItemHelper의 IsTogetherEquipableItemData에서 하고 있다.
	}
}