#pragma once

#include "GActorObserver.h"

class GEntityNPC;

class GNPCCombatAnticheat : public GActorObserver, public MTestMemPool<GNPCCombatAnticheat>
{
public:
	GNPCCombatAnticheat(GEntityNPC* pOwner);
	~GNPCCombatAnticheat(void);

	bool IsCheating() const;

private:
	void Reset();
	bool IsAvoidRange() const;

private:
	// 전투 시작
	virtual void OnCombatBegin(GEntityActor* pOwner) override;
	// 전투 종료
	virtual void OnCombatEnd(GEntityActor* pOwner) override;
	// 탤런트 사용이 시작될때 호출되는 이벤트
	virtual void OnUseTalent( GEntityActor* pUser, GTalentInfo* pTalentInfo ) override;
	// 탤런트 사용이 완료됐을때 호출되는 이벤트
	virtual void OnFinishTalent(GTalentInfo* pTalentInfo) override;
	// 적을 공격했을때 이벤트
	virtual void OnHitEnemy(GEntityActor* pOwner, uint32 nCombatResultFalg, GEntityActor* pTarget, GTalentInfo* pTalentInfo) override;
	// 적의 공격을 막았을때 이벤트
	virtual void OnGuardEnemy(GEntityActor* pOwner, GEntityActor* pTarget, GTalentInfo* pAttackTalentInfo) override;
	// 적이 탤런트를 무효화시켰을때 호출되는 이벤트
	virtual void OnTalentImmunedEnemy(GEntityActor* pOwner, GEntityActor* pTarget, GTalentInfo* pTalentInfo) override;

private:
	GEntityNPC* m_pOwner;
	int m_nAvoidCount;
	vec3 m_vLastAttackedPos;
	vec3 m_vLastAttackedDir;
};
