#pragma once

#include "XDummyBot.h"

class XDummyBot_Inven : public XDummyBot
{
	enum PHASE
	{
		PHASE_INIT,
		PHASE_INIT_DONE,
		PHASE_SWAPING_SLOT1_REQ,
		PHASE_SWAPING_SLOT1,
		PHASE_SWAPING_SLOT2_REQ,
		PHASE_SWAPING_SLOT2,
		PHASE_SLOT_SWAPED,
	};

public:
	XDummyBot_Inven(XBirdDummyAgent* pAgent, XDummyAgentInfo* pAgentInfo);
	~XDummyBot_Inven(void);

	virtual MCommandResult OnCommand(XBirdDummyAgent* pAgent, MCommand* pCommand);
	static const wchar_t* GetName()		{ return L"Inven"; }

	void ChangePhase( PHASE nPhase )	{ m_nPhase = nPhase; }

protected:
	virtual void OnRun(float fDelta);

private:
	PHASE				m_nPhase;

};
