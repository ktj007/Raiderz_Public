#pragma once

class GField;
enum QUESTPVP_TEAM;

class GQPTeamCountSender : public MTestMemPool<GQPTeamCountSender>
{
public:
	GQPTeamCountSender(GField* pField);
	~GQPTeamCountSender(void);

	void Update(float fDelta);

private:
	void SendTeamCount();

private:
	GField*		m_pField;
	MRegulator	m_Regulator;
};
