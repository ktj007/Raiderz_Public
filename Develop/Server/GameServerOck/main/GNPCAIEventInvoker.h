#pragma once

#include "GAIEventInvoker.h"

class GModuleAI;

class GNPCAIEventInvoker: public GAIEventInvoker, public MTestMemPool<GNPCAIEventInvoker>
{
public:
	GNPCAIEventInvoker(GModuleAI* pOwnerModuleAI);
	virtual ~GNPCAIEventInvoker();

	void CombatBegin(void);
	void CombatEnd(void);
	void SpawnEmptyField(void);
	void SpawnEmptySector(void);
	void SpawnPlayerSector(void);	
	void MakeMajorLodNPC(void);

	void InvokeEvent(GAIEvent::AI_EVENT_ID eventID);

private:
	GModuleAI* m_pOwnerModuleAI;
};
