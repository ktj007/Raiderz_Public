#pragma once

#include "GEFfectTimingChecker.h"
#include "CSTalentInfo.h"

class GBuff;
class GEntityActor;


class GBuffModifierApplier : public GEFfectTimingChecker::Listener, public MTestMemPool<GBuffModifierApplier>
{
public:
	GBuffModifierApplier(GBuff* pOwner);
	~GBuffModifierApplier(void);

private:
	virtual bool OnEvent(TALENT_CONDITION nTiming) override;

private:
	bool ApplyEffect();
	bool RestoreEffect();
	void OnDuplicated();
	void OnStacked();
	void ClearResotreInfo();

private:
	struct RestoreInfo
	{
		CSActorModifier		ActorModifier;
		CSPlayerModifier	PlayerModifier;
		vector<CSRiposte>	vecRiposte;
	} m_InfoRestore;

	GBuff* m_pOwner;
};
