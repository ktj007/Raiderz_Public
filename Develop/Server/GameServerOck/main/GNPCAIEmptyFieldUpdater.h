#pragma once

#include "GNPCAIUpdater.h"

class GNPCAIEmptyFieldUpdater: public GNPCAIUpdater, public MTestMemPool<GNPCAIEmptyFieldUpdater>
{
public:
	GNPCAIEmptyFieldUpdater(GNPCAI* pSubject);
	virtual ~GNPCAIEmptyFieldUpdater();

	virtual void Update(float fDelta);
};
