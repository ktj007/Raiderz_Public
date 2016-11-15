#pragma once

#include "XDummyBot.h"

class XBirdClient;

class XDummyBot_GetMaxQuest : public XDummyBot
{
public:
	XDummyBot_GetMaxQuest(XBirdDummyAgent* pAgent, XDummyAgentInfo* pAgentInfo);
	~XDummyBot_GetMaxQuest(void);

	virtual MCommandResult OnCommand(XBirdDummyAgent* pAgent, MCommand* pCommand);
	static const wchar_t* GetName() { return L"GetMaxQuest"; }

protected:
	virtual void OnRun(float fDelta);

private:
	// handling commands
	void OnCommand_MC_CHAR_MYINFO(XBirdDummyAgent& agent,XBirdClient& client,MCommand& cmd);
};
