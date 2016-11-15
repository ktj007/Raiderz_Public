#pragma once

#include "XDummyBot.h"

class XBirdDummyAgent;

class XDummyBot_LevelUp : public XDummyBot
{
public:
	XDummyBot_LevelUp(XBirdDummyAgent* pAgent, XDummyAgentInfo* pAgentInfo);
	~XDummyBot_LevelUp(void);

	virtual MCommandResult OnCommand(XBirdDummyAgent* pAgent, MCommand* pCommand);
	static const wchar_t* GetName() { return L"LevelUp"; }

protected:
	virtual void OnRun(float fDelta);
};
