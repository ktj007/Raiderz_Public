#include "StdAfx.h"
#include "GTestForward.h"
#include "GTickRegulator.h"
#include "GFieldMgr.h"

SUITE(TickRegulator)
{
	TEST(TickOnce)
	{
		GTickRegulator rgr;
		rgr.SetTickTime(1.0f);	// 1초 단위 레귤레이터
		CHECK_EQUAL(0, rgr.GetTickCounter());
		rgr.Start();
		CHECK_EQUAL(false, rgr.IsReady(0.5f));	// 0.5초 지남
		CHECK_EQUAL(0, rgr.GetTickCounter());
		CHECK_EQUAL(true, rgr.IsReady(0.5f));	// 0.5초 지남
		CHECK_EQUAL(1, rgr.GetTickCounter());
	}

	TEST(TickOver)
	{
		GTickRegulator rgr;
		rgr.SetTickTime(1.0f);	// 1초 단위 레귤레이터
		CHECK_EQUAL(0, rgr.GetTickCounter());
		rgr.Start();
		CHECK_EQUAL(false, rgr.IsReady(0.5f));	// 0.5
		CHECK_EQUAL(0, rgr.GetTickCounter());
		CHECK_EQUAL(true, rgr.IsReady(1.8f));	// 2.3
		CHECK_EQUAL(2, rgr.GetTickCounter());	// 틱 카운터 초기화
		CHECK_EQUAL(true, rgr.IsReady(0.71f));	// 0.3+0.7
		CHECK_EQUAL(1, rgr.GetTickCounter());
		CHECK_EQUAL(false, rgr.IsReady(0.5f));	// 0.5
		CHECK_EQUAL(0, rgr.GetTickCounter());
		CHECK_EQUAL(true, rgr.IsReady(0.5f));	// 1.0
		CHECK_EQUAL(1, rgr.GetTickCounter());
	}
}