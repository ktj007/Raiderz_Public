#pragma once

#include "MTest.h"

class GUnitTestRunner : public MUnitTestRunner
{
protected:
	virtual bool BeDoTest();
public:
	GUnitTestRunner();
	virtual ~GUnitTestRunner();
	bool RunStage1();
};
