#include "stdafx.h"
#include "LUnitTestRunner.h"
#include "LUnitTestStage1.h"
#include "SUnitTestStageGameDB.h"
#include "MCodeCoverage.h"

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

bool LUnitTestRunner::RunGameDBUnitTest()
{
	return Run<SUnitTestStageGameDB>();
}

LUnitTestRunner::LUnitTestRunner() : MUnitTestRunner()
{
	InitCodeCoverage();
}

LUnitTestRunner::~LUnitTestRunner()
{
	CC_End();
}

void LUnitTestRunner::InitCodeCoverage()
{
	if (m_Config.m_bCheckCodeCoverage == false) return;

	char* CodeCoverageItems[] = {

#define CC_Add(e) #e,

#include "CodeCoverages.inc"

	};

#undef CC_Add

	CC_Begin(CodeCoverageItems, m_Config.m_strCodeCoverageFilter.c_str());
}

