#pragma once

#include "GNPCAIUpdater.h"

class GNPCAILod5Updater: public GNPCAIUpdater, public MTestMemPool<GNPCAILod5Updater>
{
public:
	GNPCAILod5Updater(GNPCAI* pSubject);
	virtual ~GNPCAILod5Updater();

	virtual void Update(float fDelta);
};
