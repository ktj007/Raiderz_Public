#pragma once

#include "VValidatorInfoMgr.h"

class VDataReport
{
public:
	VDataReport() {}
	virtual ~VDataReport()	{}

	virtual void							Init(VValidatorInfoMgr* vInfoMgr) {}
	virtual void							Start(VValidatorInfoMgr* vInfoMgr, BackgroundWorker^ worker, int nFullCompleteCount, int& nCurrCompleteCount, int& nMsgIndex) {}

	virtual int								GetCheckCount() { return 0; }
	virtual VALIDATOR_STATE					GetID() { return VS_NONE; }
};
