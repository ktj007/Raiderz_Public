#pragma once

#include "XBattleArenaInfo.h"

class XBattleArena
{
private:
	float					m_fRespawnTime;

public:
	XBattleArena() {}
	~XBattleArena() {}

	void Start(MUID* pEntryUIDs, int nUIDCount);
	void Finish();

	void InNetPlayer(MUID uidPlayer);

	bool IsInBattleArena();

	float GetRespawnTime() const { return m_fRespawnTime; }
	void SetRespawnTime(float val) { m_fRespawnTime = val; }

	void SetMaxLife(int nLife);

	void SetCurrentScore(int nRed, int nBlue);
	void SetCurrentScore( int* pScores);
	MWLua::ret_int2 GetCurrentScore() const;

	void ReJoin();

	bool IsVisibleScoreBoard();
	void SetResultScore( float fWaitSecond, MUID uidWinner, int nWinnerScore, int nLoserScore, float fGameElaspedTime );
};