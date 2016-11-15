#include "stdafx.h"
#include "CSItemHelper.h"
#include "CSStrings.h"
#include "GTestForward.h"

SUITE(ItemHelper)
{
	TEST(TestItemHelper_GetCurrentWeaponType)
	{
		CHECK_EQUAL(WEAPON_NONE, CSItemHelper::GetCurrentWeaponType(WEAPON_NONE, WEAPON_NONE));
		CHECK_EQUAL(WEAPON_NONE, CSItemHelper::GetCurrentWeaponType(WEAPON_NONE, WEAPON_1H_SLASH));

		// 기본적으로 오른손에 든 무기가 현재 무기 타입
		CHECK_EQUAL(WEAPON_1H_SLASH, CSItemHelper::GetCurrentWeaponType(WEAPON_1H_SLASH, WEAPON_NONE));
		CHECK_EQUAL(WEAPON_1H_BLUNT, CSItemHelper::GetCurrentWeaponType(WEAPON_1H_BLUNT, WEAPON_NONE));
		CHECK_EQUAL(WEAPON_1H_PIERCE, CSItemHelper::GetCurrentWeaponType(WEAPON_1H_PIERCE, WEAPON_NONE));
		CHECK_EQUAL(WEAPON_STAFF, CSItemHelper::GetCurrentWeaponType(WEAPON_STAFF, WEAPON_NONE));

		// 양손
		CHECK_EQUAL(WEAPON_DUAL_WIELD, CSItemHelper::GetCurrentWeaponType(WEAPON_1H_SLASH, WEAPON_1H_SLASH));
		CHECK_EQUAL(WEAPON_DUAL_WIELD, CSItemHelper::GetCurrentWeaponType(WEAPON_1H_SLASH, WEAPON_1H_BLUNT));

		// 양손 단검
		CHECK_EQUAL(WEAPON_DUAL_PIERCE, CSItemHelper::GetCurrentWeaponType(WEAPON_1H_PIERCE, WEAPON_1H_PIERCE));
	}

	TEST(TEST_WeaponTypeString)
	{
		CHECK(wstring(L"none") == CSStrings::WeaponType(WEAPON_NONE));
		CHECK(wstring(L"1hs") == CSStrings::WeaponType(WEAPON_1H_SLASH));
		CHECK(wstring(L"1hb") == CSStrings::WeaponType(WEAPON_1H_BLUNT));
		CHECK(wstring(L"1hp") == CSStrings::WeaponType(WEAPON_1H_PIERCE));
		CHECK(wstring(L"2hd") == CSStrings::WeaponType(WEAPON_TWO_HANDED));
		CHECK(wstring(L"sta") == CSStrings::WeaponType(WEAPON_STAFF));
		CHECK(wstring(L"arc") == CSStrings::WeaponType(WEAPON_ARCHERY));
		CHECK(wstring(L"2hb") == CSStrings::WeaponType(WEAPON_2H_BLUNT));
		CHECK(wstring(L"dwd") == CSStrings::WeaponType(WEAPON_DUAL_WIELD));
		CHECK(wstring(L"dwp") == CSStrings::WeaponType(WEAPON_DUAL_PIERCE));
	}
	
	TEST(TestItemHelper_IsDualWieldWeaponType)
	{
		CHECK_EQUAL(true, CSItemHelper::IsDualWieldWeaponType(WEAPON_1H_SLASH));
		CHECK_EQUAL(true, CSItemHelper::IsDualWieldWeaponType(WEAPON_1H_BLUNT));
		CHECK_EQUAL(false, CSItemHelper::IsDualWieldWeaponType(WEAPON_1H_PIERCE));
		CHECK_EQUAL(false, CSItemHelper::IsDualWieldWeaponType(WEAPON_TWO_HANDED));
		CHECK_EQUAL(false, CSItemHelper::IsDualWieldWeaponType(WEAPON_STAFF));
		CHECK_EQUAL(false, CSItemHelper::IsDualWieldWeaponType(WEAPON_ARCHERY));
		CHECK_EQUAL(false, CSItemHelper::IsDualWieldWeaponType(WEAPON_2H_BLUNT));
	}

	TEST(TestItemHelper_IsShieldItemType)
	{
		GItemData itemData;
		itemData.m_nItemSlot = ITEMSLOT_LWEAPON;
		itemData.m_ItemType = ITEMTYPE_ARMOR;

		CHECK_EQUAL(true, CSItemHelper::IsShieldItemType(&itemData));

		// 그냥 아이템
		itemData.m_nItemSlot = ITEMSLOT_HEAD;
		itemData.m_ItemType = ITEMTYPE_ARMOR;

		CHECK_EQUAL(false, CSItemHelper::IsShieldItemType(&itemData));


		// 오른손 무기
		itemData.m_nItemSlot = ITEMSLOT_RWEAPON;
		itemData.m_ItemType = ITEMTYPE_WEAPON;

		CHECK_EQUAL(false, CSItemHelper::IsShieldItemType(&itemData));


		// 왼손 무기
		itemData.m_nItemSlot = ITEMSLOT_LWEAPON;
		itemData.m_ItemType = ITEMTYPE_WEAPON;

		CHECK_EQUAL(false, CSItemHelper::IsShieldItemType(&itemData));

	}

	TEST(TestItemHelper_IsGuardableWeaponType)
	{
		CHECK_EQUAL(true, CSItemHelper::IsGuardableWeaponType(WEAPON_1H_SLASH));
		CHECK_EQUAL(true, CSItemHelper::IsGuardableWeaponType(WEAPON_1H_BLUNT));
		CHECK_EQUAL(false, CSItemHelper::IsGuardableWeaponType(WEAPON_1H_PIERCE));
		CHECK_EQUAL(true, CSItemHelper::IsGuardableWeaponType(WEAPON_TWO_HANDED));
		CHECK_EQUAL(true, CSItemHelper::IsGuardableWeaponType(WEAPON_STAFF));
		CHECK_EQUAL(false, CSItemHelper::IsGuardableWeaponType(WEAPON_ARCHERY));
		CHECK_EQUAL(true, CSItemHelper::IsGuardableWeaponType(WEAPON_2H_BLUNT));
	}

	TEST(TestItemHelper_IsLeftWeaponSlot)
	{
		CHECK_EQUAL(false, CSItemHelper::IsLeftWeaponSlot(ITEMSLOT_HEAD));
		CHECK_EQUAL(true, CSItemHelper::IsLeftWeaponSlot(ITEMSLOT_LWEAPON));
		CHECK_EQUAL(true, CSItemHelper::IsLeftWeaponSlot(ITEMSLOT_LWEAPON2));
		CHECK_EQUAL(false, CSItemHelper::IsLeftWeaponSlot(ITEMSLOT_RWEAPON));
		CHECK_EQUAL(false, CSItemHelper::IsLeftWeaponSlot(ITEMSLOT_RWEAPON2));
	}

	TEST(TestItemHelper_IsRightWeaponSlot)
	{
		CHECK_EQUAL(false, CSItemHelper::IsRightWeaponSlot(ITEMSLOT_HEAD));
		CHECK_EQUAL(false, CSItemHelper::IsRightWeaponSlot(ITEMSLOT_LWEAPON));
		CHECK_EQUAL(false, CSItemHelper::IsRightWeaponSlot(ITEMSLOT_LWEAPON2));
		CHECK_EQUAL(true, CSItemHelper::IsRightWeaponSlot(ITEMSLOT_RWEAPON));
		CHECK_EQUAL(true, CSItemHelper::IsRightWeaponSlot(ITEMSLOT_RWEAPON2));
	}

	TEST(TestItemHelper_IsWeaponSlot)
	{
		CHECK_EQUAL(false, CSItemHelper::IsWeaponSlot(ITEMSLOT_HEAD));
		CHECK_EQUAL(false, CSItemHelper::IsWeaponSlot(ITEMSLOT_LEG));
		CHECK_EQUAL(true, CSItemHelper::IsWeaponSlot(ITEMSLOT_LWEAPON));
		CHECK_EQUAL(true, CSItemHelper::IsWeaponSlot(ITEMSLOT_LWEAPON2));
		CHECK_EQUAL(true, CSItemHelper::IsWeaponSlot(ITEMSLOT_RWEAPON));
		CHECK_EQUAL(true, CSItemHelper::IsWeaponSlot(ITEMSLOT_RWEAPON2));
	}

	TEST(TestItemHelper_IsPrimaryWeaponSlot)
	{
		CHECK_EQUAL(false, CSItemHelper::IsPrimaryWeaponSlot(ITEMSLOT_HEAD));
		CHECK_EQUAL(false, CSItemHelper::IsPrimaryWeaponSlot(ITEMSLOT_LEG));
		CHECK_EQUAL(true, CSItemHelper::IsPrimaryWeaponSlot(ITEMSLOT_LWEAPON));
		CHECK_EQUAL(false, CSItemHelper::IsPrimaryWeaponSlot(ITEMSLOT_LWEAPON2));
		CHECK_EQUAL(true, CSItemHelper::IsPrimaryWeaponSlot(ITEMSLOT_RWEAPON));
		CHECK_EQUAL(false, CSItemHelper::IsPrimaryWeaponSlot(ITEMSLOT_RWEAPON2));
	}

	TEST(TestItemHelper_IsSecondaryWeaponSlot)
	{
		CHECK_EQUAL(false, CSItemHelper::IsSecondaryWeaponSlot(ITEMSLOT_HEAD));
		CHECK_EQUAL(false, CSItemHelper::IsSecondaryWeaponSlot(ITEMSLOT_LEG));
		CHECK_EQUAL(false, CSItemHelper::IsSecondaryWeaponSlot(ITEMSLOT_LWEAPON));
		CHECK_EQUAL(true, CSItemHelper::IsSecondaryWeaponSlot(ITEMSLOT_LWEAPON2));
		CHECK_EQUAL(false, CSItemHelper::IsSecondaryWeaponSlot(ITEMSLOT_RWEAPON));
		CHECK_EQUAL(true, CSItemHelper::IsSecondaryWeaponSlot(ITEMSLOT_RWEAPON2));
	}

	TEST(TestItemHelper_GetPrimaryWeaponSlot)
	{
		CHECK_EQUAL(ITEMSLOT_LWEAPON, CSItemHelper::GetPrimaryWeaponSlot(ITEMSLOT_LWEAPON2));
		CHECK_EQUAL(ITEMSLOT_RWEAPON, CSItemHelper::GetPrimaryWeaponSlot(ITEMSLOT_RWEAPON2));
		CHECK_EQUAL(ITEMSLOT_NONE, CSItemHelper::GetPrimaryWeaponSlot(ITEMSLOT_HEAD));
	}

	TEST(TestItemHelper_GetSecondaryWeaponSlot)
	{
		CHECK_EQUAL(ITEMSLOT_LWEAPON2, CSItemHelper::GetSecondaryWeaponSlot(ITEMSLOT_LWEAPON));
		CHECK_EQUAL(ITEMSLOT_RWEAPON2, CSItemHelper::GetSecondaryWeaponSlot(ITEMSLOT_RWEAPON));
		CHECK_EQUAL(ITEMSLOT_NONE, CSItemHelper::GetSecondaryWeaponSlot(ITEMSLOT_HEAD));
	}

	TEST(TestItemHelper_GetOppositeWeaponSlot)
	{
		CHECK_EQUAL(ITEMSLOT_NONE, CSItemHelper::GetOppositeWeaponSlot(ITEMSLOT_HEAD));
		CHECK_EQUAL(ITEMSLOT_NONE, CSItemHelper::GetOppositeWeaponSlot(ITEMSLOT_LEG));
		CHECK_EQUAL(ITEMSLOT_RWEAPON, CSItemHelper::GetOppositeWeaponSlot(ITEMSLOT_LWEAPON));
		CHECK_EQUAL(ITEMSLOT_RWEAPON2, CSItemHelper::GetOppositeWeaponSlot(ITEMSLOT_LWEAPON2));
		CHECK_EQUAL(ITEMSLOT_LWEAPON, CSItemHelper::GetOppositeWeaponSlot(ITEMSLOT_RWEAPON));
		CHECK_EQUAL(ITEMSLOT_LWEAPON2, CSItemHelper::GetOppositeWeaponSlot(ITEMSLOT_RWEAPON2));
	}

	TEST(TestItemHelper_GetSubSlot)
	{
		GItemData* pRSubData = test.item.NewItemData();
		pRSubData->m_nSubSlot = ITEMSLOT_RWEAPON;
		GItemData* pLSubData = test.item.NewItemData();
		pLSubData->m_nSubSlot = ITEMSLOT_LWEAPON;

		CHECK_EQUAL(ITEMSLOT_RWEAPON2, CSItemHelper::GetSubSlot(ITEMSLOT_LWEAPON2, pRSubData));
		CHECK_EQUAL(ITEMSLOT_LWEAPON2, CSItemHelper::GetSubSlot(ITEMSLOT_RWEAPON2, pLSubData));	
		CHECK_EQUAL(ITEMSLOT_RWEAPON, CSItemHelper::GetSubSlot(ITEMSLOT_LWEAPON, pRSubData));
		CHECK_EQUAL(ITEMSLOT_LWEAPON, CSItemHelper::GetSubSlot(ITEMSLOT_RWEAPON, pLSubData));		
	}

	
	TEST(TestItemHelper_IsEquipableSlot)
	{
		GItemData* pRWeaponData = test.item.NewItemData();
		pRWeaponData->m_nItemSlot = ITEMSLOT_RWEAPON;
		GItemData* pLWeaponData = test.item.NewItemData();
		pLWeaponData->m_nItemSlot = ITEMSLOT_LWEAPON;
		GItemData* pLEquipableRWeaponData = test.item.NewItemData();
		pLEquipableRWeaponData->m_nItemSlot = ITEMSLOT_RWEAPON;
		pLEquipableRWeaponData->m_bLeftWeaponEquipable = true;
		GItemData* pRFingerData = test.item.NewItemData();
		pRFingerData->m_nItemSlot = ITEMSLOT_RFINGER;
		GItemData* pLFingerData = test.item.NewItemData();
		pLFingerData->m_nItemSlot = ITEMSLOT_LFINGER;
	
		// 오른손무기지만 왼손에도 장착가능한 아이템은 왼손무기슬롯에 장비할수 있다.
		CHECK_EQUAL(true, CSItemHelper::IsEquipableSlot(ITEMSLOT_LWEAPON, pLEquipableRWeaponData));

		// 반지는 왼손 오른손 상관없이 장비할수 있다.
		CHECK_EQUAL(true, CSItemHelper::IsEquipableSlot(ITEMSLOT_RFINGER, pLFingerData));
		CHECK_EQUAL(true, CSItemHelper::IsEquipableSlot(ITEMSLOT_LFINGER, pRFingerData));		

		// 무기는 세컨더리 슬롯에 장비할수 있다.
		CHECK_EQUAL(true, CSItemHelper::IsEquipableSlot(ITEMSLOT_RWEAPON2, pRWeaponData));
		CHECK_EQUAL(true, CSItemHelper::IsEquipableSlot(ITEMSLOT_LWEAPON2, pLWeaponData));

		// 위 경우외에 슬롯이 다르면 장비할수 없다.
		CHECK_EQUAL(false, CSItemHelper::IsEquipableSlot(ITEMSLOT_LWEAPON, pRWeaponData));
	}


	TEST(TestItemHelper_IsTogetherEquipableItemData)
	{
		GItemData* p1HSData = test.item.NewWeaponItemData(INVALID_ID, WEAPON_1H_SLASH);
		GItemData* p1HBData = test.item.NewWeaponItemData(INVALID_ID, WEAPON_1H_BLUNT);
		GItemData* p1HPData = test.item.NewWeaponItemData(INVALID_ID, WEAPON_1H_PIERCE);
		GItemData* pShieldData = test.item.NewShieldItemData(INVALID_ID);

		// 방패는 한손둔기와 한손검과만 함께 착용할수 있다. 
		CHECK_EQUAL(true, CSItemHelper::IsTogetherEquipableItemData(p1HSData, pShieldData));
		CHECK_EQUAL(true, CSItemHelper::IsTogetherEquipableItemData(p1HBData, pShieldData));
		CHECK_EQUAL(false, CSItemHelper::IsTogetherEquipableItemData(p1HPData, pShieldData));

		// 단검은 단검끼리만 같이 낄수 있다.
		CHECK_EQUAL(true, CSItemHelper::IsTogetherEquipableItemData(p1HPData, p1HPData));
		CHECK_EQUAL(false, CSItemHelper::IsTogetherEquipableItemData(p1HPData, p1HSData));

		// 쌍수 무기는 쌍수무기와만 같이 착용 할수 있다.
		CHECK_EQUAL(true, CSItemHelper::IsTogetherEquipableItemData(p1HSData, p1HSData));
		CHECK_EQUAL(true, CSItemHelper::IsTogetherEquipableItemData(p1HBData, p1HBData));
		CHECK_EQUAL(true, CSItemHelper::IsTogetherEquipableItemData(p1HSData, p1HBData));		
		CHECK_EQUAL(false, CSItemHelper::IsTogetherEquipableItemData(p1HSData, p1HPData));
	}

	TEST(TestItemHelper_IsEquipableWithShield)
	{
		// 방패만도 장착 가능
		CHECK_EQUAL(true, CSItemHelper::IsEquipableWithShield(WEAPON_NONE));

		CHECK_EQUAL(true, CSItemHelper::IsEquipableWithShield(WEAPON_1H_SLASH));
		CHECK_EQUAL(true, CSItemHelper::IsEquipableWithShield(WEAPON_1H_BLUNT));

		CHECK_EQUAL(false, CSItemHelper::IsEquipableWithShield(WEAPON_1H_PIERCE));
		CHECK_EQUAL(false, CSItemHelper::IsEquipableWithShield(WEAPON_TWO_HANDED));
		CHECK_EQUAL(false, CSItemHelper::IsEquipableWithShield(WEAPON_STAFF));
		CHECK_EQUAL(false, CSItemHelper::IsEquipableWithShield(WEAPON_ARCHERY));
		CHECK_EQUAL(false, CSItemHelper::IsEquipableWithShield(WEAPON_2H_BLUNT));
		CHECK_EQUAL(false, CSItemHelper::IsEquipableWithShield(WEAPON_DUAL_WIELD));
		CHECK_EQUAL(false, CSItemHelper::IsEquipableWithShield(WEAPON_DUAL_PIERCE));
	}

	TEST(TestItemHelper_GetProperWeaponSlot)
	{
		CSItemData leftWeaponItemType;
		CSItemData rightWeaponItemType;
		CSItemData shieldItemType;

		leftWeaponItemType.m_ItemType = ITEMTYPE_WEAPON;
		rightWeaponItemType.m_ItemType = ITEMTYPE_WEAPON;
		shieldItemType.m_ItemType = ITEMTYPE_ARMOR;
		shieldItemType.m_nItemSlot = ITEMSLOT_LWEAPON;

		leftWeaponItemType.m_nWeaponType = WEAPON_1H_SLASH;
		rightWeaponItemType.m_nWeaponType = WEAPON_1H_SLASH;

		CSItemData itemData;
		itemData.m_ItemType = ITEMTYPE_WEAPON;
		itemData.m_nWeaponType = WEAPON_1H_SLASH;

		// 1Slash
		CHECK_EQUAL(ITEMSLOT_RWEAPON, CSItemHelper::GetProperWeaponSlot(ITEMSLOT_RWEAPON, &itemData, NULL, NULL));
		CHECK_EQUAL(ITEMSLOT_RWEAPON2, CSItemHelper::GetProperWeaponSlot(ITEMSLOT_RWEAPON2, &itemData, NULL, NULL));

		CHECK_EQUAL(ITEMSLOT_RWEAPON, CSItemHelper::GetProperWeaponSlot(ITEMSLOT_LWEAPON, &itemData, NULL, NULL));
		CHECK_EQUAL(ITEMSLOT_RWEAPON2, CSItemHelper::GetProperWeaponSlot(ITEMSLOT_LWEAPON2, &itemData, NULL, NULL));

		rightWeaponItemType.m_nWeaponType = WEAPON_1H_SLASH;
		CHECK_EQUAL(ITEMSLOT_LWEAPON, CSItemHelper::GetProperWeaponSlot(ITEMSLOT_LWEAPON, &itemData, &rightWeaponItemType, NULL));
		CHECK_EQUAL(ITEMSLOT_LWEAPON2, CSItemHelper::GetProperWeaponSlot(ITEMSLOT_LWEAPON2, &itemData, &rightWeaponItemType, NULL));

		rightWeaponItemType.m_nWeaponType = WEAPON_1H_BLUNT;
		CHECK_EQUAL(ITEMSLOT_LWEAPON, CSItemHelper::GetProperWeaponSlot(ITEMSLOT_LWEAPON, &itemData, &rightWeaponItemType, NULL));
		CHECK_EQUAL(ITEMSLOT_LWEAPON2, CSItemHelper::GetProperWeaponSlot(ITEMSLOT_LWEAPON2, &itemData, &rightWeaponItemType, NULL));

		rightWeaponItemType.m_nWeaponType = WEAPON_1H_PIERCE;
		CHECK_EQUAL(ITEMSLOT_RWEAPON, CSItemHelper::GetProperWeaponSlot(ITEMSLOT_LWEAPON, &itemData, &rightWeaponItemType, NULL));
		CHECK_EQUAL(ITEMSLOT_RWEAPON2, CSItemHelper::GetProperWeaponSlot(ITEMSLOT_LWEAPON2, &itemData, &rightWeaponItemType, NULL));

		rightWeaponItemType.m_nWeaponType = WEAPON_TWO_HANDED;
		CHECK_EQUAL(ITEMSLOT_RWEAPON, CSItemHelper::GetProperWeaponSlot(ITEMSLOT_LWEAPON, &itemData, &rightWeaponItemType, NULL));
		CHECK_EQUAL(ITEMSLOT_RWEAPON2, CSItemHelper::GetProperWeaponSlot(ITEMSLOT_LWEAPON2, &itemData, &rightWeaponItemType, NULL));

		rightWeaponItemType.m_nWeaponType = WEAPON_1H_SLASH;
		CHECK_EQUAL(ITEMSLOT_LWEAPON, CSItemHelper::GetProperWeaponSlot(ITEMSLOT_LWEAPON, &itemData, &rightWeaponItemType, &leftWeaponItemType));
		CHECK_EQUAL(ITEMSLOT_LWEAPON2, CSItemHelper::GetProperWeaponSlot(ITEMSLOT_LWEAPON2, &itemData, &rightWeaponItemType, &leftWeaponItemType));

		rightWeaponItemType.m_nWeaponType = WEAPON_1H_SLASH;
		CHECK_EQUAL(ITEMSLOT_RWEAPON, CSItemHelper::GetProperWeaponSlot(ITEMSLOT_LWEAPON, &itemData, &rightWeaponItemType, &shieldItemType));
		CHECK_EQUAL(ITEMSLOT_RWEAPON2, CSItemHelper::GetProperWeaponSlot(ITEMSLOT_LWEAPON2, &itemData, &rightWeaponItemType, &shieldItemType));

		// 1Pierce
		itemData.m_nWeaponType = WEAPON_1H_PIERCE;

		CHECK_EQUAL(ITEMSLOT_RWEAPON, CSItemHelper::GetProperWeaponSlot(ITEMSLOT_RWEAPON, &itemData, NULL, NULL));
		CHECK_EQUAL(ITEMSLOT_RWEAPON2, CSItemHelper::GetProperWeaponSlot(ITEMSLOT_RWEAPON2, &itemData, NULL, NULL));

		CHECK_EQUAL(ITEMSLOT_RWEAPON, CSItemHelper::GetProperWeaponSlot(ITEMSLOT_LWEAPON, &itemData, NULL, NULL));
		CHECK_EQUAL(ITEMSLOT_RWEAPON2, CSItemHelper::GetProperWeaponSlot(ITEMSLOT_LWEAPON2, &itemData, NULL, NULL));

		rightWeaponItemType.m_nWeaponType = WEAPON_1H_SLASH;
		CHECK_EQUAL(ITEMSLOT_RWEAPON, CSItemHelper::GetProperWeaponSlot(ITEMSLOT_LWEAPON, &itemData, &rightWeaponItemType, NULL));
		CHECK_EQUAL(ITEMSLOT_RWEAPON2, CSItemHelper::GetProperWeaponSlot(ITEMSLOT_LWEAPON2, &itemData, &rightWeaponItemType, NULL));

		rightWeaponItemType.m_nWeaponType = WEAPON_1H_PIERCE;
		CHECK_EQUAL(ITEMSLOT_LWEAPON, CSItemHelper::GetProperWeaponSlot(ITEMSLOT_LWEAPON, &itemData, &rightWeaponItemType, NULL));
		CHECK_EQUAL(ITEMSLOT_LWEAPON2, CSItemHelper::GetProperWeaponSlot(ITEMSLOT_LWEAPON2, &itemData, &rightWeaponItemType, NULL));
	}	
}