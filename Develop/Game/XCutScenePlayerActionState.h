#pragma once

#include "XState.h"
#include "XCutScenePlayerActionID.h"

class XModuleCutSceneControl;
class XCutScenePlayer;

class XCutScenePlayerActionState : public XState<XModuleCutSceneControl>
{
public:
	XCutScenePlayerActionState(XModuleCutSceneControl* pOwner);
	virtual void			Enter(void* pParam=NULL);
	virtual void			Update(float fDelta) {}
	virtual void			PostUpdate(float fDelta) {}
	virtual void			Exit();
	virtual XEventResult	Event(XEvent& msg);
	virtual int				GetID() { return CUTSCENEPLAYER_ACTION_STATE_NONE; }
	virtual const wchar_t*		GetDebugName() { return L"none"; }
	XCutScenePlayer*				GetOwnerCutScenePlayer();
};

class XCutScenePlayerActionFSM : public XStateMachine<XModuleCutSceneControl>
{
public:
	XCutScenePlayerActionFSM(XModuleCutSceneControl* pOwner);
	virtual ~XCutScenePlayerActionFSM();

	XCutScenePlayerActionState* GetCurrentState() { return static_cast<XCutScenePlayerActionState*>(XStateMachine<XModuleCutSceneControl>::GetCurrentState()); }
};
