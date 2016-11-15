#pragma once

#include "XDummyBot.h"

class XBirdClient;
class XBirdDummyAgent;

class XDummyBot_SetBerserker : public XDummyBot
{
public:
	XDummyBot_SetBerserker(XBirdDummyAgent* pAgent, XDummyAgentInfo* pAgentInfo);
	~XDummyBot_SetBerserker(void);

	virtual MCommandResult OnCommand(XBirdDummyAgent* pAgent, MCommand* pCommand);
	static const wchar_t* GetName() { return L"SetBerserker"; }

protected:
	virtual void OnRun(float fDelta);
};
