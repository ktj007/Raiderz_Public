#pragma once

#include "XDummyBot.h"

class XDummyBot_AutoParty : public XDummyBot
{
	enum BEHAVIOR_TYPE
	{
		BEHAVIOR_PREPARE = 0,
		BEHAVIOR_START = 1,
		BEHAVIOR_MOVE_OUT = BEHAVIOR_START,
		BEHAVIOR_MOVE_IN,
		BEHAVIOR_MOVE_GAIN_QUEST,
		BEHAVIOR_MOVE_DROP_QUEST,
		BEHAVIOR_PARTY_LEAVE,
		BEHAVIOR_MAX,
	};

public:
	XDummyBot_AutoParty(XBirdDummyAgent* pAgent, XDummyAgentInfo* pAgentInfo);
	virtual ~XDummyBot_AutoParty();

	static const wchar_t* GetName() { return L"Autoparty"; }

private:
	virtual void OnRun(float fDelta) override;

	void MovePosReq(vec3 vPos);

private:
	BEHAVIOR_TYPE m_nBehavior;
	MRegulator m_rgrTick;
};
