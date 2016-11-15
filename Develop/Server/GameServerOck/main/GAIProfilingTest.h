#pragma once

#include "MAutoTest.h"

class GAIProfilingTest: public MAutoTest, public MTestMemPool<GAIProfilingTest>
{
public:
	GAIProfilingTest();
	virtual ~GAIProfilingTest();

	virtual	const char* GetName();
	virtual void SaveTestResult(MMaietTestResult& outTestResult);
	
protected:
	virtual void OnCreate();
	virtual void OnDestroy();
	virtual void OnUpdate(float fDelta);
};
