#pragma once

#include "GNPCAIUpdater.h"

class GNPCAILod1Updater: public GNPCAIUpdater, public MTestMemPool<GNPCAILod1Updater>
{
public:
	GNPCAILod1Updater(GNPCAI* pSubject);
	virtual ~GNPCAILod1Updater();

	virtual void Update(float fDelta);
};
