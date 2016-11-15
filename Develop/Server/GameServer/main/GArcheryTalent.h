#pragma once

#include "GTalent.h"
#include "MMemPool.h"
#include "GTalentProjectile.h"

class GArcheryTalent : public GTalent, public MMemPool<GArcheryTalent>
{
private:
	GTalentProjectile	m_TalentProjectile;
protected:
	virtual void	OnStart();
	virtual void	OnUpdatePhaseAct(float fDelta);
public:
	GArcheryTalent(GEntityActor* pOwner, GTalentInfo* pTalentInfo, TALENT_TARGET_INFO Target=TALENT_TARGET_INFO::Invalid(), bool bGainStress=true) : GTalent(pOwner, pTalentInfo, Target, bGainStress) {}
	virtual ~GArcheryTalent() {}
};
