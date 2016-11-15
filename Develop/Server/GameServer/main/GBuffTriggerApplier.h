#pragma once

#include "GEFfectTimingChecker.h"

class GBuff;

class GBuffTriggerApplier : public GEFfectTimingChecker::Listener, public MTestMemPool<GBuffTriggerApplier>
{
public:
	GBuffTriggerApplier(GBuff* pOwner);
	~GBuffTriggerApplier(void);

private:
	virtual bool OnEvent(TALENT_CONDITION nTiming) override;

private:
	bool ApplyEffect();

private:
	GBuff* m_pOwner;
};
