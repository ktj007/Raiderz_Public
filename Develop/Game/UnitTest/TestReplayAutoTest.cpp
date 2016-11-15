#include "stdafx.h"
#include "XReplayFrame.h"
#include "XReplayAutoTest.h"

SUITE(ReplayAutoTest)
{
	TEST(TestReplayTestData_AverageFPS)
	{
		XReplayTestData data;

		data.m_nGameStartTime = 1000;
		data.m_nGameEndTime = 2000;
		data.m_nTotalFrame = 10;

		CHECK_EQUAL(10, data.GetAverageFPS());
	}
}