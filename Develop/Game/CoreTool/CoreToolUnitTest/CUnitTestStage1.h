#pragma once

#include "MBaseUnitTest.h"

#define UNITTEST_STAGE1_NAME		"Stage1"

class CUnitTestStage1 : public MBaseUnitTest
{
private:

public:
	CUnitTestStage1() {}
	virtual ~CUnitTestStage1() {}
	virtual void Create(MUnitTestConfig& config);
	virtual void Destroy();
	virtual void OnTestStart();
	virtual void OnTestFinish() {}

	virtual const char* GetName() { return UNITTEST_STAGE1_NAME; }
	virtual UnitTest::TestList const& GetTestList() { return UnitTest::Test::GetTestList(); }
	virtual const char* GetOutputFileName() { return "UnitTestStage1_Results.xml"; }
};
