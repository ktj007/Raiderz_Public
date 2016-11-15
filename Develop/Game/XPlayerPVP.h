#pragma once

class XPlayerPVP
{
public:
	enum XPvPAreaTeam
	{
		ePvPAreaTeam_NA,
		ePvPAreaTeam_1,
		ePvPAreaTeam_2
	};
private:

	//bool				m_bDuel;					///< 듀얼 팩션. 나랑 듀얼중인 플레이어는 이 값이 true
	uint32				m_nBattleArenaFaction;		///< 배틀 아레나용 팩션

	XPvPAreaTeam		m_ePvPAreaFaction;

	bool IsAllyInPvPArea(XPlayerPVP* target);		///< pvp 지역에서 아군인지만 체크

public:
	XPlayerPVP(void);
	~XPlayerPVP(void);

	void SetBattleArenaEnemy();
	//void SetDuel();
	//bool GetDuel() { return m_bDuel; }

	void ReleaseBattleArenaEnemy();
	//void ReleaseDuel();

	void SetPvPAreaFaction(bool bFaction1, bool bFaction2);
	void ReleasePvPAreaFaction();

	bool IsAlly(XPlayerPVP* target);		///< 동맹, 같은편
	bool IsEnemy( XPlayerPVP* target );		///< 적

	XPvPAreaTeam GetPvPAreaTeam() { return m_ePvPAreaFaction; }
};
