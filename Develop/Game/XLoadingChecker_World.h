#pragma once

#include "XLoadingChecker.h"

class XLoadingChecker_World : public XLoadingChecker
{
private:
	bool CheckLoaded();
public:
	XLoadingChecker_World() {}
	virtual ~XLoadingChecker_World() {}
	virtual bool IsWorldLoaded() { return CheckLoaded(); }
	virtual bool IsCompleted() { return true; }
};

