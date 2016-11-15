#pragma once

#include "MUnitTestRunner.h"

class MiNetUnitTestRunner : public MUnitTestRunner
{
protected:
	virtual bool BeDoTest();
public:
	MiNetUnitTestRunner();
	virtual ~MiNetUnitTestRunner();
	bool RunStage1();
};
