#pragma once

#include "CSIndexedIDMap.h"

#define PLAYER_UIID_SEED			0
#define MAX_PLAYER_INDEXED_ID		5000

class GIndexedPlayerUIDMap : public CSIndexedIDMap<MUID, MAX_PLAYER_INDEXED_ID>
{
private:

public:
	GIndexedPlayerUIDMap() : CSIndexedIDMap<MUID, MAX_PLAYER_INDEXED_ID>(PLAYER_UIID_SEED)
	{
		Reserve(MAX_PLAYER_INDEXED_ID);
	}
	virtual ~GIndexedPlayerUIDMap() {}
	virtual void Clear()
	{
		CSIndexedIDMap<MUID, MAX_PLAYER_INDEXED_ID>::Clear();

		Reserve(MAX_PLAYER_INDEXED_ID);
	}
};



#define NPC_UIID_SEED			10000		// NPC의 UIID는 10001부터 시작. 플레이어와 겹치지 않기 위함
#define MAX_NPC_INDEXED_ID		2000


class GIndexedNPCUIDMap : public CSIndexedIDMap<MUID, MAX_NPC_INDEXED_ID>
{
private:

public:
	GIndexedNPCUIDMap() : CSIndexedIDMap<MUID, MAX_NPC_INDEXED_ID>(NPC_UIID_SEED)
	{
		Reserve(MAX_NPC_INDEXED_ID);
	}
	virtual ~GIndexedNPCUIDMap() {}
	virtual void Clear()
	{
		CSIndexedIDMap<MUID, MAX_NPC_INDEXED_ID>::Clear();

		Reserve(MAX_NPC_INDEXED_ID);
	}
};


inline bool IsPlayerUIID(UIID nUIID)
{
	if (PLAYER_UIID_SEED < nUIID && PLAYER_UIID_SEED + MAX_PLAYER_INDEXED_ID >= nUIID) return true;
	return false;
}

inline bool IsNPCUIID(UIID nUIID)
{
	if (NPC_UIID_SEED < nUIID && NPC_UIID_SEED + MAX_NPC_INDEXED_ID >= nUIID) return true;
	return false;
}