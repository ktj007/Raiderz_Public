#ifndef _GCOMBAT_FLEE_H
#define _GCOMBAT_FLEE_H

#include "GCombatState.h"

class GCombatFlee : public GCombatState, public MTestMemPool<GCombatFlee>
{
public:
	GCombatFlee(GModuleAI* pOwner, GStateMachine<GModuleAI>* pFSM);
	virtual ~GCombatFlee();

	virtual void			OnRegisterTrainstionRule();
	virtual void			Enter();
	virtual void			Exit(int nNextStateID);

	virtual void			Update(float fDelta);
	virtual GMessageResult	Message(GMessage& msg);

	int						GetID() { return AI_COMBAT_FLEE; }
	virtual const wchar_t*		DebugString() { return L"flee"; }

	float					GetDurationTime();
private:
	void					Flee();

private:
	MRegulator				m_ExpirationRegulator;
	MRegulator				m_YellIntervalRegulator;
	GJobRunner*				m_pJobRunner;
	FleeType				m_nFleeType;
};

#endif // _GCOMBAT_FLEE_H