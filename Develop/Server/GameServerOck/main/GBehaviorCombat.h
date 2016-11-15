#ifndef _GBEHAVIOR_COMBAT_H
#define _GBEHAVIOR_COMBAT_H

#include "GBehaviorState.h"
#include "MMemPool.h"

class GCombatStateMachine;
class GJobRunner;

// Combat Behavior State
class GBehaviorCombat : public GBehaviorState, public MMemPool<GBehaviorCombat>
{
private:
public:
	GBehaviorCombat(GModuleAI* pOwner, GStateMachine<GModuleAI>* pFSM);
	virtual ~GBehaviorCombat();

	virtual void			OnRegisterTrainstionRule();
	virtual void			Enter();
	virtual void			Update(float fDelta);
	virtual void			Exit(int nNextStateID);
	virtual GMessageResult	Message(GMessage& msg);
	int						GetID() { return AI_BEHAVIOR_COMBAT; }
	void					ChangeCombat(AI_COMBAT_STATE nNextCombat);
	GState<GModuleAI>*		FindCombatState(int nStateID); 
	AI_COMBAT_STATE			GetCurrentStateID();
	AI_COMBAT_STATE			GetPrevStateID();
	virtual const wchar_t*		DebugString() { return L"combat"; }
	void					SetCombatFSMLog(bool bLog);
private:
	GMessageResult			OnDefaultMessage(GMessage& msg);
	GCombatStateMachine*	m_pCombatStateMachine;
};

#endif // _GBEHAVIOR_COMBAT_H
