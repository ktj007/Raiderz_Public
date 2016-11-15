#pragma once

#include "CSDef.h"

class GEntityActor;
class GTalentInfo;

class GuardEffector
{
public:
	enum Result
	{
		GUARD_FAILED = 0,
		GUARD_SUCCESS,
	};

public:
	// 가드가 성공했는지 여부, 가드를 할 수 있고 성공했다면 true를 반환
	Result CheckGuard(GEntityActor* pGuarder, GEntityActor* pAttacker, GTalentInfo* pTalentInfo);

	// 가드가 성공했는지 여부, 가드를 할 수 있고 성공했다면 true를 반환
	GUARD_TYPE GetGuardLevel( GEntityActor* pGuarder, GEntityActor* pAttacker, GTalentInfo* pAttackTalentInfo );
	
	// 방어성공 효과 적용
	void ApplyEffect_Guard( GEntityActor* pGuarder, GEntityActor* pAttacker, GTalentInfo* pAttackTalentInfo, GUARD_TYPE nGuardType, uint16 nCheckTime, int nDamage);

	// 완전방어 효과 적용
	void ApplyEffect_PerfectGuard( GEntityActor* pGuarder, GEntityActor* pAttacker, GTalentInfo* pAttackTalentInfo, int nDamage );

	// 부분방어 효과 적용
	void ApplyEffect_PartialGuard( GEntityActor* pGuarder, GEntityActor* pAttacker, GTalentInfo* pAttackTalentInfo, int nDamage );

	// 가드를 시도 했는지 여부
	bool IsTryGuard(GEntityActor* pGuarder);

	// 가드데미지 구하기
	int CalcGuardedDamage(GEntityActor* pGuarder, GEntityActor* pAttacker, GUARD_TYPE nGuardType, int nDamage);

	// 감쇄데미지 구하기
	float CalcReducedDamage(GEntityActor* pGuarder, GEntityActor* pAttacker, GUARD_TYPE nGuardType);
private:
	// 완전 방어가 가능한지 여부
	bool CheckPerfectGuard(GEntityActor* pGuarder, GEntityActor* pAttacker);

	// 방어 성공 패킷
	void RouteGuardDefense(GEntityActor* pGuarder, GUARD_TYPE nType, UIID nAttackerUIID, int nTalentID, uint32 nHitTime, int nGuardedDamage);

	// 세부방어 효과적용 구현함수
	void ApplyEffect_SpecificGuardImpl( GEntityActor* pGuarder, GEntityActor* pAttacker, GTalentInfo* pAttackTalentInfo, GUARD_TYPE nGuardType, int nDamage );

	// 절대방어 가능한지 여부
	bool IsAbsoluteGuard( GEntityActor* pGuarder );
};
