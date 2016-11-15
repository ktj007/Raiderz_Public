#include "stdafx.h"
#include "LUnitTestRunner.h"
#include "LUnitTestStage1.h"

bool LUnitTestRunner::BeDoTest()
{
	// publish이면 유닛테스트 안함.
#ifdef _PUBLISH
	return false;
#endif

	return MUnitTestRunner::BeDoTest();
}

bool LUnitTestRunner::RunStage1()
{
	return Run<LUnitTestStage1>();
}

