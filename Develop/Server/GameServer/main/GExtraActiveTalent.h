#pragma once

#include "GTalent.h"
#include "GExtraActiveTalentRunner.h"
#include "MMemPool.h"
#include "MTime.h"

class GExtraActiveTalent : public GTalent, public MTestMemPool<GExtraActiveTalent>
{
public:
	GExtraActiveTalent(GEntityActor* pOwner, GTalentInfo* pTalentInfo, TALENT_TARGET_INFO Target, bool bGainStress=true) : GTalent(pOwner, pTalentInfo, Target, bGainStress) {}
	virtual ~GExtraActiveTalent() {}
};


class GExtraActiveTalent_Grapple : public GTalent, public MMemPool<GExtraActiveTalent_Grapple>
{
protected:
	virtual void	OnStart() override;
	virtual void	OnEnterPhaseExtra() override;
	virtual void	OnHitArea( GEntityActor* pVictim, uint16 nCheckTime, int8 nCapsuleGroupIndex, int8 nCapsuleIndex ) override;
public:
	GExtraActiveTalent_Grapple(GEntityActor* pOwner, GTalentInfo* pTalentInfo, TALENT_TARGET_INFO Target, bool bGainStress=true);
	virtual ~GExtraActiveTalent_Grapple() {}
};

class GExtraActiveTalent_Swallowed : public GTalent, public MMemPool<GExtraActiveTalent_Swallowed>
{
private:
	MRegulator	m_rgrPhaseExtra2;
	MUID		m_uidTarget;
protected:
	virtual void	OnStart() override;
	virtual void	OnEnterPhaseExtra() override;
	virtual void	OnEnterPhaseExtra2() override;
	virtual void	OnEnterPhaseExtra3() override;
	virtual void	OnUpdatePhaseExtra(float fDelta) override;
	virtual void	OnHitArea( GEntityActor* pVictim, uint16 nCheckTime, int8 nCapsuleGroupIndex, int8 nCapsuleIndex ) override;
public:
	GExtraActiveTalent_Swallowed(GEntityActor* pOwner, GTalentInfo* pTalentInfo, TALENT_TARGET_INFO Target, bool bGainStress=true);
	virtual ~GExtraActiveTalent_Swallowed() {}
	
};