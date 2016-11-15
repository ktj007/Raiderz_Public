#pragma once

class GField;
class MRegulator;

class GDespawnAuto : public MTestMemPool<GDespawnAuto>
{
public:
	GDespawnAuto(GField* pField, MUID uidNPC, float fDespawnTime);
	GDespawnAuto(GField* pField, SPAWN_ID nSpawnID, float fDespawnTime);	
	~GDespawnAuto(void);

	void Update(float fDelta);
	bool IsDespawned()			{ return m_bDespawned; }

private:
	void Despawn();
	bool IsDespawnBySpawnID();
	
private:
	GField*		m_pField;
	MUID		m_uidNPC;
	SPAWN_ID	m_nSpawnID;

	MRegulator*	m_pRgrDespawn;
	bool		m_bDespawned;
};
