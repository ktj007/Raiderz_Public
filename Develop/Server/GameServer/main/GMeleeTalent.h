#pragma once

#include "GTalent.h"
#include "MMemPool.h"

class GMeleeTalent : public GTalent, public MMemPool<GMeleeTalent>
{
protected:
	virtual void	OnStart();
public:
	GMeleeTalent(GEntityActor* pOwner, GTalentInfo* pTalentInfo, TALENT_TARGET_INFO Target=TALENT_TARGET_INFO::Invalid(), bool bGainStress=true) : GTalent(pOwner, pTalentInfo, Target, bGainStress) {}
	virtual ~GMeleeTalent() {}
};
