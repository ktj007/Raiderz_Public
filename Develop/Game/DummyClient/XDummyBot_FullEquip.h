#pragma once

#include "XDummyBot.h"

class XBirdClient;
class XBirdDummyAgent;

class XDummyBot_FullEquip : public XDummyBot
{
public:
	XDummyBot_FullEquip(XBirdDummyAgent* pAgent, XDummyAgentInfo* pAgentInfo);
	~XDummyBot_FullEquip(void);

	virtual MCommandResult OnCommand(XBirdDummyAgent* pAgent, MCommand* pCommand);
	static const wchar_t* GetName() { return L"FullEquip"; }

protected:
	virtual void OnRun(float fDelta);

private:
	// handling commands
	void OnCommand_MC_CHAR_MYINFO(XBirdDummyAgent& agent,XBirdClient& client,MCommand& cmd);
	void OnCommand_MC_ITEM_ADD(XBirdDummyAgent& agent,XBirdClient& client,MCommand& cmd);

private:
	DWORD	m_requestedEquipmentTick;
	bool	m_charMyInfoProcessed;
};
