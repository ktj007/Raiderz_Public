#ifndef _GBEHAVIOR_MISSION_H
#define _GBEHAVIOR_MISSION_H

#include "GBehaviorState.h"
#include "GEnemyFinder.h"
#include "MTime.h"

class GJob;

// Mission Behavior State
class GBehaviorMission : public GBehaviorState, public MMemPool<GBehaviorMission>
{
public:
	GBehaviorMission(GModuleAI* pOwner, GStateMachine<GModuleAI>* pFSM);
	virtual ~GBehaviorMission();

	virtual void			OnRegisterTrainstionRule();
	virtual void			Enter();

	virtual void			Update(float fDelta);
	virtual void			Exit(int nNextStateID);
	virtual GMessageResult	Message(GMessage& msg);
	int						GetID() { return AI_BEHAVIOR_MISSION; }	

	void					PushMissionJob(GJob* pTask);
	void					ClearMissionJob();
	GJob*					GetCurMissionJob();
	int						GetMissionJobCount() const;
	bool					AdvanceJob();
	bool					HasMissionJob() const;

	deque<GJob*> GetMissionJobs();

	virtual const wchar_t*		DebugString() { return L"mission"; }

private:
	void					OnJobFinished(const GJob* pLastJob);

private:
	bool			m_bJobComplete;
	deque<GJob*>	m_qScheduledJob;
};








#endif // _GBEHAVIOR_MISSION_H