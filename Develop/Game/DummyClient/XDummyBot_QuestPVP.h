#pragma once

#include "XDummyBot.h"
#include "XDummyBot_Combat.h"

class XDummySpawnMgr;
class XDummyPlayerMgr;
enum QUESTPVP_TEAM;

class XDummyBot_QuestPVP : public XDummyBot_Combat
{
	enum PHASE
	{
		PHASE_INIT_FACTION,
		PHASE_INIT_FACTION_DONE,
		PHASE_REFRESH_FACTION,
		PHASE_RUN,
	};

public:
	XDummyBot_QuestPVP(XBirdDummyAgent* pAgent, XDummyAgentInfo* pAgentInfo);
	~XDummyBot_QuestPVP(void);

	virtual MCommandResult OnCommand(XBirdDummyAgent* pAgent, MCommand* pCommand);
	static const wchar_t* GetName() { return L"QPVP"; }

protected:
	virtual	void OnRun(float fDelta);

	virtual void SelectTarget() override;
	virtual bool IsValidTarget() override;
	virtual vec3 GetTargetPos() override;
	virtual void UpdateTarget(UIID nUIID) override;

private:
	void ChangePhase(PHASE nPhase);

private:
	PHASE				m_nPhase;
	XDummyPlayerMgr*	m_pPlayerMgr;
	QUESTPVP_TEAM		m_nTeam;

	bool				m_bInitFactionInc;
	bool				m_bInitFactionDec;
};
