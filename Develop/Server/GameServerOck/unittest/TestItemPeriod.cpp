#include "stdafx.h"
#include "GItemPeriod.h"


SUITE(ItemPeriod)
{
	TEST(RemainTimeSec)
	{
		GItemPeriod gp;

		const int nPeriod			= 10;
		const int nStartPlayTime	= 5;
		const int nOneSec			= 1;
		const int nRemainTime		= nPeriod - nStartPlayTime;

		gp.Set(false, false, nPeriod, nStartPlayTime);

		for (int i = 0; i < nPeriod; ++i)
		{
			CHECK_EQUAL(nPeriod - (nOneSec * i), gp.GetRemainTimeSec(nStartPlayTime + (nOneSec * i)));
		}		
	}

	TEST(GetPeriodSec)
	{
		GItemPeriod gp;

		const int nPeriod = 10;

		gp.Set(false, false, nPeriod, 0);

		CHECK_EQUAL(nPeriod, gp.GetPeriodSec());
	}

	TEST(PeriodItemExpired)
	{
		GItemPeriod gp;
		gp.Set(true, true, 10, 0);

		CHECK(!gp.IsExpired(9));
		CHECK(gp.IsExpired(10));
	}

	TEST(NotPeriodItemExpired)
	{
		GItemPeriod gp;

		gp.Set(false, true, 10, 0);

		CHECK(!gp.IsExpired(9));
		CHECK(!gp.IsExpired(10));		
	}
}