#include "stdafx.h"
#include "XNPCActionState.h"
#include "XEventID.h"
#include "XNonPlayer.h"
#include "XModuleNonControl.h"

void XNPCActionState::Enter(void* pParam)
{

}

void XNPCActionState::Exit()
{
}

XEventResult XNPCActionState::Event(XEvent& msg)
{
	return MR_FALSE;
}

XNonPlayer* XNPCActionState::GetOwnerNPC()
{
	return AsNPC(m_pOwner->GetOwner());
}

XNPCActionState::XNPCActionState( XModuleNonControl* pOwner ) 
: XState<XModuleNonControl>(pOwner)
{

}
////////////////////////////////////////////////////////////////////////////////////
// XNpcActionFSM ///////////////////////////////////////////////////////////////////

XNPCActionFSM::XNPCActionFSM( XModuleNonControl* pOwner ) : XStateMachine<XModuleNonControl>(pOwner)
{

}

XNPCActionFSM::~XNPCActionFSM()
{

}

