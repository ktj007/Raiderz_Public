#pragma once


#include "XDummyBot.h"

class XDummyBot_Chat : public XDummyBot
{
private:
	vec3	m_vCenter;
	float	m_fRadius;
	int		m_nFieldID;

	uint32	m_nLastChatPost;
protected:
	virtual void OnRun(float fDelta) override;

	void PostChat();
	wstring MakeRandomMsg();
public:
	XDummyBot_Chat(XBirdDummyAgent* pAgent, XDummyAgentInfo* pAgentInfo);

	virtual ~XDummyBot_Chat();
	virtual MCommandResult OnCommand(XBirdDummyAgent* pAgent, MCommand* pCommand);
	static const wchar_t* GetName() { return L"Chat"; }
};