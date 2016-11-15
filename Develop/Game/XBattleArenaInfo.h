#pragma once

class XBattleArenaInfo
{
private:
	bool			m_bInBattleArena;
	int				m_nMaxLife;
	
	float			m_fGameElaspedTime;
	MUID			m_uidTeam[2];
	int				m_nScore[2];

	void Clear()
	{
		m_bInBattleArena = false;
		m_nMaxLife	= 0;
		
		m_fGameElaspedTime = 0.f;
		m_uidTeam[0] = m_uidTeam[1] = MUID::ZERO;
		m_nScore[0] = m_nScore[1] = 0;
	}
public:
	XBattleArenaInfo()
	{
		Clear();
	}
	void SetInBattle()
	{
		_ASSERT(m_bInBattleArena != true);

		//Clear();

		m_bInBattleArena = true;
	}
	void SetOutBattle()
	{
		Clear();
	}
	bool IsInBattleArena() { return m_bInBattleArena; }

	void SetMaxLife(int val) { m_nMaxLife = val; }
	int GetMaxLife() const { return m_nMaxLife; }

	MUID* const GetUidTeam() { return m_uidTeam; }

	int* const GetScore() { return m_nScore; }
	void SetScore(int nRed, int nBlue)
	{
		m_nScore[0]	= nRed;
		m_nScore[1]	= nBlue;
	}

	void SetInfo(float fGameElaspedTime, int nMaxLife, MUID* pEntryUIDs, int* pEntryScores)
	{
		m_fGameElaspedTime = fGameElaspedTime;
		m_nMaxLife = nMaxLife;
		m_uidTeam[0] = pEntryUIDs[0];
		m_uidTeam[1] = pEntryUIDs[1];
		m_nScore[0]	= pEntryScores[0];
		m_nScore[1]	= pEntryScores[1];
	}
};
