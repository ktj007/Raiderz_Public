#pragma once

#include "MUnitTestRunner.h"

class LUnitTestRunner : public MUnitTestRunner
{
protected:
	virtual bool BeDoTest();
	void InitCodeCoverage();
public:
	LUnitTestRunner();
	virtual ~LUnitTestRunner();
	bool RunStage1();
	bool RunGameDBUnitTest();
};
