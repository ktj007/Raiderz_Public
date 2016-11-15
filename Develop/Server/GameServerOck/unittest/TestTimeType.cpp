#include "stdafx.h"
#include "GTimeType.h"


SUITE(TestTimeType)
{
	TEST(TestTimeType_Sec)
	{
		const time_t tSecOffset = 10;

		GTimeSec tSec(tSecOffset);

		CHECK_EQUAL(tSecOffset, tSec.GetSec());
	}

	TEST(TestTimeType_Day)
	{
		const time_t tDayOffset = 10;

		GTimeDay tDay(tDayOffset);

		CHECK_EQUAL(tDayOffset * GTimeDay::ONE_DAY, tDay.GetSec());
	}
}