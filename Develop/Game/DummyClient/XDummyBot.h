#pragma once

class XBirdDummyAgent;
struct XDummyAgentInfo;

class XDummyBot
{
protected:
	XBirdDummyAgent*		m_pAgent;
	XDummyAgentInfo*		m_pAgentInfo;
	bool					m_bRun;

	virtual void OnRun(float fDelta);
	void PostMove(vec3 vPos);

public:
	XDummyBot(XBirdDummyAgent* pAgent, XDummyAgentInfo* pAgentInfo);
	virtual ~XDummyBot();
	void Run(float fDelta);

	virtual MCommandResult OnCommand(XBirdDummyAgent* pAgent, MCommand* pCommand);
	virtual const wchar_t* _GetName() { return L""; }
	const wchar_t* GetAgentName();
};


