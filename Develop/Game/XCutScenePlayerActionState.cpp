#include "stdafx.h"
#include "XCutScenePlayerActionState.h"
#include "XEventID.h"
#include "XCutScenePlayer.h"
#include "XModuleCutSceneControl.h"

void XCutScenePlayerActionState::Enter(void* pParam)
{

}

void XCutScenePlayerActionState::Exit()
{
}

XEventResult XCutScenePlayerActionState::Event(XEvent& msg)
{
	return MR_FALSE;
}

XCutScenePlayer* XCutScenePlayerActionState::GetOwnerCutScenePlayer()
{
	return AsCutScenePlayer(m_pOwner->GetOwner());
}

XCutScenePlayerActionState::XCutScenePlayerActionState( XModuleCutSceneControl* pOwner ) 
: XState<XModuleCutSceneControl>(pOwner)
{

}
////////////////////////////////////////////////////////////////////////////////////
// XNpcActionFSM ///////////////////////////////////////////////////////////////////

XCutScenePlayerActionFSM::XCutScenePlayerActionFSM( XModuleCutSceneControl* pOwner ) : XStateMachine<XModuleCutSceneControl>(pOwner)
{

}

XCutScenePlayerActionFSM::~XCutScenePlayerActionFSM()
{

}

