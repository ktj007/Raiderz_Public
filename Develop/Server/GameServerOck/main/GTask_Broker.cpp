#include "stdafx.h"
#include "GTask_Broker.h"
#include "GModuleAI.h"
#include "GEntityNPC.h"
#include "GJob.h"
#include "GTask_Move.h"
#include "GTask_Talent.h"
#include "GTask_Idle.h"
#include "GTask_WaitFor.h"

/////////////////////////////////////////////////////////////////////////
// GTaskPushBroker //////////////////////////////////////////////////////////////

GTask* GTaskBroker::New_MoveStraight(GEntityNPC* pOwnerNPC, vec3 vTarPos, float fLeastGap)
{
	return new GTask_Move(pOwnerNPC, vTarPos, fLeastGap, MS_STRAIGHT);
}

GTask* GTaskBroker::New_MoveDefinite(GEntityNPC* pOwnerNPC, vec3 vTarPos, float fLeastGap)
{
	return new GTask_Move(pOwnerNPC, vTarPos, fLeastGap, MS_DEFINITE);
}

GTask* GTaskBroker::New_UseTalent(GEntityNPC* pOwnerNPC, GTalentInfo* pTalentInfo, MUID uidTarget, bool bGainStress, bool bSetDirection)
{
	return new GTask_UseTalent(pOwnerNPC, pTalentInfo, uidTarget, bGainStress, bSetDirection);
}

GTask* GTaskBroker::New_Idle(GEntityNPC* pOwnerNPC, float fIdleTime)
{
	return new GTask_Idle(pOwnerNPC, fIdleTime);
}

GTask* GTaskBroker::New_WaitFor(GEntityNPC* pOwnerNPC, MUID uidTarget)
{
	return new GTask_WaitFor(pOwnerNPC, uidTarget);
}

GTask* GTaskBroker::New_Guard(GEntityNPC* pOwnerNPC, int nGuardTalentID, float fGuardingTime)
{
	return new GTask_Guard(pOwnerNPC, nGuardTalentID, fGuardingTime);
}
