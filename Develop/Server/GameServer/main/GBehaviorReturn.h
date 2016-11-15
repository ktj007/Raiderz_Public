#pragma once

#include "GBehaviorState.h"

class GBehaviorReturn : public GBehaviorState, public MMemPool<GBehaviorReturn>
{
public:
	GBehaviorReturn(GModuleAI* pOwner, GStateMachine<GModuleAI>* pFSM);
	virtual ~GBehaviorReturn(void);

	int						GetID() { return AI_BEHAVIOR_RETURN; }
	virtual const wchar_t*		DebugString() { return L"return"; }	
	virtual void			OnRegisterTrainstionRule() override;

	virtual void			Enter() override;
	virtual void			Update(float fDelta) override;
	virtual void			Exit(int nNextStateID) override;
};
