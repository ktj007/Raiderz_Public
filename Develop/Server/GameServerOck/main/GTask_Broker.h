#ifndef _GTASK_BROKER_H
#define _GTASK_BROKER_H

class GModuleAI;
class GEntityNPC; 
class GTalentInfo;

class GTask;

class GTaskBroker
{
public:
	GTask* New_MoveStraight(GEntityNPC* pOwnerNPC, vec3 vTarPos, float fLeastGap);
	GTask* New_MoveDefinite(GEntityNPC* pOwnerNPC, vec3 vTarPos, float fLeastGap);
	GTask* New_UseTalent(GEntityNPC* pOwnerNPC, GTalentInfo* pTalentInfo, MUID uidTarget, bool bGainStress, bool bSetDirection);
	GTask* New_Idle(GEntityNPC* pOwnerNPC, float fIdleTime);
	GTask* New_WaitFor(GEntityNPC* pOwnerNPC, MUID uidTarget);
	GTask* New_Guard(GEntityNPC* pOwnerNPC, int nGuardTalentID, float fGuardingTime);
};

#endif