#include "stdafx.h"
#include "PUnitTestRunner.h"
#include "PUnitTestStage1.h"

bool PUnitTestRunner::BeDoTest()
{
	// publish이면 유닛테스트 안함.
#ifdef _PUBLISH
	return false;
#endif

	return MUnitTestRunner::BeDoTest();
}

bool PUnitTestRunner::RunStage1()
{
	return Run<PUnitTestStage1>();
}

