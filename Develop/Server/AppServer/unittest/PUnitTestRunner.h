#pragma once

#include "MUnitTestRunner.h"

class PUnitTestRunner : public MUnitTestRunner
{
protected:
	virtual bool BeDoTest();
public:
	PUnitTestRunner() : MUnitTestRunner() {}
	bool RunStage1();
};
