#pragma once

#include "MBaseUnitTest.h"

#define UNITTEST_CUTSCENE_NAME		"CutScene"

class CUnitTestCutScene : public MBaseUnitTest
{
private:

public:
	CUnitTestCutScene() {}
	virtual ~CUnitTestCutScene() {}

	virtual void Create(MUnitTestConfig& config);
	virtual void Destroy();
	virtual void OnTestStart();
	virtual void OnTestFinish() {}

	virtual const char* GetName() { return UNITTEST_CUTSCENE_NAME; }
	virtual UnitTest::TestList const& GetTestList() { return UnitTest::Test::GetTestList(); }
	virtual const char* GetOutputFileName() { return "UnitTestStage1_Results.xml"; }
};
