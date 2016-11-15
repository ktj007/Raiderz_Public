#pragma once

#include "XDummyBot.h"

class XDummyBot_FirstCutscene : public XDummyBot
{
	enum PHASE
	{
		PHASE_INIT,
		PHASE_REQ_CUTSCENE,
		PHASE_WAIT_CUTSCENE,
		PHASE_REQ_CUTSCENE_END,
		PHASE_FIN,
	};

public:
	XDummyBot_FirstCutscene(XBirdDummyAgent* pAgent, XDummyAgentInfo* pAgentInfo);
	~XDummyBot_FirstCutscene();

	virtual MCommandResult OnCommand(XBirdDummyAgent* pAgent, MCommand* pCommand);
	static const wchar_t* GetName() { return L"FirstCutscene"; }

protected:
	virtual void OnRun(float fDelta);

private:
	PHASE		m_Phase;
	MRegulator	m_rgltCutscene;

	void ChangePhase(PHASE nPhase) { m_Phase = nPhase; }

};
