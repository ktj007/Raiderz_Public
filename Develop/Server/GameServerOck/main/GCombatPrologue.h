#pragma once

#include "GCombatState.h"

class GCombatPrologue : public GCombatState, public MTestMemPool<GCombatPrologue>
{
public:
	GCombatPrologue(GModuleAI* pOwner, GStateMachine<GModuleAI>* pFSM);
	virtual~GCombatPrologue(void);
	
	int						GetID() { return AI_COMBAT_PROLOGUE; }
	virtual const wchar_t*		DebugString() { return L"prologue"; }

private:
	virtual void			OnRegisterTrainstionRule() override;
	virtual void			Enter() override;
	virtual void			Exit(int nNextStateID) override;
	virtual void			Update(float fDelta) override;

private:
	bool m_bAlreadyUsed;
};
