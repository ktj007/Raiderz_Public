#include "stdafx.h"
#include "GConst.h"
#include "GJob_Broker.h"
#include "GJob_Talent.h"
#include "GJob_MoveToEntity.h"
#include "GJob_MoveToPos.h"
#include "GJob_FleeFromEntity.h"
#include "GJob_FleeToHome.h"
#include "GJob_Patrol.h"
#include "GJob_Roam.h"
#include "GJob_RoundToPos.h"
#include "GJob_Idle.h"
#include "GJob_Curve.h"
#include "GJobT_Script.h"
#include "GJob_Speak.h"
#include "GJobT_Flee.h"
#include "GJobT_Yell.h"
#include "GJobT_Warp.h"
#include "GJobT_GainBuff.h"
#include "GJobT_Despawn.h"
#include "GJobT_Die.h"
#include "GJobT_Assist.h"
#include "GJobT_Attack.h"
#include "GJobT_BreakPart.h"
#include "GJobT_FaceTo.h"
#include "GJob_WaitFor.h"
#include "GJobT_NextAction.h"
#include "GJobT_Summon.h"

GJob*
GJobBroker::NewJob_UseTalent(GModuleAI* pmAI, GTalentInfo* pTalentInfo, MUID uidTarget)
{
	return new GJob_UseTalent(pmAI, pTalentInfo, uidTarget);
}

GJob*
GJobBroker::NewJob_Guard(GModuleAI* pmAI, int nGuardTalentID, float fGuardingTime)
{
	return new GJob_Guard(pmAI, nGuardTalentID, fGuardingTime);
}

GJob*
GJobBroker::NewJob_MoveToEntity(GModuleAI* pmAI, MUID uidTargetEntity, float fLeastDistance, bool bSetInifity)
{
	return new GJob_MoveToEntity(pmAI, uidTargetEntity, fLeastDistance, bSetInifity);
}

GJob*		
GJobBroker::NewJob_MoveToPos(GModuleAI* pmAI, vec3 vTarPos, vec3 vTarDir, float fLimitSumOfPathLength, unsigned long nMarkerID)
{
	_ASSERT(vTarPos.x >=-1000000);
	_ASSERT(vTarPos.x <= 1000000);
	_ASSERT(vTarPos.y >=-1000000);
	_ASSERT(vTarPos.y <= 1000000);
	_ASSERT(vTarPos.z >=-1000000);
	_ASSERT(vTarPos.z <= 1000000);

	return new GJob_MoveToPos(pmAI, vTarPos, vTarDir, fLimitSumOfPathLength, nMarkerID);
}

GJob*	
GJobBroker::NewJob_FleeFromEntity(GModuleAI* pmAI, MUID& uidTarget)
{
	return new GJob_FleeFromEntity(pmAI, uidTarget);
}


GJob* 
GJobBroker::NewJob_FleeToHome( GModuleAI* pmAI )
{
	return new GJob_FleeToHome(pmAI);
}

GJob*			
GJobBroker::NewJob_Patrol(GModuleAI* pmAI, MARKER_LIST& MarkerList, PATROL_TYPE PatrolType)
{
	return new GJob_Patrol(pmAI, MarkerList, PatrolType);
}

GJob*			
GJobBroker::NewJob_Patrol(GModuleAI* pmAI, const vector<MARKER_INFO>& MarkerList, PATROL_TYPE PatrolType)
{
	return new GJob_Patrol(pmAI, MarkerList, PatrolType);
}

GJob*			
GJobBroker::NewJob_Roam(GModuleAI* pmAI, float fRadius, float fDurationTime)
{
	return new GJob_Roam(pmAI, fRadius, fDurationTime);
}


GJob*			
GJobBroker::NewJob_Idle(GModuleAI* pmAI, float fIdleTime)
{
	return new GJob_Idle(pmAI, fIdleTime);
}

GJob* 
GJobBroker::NewJob_Script( GModuleAI* pmAI, MUID uidOpponent, wstring strTable, wstring strFunc )
{
	return new GJobT_Script(pmAI, uidOpponent, strTable, strFunc);
}

GJob* 
GJobBroker::NewJob_Speak(GModuleAI* pmAI, wstring strType, wstring strText, float fMaintainTime)
{
	return new GJob_Speak(pmAI, strType, strText, fMaintainTime);
}

GJob* GJobBroker::NewJob_Flee( GModuleAI* pmAI, FleeType nFleeType, float fDurationTime )
{
	return new GJobT_Flee(pmAI, nFleeType, fDurationTime);
}

GJob* GJobBroker::NewJob_Yell( GModuleAI* pmAI, int nRange )
{
	return new GJobT_Yell(pmAI, nRange);
}

GJob* GJobBroker::NewJob_Warp(GModuleAI* pmAI, vec3 vecPos, vec3 vecDir, bool bRouteToMe)
{
	return new GJobT_Warp(pmAI, vecPos, vecDir, bRouteToMe);
}

GJob* GJobBroker::NewJob_GainBuff(GModuleAI* pmAI, int nBuffID)
{
	return new GJobT_GainBuff(pmAI, nBuffID);
}

GJob* GJobBroker::NewJob_Despawn(GModuleAI* pmAI, bool bSpawnEnable)
{
	return new GJobT_Despawn(pmAI, bSpawnEnable);
}

GJob* GJobBroker::NewJob_Die(GModuleAI* pmAI, MUID uidTarget)
{
	return new GJobT_Die(pmAI, uidTarget);
}

GJob* GJobBroker::NewJob_Assist(GModuleAI* pmAI, MUID uidTarget)
{
	return new GJobT_Assist(pmAI, uidTarget);
}

GJob* GJobBroker::NewJob_Attack(GModuleAI* pmAI, MUID uidTarget, int nDamage)
{
	return new GJobT_Attack(pmAI, uidTarget, nDamage);
}

GJob* GJobBroker::NewJob_BreakPart(GModuleAI* pmAI, uint8 nPartID, MUID uidOpponent)
{
	return new GJobT_BreakPart(pmAI, nPartID, uidOpponent);
}

GJob* GJobBroker::NewJob_FaceTo(GModuleAI* pmAI, MUID uidTarget)
{
	return new GJobT_FaceTo(pmAI, uidTarget);
}

GJob* GJobBroker::NewJob_WaitFor( GModuleAI* pmAI, MUID uidTarget )
{
	return new GJob_WaitFor(pmAI, uidTarget);
}

GJob* GJobBroker::NewJob_NextAction( GModuleAI* pmAI )
{
	return new GJobT_NextAction(pmAI);
}

GJob* GJobBroker::NewJob_Summon( GModuleAI* pmAI, int nNPCID, vec3 vStartPos )
{
	return new GJobT_Summon(pmAI, nNPCID, vStartPos);
}
