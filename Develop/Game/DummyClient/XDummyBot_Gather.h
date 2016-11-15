#pragma once

#include "XDummyBot.h"

class XDummyBot_Gather : public XDummyBot
{
	enum PHASE
	{
		PHASE_INIT,
		PHASE_READY,
		PHASE_START,
		PHASE_WAIT_TO_INTERACTION,
		PHASE_GATHERING,
		PHASE_FAIL,
		PHASE_FIN,
	};

public:
	XDummyBot_Gather(XBirdDummyAgent* pAgent, XDummyAgentInfo* pAgentInfo);
	~XDummyBot_Gather();

	virtual MCommandResult OnCommand(XBirdDummyAgent* pAgent, MCommand* pCommand);
	static const wchar_t* GetName() { return L"Gather"; }

protected:
	virtual void OnRun(float fDelta);

private:
	PHASE		m_Phase;
	vector<int>	m_vecGatherNPCIDList;
	int			m_nCurGatherNPCIndex;

	MUID		m_uidCurGatherNPC;

	struct ST_INIT_TOOL
	{
		bool bTakePickAxe;
		bool bTakeTrowel;
		bool bTakeKey;

		ST_INIT_TOOL() : bTakePickAxe(false), bTakeTrowel(false), bTakeKey(false) {}

		bool IsReady() { return (bTakePickAxe && bTakeTrowel && bTakeKey); }
	};
	ST_INIT_TOOL	m_InitTools;

	struct ST_GATHER_NPC_INFO
	{
		MUID nUID;
		UIID nUIID;
		vec3 vPos;
	};
	typedef vector<ST_GATHER_NPC_INFO> GATHER_NPC_LIST;
	GATHER_NPC_LIST m_vecGatherNPC;

	void ChangePhase(PHASE nPhase) { m_Phase = nPhase; }

	bool InitParam();

};
