#pragma once

class PLogicChecker
{
public:
	virtual bool Check(void) = 0;
	
	virtual void OnSuccess(void)	{}
	virtual void OnFail(void)		{}
};
