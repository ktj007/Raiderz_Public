#pragma once

#include "XDummyBot.h"

class XBirdClient;
class XBirdDummyAgent;

class XDummyBot_FillInventory : public XDummyBot
{
public:
	XDummyBot_FillInventory(XBirdDummyAgent* pAgent, XDummyAgentInfo* pAgentInfo);
	~XDummyBot_FillInventory(void);

	virtual MCommandResult OnCommand(XBirdDummyAgent* pAgent, MCommand* pCommand);
	static const wchar_t* GetName() { return L"FillInventory"; }

protected:
	virtual void OnRun(float fDelta);

private:
	// handling commands
	void OnCommand_MC_CHAR_MYINFO(XBirdDummyAgent& agent,XBirdClient& client,MCommand& cmd);
};
