#pragma once

#include "MUID.h"
#include "MSingleton.h"

class GEntityNPC;

typedef map<MUID, GEntityNPC*> MAP_NPC;
class GNPCMgr : public MTestMemPool<GNPCMgr>
{
public:
	GNPCMgr(void);
	~GNPCMgr(void);

	void Clear();

	void AddNPC(GEntityNPC* pNPC);
	void RemoveNPC(GEntityNPC* pNPC);
	MAP_NPC& GetNPCMap()		{ return m_mapUIDNPC; }	

	size_t GetSize() const;
	GEntityNPC* GetEntityNPC(MUID uid);
	
	void Update(float fDelta);

private:
	MRegulator m_rgrAITick;
	size_t m_nLastUpdatedIndex;
	float fTickElapsedTime;
	MAP_NPC m_mapUIDNPC;
	vector<GEntityNPC*> m_vecNPCs;
	vector<GEntityNPC*> m_vecAddNPCs;
};