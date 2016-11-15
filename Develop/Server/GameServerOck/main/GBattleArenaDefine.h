#pragma once

namespace BattleArena {

enum GameType
{
	GAMETYPE_INVALID	 = 0,
	GAMETYPE_DEATHMATCH	 = 1,
};

struct Team
{
	enum Status
	{
		INVALID = 0,	// 무효한 상태
		WAITING_FOR_MATCH = 1,	// 매칭 대기중
		WAITING_FOR_PLAY = 2,	// 대전 대기중
		PLAYING = 3,	// 대전중
	};

	MUID	UID;
	MUID	uidBattleArena;
	Status	nStatus;
	MUID	uidParty;

	Team()	{ Reset(); }
	void Reset()
	{
		UID.SetInvalid();
		nStatus = INVALID;
		uidParty.SetInvalid();
	}
	bool IsInvalid() const 
	{
		return nStatus == INVALID;
	}

	bool IsMatched() const { return  nStatus != WAITING_FOR_MATCH; }
};

typedef std::map<MUID, Team>	TEAM_MAP;

};	// namespace BattleArena