#pragma once

#include "XDummyBot.h"

class XBirdClient;
class XFieldInfo;

class XDummyBot_AnnounceRefreshServer : public XDummyBot
{
	enum BOT_PHASE
	{
		PHASE_0_ANNOUNCE,
		PHASE_1_COUNTING,
		PHASE_2_END,
	};

	BOT_PHASE	m_ePhase;
	wstring		m_strAnnounceMsg;
	MRegulator	m_rgltCounting;
	MRegulator	m_rgltTimeout;
	int			m_nCounting;

private:
	bool InitParam(XBirdDummyAgent* pAgent);

	void OnRun(float fDelta) override;

	minet::MCommandResult OnFieldStartGame(XBirdDummyAgent* pAgent, XBirdClient* pClient, MCommand* pCmd);

public:
	XDummyBot_AnnounceRefreshServer(XBirdDummyAgent* pAgent, XDummyAgentInfo* pAgentInfo);
	virtual ~XDummyBot_AnnounceRefreshServer() {}

	virtual MCommandResult	OnCommand(XBirdDummyAgent* pAgent, MCommand* pCmd) override;
	static const wchar_t*	GetName() { return L"AnnounceRefreshServer"; }
	void					OnError(string strLog);
};
