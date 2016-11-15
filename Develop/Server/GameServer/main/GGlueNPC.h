#pragma once

#include "GGlueActor.h"
#include "CSChrInfo.h"

class GEntityNPC;
struct GDamageInfo;
struct SPAWN_INFO;
class GGluePlayer;

class GGlueNPC: public GGlueActor, public MTestMemPool<GGlueNPC>
{
public:
	GGlueNPC(GEntityNPC* pOwner);
	virtual ~GGlueNPC();

	const string		GetLuaTableName();

	void RunSpawnScript(const SPAWN_INFO* pSpawnInfo);
	void RunDieScript(const SPAWN_INFO* pSpawnInfo);
public:
	int GetID();
	int GetUserData(int index);
	void SetUserData(int index, int value);
	void ReserveUserData(int nNPCID, int nSize);
	void Move(vec3 vTarPos);
	void MoveToMarker(int nMarkerID);
	void MoveToSensor(unsigned long nMarkerID);
	void Attack(GGlueActor* pTargetActorDelegator);
	void Follow(GGlueActor* pActorDelegator, float fLeastDistance);
	void EnableCombat(void);
	void EnableCombatWithFaction(void);
	void DisableCombat(void);
	void EnableInteract( bool bEnable );
	bool UseTalent(int nTalentID, GGlueActor* pActorDelegator);
	bool UseTalentSelf(int nTalentID);	
	bool MoveToActor(GGlueActor* pTargetActorDelegator, int nTalentID);
	bool MoveToActorWithDuration(GGlueActor* pTargetActorDelegator, int nTalentID, float fDuration);
	void Roam(float fRadius, float fDurationTime = 0.0f);
	void Patrol(MWLua::table tableMarkers, int nPatrolType);
	void PatrolInstant(MWLua::table tableMarkers, int nPatrolType);
	void ClearJob(void);
	void Say(const char* szText, float fMaintainTime = 0.0f);
	void Ballon(const char* szText, float fMaintainTime = 0.0f);
	void SayNow(const char* szText, float fMaintainTime = 0.0f);
	void BallonNow(const char* szText, float fMaintainTime = 0.0f);
	void Shout(const char* szText, float fMaintainTime = 0.0f);	
	void SayAndNarration(const char* szText, float fMaintainTime = 0.0f);
	void SayAndNarrationNow(const char* szText);
	void NonDelaySay(const char* szText);
	void NonDelayBallon(const char* szText);
	void NonDelayShout(const char* szText);
	void NonDelaySayAndNarration(const char* szText);
	void ScriptSelf(const char* szText);	
	void Wait(float fSecond);
	GGlueActor* Aggro(const char* pszType, float fDuration);
	int GetCurrentTalent();
	void BreakPart(uint8 nPartID, GGlueActor* pAttacker);
	bool SetTimer( int nTimerID, float fIntervalTime, bool bRepeat/*=false*/ );
	bool KillTimer( int nTimerID );
	int GetMode(void);
	void Despawn(bool bSpawnEnable);
	void DespawnNow(bool bSpawnEnable);
	void Die(GGlueActor* pKiller);
	void Assist(GGlueNPC* pAlliedNPCDelegator);
	void Yell(int nRange);
	int GetSightRange(GGlueActor* pEnemy);
	void ReturnToHome(void);
	bool IsCheckBPart(int nPartID);
	bool IsAlwaysAttackable(void);
	bool IsNowCombat(void);
	vec3 GetSpawnPos(void);
	void FleeFromEnemy(float fDurationTime);
	void FleeToHome(float fDurationTime);
	GDamageInfo& GetDamagedInfo(void);
	void SetDecayTimeSec(float fTime);
	void FaceTo(GGlueActor* pTargetActorDelegator);
	void SetAlwaysRun(bool bRun);
	void EnableInteraction();
	void DisableInteraction();
	GGluePlayer* GetLastInteractPlayer();	
	bool IsCooldown(int nTalentID);
	void ChangeAA(int nType);
	// 목표액터로부터 일정거리 유지하기
	void StayawayFrom(GGlueActor* pTargetActor, float fDistance);


	// 먹을 수 있는지 여부
	bool IsSwallowable();
	// 먹은 것 뱉기
	bool Spewup();
	// 타운트 가능여부 설정
	void SetTauntable(bool bEnable);

	bool EnableSensorCheck(unsigned long nSensorID);

	// 목표 액터와 연계하여 대기하기
	void WaitFor(GGlueNPC* pTarget);
	void NextAction();

	// 목표 타게팅하기
	void SetTarget(GGlueNPC* pTarget);
	GGlueActor* GetTarget();

	// 부하 소환
	void Summon(int nNPCID, vec3 vStartPos);
	// 부하 즉시 소환 
	GGlueNPC* SummonNow(int nNPCID, vec3 vStartPos);
	// 자신을 소환한 NPC 반환 (없다면 NULL)
	GGlueNPC* GetSummoner();
	// 소환된 부하 갯수
	int GetSummonCount();
	// NPC 상태들 초기화
	void ResetNPC();
	// 현재 위치를 홈포인트로 저장
	void SaveHomePoint();

	// 결투 관람
	void AttachObserveDuel( GGluePlayer* pObserver );
	void DetachObserveDuel( GGluePlayer* pObserver );

public:
	GEntityNPC* GetOwnerNPC(); // TODO: private로 이동되야함
	
};
