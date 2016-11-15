#pragma once

class GEntityPlayer;
class GPlayerGameStart_State;

class GPlayerGameStart_FieldGater
{
private:
	GEntityPlayer*	m_pOwner;

private:
	bool FirstLogin();
	bool BattleArena(GPlayerGameStart_State& PlayerGameStartState);	
	bool Dynamic(GPlayerGameStart_State& PlayerGameStartState);
	bool Share(GPlayerGameStart_State& PlayerGameStartState);

public:
	GPlayerGameStart_FieldGater(GEntityPlayer* pOwner);

	bool Gate(GPlayerGameStart_State& PlayerGameStartState);
};
