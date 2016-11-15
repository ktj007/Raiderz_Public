#pragma once

#include "GActorObserver.h"

class GEntityActor;
class GTalentInfo;

class GActorCounter : private GActorObserver, public MTestMemPool<GActorCounter>
{
public:
	GActorCounter(GEntityActor* pOwner);
	~GActorCounter(void);

private:
	// 탤런트 사용이 시작될때 호출되는 이벤트
	virtual void OnUseTalent( GEntityActor* pUser, GTalentInfo* pTalentInfo ) override;
	// 탤런트 사용이 완료됐을때 호출되는 이벤트
	virtual void OnFinishTalent(GTalentInfo* pTalentInfo) override;
	// 탤런트를 무효시켰을때 호출되는 이벤트
	virtual void OnTalentImmuned(GEntityActor* pOwner, GEntityActor* pAttacker, GTalentInfo* pTalentInfo) override;
	// 탤런트를 회피시켰을때 호출되는 이벤트
	virtual void OnTalentAvoid(GEntityActor* pOwner, GEntityActor* pAttacker, GTalentInfo* pTalentInfo) override;
	
	// 적을 공격했을때 이벤트
	virtual void OnHitEnemy(GEntityActor* pOwner, uint32 nCombatResultFalg, GEntityActor* pTarget, GTalentInfo* pTalentInfo) override;

private:
	void Clear();
	bool IsCounter() const;
private:
	// 소유자
	GEntityActor*	m_pOwner;
	// 사용중인 탤런트 정보
	GTalentInfo*	m_pUsingTalentInfo;
	// 무효화된 공격을 받았는지 여부
	bool			m_bImmunedTalent;
};
