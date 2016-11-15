#pragma once

#include "GTalentInfo.h"

class GEntityActor;
class GField;

class GEffecteeCalculator
{
public:
	GEffecteeCalculator(void);
	~GEffecteeCalculator(void);

	// 해당 탤런트 타겟타입에 맞는 조건의 액터들을 반환
	vector<GEntityActor*> Calculate(GField* pField, 
									GEntityActor* pUser, 
									TALENT_TARGET nTargetType,
									GEntityActor* pTargetActor, 
									vec3 vPoint,
									const float fRadius, 
									GEntityActor* pAttacker=NULL,
									GEntityActor* pDefender=NULL) const;
private:
	// 탤런트 타겟타입에 맞는 엔티티타입을 반환
	ENTITY_TYPEID	GetFindActorType(GEntityActor* pUser, TALENT_TARGET nTargetType) const;
	// 목표 액터가 유효한지 반환
	bool			IsValidTarget( GEntityActor* pTarget ) const;
	// 목표 액터가 효과 범위 안에 있는지 반환
	bool			IsValidDistance(vec3 vPoint, 
									GEntityActor* pTarget, 
									const float fEffectRadius ) const;
	// 목표 액터와 같은 필드에 있는지 반환
	bool			IsSameField(GEntityActor* pUser, GEntityActor* pTarget) const;
	// 이미 추가된 액터인지 반환
	bool			IsAlreadyInserted(const vector<GEntityActor*> &vecEffectees, GEntityActor* pTarget) const;

	// 유효한 결투 상대자를 반환
	GEntityActor*	FindValidDuelOpponent(	GEntityActor* pUser, 
											TALENT_TARGET nTargetType,
											vec3 vPoint, 
											const float fRadius) const;
	// 파티를 대상으로 목표를 찾아냄
	void OnTargetParty( GField* pField, 
						GEntityActor* pUser, 
						GEntityActor* pCenterActor, 
						const float fRadius,
						vector<GEntityActor*> &vecEffectees) const;
	// 자신을 대상으로 목표를 찾아냄
	void OnTargetSelf(GEntityActor* pUser, vector<GEntityActor*> &vecEffectees) const;
	// 영역을 대상으로 목표를 찾아냄
	void OnTargetArea(	GField* pField, 
						GEntityActor* pUser,
						ENTITY_TYPEID nFindActorType, 
						vec3 vPoint, 
						const float fRadius,
						vector<GEntityActor*> &vecEffectees) const;
	// 지정된 상대를 대상으로 목표를 찾아냄
	void OnTargetVictim(GEntityActor* pTargetActor, vector<GEntityActor*> &vecEffectees ) const;
	// 지정된 상대를 대상으로 목표를 찾아냄
	void OnTargetAttacker(GEntityActor* pAttacker, vector<GEntityActor*> &vecEffectees ) const;
	// 지정된 상대를 대상으로 목표를 찾아냄
	void OnTargetDefender(GEntityActor* pDefender, vector<GEntityActor*> &vecEffectees ) const;
};
