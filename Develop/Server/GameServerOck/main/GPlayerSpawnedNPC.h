#pragma once

class GEntityNPC;

typedef map<int, MUID> MAP_SPAWNEDNPC;

class GPlayerSpawnedNPC : public MTestMemPool<GPlayerSpawnedNPC>
{
public:
	GPlayerSpawnedNPC(GEntityPlayer* pPlayer);
	~GPlayerSpawnedNPC(void);

	void Spawn(int nNPCID, float fDespawnTime);
	void DespawnAll();
	bool IsSpawend(int nNPCID);

private:
	GEntityNPC* Get(int nNPCID);		

private:
	GEntityPlayer*	m_pOwner;
	MAP_SPAWNEDNPC	m_mapSpawnedNPC;
};
