#pragma once

#include "XLoadingChecker.h"

class XLoadingChecker_BackgroundWork : public XLoadingChecker
{
private:
	bool CheckLoaded();
public:
	XLoadingChecker_BackgroundWork() {}
	virtual ~XLoadingChecker_BackgroundWork() {}
	virtual bool IsWorldLoaded() { return CheckLoaded(); }
	virtual bool IsCompleted() { return true; }
};


