#pragma once

#include "XDummyBot.h"

class XDummyBot_Enchant : public XDummyBot
{
	enum PHASE
	{
		PHASE_INIT	= 0,
		PHASE_PREPARE,
		PHASE_ENCHANT,
	};

public:
	XDummyBot_Enchant(XBirdDummyAgent* pAgent, XDummyAgentInfo* pAgentInfo);
	virtual ~XDummyBot_Enchant();

	static const wchar_t* GetName() { return L"Enchant"; }

private:
	virtual void OnRun(float fDelta) override;
	
	void MakeTestItem();
private:
	PHASE m_nPhase;
	MRegulator m_rgrTick;
};
