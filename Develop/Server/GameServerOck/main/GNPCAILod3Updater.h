#pragma once

#include "GNPCAIUpdater.h"

class GNPCAILod3Updater: public GNPCAIUpdater, public MTestMemPool<GNPCAILod3Updater>
{
public:
	GNPCAILod3Updater(GNPCAI* pSubject);
	virtual ~GNPCAILod3Updater();

	virtual void Update(float fDelta);
};
