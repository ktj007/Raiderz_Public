#pragma once

#include "XNPCActionState.h"


class XNPCActionTalent : public XNPCActionState
{
private:
public:
	XNPCActionTalent(XModuleNonControl* pOwner) : XNPCActionState(pOwner) {}
	virtual ~XNPCActionTalent() {}

	virtual XEventResult	Event(XEvent& msg);
	virtual void			Enter(void* pParam=NULL);
	virtual void			Exit();

	virtual int				GetID() { return NPC_ACTION_STATE_TALENT; }
	const wchar_t*			GetDebugName()		{ return L"talent"; }
};


