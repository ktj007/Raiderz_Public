#include "stdafx.h"
#include "GUnitTestRunner.h"
#include "GUnitTestStage1.h"

bool GUnitTestRunner::BeDoTest()
{
	// publish이면 유닛테스트 안함.
#ifdef _PUBLISH
	return false;
#endif

	return MUnitTestRunner::BeDoTest();
}

bool GUnitTestRunner::RunStage1()
{
	return Run<GUnitTestStage1>();
}


GUnitTestRunner::GUnitTestRunner() : MUnitTestRunner()
{

}

GUnitTestRunner::~GUnitTestRunner()
{

}

