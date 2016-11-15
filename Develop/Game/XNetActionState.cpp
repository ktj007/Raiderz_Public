#include "stdafx.h"
#include "XNetActionState.h"
#include "XEventID.h"
#include "XNetPlayer.h"
#include "XModuleNetControl.h"

void XNetActionState::Enter(void* pParam)
{
	
}

void XNetActionState::Exit()
{
}

XEventResult XNetActionState::Event(XEvent& msg)
{
	return MR_FALSE;
}

XNetPlayer* XNetActionState::GetOwnerPlayer()
{
	return AsNetPlayer(m_pOwner->GetOwner());
}

XNetActionState::XNetActionState( XModuleNetControl* pOwner ) 
	: XState<XModuleNetControl>(pOwner), m_nFlags(0)
{

}
////////////////////////////////////////////////////////////////////////////////////
// XNetActionFSM ///////////////////////////////////////////////////////////////////

XNetActionFSM::XNetActionFSM( XModuleNetControl* pOwner ) : XStateMachine<XModuleNetControl>(pOwner)
{

}

XNetActionFSM::~XNetActionFSM()
{

}

bool XNetActionFSM::CheckFlag( uint32 nFlag )
{
	XNetActionState* pCurrState = GetCurrentState();
	if (pCurrState)
	{
		return pCurrState->CheckFlag(nFlag);
	}
	return false;
}