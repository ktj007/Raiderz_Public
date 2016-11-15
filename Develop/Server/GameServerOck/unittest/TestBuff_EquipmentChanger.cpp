#include "stdafx.h"
#include "GTestForward.h"
#include "GModuleBuff.h"
#include "GBuffEquipmentChanger.h"
#include "FBaseGame.h"
#include "GItemHolder.h"
#include "GEquipmentSlot.h"
#include "CCommandResultTable.h"
#include "GModuleCombat.h"

SUITE(Buff_EquipmentChanger)
{
	struct FBuff : public FBaseGame
	{
		FBuff()
		{
		}

		virtual ~FBuff()
		{
		}

		WEAPON_TYPE GetPlayerWeaponType()
		{
			GItemHolder* pItemHolder = m_pPlayer->GetItemHolder();
			WEAPON_TYPE nCurrWeaponType = pItemHolder->GetEquipment().GetWeaponType();
			return nCurrWeaponType;
		}

		DECLWRAPPER_FieldInfoMgr;
		DECLWRAPPER_FieldMgr;
		DECLWRAPPER_BuffMgr;
		DECLWRAPPER_ItemMgr;
	};

	TEST_FIXTURE(FBuff, Change)
	{
		CHECK_EQUAL(WEAPON_NONE, GetPlayerWeaponType());

		GBuffInfo* pBuff1=test.buff.NewBuffInfo(123);
		pBuff1->m_fDuration = 5.0f;
		pBuff1->m_bUseChangeEquipmentSlot = true;
		pBuff1->m_nEquipItemIDs[ITEMSLOT_RWEAPON] = 777;

		GItemData* pItemData = test.item.NewItemData(777);
		pItemData->m_nWeaponType = WEAPON_1H_BLUNT;
		test.buff.GainBuffSelf(m_pPlayer, pBuff1);
		CHECK_EQUAL(WEAPON_1H_BLUNT, GetPlayerWeaponType());

		m_pPlayer->GetModuleBuff()->Update(pBuff1->m_fDuration);
		CHECK_EQUAL(WEAPON_NONE, GetPlayerWeaponType());
	}

	TEST_FIXTURE(FBuff, ApplyLastActionWhenDuplicated)
	{
		CHECK_EQUAL(WEAPON_NONE, GetPlayerWeaponType());

		GBuffInfo* pBuff1=test.buff.NewBuffInfo(123);
		pBuff1->m_fDuration = 5.0f;
		pBuff1->m_bUseChangeEquipmentSlot = true;
		pBuff1->m_nEquipItemIDs[ITEMSLOT_RWEAPON] = 777;

		GBuffInfo* pBuff2=test.buff.NewBuffInfo(124);
		pBuff2->m_fDuration = 5.0f;
		pBuff2->m_bUseChangeEquipmentSlot = true;
		pBuff2->m_nEquipItemIDs[ITEMSLOT_RWEAPON] = 888;

		{
			GItemData* pItemData = test.item.NewItemData(777);
			pItemData->m_nWeaponType = WEAPON_1H_BLUNT;
		}
		
		{
			GItemData* pItemData = test.item.NewItemData(888);
			pItemData->m_nWeaponType = WEAPON_1H_PIERCE;
		}

		test.buff.GainBuffSelf(m_pPlayer, pBuff1);
		CHECK_EQUAL(WEAPON_1H_BLUNT, GetPlayerWeaponType());

		m_pPlayer->GainBuff(124);
		CHECK_EQUAL(WEAPON_1H_PIERCE, GetPlayerWeaponType());

		m_pPlayer->GetModuleBuff()->Update(pBuff2->m_fDuration);
		CHECK_EQUAL(WEAPON_NONE, GetPlayerWeaponType());
	}

	TEST_FIXTURE(FBuff, EnableUseTalentInPalettes)
	{
		GTalentInfo* pTalentInfo = test.talent.NewTalentInfo(777);
		GTalentInfo* pTalentInfo2 = test.talent.NewTalentInfo(778);

		GBuffInfo* pBuff1=test.buff.NewBuffInfo(123);
		pBuff1->m_fDuration = 5.0f;
		pBuff1->m_bUseChangeEquipmentSlot = true;
		pBuff1->m_nEquipItemIDs[ITEMSLOT_RWEAPON] = 777;
		pBuff1->m_bUseChangePalette = true;
		pBuff1->m_arrPalettes[PALETTESLOT_5] = 777;
		{
			GItemData* pItemData = test.item.NewItemData(777);
			pItemData->m_nWeaponType = WEAPON_1H_BLUNT;
		}
		
		CHECK_EQUAL(CR_SUCCESS, m_pPlayer->GetModuleCombat()->UseTalent(pTalentInfo, m_pPlayer->GetUID(), true));
		CHECK_EQUAL(CR_SUCCESS, m_pPlayer->GetModuleCombat()->UseTalent(pTalentInfo2, m_pPlayer->GetUID(), true));

		test.buff.GainBuffSelf(m_pPlayer, pBuff1);
		CHECK_EQUAL(CR_SUCCESS, m_pPlayer->GetModuleCombat()->UseTalent(pTalentInfo, m_pPlayer->GetUID(), true));
		CHECK_EQUAL(CR_FAIL_USE_TALENT, m_pPlayer->GetModuleCombat()->UseTalent(pTalentInfo2, m_pPlayer->GetUID(), true));
	}
}
