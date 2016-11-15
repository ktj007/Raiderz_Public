#pragma once

class GField;
class MRegulator;

class GSpawnDelay : public MTestMemPool<GSpawnDelay>
{
private:
	GField*			m_pField;
	int				m_nNPCID;
	vec3			m_vPos;
	vec3			m_vDir;
	SPAWN_ID		m_nSpawnID;

	MRegulator*		m_pRgrSpawn;
	bool			m_bSpawned;

private:
	void Spawn();
	bool IsSpawnBySpawnID();

public:
	GSpawnDelay(GField* pField, int nNPCID, const vec3& vPos, const vec3& vDir, float fDelayTime);
	GSpawnDelay(GField* pField, SPAWN_ID nSpawnID, float fDelayTime);
	~GSpawnDelay();

	void Update(float fDelta);
	bool IsSpawned()			{ return m_bSpawned; }
};
