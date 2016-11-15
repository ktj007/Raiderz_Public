#pragma once

#include "XNPCActionState.h"

class XNPCActionHit : public XNPCActionState
{
private:
public:
	XNPCActionHit(XModuleNonControl* pOwner) : XNPCActionState(pOwner) {}
	virtual ~XNPCActionHit() {}

	virtual XEventResult	Event(XEvent& msg);
	virtual void			Enter(void* pParam=NULL);
	virtual void			Exit();

	virtual int				GetID() { return NPC_ACTION_STATE_HIT; }
	const wchar_t*			GetDebugName()		{ return L"hit"; }
};


