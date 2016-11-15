#pragma once

#include "GEFfectTimingChecker.h"

class GBuff;
class GBuffInfo;
class GEntityActor;

class GBuffInstantApplier : public GEFfectTimingChecker::Listener, public MTestMemPool<GBuffInstantApplier>
{
public:
	GBuffInstantApplier(GBuff* pOwner);
	~GBuffInstantApplier(void);

private:
	virtual bool OnEvent(TALENT_CONDITION nTiming) override;

private:
	void GainBuffDamageAndHeal(GEntityActor* pEffecteeTarget, GBuff* pBuff, MUID uidUser, GEntityActor* pUser );
	void BuffHealed(GEntityActor* pEffecteeTarget, GBuffInfo* pBuffInfo, int nHealHPAmount);
	void ApplySingleEffect(GEntityActor* pEffecteeTarget, MUID uidUser);
	bool ApplyEffects();
	bool IsNeedEffect();
	void RouteGainInstantEffect(GEntityActor* pActor, GBuff* pBuff);
	
private:
	GBuff* m_pOwner;
};
