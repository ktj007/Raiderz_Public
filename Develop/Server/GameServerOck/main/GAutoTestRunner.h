#ifndef	_G_AUTO_TEST_RUNNER_H_
#define	_G_AUTO_TEST_RUNNER_H_

#include "MAutoTestRunner.h"

class GAutoTestInfo;

class GAutoTestRunner :	public MAutoTestRunner
{
private:
	virtual MAutoTest* NewTest();
	virtual void OnCreate();
	virtual bool OnUpdate(float fDelta) override;

public:
	GAutoTestRunner();
	virtual ~GAutoTestRunner();
};

#endif
