#pragma once

#include "MUnitTestRunner.h"

class DummyClientTestRunner : public MUnitTestRunner
{
public:
	DummyClientTestRunner();
	virtual ~DummyClientTestRunner();

	bool RunStage1();

protected:
	virtual bool BeDoTest();
};
