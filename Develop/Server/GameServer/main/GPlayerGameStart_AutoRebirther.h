#ifndef _G_PLAYER_GAME_START_REBIRTHER_H_
#define _G_PLAYER_GAME_START_REBIRTHER_H_

class GEntityPlayer;
class GPlayerGameStart_State;

class GPlayerGameStart_AutoRebirther
{
private:
	GEntityPlayer*	m_pOwner;

private:
	void BattleArena(GPlayerGameStart_State& PlayerGameStartState);
	void Dynamic(GPlayerGameStart_State& PlayerGameStartState);
	void Share(GPlayerGameStart_State& PlayerGameStartState);

public:
	GPlayerGameStart_AutoRebirther(GEntityPlayer* pOwner);

	void Rebirth(GPlayerGameStart_State& PlayerGameStartState);
};

#endif
