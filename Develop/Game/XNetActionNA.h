#pragma once

#include "XNetActionState.h"


class XNetActionNA : public XNetActionState
{
private:
public:
	XNetActionNA(XModuleNetControl* pOwner) : XNetActionState(pOwner) {}
	virtual ~XNetActionNA() {}
	virtual int				GetID() { return NET_ACTION_STATE_NA; }
	const wchar_t*				GetDebugName()		{ return L"na"; }
};


