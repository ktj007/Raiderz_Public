#include "stdafx.h"
#include "XNPCActionNA.h"
#include "XModuleNonControl.h"
#include "XNPCAligner.h"


void XNPCActionNA::Enter( void* pParam/*=NULL*/ )
{
	m_pOwner->GetAligner()->OnEnterState(GetOwnerNPC(), (NPC_ACTION_STATE)GetID());
}

void XNPCActionNA::Exit()
{
	m_pOwner->GetAligner()->OnExitState(GetOwnerNPC());
}