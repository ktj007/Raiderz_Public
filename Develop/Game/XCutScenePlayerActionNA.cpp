#include "stdafx.h"
#include "XCutScenePlayerActionNA.h"
#include "XModuleCutSceneControl.h"
#include "XCutScenePlayerAligner.h"


void XCutScenePlayerActionNA::Enter( void* pParam/*=NULL*/ )
{
	m_pOwner->GetAligner()->OnEnterState(GetOwnerCutScenePlayer(), (CUTSCENEPLAYER_ACTION_STATE)GetID());
}

void XCutScenePlayerActionNA::Exit()
{
	m_pOwner->GetAligner()->OnExitState(GetOwnerCutScenePlayer());
}