#pragma once

#include "CSMotionFactorInfo.h"

class GEntityActor;
class GGlueField;

class GGlueActor
{
public:
	GGlueActor(GEntityActor* pOwner);
	virtual ~GGlueActor();

	GEntityActor* GetOwnerActor() const;
	bool IsInvalidOwner() const;

public:
	virtual bool IsNull(void) const;
	bool Warp(vec3 vPos);
	bool CheckBuff(int nBuffID );
	int GetDebuffCount(void);
	bool GainBuff(int nBuffID );
	bool GainBuffDetail(int nBuffID, float fDuration, float fPeriod );
	bool RemoveBuff(int nBuffID);
	void RemoveAllBuff();
	MUID GetUID(void);	
	vec3 GetDir(void);
	vec3 GetFowardDir(GGlueActor* pTargetActorDelegator);
	vec3 GetPos(void);
	float DistanceTo(GGlueActor* pTargetActorDelegator);
	MF_STATE GetCurrentMotionFactor(void);
	int GetFieldID(void);
	bool IsPlayer(void);
	bool IsNPC(void);
	bool SetMFModValue(int nType, float fValue);
	bool SetMFModWeight(int nType, float fWeight);
	GGlueField* GetField(void);
	int GetMaxHP(void);
	int GetMaxEN(void);
	int GetMaxSTA(void);
	int GetHP(void);
	int GetEN(void);
	int	GetSTA(void);
	bool Narration(const char* szText);
	bool NarrationNow(const char* szText);
	const char* GetName(void);
	bool IsDead() const;

	// 목표액터 뒤에 액터가 있는가?
	bool IsRear(GGlueActor* pTarget);

	// 목표액터 앞에 액터가 있는가?
	bool IsFront(GGlueActor* pTarget);
	
	// 전투 종료
	void MakePeace();

	// 레벨 반환
	int GetLevel() const;

	// 사용자 이벤트 호출
	void RunFieldEvent(const char* pszName);
protected:
	GEntityActor* m_pOwner;		
	char m_szName[PLAYER_NAME_LEN + 1];
};
