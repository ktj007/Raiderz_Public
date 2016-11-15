#ifndef _GBEHAVIOR_STATE_H
#define _GBEHAVIOR_STATE_H

#include "GState.h"
#include "GAIState.h"

class GModuleAI;
class GEntityNPC;

class GBehaviorState : public GState<GModuleAI>
{
protected:
	GEntityNPC*			m_pOwnerNPC;
public:
	GBehaviorState(GModuleAI* pOwner, GStateMachine<GModuleAI>* pFSM);
	virtual void			Enter() {}
	virtual void			Update(float fDelta)	{}
	virtual void			Exit(int nNextStateID) {}
	int						GetID() { return AI_BEHAVIOR_NONE; }
};




#endif // _GBEHAVIOR_STATE_H