#pragma once

#include "GTalent.h"
#include "MMemPool.h"

/// ¸¶¹ý ÅÅ·±Æ®
class GMagicTalent : public GTalent, public MMemPool<GMagicTalent>
{
protected:
	virtual void	OnStart() override;
	virtual void	OnEnterPhaseAct() override;

	void AimingDirection();

public:
	GMagicTalent(GEntityActor* pOwner, GTalentInfo* pTalentInfo, TALENT_TARGET_INFO Target=TALENT_TARGET_INFO::Invalid(), bool bGainStress=true);
	virtual ~GMagicTalent() {}

};

