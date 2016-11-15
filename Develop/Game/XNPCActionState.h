#pragma once

#include "XState.h"
#include "XNPCActionID.h"

class XModuleNonControl;
class XNonPlayer;

class XNPCActionState : public XState<XModuleNonControl>
{
public:
	XNPCActionState(XModuleNonControl* pOwner);
	virtual void			Enter(void* pParam=NULL);
	virtual void			Update(float fDelta) {}
	virtual void			PostUpdate(float fDelta) {}
	virtual void			Exit();
	virtual XEventResult	Event(XEvent& msg);
	virtual int				GetID() { return NPC_ACTION_STATE_NONE; }
	virtual const wchar_t*		GetDebugName() { return L"none"; }
	XNonPlayer*				GetOwnerNPC();
};


class XNPCActionFSM : public XStateMachine<XModuleNonControl>
{
public:
	XNPCActionFSM(XModuleNonControl* pOwner);
	virtual ~XNPCActionFSM();

	XNPCActionState* GetCurrentState() { return static_cast<XNPCActionState*>(XStateMachine<XModuleNonControl>::GetCurrentState()); }
};
