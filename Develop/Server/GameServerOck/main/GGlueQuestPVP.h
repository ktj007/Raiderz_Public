#pragma once

class GQuestPVP;

class GGlueQuestPVP : public MTestMemPool<GGlueQuestPVP>
{
public:
	GGlueQuestPVP(GQuestPVP* pOwner);
	~GGlueQuestPVP(void);

	GQuestPVP* GetOwner() const;
	bool IsInvalidOwner() const;

public:
	virtual bool IsNull(void) const;

	int GetTeamCount(int nTeam);
	void BeginEvent(int nEventID);
	void EndEvent(int nEventID, int nWinnerTeam);
	void EnableMarkPos(int nNPCID);
	void DisableMarkPos(int nNPCID);

	void DisableBeginEvent();

protected:
	GQuestPVP*	m_pOwner;

	bool		m_bDisableBeginEvent;
};
