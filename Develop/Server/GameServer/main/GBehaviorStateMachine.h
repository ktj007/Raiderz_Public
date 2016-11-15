#pragma once

#include "GState.h"
#include "GModuleAI.h"

class GJob;
class GFieldNPCSession;

class GBehaviorStateMachine : public GStateMachine<GModuleAI>, public MTestMemPool<GBehaviorStateMachine>
{
public:
	GBehaviorStateMachine(GModuleAI* pOwner);
	virtual ~GBehaviorStateMachine();
	
	void Init(void);
	void Reset();	
	void PushMissionJob(GJob* pJob);
	void ClearMissionJob();	
	
	deque<GJob*> GetMissionJobs();
	GJob*		 GetCurSessionJob();
	int GetMissionJobCounter();
	GFieldNPCSession* GetNPCSession();

	int GetCurrentCombatStateID();
	int GetPrevCombatStateID();
	void SetFSMLog(bool bLog);
	bool IsNowCombat();
	int GetPrevStateID();

protected:
	virtual void OnPreChangeState(GState<GModuleAI>* pNextState);
	
private:
	int	m_nPrevStateID;	
};
