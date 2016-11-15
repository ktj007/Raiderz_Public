#include "stdafx.h"
#include "ZUnitTestRunner.h"
#include "ZUnitTestStage1.h"

bool ZUnitTestRunner::BeDoTest()
{
	// publish이면 유닛테스트 안함.
#ifdef _PUBLISH
	return false;
#endif

	return MUnitTestRunner::BeDoTest();
}

bool ZUnitTestRunner::RunStage1()
{
	return Run<ZUnitTestStage1>();
}

ZUnitTestRunner::ZUnitTestRunner() : MUnitTestRunner()
{
	InitCodeCoverage();
}

ZUnitTestRunner::~ZUnitTestRunner()
{
	CC_End();
}

void ZUnitTestRunner::InitCodeCoverage()
{
	if (m_Config.m_bCheckCodeCoverage == false) return;

	char* CodeCoverageItems[] = {

#define CC_Add(e) #e,

#include "CodeCoverages.inc"

	};

#undef CC_Add

	CC_Begin(CodeCoverageItems, m_Config.m_strCodeCoverageFilter.c_str());
}