#pragma once

#include "XDummyBot.h"

class XDummyBot_MultiLogin : public XDummyBot
{
public:
	XDummyBot_MultiLogin(XBirdDummyAgent* pAgent, XDummyAgentInfo* pAgentInfo);
	~XDummyBot_MultiLogin(void);

	virtual MCommandResult OnCommand(XBirdDummyAgent* pAgent, MCommand* pCommand);
	static const wchar_t* GetName()		{ return L"MultiLogin"; }
};
