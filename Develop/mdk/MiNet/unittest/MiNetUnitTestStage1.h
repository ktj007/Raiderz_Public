#pragma once

#include "MBaseUnitTest.h"
#include "MiNetUnitTest.h"


class MiNetUnitTestStage1 : public MBaseUnitTest
{
private:
public:
	MiNetUnitTestStage1() {}
	virtual ~MiNetUnitTestStage1() {}
	virtual void Create(MUnitTestConfig& config);
	virtual void Destroy();
	virtual void OnTestFinish();

	virtual const char* GetName() { return UNITTEST_STAGE1_NAME; }
	virtual UnitTest::TestList const& GetTestList() { return UnitTest::Test::GetTestList(); }
	virtual const char* GetOutputFileName() { return "UnitTest_Results.xml"; }
};
