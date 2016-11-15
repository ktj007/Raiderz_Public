#pragma once

#include "MUnitTestRunner.h"

class ZUnitTestRunner : public MUnitTestRunner
{
protected:
	virtual bool BeDoTest();
	void InitCodeCoverage();
public:
	ZUnitTestRunner();
	virtual ~ZUnitTestRunner();
	bool RunStage1();
};
