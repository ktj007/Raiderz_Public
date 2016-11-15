#pragma once

#include "GEnemyFinder.h"
#include "GNPCAILod.h"

class GModuleAI;
class GHateTable;
class GBehaviorStateMachine;
class GJobMgr;
class GNPCAILod;
class GNPCAILodEventSelector;
class GNPCAIUpdater;
class GJob;

class GNPCAI : public MTestMemPool<GNPCAI>
{
public:
	GNPCAI(GModuleAI* pModuleAI);
	virtual ~GNPCAI();

	void Initialize(void);
	void Finalize(void);
	void Update(float fDelta);
	
	void UpdateBehaviorFSM(float fDelta);
	void UpdateFindEnemy(float fDelta);
	void UpdateJobMgr(float fDelta);

	void BehaviorRecreate(void);
	void BehaviorReset(void);	

	AI_LOD_LEVEL GetLodLevel(void) const;
	GBehaviorStateMachine* GetBehaviorFSM(void);
	GJobMgr* GetJobMgr(void);	

	void OnNotify(GAIEvent aiEvent) {m_pLod->OnNotify(aiEvent);	}

	deque<GJob*> GetMissionJobs();

private:
	GModuleAI*					m_pModuleAI;
	MUID						m_uidOwner;
	GBehaviorStateMachine*		m_pBehaviorFSM;
	GJobMgr*					m_pJobMgr;

	GNPCAILod*					m_pLod;
	GNPCAILodEventSelector*		m_pLodSelector;

	GEnemyFinder				m_EnemyFinder;

	GNPCAIUpdater*				m_pDefaultUpdater;
	GNPCAIUpdater*				m_pEmptyFieldUpdater;
	GNPCAIUpdater*				m_pLodUpdater[MAX_AI_LOD_LEVEL];
	AI_LOD_LEVEL				m_aiLastUpdateLevel;
};
