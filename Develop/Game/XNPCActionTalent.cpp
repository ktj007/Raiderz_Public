#include "stdafx.h"
#include "XNPCActionTalent.h"
#include "XModuleNonControl.h"
#include "XNPCAligner.h"


XEventResult XNPCActionTalent::Event( XEvent& msg )
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

void XNPCActionTalent::Enter( void* pParam/*=NULL*/ )
{
	m_pOwner->GetAligner()->OnEnterState(GetOwnerNPC(), (NPC_ACTION_STATE)GetID());
}

void XNPCActionTalent::Exit()
{
	m_pOwner->GetAligner()->OnExitState(GetOwnerNPC());
}
