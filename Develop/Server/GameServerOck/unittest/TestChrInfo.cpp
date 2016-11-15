#include "stdafx.h"
#include "CSChrInfo.h"

SUITE(ChrInfo)
{
	TEST(TestPLAYER_INFO_God)
	{
		PLAYER_INFO pi;
		pi.SetGod(true);
		CHECK_EQUAL(true, pi.IsGod());

		pi.SetGod(false);
		CHECK_EQUAL(false, pi.IsGod());
	}

}