#include "stdafx.h"
#include "XNPCActionMove.h"
#include "XModuleNonControl.h"
#include "XNPCAligner.h"
#include "XNonPlayer.h"
#include "XGlobal.h"
#include "XModuleMotion.h"



XNPCActionMove::XNPCActionMove( XModuleNonControl* pOwner )
: XNPCActionState(pOwner)
, m_pEnemyAligner(NULL)
{
	m_pEnemyAligner = pOwner->GetAligner();
}

XNPCActionMove::~XNPCActionMove()
{

}

void XNPCActionMove::Enter( void* pParam/*=NULL*/ )
{
	XNPCActionMoveParam* pMoveParam = (XNPCActionMoveParam*)pParam;

	if (pMoveParam)
	{
		m_MoveParam.vTarPos = pMoveParam->vTarPos;
		m_MoveParam.fSpeed = pMoveParam->fSpeed;

		m_pEnemyAligner->OnEnterMoveState(GetOwnerNPC(), pMoveParam->vTarPos);
	}
}

void XNPCActionMove::Exit()
{
	m_pEnemyAligner->OnExitState(GetOwnerNPC());
}

void XNPCActionMove::Update( float fDelta )
{
	m_pEnemyAligner->OnMoveUpdate(GetOwnerNPC(), m_MoveParam.vTarPos);

	vec3 vMyPos = m_pOwner->GetOwner()->GetPosition();
	float fDistanceSq = vMyPos.DistanceToSq(m_MoveParam.vTarPos);

	if (fDistanceSq < 30.0f)
	{
		wstring strMotion = m_pOwner->GetOwner()->GetModuleMotion()->GetCurrMotion();
		if (strMotion != MOTION_NAME_NPC_WALK  &&  strMotion != MOTION_NAME_NPC_RUN)
		{
			m_pOwner->DoAction(NPC_ACTION_STATE_NA);
		}
	}
}

XEventResult XNPCActionMove::Event( XEvent& msg )
{
	return XNPCActionState::Event(msg);
}