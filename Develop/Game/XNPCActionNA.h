#pragma once

#include "XNPCActionState.h"


class XNPCActionNA : public XNPCActionState
{
private:
public:
	XNPCActionNA(XModuleNonControl* pOwner) : XNPCActionState(pOwner) {}
	virtual ~XNPCActionNA() {}

	virtual void			Enter(void* pParam=NULL);
	virtual void			Exit();

	virtual int				GetID() { return NPC_ACTION_STATE_NA; }
	const wchar_t*			GetDebugName()		{ return L"na"; }
};


