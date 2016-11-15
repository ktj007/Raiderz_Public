#pragma once

#include "MBaseUnitTest.h"

class DummyClientUnitTestStage1 : public MBaseUnitTest
{
private:
public:
	DummyClientUnitTestStage1() {}
	virtual ~DummyClientUnitTestStage1() {}
	virtual void Create(MUnitTestConfig& config);
	virtual void Destroy();
	virtual void OnTestFinish();

	virtual const char* GetName() { return "Stage1"; }
	virtual UnitTest::TestList const& GetTestList() { return UnitTest::Test::GetTestList(); }
	virtual const char* GetOutputFileName() { return "UnitTest_Results.xml"; }
};
