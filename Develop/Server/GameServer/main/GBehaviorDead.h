#ifndef _GBEHAVIOR_DEAD_H
#define _GBEHAVIOR_DEAD_H

#include "GBehaviorState.h"

class GCombatStateMachine;


// Death Behavior State
class GBehaviorDead :	public GBehaviorState, public MMemPool<GBehaviorDead>
{
public:
	GBehaviorDead(GModuleAI* pOwner, GStateMachine<GModuleAI>* pFSM);
	virtual ~GBehaviorDead();

	virtual void			OnRegisterTrainstionRule();
	virtual void			Enter();
	virtual void			Update(float fDelta);
	virtual void			Exit(int nNextStateID);
	virtual GMessageResult	Message(GMessage& msg);
	int						GetID() { return AI_BEHAVIOR_DEAD; }
	virtual const wchar_t*		DebugString() { return L"dead"; }

private:
	void OnDie();
};

#endif//_GBEHAVIOR_DEAD_H