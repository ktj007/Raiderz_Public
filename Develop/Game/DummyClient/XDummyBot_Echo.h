#pragma once

#include "XDummyBot.h"
#include "XDummyBot_Roam.h"

class XDummyBot_Echo : public XDummyBot_Roam
{
private:
	float		m_fElapsedPostTime;

	float		m_fEchoTickTime;
	int			m_nDataSize;
	int			m_nRouteType;
protected:
	virtual void OnRun(float fDelta) override;
public:
	XDummyBot_Echo(XBirdDummyAgent* pAgent, XDummyAgentInfo* pAgentInfo);


	virtual ~XDummyBot_Echo();
	virtual MCommandResult OnCommand(XBirdDummyAgent* pAgent, MCommand* pCommand);
	static const wchar_t* GetName() { return L"Echo"; }
};