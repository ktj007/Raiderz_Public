#include "stdafx.h"
#include "XSpecialActionTransition.h"
#include "TestGame.h"
#include "XTestWrapper.h"
#include "XMyPlayer.h"
#include "MockItemInfo.h"
#include "XUnitTestHelper.h"

SUITE(SpecialAction)
{
	struct FSpecialAction
	{
		FSpecialAction()
		{
			pMyPlayer = tg.GetMyPlayer();
		}
		~FSpecialAction()
		{

		}

		XTestInfoWrapper2<XItemManager, MockItemInfoMgr>	m_ItemInfoWrapper;
		XUnitTestHelper	helper;
		TestGame		tg;
		XMyPlayer*		pMyPlayer;
		XMyTalent		myTalent;
	};

	TEST_FIXTURE(FSpecialAction, TestTransChecker_Init)
	{
		XSpecialActionTransChecker checker(pMyPlayer, &myTalent);
		checker.Init();

		CHECK_EQUAL(pMyPlayer->GetStance(), CS_NORMAL);
		CHECK_EQUAL(checker.Trans(SPECIAL_KEY_DODGE_FRONT), NA_DODGE_FRONT);

		XUnitTestHelper::CreateWeaponItemData(1, WEAPON_1H_SLASH);
		XUnitTestHelper::CreateItemAndEquip(pMyPlayer, 1, ITEMSLOT_RWEAPON);

		pMyPlayer->SetStance(CS_BATTLE);
		CHECK_EQUAL(pMyPlayer->GetStance(), CS_BATTLE);
		CHECK_EQUAL(pMyPlayer->GetCurrWeaponType(), WEAPON_1H_SLASH);
		CHECK_EQUAL(checker.Trans(SPECIAL_KEY_ADVANCE_ATTACK), NA_ADVANCE);
	}

	class TestSpecialActionTransChecker : public XSpecialActionTransChecker
	{
	public:
		using XSpecialActionTransChecker::DefTrans;
		using XSpecialActionTransChecker::DefTrans_AllWeapons;
		using XSpecialActionTransChecker::DefConst;
		using XSpecialActionTransChecker::DefConst_AllWeapons;
		using XSpecialActionTransChecker::DefCombo;
		using XSpecialActionTransChecker::DefCombo_AllWeapons;
		using XSpecialActionTransChecker::DefCombo_1HS;
	public:
		TestSpecialActionTransChecker(XMyPlayer* pMyPlayer, XMyTalent* pMyTalent) : XSpecialActionTransChecker(pMyPlayer, pMyTalent) {}
	};


	TEST_FIXTURE(FSpecialAction, TestSpecialActionTransChecker_Trans)
	{
		XUnitTestHelper::CreateWeaponItemData(1, WEAPON_1H_SLASH);
		XUnitTestHelper::CreateItemAndEquip(pMyPlayer, 1, ITEMSLOT_RWEAPON);

		TestSpecialActionTransChecker checker(pMyPlayer, &myTalent);


		checker.DefTrans_AllWeapons(SPECIAL_KEY_DODGE_FRONT, NA_DODGE_FRONT);

		CHECK_EQUAL(checker.Trans(SPECIAL_KEY_DODGE_FRONT), NA_DODGE_FRONT);
		CHECK_EQUAL(checker.Trans(SPECIAL_KEY_DODGE_BACK), NA_NA);
	}


	TEST_FIXTURE(FSpecialAction, TestSpecialActionTransChecker_Const)
	{
		XUnitTestHelper::CreateWeaponItemData(1, WEAPON_1H_SLASH);
		XUnitTestHelper::CreateItemAndEquip(pMyPlayer, 1, ITEMSLOT_RWEAPON);

		TestSpecialActionTransChecker checker(pMyPlayer, &myTalent);

		checker.DefConst_AllWeapons(NA_NORMAL1, XNormalAttackTypeConstraint(TEPT_NONE, _STANCE_ALL, 0));
		checker.DefConst_AllWeapons(NA_NORMAL2, XNormalAttackTypeConstraint(TEPT_NONE, _STANCE_ALL, 0));
		checker.DefConst_AllWeapons(NA_NORMAL3, XNormalAttackTypeConstraint(TEPT_NONE, _STANCE_ALL, 0));

		// 콤보 체크
		checker.DefCombo_1HS(NA_NORMAL1, NA_NORMAL2);
		CHECK_EQUAL(checker.CheckConst(NA_NORMAL1, NA_NORMAL2), true);
		CHECK_EQUAL(checker.CheckConst(NA_NORMAL1, NA_NORMAL3), false);

		// 제약사항 체크
		checker.DefConst_AllWeapons(NA_COUNTER_ATTACK, XNormalAttackTypeConstraint(TEPT_COUNTER, _STANCE_ALL, 0));
		CHECK_EQUAL(checker.CheckConst(NA_NA, NA_COUNTER_ATTACK), true);

		checker.DefConst_AllWeapons(NA_SWORD_DRAWING, XNormalAttackTypeConstraint(TEPT_NONE, _STANCE_BATTLE, 0));
		CHECK_EQUAL(checker.CheckConst(NA_NA, NA_SWORD_DRAWING), false);

		checker.DefConst_AllWeapons(NA_SWORD_DRAWING, XNormalAttackTypeConstraint(TEPT_NONE, _STANCE_ALL, 0));
		CHECK_EQUAL(checker.CheckConst(NA_NA, NA_SWORD_DRAWING), true);

	} 

}