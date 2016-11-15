#pragma once

#include "XNPCActionState.h"


class XNPCActionDie : public XNPCActionState
{
private:
public:
	XNPCActionDie(XModuleNonControl* pOwner) : XNPCActionState(pOwner) {}
	virtual ~XNPCActionDie() {}
	virtual int				GetID() { return NPC_ACTION_STATE_DIE; }
	const wchar_t*			GetDebugName()		{ return L"die"; }
};


