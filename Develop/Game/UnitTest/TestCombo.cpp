#include "stdafx.h"
#include "XCombo.h"

SUITE(COMBO)
{
	class FCombo
	{
	public:
		FCombo()
		{

		}
		~FCombo()
		{

		}
		XCombo m_Combo;
	};

	TEST_FIXTURE(FCombo, TestComboNextNormalAttack)
	{
		//std::auto_ptr<TestMyActionNormalAttack> apMyActionNormalAttack(new TestMyActionNormalAttack(m_pModuleMyControl));

		CHECK_EQUAL(m_Combo.GetNextNormalAttack(), NA_NORMAL1);

		m_Combo.OnAttack(NA_NORMAL1);
		CHECK_EQUAL(m_Combo.GetNextNormalAttack(), NA_NORMAL2);

		m_Combo.OnAttack(NA_NORMAL2);
		CHECK_EQUAL(m_Combo.GetNextNormalAttack(), NA_NORMAL3);

		m_Combo.OnAttack(NA_NORMAL3);
		CHECK_EQUAL(m_Combo.GetNextNormalAttack(), NA_NORMAL4);

		m_Combo.OnAttack(NA_NORMAL4);
		CHECK_EQUAL(m_Combo.GetNextNormalAttack(), NA_NORMAL5);

		m_Combo.OnAttack(NA_NORMAL5);
		CHECK_EQUAL(m_Combo.GetNextNormalAttack(), NA_NORMAL6);

		m_Combo.OnAttack(NA_NORMAL6);
		CHECK_EQUAL(m_Combo.GetNextNormalAttack(), NA_NORMAL1);

	}

	TEST_FIXTURE(FCombo, TestNextNormalAttack_Combo)
	{
		m_Combo.Init();

		CHECK_EQUAL(m_Combo.IsReserved(), false);
		CHECK_EQUAL(m_Combo.GetNextAttackType(), NA_NA);


		m_Combo.Reserve(NA_NORMAL1);
		CHECK_EQUAL(m_Combo.IsReserved(), true);
		CHECK_EQUAL(m_Combo.GetNextAttackType(), NA_NORMAL1);

		m_Combo.ReleaseReserved();
		CHECK_EQUAL(m_Combo.IsReserved(), false);
		CHECK_EQUAL(m_Combo.GetNextAttackType(), NA_NA);
		CHECK_EQUAL(m_Combo.CheckCanReserveCombo(), true);
	}

	TEST_FIXTURE(FCombo, TestComboEnable)
	{
		m_Combo.Init();

		m_Combo.OnAttack(NA_NORMAL1);
		CHECK_EQUAL(m_Combo.CheckCanReserveCombo(), true);

		m_Combo.OnAttack(NA_NORMAL2);
		CHECK_EQUAL(m_Combo.CheckCanReserveCombo(), true);

		m_Combo.OnAttack(NA_NORMAL3);
		CHECK_EQUAL(m_Combo.CheckCanReserveCombo(), true);

		m_Combo.OnAttack(NA_NORMAL4);
		CHECK_EQUAL(m_Combo.CheckCanReserveCombo(), true);

		m_Combo.OnAttack(NA_NORMAL5);
		CHECK_EQUAL(m_Combo.CheckCanReserveCombo(), true);

		m_Combo.OnAttack(NA_NORMAL6);
		CHECK_EQUAL(m_Combo.CheckCanReserveCombo(), false);

	}

}