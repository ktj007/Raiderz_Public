#pragma once

#include "GNPCAIUpdater.h"

class GNPCAIDefaultUpdater: public GNPCAIUpdater, public MTestMemPool<GNPCAIDefaultUpdater>
{
public:
	GNPCAIDefaultUpdater(GNPCAI* pSubject);
	virtual ~GNPCAIDefaultUpdater();

	virtual void Update(float fDelta);

private:
	MRegulator m_TargetUpdateRegulator;
	float m_fTargetCumulateDeltaSeconds;
};
