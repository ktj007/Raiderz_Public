#pragma once

#include "GActorObserver.h"

class GEntityNPC;

class GNPCStress : private GActorObserver, public MTestMemPool<GNPCStress>
{
public:
	GNPCStress(GEntityNPC* pOwner);
	~GNPCStress(void);

	int	 GetStress() const;
	void SetStress(int val);
	void AddStress(int val);
	void Clear();

	void Update(float fDelta);

private:
	// 전투 시작
	virtual void OnCombatBegin(GEntityActor* pOwner) override;
	// 적을 공격했을때 이벤트
	virtual void OnHitEnemy(GEntityActor* pOwner, uint32 nCombatResultFalg, GEntityActor* pTarget, GTalentInfo* pTalentInfo) override;
private:
	GEntityNPC* m_pOwner;
	int	m_nStress;
	bool m_bStressChanged;
};
