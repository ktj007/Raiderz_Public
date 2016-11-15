#pragma once

class GEntityPlayer;

typedef std::set<int> SET_SAWN_CUTSCENE;

class GPlayerCutscene : public MTestMemPool<GPlayerCutscene>
{
public:
	GPlayerCutscene(GEntityPlayer* pOwner);
	~GPlayerCutscene();

	void AssignSawnCutscene(std::vector<int>& vecSawnCutscene);
	bool IsSawn(int nCutsceneID);

	bool BeginCutscene(int nCutsceneID);
	bool BeginPartyCutscene(int nCutsceneID);
	bool EndCutscene(int nCutsceneID);

	SET_SAWN_CUTSCENE& GetContainer()	{ return m_setSawnCutscene;	}

private:
	GEntityPlayer*		m_pOwner;
	SET_SAWN_CUTSCENE	m_setSawnCutscene;

	void InsertSawnCutscene(int nCutsceneID);

};
