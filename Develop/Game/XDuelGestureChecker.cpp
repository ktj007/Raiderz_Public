#include "stdafx.h"
#include "XDuelGestureChecker.h"
#include "XModuleMotion.h"
#include "XModuleActorControl.h"

void XDuelGestureChecker::Init()
{
	m_strMotionName.clear();
}

void XDuelGestureChecker::SetGesture( const wchar_t* szMotionName )
{
	m_strMotionName = szMotionName;
}

bool XDuelGestureChecker::CheckUsableGesture( XObject* pObject )
{
	if(pObject == NULL || m_strMotionName.empty())
		return false;

	XModuleMotion* pModuleMotion = pObject->GetModuleMotion();
	if(pModuleMotion == NULL)
		return false;

	XModuleActorControl* pModuleActorControl = pObject->GetModuleActorControl();

	if(pModuleMotion->GetCurrMotion() == MOTION_NAME_IDLE && pModuleActorControl->IsReservedHitByTalent() == false)
	{
		return true;
	}

	return false;
}
