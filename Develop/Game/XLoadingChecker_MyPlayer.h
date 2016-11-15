#pragma once

#include "XLoadingChecker.h"

class XLoadingChecker_MyPlayer : public XLoadingChecker
{
private:
	bool CheckLoaded();
public:
	XLoadingChecker_MyPlayer() {}
	virtual ~XLoadingChecker_MyPlayer() {}
	virtual bool IsWorldLoaded() { return CheckLoaded(); }
	virtual bool IsCompleted() { return true; }
};
