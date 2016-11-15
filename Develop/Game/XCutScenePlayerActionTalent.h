#pragma once

#include "XCutScenePlayerActionState.h"


class XCutScenePlayerActionTalent : public XCutScenePlayerActionState
{
private:
public:
	XCutScenePlayerActionTalent(XModuleCutSceneControl* pOwner) : XCutScenePlayerActionState(pOwner) {}
	virtual ~XCutScenePlayerActionTalent() {}

	virtual XEventResult	Event(XEvent& msg);
	virtual void			Enter(void* pParam=NULL);
	virtual void			Exit();

	virtual int				GetID() { return CUTSCENEPLAYER_ACTION_STATE_TALENT; }
	const wchar_t*			GetDebugName()		{ return L"talent"; }
};


