#pragma once

#include "UnitTest++.h"
#include "MUnitTestRunner.h"
#include "XUnitTestList.h"

class XUnitTestRunner : public MUnitTestRunner
{
protected:
	virtual bool BeDoTest();
	void InitCodeCoverage();
public:
	XUnitTestRunner();
	virtual ~XUnitTestRunner();
	bool RunStage1();
	bool RunStage2();
	bool RunStage3();
};

inline bool RunUnitTestStage1()
{
	XUnitTestRunner unitTestRunner;
	if (unitTestRunner.RunStage1() == false) return false;

	return true;
}

inline bool RunUnitTestStage2()
{
	XUnitTestRunner unitTestRunner;
	if (unitTestRunner.RunStage2() == false) return false;

	return true;
}

inline bool RunUnitTestStage3()
{
	XUnitTestRunner unitTestRunner;
	if (unitTestRunner.RunStage3() == false) return false;

	return true;
}

