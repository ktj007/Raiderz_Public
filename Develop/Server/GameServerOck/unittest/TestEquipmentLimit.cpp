#include "StdAfx.h"
#include "GTestForward.h"
#include "GEntityPlayer.h"
#include "GEquipmentLimiter.h"
#include "GItemManager.h"
#include "FBasePlayer.h"
#include "GItemEquiper.h"
#include "GItemHolder.h"

SUITE(TestEquipmentLimit)
{
	struct FTestEquipmentLimit : public FBasePlayer
	{
		FTestEquipmentLimit()
		{ 
			m_pField = GUTHelper_Field::DefaultMockField();
			m_pPlayer = NewEntityPlayer(m_pField, vec3(100,100,0));
			swprintf_s(m_pPlayer->GetPlayerInfo()->szName, L"Tester");

			GItemHolder* pItemHolder = m_pPlayer->GetItemHolder();
			m_pEquipmentLimiter = new GEquipmentLimiter(pItemHolder);
		}

		~FTestEquipmentLimit() 
		{
			SAFE_DELETE(m_pEquipmentLimiter);
			m_pField->Destroy();
		}

		MockField*		m_pField;
		GEntityPlayer*	m_pPlayer;
		GEquipmentLimiter* m_pEquipmentLimiter;
		GInventory*			m_pInven;
		DECLWRAPPER_ItemMgr;
	};

	TEST_FIXTURE(FTestEquipmentLimit, EmptyCondition)
	{
		// 아무 조건도 등록시키지 않으면 사용가능하다.
		CHECK_EQUAL(true, m_pEquipmentLimiter->IsEnable());
	}

	TEST_FIXTURE(FTestEquipmentLimit, OneCondition)
	{
		// 조건이 하나라도 있으면 사용할 수 없다.
		m_pEquipmentLimiter->AddEnableWeapon(WEAPON_1H_SLASH);
		CHECK_EQUAL(false, m_pEquipmentLimiter->IsEnable());
	}

	TEST_FIXTURE(FTestEquipmentLimit, WeaponCheck)
	{
		// 슬래쉬와 피어스 타입의 무기만 허용함
		m_pEquipmentLimiter->AddEnableWeapon(WEAPON_1H_SLASH);
		m_pEquipmentLimiter->AddEnableWeapon(WEAPON_1H_PIERCE);
		CHECK_EQUAL(false, m_pEquipmentLimiter->IsEnable());

		// 무기 정보 생성
		GItem* pWeaponSlash		= test.item.GainWeaponItem(m_pPlayer, 1001, WEAPON_1H_SLASH);
		GItem* pWeaponPierce	= test.item.GainWeaponItem(m_pPlayer, 1002, WEAPON_1H_PIERCE);
		GItem* pWeaponBlunt		= test.item.GainWeaponItem(m_pPlayer, 1003, WEAPON_1H_BLUNT);

		// 블런트타입 무기 장착
		CHECK_EQUAL(true, GUTHelper_Item::EquipItem(m_pPlayer, pWeaponBlunt, ITEMSLOT_RWEAPON));
		CHECK_EQUAL(false, m_pEquipmentLimiter->IsEnable());	// 쓸 수 없는 무기타입

		// 슬래쉬타입 무기 장착
		CHECK_EQUAL(true, GUTHelper_Item::EquipItem(m_pPlayer, pWeaponSlash, ITEMSLOT_RWEAPON));
		CHECK_EQUAL(true, m_pEquipmentLimiter->IsEnable());

		// 피어스타입 무기 장착
		CHECK_EQUAL(true, GUTHelper_Item::EquipItem(m_pPlayer, pWeaponPierce, ITEMSLOT_RWEAPON));
		CHECK_EQUAL(true, m_pEquipmentLimiter->IsEnable());

		// 다시 블런트타입 무기 장착
		CHECK_EQUAL(true, GUTHelper_Item::EquipItem(m_pPlayer, pWeaponBlunt, ITEMSLOT_RWEAPON));
		CHECK_EQUAL(false, m_pEquipmentLimiter->IsEnable());	// 쓸 수 없는 무기타입
	}
}