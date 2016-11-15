#pragma once

#include "UnitTest++.h"
#include "MUnitTestRunner.h"

class CUnitTestRunner : public MUnitTestRunner
{
protected:
	virtual bool BeDoTest();
public:
	CUnitTestRunner();
	virtual ~CUnitTestRunner();
	bool RunStage1();
};