#include "stdafx.h"
#include "cache_value_type.h"


SUITE(TestDBCache_ModValueType)
{
	typedef cache_value_type<int>	mod_int;
	typedef cache_value_type<int16> mod_int16;

	TEST(Create)
	{
		mod_int v(0);

		CHECK_EQUAL(0, v.GetIncrease());
		CHECK_EQUAL(0, v.GetDecrease());
	}

	TEST(Zero)
	{
		mod_int v(0);
		v.Increase(1);
		v.Decrease(1);
		v.SetZero();

		CHECK_EQUAL(0, v.GetIncrease());
		CHECK_EQUAL(0, v.GetDecrease());		
	}

	TEST(MaxModValue)
	{
		const int nMaxModValue = 10;
		mod_int v(nMaxModValue);
		CHECK_EQUAL(nMaxModValue, v.GetMaxModifyRange());
	}

	TEST(ModOverflow_AddMaxValue)
	{
		const int16 nMaxModValue = 255;
		mod_int16 v(nMaxModValue);

		v.Increase(nMaxModValue);

		CHECK(v.IsOverflowMaxModifyRange());		
	}

	TEST(ModOverflow_AddBiggerThanMaxValue)
	{
		const int16 nMaxModValue = 1;
		mod_int16 v(nMaxModValue);
		v.Increase(nMaxModValue + 1);

		CHECK(v.IsOverflowMaxModifyRange());		
	}

	TEST(ModOverflow_MinusMaxValue)
	{
		const int16 nMaxModValue = 255;
		mod_int16 v(nMaxModValue);
		v.Decrease(nMaxModValue);
		CHECK(v.IsOverflowMaxModifyRange());		
	}

	TEST(ModOverflow_MinusBiggerThanMaxValue)
	{
		const int16 nMaxModValue = 1;
		mod_int16 v(nMaxModValue);
		v.Decrease(nMaxModValue + 1);
		CHECK(v.IsOverflowMaxModifyRange());	
	}

	TEST(ModNotOverflow_AddValue)
	{
		const int nMaxModValue = 2;
		mod_int v(nMaxModValue);

		v.Increase(1);

		CHECK(!v.IsOverflowMaxModifyRange());
		CHECK_EQUAL(1, v.GetIncrease());		
	}

	TEST(ModNotOverflow_MinusValue)
	{
		const int nMaxModValue = 2;
		mod_int v(nMaxModValue);
		v.Decrease(1);
		CHECK(!v.IsOverflowMaxModifyRange());		
	}

	TEST(ResetModOverflow)
	{
		const int nMaxModValue = 1;
		mod_int v(nMaxModValue);
		v.Increase(nMaxModValue);
		CHECK(v.IsOverflowMaxModifyRange());
		v.SetZero();
		CHECK(!v.IsOverflowMaxModifyRange());
	}

	TEST(GetModifiedCnt)
	{
		mod_int v(10);
		v.Increase(1);
		CHECK_EQUAL(1, v.GetModifiedCnt());
	}
}