#pragma once

#include "MBaseUnitTest.h"
#include "XUnitTestRunner.h"


#define UNITTEST_STAGE3_NAME		"Stage3"

class XUnitTestStage3 : public MBaseUnitTest
{
private:
public:
	XUnitTestStage3() {}
	virtual ~XUnitTestStage3() {}
	virtual void Create(MUnitTestConfig& config);
	virtual void Destroy();

	virtual const char* GetName() { return UNITTEST_STAGE3_NAME; }
	virtual UnitTest::TestList const& GetTestList() { return GetStage3TestList(); }
	virtual const char* GetOutputFileName() { return "UnitTestStage3_Results.xml"; }
};
