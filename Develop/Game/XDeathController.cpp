#include "stdafx.h"
#include "XDeathController.h"


bool XDeathController::IsDeadMotionImmediately( MF_STATE nMFState )
{
	if (nMFState != MF_KNOCKBACK &&
		nMFState != MF_THROWUP &&
		nMFState != MF_GRAPPLED &&
		nMFState != MF_SWALLOWED &&
		nMFState != MF_DRAG &&
		nMFState != MF_UPPERED)
	{
		return true;
	}
	return false;
}

bool XDeathController::IsDeadActionImmediatelyOnHit( XActor* pActor, MF_STATE nMFState )
{
	if(pActor->IsDead())
	{
		return IsDeadMotionImmediately(nMFState);
	}
	return false;
}