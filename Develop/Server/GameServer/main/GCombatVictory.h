#ifndef _GCOMBAT_VICTORY_H
#define _GCOMBAT_VICTORY_H

#include "GCombatState.h"

class GJobRunner;

///////////////////////////////////////////////////////////////////////////////////////////
// GCombatVictory //////////

class GCombatVictory : public GCombatState, public MTestMemPool<GCombatVictory>
{
public:
	GCombatVictory(GModuleAI* pOwner, GStateMachine<GModuleAI>* pFSM);
	virtual ~GCombatVictory();	

	virtual void			OnRegisterTrainstionRule();
	virtual void			Enter();
	virtual void			Update(float fDelta);
	virtual void			Exit(int nNextStateID);
	virtual GMessageResult	Message(GMessage& msg);
	int						GetID() { return AI_COMBAT_VICTORY; }
	virtual const wchar_t*		DebugString() { return L"victory"; }

private:
	bool HasMissionJobs();

private:
	GJobRunner* m_pJobRunner;
};



#endif // _GCOMBAT_COMBAT_H