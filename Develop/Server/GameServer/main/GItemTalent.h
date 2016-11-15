#pragma once

#include "GTalent.h"
#include "GTalentEventChecker.h"
#include "MMemPool.h"

/// ¸¶¹ý ÅÅ·±Æ®
class GItemTalent : public GTalent, public MMemPool<GItemTalent>
{
protected:
	virtual void	OnEnterPhaseAct();
	
public:
	GItemTalent(GEntityActor* pOwner, GTalentInfo* pTalentInfo, TALENT_TARGET_INFO Target, bool bGainStress=true);
	virtual ~GItemTalent() {}

	
};

