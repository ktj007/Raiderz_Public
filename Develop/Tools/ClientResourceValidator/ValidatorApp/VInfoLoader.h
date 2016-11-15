#pragma once

#include "VValidatorInfoMgr.h"

class VInfoLoader
{
public:
	VInfoLoader() {}
	virtual ~VInfoLoader()	{}

	virtual void							Init()	{}
	virtual void							Load(BackgroundWorker^ worker, VValidatorInfoMgr * pValidatorInfoMgr, int nFullCompleteCount, int& nCurrCompleteCount, int& nMsgIndex) {}

	virtual int								GetLoadingCount() { return 0; }
	virtual LOADING_STATE					GetID() { return LS_NONE; }
};
