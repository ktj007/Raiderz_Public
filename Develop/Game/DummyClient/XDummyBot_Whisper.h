#pragma once

#include "XDummyBot.h"


class XDummyBot_Whisper : public XDummyBot
{
	enum PHASE
	{
		PHASE_INIT,
		PHASE_WAIT_TO_START,
		PHASE_REQ_WHISPER,
		PHASE_WAIT_WHISPER,
		PHASE_ERROR,
	};

public:
	XDummyBot_Whisper(XBirdDummyAgent* pAgent, XDummyAgentInfo* pAgentInfo);
	~XDummyBot_Whisper();

	virtual MCommandResult OnCommand(XBirdDummyAgent* pAgent, MCommand* pCommand);
	static const wchar_t* GetName() { return L"Whisper"; }

protected:
	virtual void OnRun(float fDelta);

private:
	PHASE		m_Phase;
	MRegulator	m_rgltWaitToStart;
	MRegulator	m_rgltTimout;
	wstring		m_strTargetName;
	bool		m_bEvenNumber;

	wstring		m_strMsg;

	void ChangePhase(PHASE nPhase) { m_Phase = nPhase; }
	void Init();
};
