#pragma once

#include "CSTriggerInfo.h"

class GModuleAI;
class GEntityNPC;
class GEntityActor;
class GJob;
class GNPCSession;

struct USETALENT_PARAM
{
	bool bCheckValidate;
	bool bRotatable;
	bool bGainStress;
	float fChaseLimitTime;

	USETALENT_PARAM()
		: bCheckValidate(false)
		, bRotatable(true)
		, bGainStress(false)
		, fChaseLimitTime(0.0f)
	{}
};

class GJobRunner : public MTestMemPool<GJobRunner>
{
public:
	GJobRunner();

	bool IsNowCombat(GModuleAI* pmAI);
	bool IsPeaceBehaviorMode( GModuleAI* pmAI );

	// 모든 잡을 삭제 (미션모드일 경우엔 미션잡 적용)
	void ClearJob(GModuleAI* pmAI);

	void Move(GModuleAI* pmAI, vec3 vTarPos, vec3 vTarDir=vec3::ZERO, float fLimitSumOfPathLength=FLT_MAX, unsigned long nMarkerID=0);
	void MoveToMarker(GModuleAI* pmAI, const MARKER_INFO& Marker, float fLimitSumOfPathLength=FLT_MAX);
	void MoveToSensor(GModuleAI* pmAI, const SENSOR_INFO& Sensor, float fLimitSumOfPathLength=FLT_MAX);
	bool UseTalent(GEntityNPC* pThisNPC, GEntityActor* pTargetActor, int nTalentID, USETALENT_PARAM param = USETALENT_PARAM());
	void MoveToEntity(GModuleAI* pmAI, GEntityActor* pActor, float fLeastDistance, bool bInfinity=true, float fDurationTime = JOB_DURATION_INFINITE_TIME);
	void Follow(GModuleAI* pmAI, GEntityActor* pActor, float fLeastDistance, float fDurationTime);
	void Idle( GModuleAI* pmAI, float fIdleTime );

	void FleeFromEntity(GModuleAI* pmAI, MUID uidTarget);
	void FleeToHome(GModuleAI* pmAI);
	bool MoveToTargetCloser(GModuleAI* pmAI, GEntityActor* pTargetActor, int nTalentID, float fChaseLimitTime=0.0f);

	void Roam(GModuleAI* pmAI, float fRadius, float fDurationTime=0.0f);
	void Patrol(GModuleAI* pmAI, MARKER_LIST MarkerList, int nPatrolType);
	void Patrol( GModuleAI* pmAI, const vector<MARKER_INFO>& MarkerList, int nPatrolType );
	void Speak( GModuleAI* pmAI, const wchar_t* pszSpeakType, wstring strText, float fMaintainTime = -1.0f);
	void RunScript( GModuleAI* pmAI, MUID uidOpponent, wstring strTable, wstring strFunc );
	void Guard( GModuleAI* pmAI, int nGuardTalentID, float fGuardingTime );
	void Flee( GModuleAI* pmAI, FleeType nFleeType, float fDurationTime );
	void Yell( GModuleAI* pmAI, int nRange );

	void Warp(GModuleAI* pmAI, vec3 vecPos, vec3 vecDir, bool bRouteToMe);
	void GainBuff(GModuleAI* pmAI, int nBuffID);
	
	void Despawn(GModuleAI* pmAI, bool bSpawnEnable);
	void Die(GModuleAI* pmAI, MUID uidTarget);
	void Assist(GModuleAI* pmAI, MUID uidTarget);
	void Attack(GModuleAI* pmAI, MUID uidTarget, int nDamage);
	void BreakPart(GModuleAI* pmAI, uint8 nPartID, MUID uidOpponent);
	void FaceTo(GModuleAI* pmAI, MUID uidTarget);
	void WaitFor(GModuleAI* pmAI, GEntityNPC* pTarget);
	void NextAction(GModuleAI* pmAI);

	void Summon( GModuleAI* pmAI, int nNPCID, vec3 vStartPos );

private:
	void PushJob( GModuleAI* pmAI, GJob* pJob );
	void PushMissionJob( GModuleAI* pmAI, GJob* pJob );
	void PushJobByMode( GModuleAI* pmAI, GJob* pJob );
	bool IsSessionMode( GEntityNPC* pNPC );
};
