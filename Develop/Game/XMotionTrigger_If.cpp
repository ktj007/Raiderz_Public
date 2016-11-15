#include "stdafx.h"
#include "XMotionTrigger_If.h"

bool XMotionTrigger_IfDead::Check( XActor* pActor )
{
	return pActor->IsDead();
}