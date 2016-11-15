#pragma once

#include "XCutScenePlayerActionState.h"


class XCutScenePlayerActionNA : public XCutScenePlayerActionState
{
private:
public:
	XCutScenePlayerActionNA(XModuleCutSceneControl* pOwner) : XCutScenePlayerActionState(pOwner) {}
	virtual ~XCutScenePlayerActionNA() {}

	virtual void			Enter(void* pParam=NULL);
	virtual void			Exit();

	virtual int				GetID() { return CUTSCENEPLAYER_ACTION_STATE_NA; }
	const wchar_t*			GetDebugName()		{ return L"na"; }
};