#pragma once

#include "XLoadingChecker.h"

class XLoadingChecker_Server : public XLoadingChecker
{
private:
	bool m_bRecevedCommand_MC_FIELD_START_GAME;
	bool CheckCompleted();
protected:
	void OnEvent(XEvent& evt);
public:
	XLoadingChecker_Server() : m_bRecevedCommand_MC_FIELD_START_GAME(false) {}
	virtual ~XLoadingChecker_Server() {}
	virtual bool IsWorldLoaded() { return true; }
	virtual bool IsCompleted() { return CheckCompleted(); }
};

