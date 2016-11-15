#include "stdafx.h"
#include "XNPCActionHit.h"
#include "XModuleNonControl.h"

XEventResult XNPCActionHit::Event( XEvent& msg )
{
	switch (msg.m_nID)
	{
	case XEVTL_ON_CHANGED_MOTION_TO_IDLE:
		{
			m_pOwner->DoAction(NPC_ACTION_STATE_NA);
			return MR_TRUE;
		}
		break;
	}

	return XNPCActionState::Event(msg);
}

void XNPCActionHit::Enter( void* pParam/*=NULL*/ )
{
	
}

void XNPCActionHit::Exit()
{
	
}
