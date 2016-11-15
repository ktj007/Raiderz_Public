#include "stdafx.h"
#include "XMotionHelper.h"

SUITE(MotionHelper)
{
	TEST(TestMotionHelper_IsIdleMotion)
	{
		CHECK_EQUAL(true, XMotionHelper::IsIdleMotion(wstring(MOTION_NAME_IDLE)));
		CHECK_EQUAL(true, XMotionHelper::IsIdleMotion(wstring(MOTION_NAME_SWIM_IDLE)));
		CHECK_EQUAL(true, XMotionHelper::IsIdleMotion(wstring(MOTION_NAME_NPC_IDLE)));
		CHECK_EQUAL(true, XMotionHelper::IsIdleMotion(wstring(MOTION_NAME_NPC_IDLE_COMBAT)));
	}
	TEST(TestMotionHelper_IsIdleMotion_Netagive)
	{
		CHECK_EQUAL(false, XMotionHelper::IsIdleMotion(wstring(MOTION_NAME_RUN)));
		CHECK_EQUAL(false, XMotionHelper::IsIdleMotion(wstring(MOTION_NAME_ROTATE)));
	}

}