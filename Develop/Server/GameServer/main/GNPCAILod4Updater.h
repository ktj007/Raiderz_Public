#pragma once

#include "GNPCAIUpdater.h"
#include "MTime.h"

class GNPCAILod4Updater: public GNPCAIUpdater, public MTestMemPool<GNPCAILod4Updater>
{
public:
	GNPCAILod4Updater(GNPCAI* pSubject);
	virtual ~GNPCAILod4Updater();

	virtual void Update(float fDelta);
	virtual float ResetRemainDelta(void);

private:
	MRegulator m_UpdateRagulator;
	float m_fCumulateDeltaSeconds;
};
