#include "stdafx.h"
#include "XCutScenePlayerActionTalent.h"
#include "XModuleCutSceneControl.h"
#include "XCutScenePlayerAligner.h"


XEventResult XCutScenePlayerActionTalent::Event( XEvent& msg )
{
	switch (msg.m_nID)
	{
	case XEVTL_ON_CHANGED_MOTION_TO_IDLE:
		{
			m_pOwner->DoAction(CUTSCENEPLAYER_ACTION_STATE_NA);
			return MR_TRUE;
		}
		break;
	}

	return XCutScenePlayerActionState::Event(msg);
}

void XCutScenePlayerActionTalent::Enter( void* pParam/*=NULL*/ )
{
	m_pOwner->GetAligner()->OnEnterState(GetOwnerCutScenePlayer(), (CUTSCENEPLAYER_ACTION_STATE)GetID());
}

void XCutScenePlayerActionTalent::Exit()
{
	m_pOwner->GetAligner()->OnExitState(GetOwnerCutScenePlayer());
}
