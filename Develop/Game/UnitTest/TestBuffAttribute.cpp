#include "stdafx.h"
#include "XBuffAttribute.h"
#include "FBaseTalentInfo.h"

SUITE(BuffAttribute)
{
	class FBuffAttribute : public FBaseTalentInfo
	{
	public:
		FBuffAttribute()
		{

		}
		virtual ~FBuffAttribute()
		{

		}

		XBuffInfo			m_BuffInfo;
		XBuffAttribute		m_BuffAttribute;
	};

	TEST_FIXTURE(FBuffAttribute, TestBuffAttribute_IsActivated)
	{
		CHECK_EQUAL(false, m_BuffAttribute.IsMesmerizable());

		m_BuffInfo.m_nPassiveExtraAttrib = BUFPEA_ROOT;
		m_BuffAttribute.ParseBuffAttr(&m_BuffInfo);

		CHECK_EQUAL(true, m_BuffAttribute.IsMesmerizable());

		m_BuffInfo.m_nPassiveExtraAttrib = BUFPEA_NONE;
		m_BuffInfo.m_nPassiveExtraAttrib2 = BUFPEA_SLEEP;
		m_BuffAttribute.ParseBuffAttr(&m_BuffInfo);

		CHECK_EQUAL(true, m_BuffAttribute.IsMesmerizable());
	}

	TEST_FIXTURE(FBuffAttribute, TestBuffAttribute_IsAttackableMelee)
	{
		CHECK_EQUAL(true, m_BuffAttribute.IsAttackableMelee());

		m_BuffInfo.m_nPassiveExtraAttrib = BUFPEA_DISABLE_TALENT_MELEE;
		m_BuffAttribute.ParseBuffAttr(&m_BuffInfo);

		CHECK_EQUAL(false, m_BuffAttribute.IsAttackableMelee());

		m_BuffInfo.m_nPassiveExtraAttrib = BUFPEA_NONE;
		m_BuffInfo.m_nPassiveExtraAttrib2 = BUFPEA_DISABLE_TALENT_MELEE;
		m_BuffAttribute.ParseBuffAttr(&m_BuffInfo);

		CHECK_EQUAL(false, m_BuffAttribute.IsAttackableMelee());
	}

	TEST_FIXTURE(FBuffAttribute, TestBuffAttribute_IsAttackableRange)
	{
		CHECK_EQUAL(true, m_BuffAttribute.IsAttackableRange());

		m_BuffInfo.m_nPassiveExtraAttrib = BUFPEA_DISABLE_TALENT_RANGE;
		m_BuffAttribute.ParseBuffAttr(&m_BuffInfo);

		CHECK_EQUAL(false, m_BuffAttribute.IsAttackableRange());

		m_BuffInfo.m_nPassiveExtraAttrib = BUFPEA_NONE;
		m_BuffInfo.m_nPassiveExtraAttrib2 = BUFPEA_DISABLE_TALENT_RANGE;
		m_BuffAttribute.ParseBuffAttr(&m_BuffInfo);

		CHECK_EQUAL(false, m_BuffAttribute.IsAttackableRange());
	}

	TEST_FIXTURE(FBuffAttribute, TestBuffAttribute_IsAttackableMagic)
	{
		CHECK_EQUAL(true, m_BuffAttribute.IsAttackableMagic());

		m_BuffInfo.m_nPassiveExtraAttrib = BUFPEA_DISABLE_TALENT_MAGIC;
		m_BuffAttribute.ParseBuffAttr(&m_BuffInfo);

		CHECK_EQUAL(false, m_BuffAttribute.IsAttackableMagic());

		m_BuffInfo.m_nPassiveExtraAttrib = BUFPEA_NONE;
		m_BuffInfo.m_nPassiveExtraAttrib2 = BUFPEA_DISABLE_TALENT_MAGIC;
		m_BuffAttribute.ParseBuffAttr(&m_BuffInfo);

		CHECK_EQUAL(false, m_BuffAttribute.IsAttackableMagic());
	}

	TEST_FIXTURE(FBuffAttribute, TestBuffAttribute_IsMovable)
	{
		CHECK_EQUAL(true, m_BuffAttribute.IsMovable());

		m_BuffInfo.m_nPassiveExtraAttrib = BUFPEA_ROOT;
		m_BuffAttribute.ParseBuffAttr(&m_BuffInfo);

		CHECK_EQUAL(false, m_BuffAttribute.IsMovable());

		m_BuffInfo.m_nPassiveExtraAttrib = BUFPEA_NONE;
		m_BuffInfo.m_nPassiveExtraAttrib2 = BUFPEA_ROOT;
		m_BuffAttribute.ParseBuffAttr(&m_BuffInfo);

		CHECK_EQUAL(false, m_BuffAttribute.IsMovable());
	}

}