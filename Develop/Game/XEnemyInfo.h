#pragma once


class XEnemyInfo
{
private:
	vector<TD_ENEMY_INFO>	m_vecEnemies;

public:
	void Clear();
	void ClearEnemyInfo(UIID nUIID);

	vector<TD_ENEMY_INFO>&	GetEnemyList()	{ return m_vecEnemies; }

	TD_ENEMY_INFO* GetEnemyInfoByUIID(UIID nUIID);

	void	CheckDamageInfo();
	void	NPCGradeSort();
	void	CheckEnemyInfoList();

	int		GetEnemyCount()	{ return m_vecEnemies.size(); }
	TD_ENEMY_INFO* GetEnemyInfo( int nIndex );
	bool	IsEmpty()		{ return m_vecEnemies.empty(); }
};
