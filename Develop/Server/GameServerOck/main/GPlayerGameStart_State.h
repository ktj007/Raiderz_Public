#pragma once

class GEntityPlayer;

enum GAME_START_FIELD_TYPE
{
	GSFT_NONE = 0,
	GSFT_FIRST_LOGIN,
	GSFT_SHARE,
	GSFT_DYNAMIC,
	GSFT_BATTLE_ARENA,
	GSFT_MAX
};

class GPlayerGameStart_State
{
public:
	struct InputParam
	{
		int		nExitFieldID;
		bool	bBattleArenaEntered;
		MUID	uidDynamicFieldGroup;
		bool	bIsDead;
		uint32	nElapsedDeadTimeSec;

		InputParam(GEntityPlayer* pEntityPlayer);
		InputParam();
	};
private:
	GAME_START_FIELD_TYPE	m_nGameStartFieldType;
	bool	m_bFieldExist;
	bool	m_bDead;
	bool	m_bAutoRebirthTimeOver;
	int		m_nExitFieldID;

private:
	void	Setup(InputParam& inputParam);
	void	SetExitFieldID(InputParam& inputParam);
	void	SetFieldType(InputParam& inputParam);
	void	SetFieldExist(InputParam& inputParam);
	void	SetDead(InputParam& inputParam);
	void	SetAutoRebirthTimeOver(InputParam& inputParam);
	
public:
	GPlayerGameStart_State(InputParam& inputParam);
	GPlayerGameStart_State(GEntityPlayer* pPlayer);

	bool	IsFirstLogin()			{ return GSFT_FIRST_LOGIN == m_nGameStartFieldType; }
	bool	IsShareField()			{ return GSFT_SHARE == m_nGameStartFieldType; }
	bool	IsDynamicField()		{ return GSFT_DYNAMIC == m_nGameStartFieldType; }
	bool	IsBattleArena()			{ return GSFT_BATTLE_ARENA == m_nGameStartFieldType; }

	bool	IsFieldExist()			{ return m_bFieldExist; }

	bool	IsDead()				{ return m_bDead; }
	bool	IsAutoRebirthTimeOver()	{ return m_bAutoRebirthTimeOver; }
	int		GetExitFieldID()		{ return m_nExitFieldID; }
};
