#pragma once

#include "MUnitTestRunner.h"

class LUnitTestRunner : public MUnitTestRunner
{
protected:
	virtual bool BeDoTest();
public:
	LUnitTestRunner() : MUnitTestRunner() {}
	bool RunStage1();
};
