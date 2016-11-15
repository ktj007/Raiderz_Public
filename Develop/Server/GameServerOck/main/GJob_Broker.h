#ifndef _GJOB_BROKER_H
#define _GJOB_BROKER_H

#include "CSTriggerInfo.h"

class GEntityActor;
class GModuleAI;
class GTalentInfo;
class GJob;
class GJobRunner;
class GEntityNPC;

class GJobBroker
{
	friend GJobRunner;
public:
	static GJob* NewJob_UseTalent(GModuleAI* pmAI, GTalentInfo* pTalentInfo, MUID uidTarget=0);
	static GJob* NewJob_Guard(GModuleAI* pmAI, int nGuardTalentID, float fGuardingTime);
	static GJob* NewJob_MoveToEntity(GModuleAI* pmAI, MUID uidTargetEntity, float fLeastDistance=0.0f, bool bSetInifity=false);
	static GJob* NewJob_MoveToPos(GModuleAI* pmAI, vec3 vTarPos, vec3 vTarDir, float fLimitSumOfPathLength, unsigned long nMarkerID=0);
	static GJob* NewJob_FleeFromEntity(GModuleAI* pmAI, MUID& uidTarget);
	static GJob* NewJob_FleeToHome(GModuleAI* pmAI);
	static GJob* NewJob_Patrol(GModuleAI* pmAI, MARKER_LIST& MarkerList, PATROL_TYPE PatrolType);
	static GJob* NewJob_Patrol(GModuleAI* pmAI, const vector<MARKER_INFO>& MarkerList, PATROL_TYPE PatrolType);
	static GJob* NewJob_Roam(GModuleAI* pmAI, float fRadius, float fDurationTime=0.0f);
	static GJob* NewJob_Idle(GModuleAI* pmAI, float fIdleTime);
	static GJob* NewJob_Script( GModuleAI* pmAI, MUID uidOpponent, wstring strTable, wstring strFunc );
	static GJob* NewJob_Speak(GModuleAI* pmAI, wstring strType, wstring strText, float fMaintainTime);
	static GJob* NewJob_Flee( GModuleAI* pmAI, FleeType nFleeType, float fDurationTime );
	static GJob* NewJob_Yell(GModuleAI* pmAI, int nRange);	
	static GJob* NewJob_Warp(GModuleAI* pmAI, vec3 vecPos, vec3 vecDir, bool bRouteToMe);	
	static GJob* NewJob_GainBuff(GModuleAI* pmAI, int nBuffID);
	static GJob* NewJob_Despawn(GModuleAI* pmAI, bool bSpawnEnable);
	static GJob* NewJob_Die(GModuleAI* pmAI, MUID uidTarget);
	static GJob* NewJob_Assist(GModuleAI* pmAI, MUID uidTarget);
	static GJob* NewJob_Attack(GModuleAI* pmAI, MUID uidTarget, int nDamage);
	static GJob* NewJob_BreakPart(GModuleAI* pmAI, uint8 nPartID, MUID uidOpponent);
	static GJob* NewJob_FaceTo(GModuleAI* pmAI, MUID uidTarget);
	static GJob* NewJob_WaitFor( GModuleAI* pmAI, MUID uidTarget );
	static GJob* NewJob_NextAction( GModuleAI* pmAI );
	static GJob* NewJob_Summon( GModuleAI* pmAI, int nNPCID, vec3 vStartPos );
};

#endif
