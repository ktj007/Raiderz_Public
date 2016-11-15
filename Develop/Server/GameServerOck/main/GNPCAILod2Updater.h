#pragma once

#include "GNPCAIUpdater.h"
#include "MTime.h"

class GNPCAILod2Updater: public GNPCAIUpdater, public MTestMemPool<GNPCAILod2Updater>
{
public:
	GNPCAILod2Updater(GNPCAI* pSubject);
	virtual ~GNPCAILod2Updater();

	virtual void Update(float fDelta);
	virtual float ResetRemainDelta(void);

private:
	MRegulator m_ActionUpdateRegulator;
	MRegulator m_TargetUpdateRegulator;
	float m_fActionCumulateDeltaSeconds;
	float m_fTargetCumulateDeltaSeconds;
};
