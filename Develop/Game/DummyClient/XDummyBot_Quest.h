#pragma once

#include "XDummyBot.h"

class XDummyLazyCommandPoster;

class XDummyBot_Quest : public XDummyBot
{
	enum PHASE
	{
		PHASE_INIT,
		PHASE_INIT_DONE,
		PHASE_GIVE,
		PHASE_GIVE_DONE,
		PHASE_COMPLETE,
		PHASE_COMPLETE_DONE,
		PHASE_REWARD,
		PHASE_REWARD_DONE,
	};

public:
	XDummyBot_Quest(XBirdDummyAgent* pAgent, XDummyAgentInfo* pAgentInfo);
	~XDummyBot_Quest(void);

	virtual MCommandResult OnCommand(XBirdDummyAgent* pAgent, MCommand* pCommand);
	static const wchar_t* GetName() { return L"Quest"; }

protected:
	virtual	void OnRun(float fDelta);

private:
	void ChangePhase(PHASE nPhase);	

private:
	MRegulator					m_rgltTimeout;
	PHASE						m_nPhase;	
	int							m_nQuestID;
};
