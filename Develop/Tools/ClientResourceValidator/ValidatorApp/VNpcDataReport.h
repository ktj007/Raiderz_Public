#pragma once

#include "VNpcDataValidator.h"
#include "VDataReport.h"

class CProgressCheck;

class VNpcDataReport : public VDataReport
{
private:
	VNpcDataValidator						m_NpcDataValidator;

public:
	VNpcDataReport();
	virtual ~VNpcDataReport()	{}

	virtual void							Init(VValidatorInfoMgr* vInfoMgr);
	virtual void							Start(VValidatorInfoMgr* vInfoMgr, BackgroundWorker^ worker, int nFullCompleteCount, int& nCurrCompleteCount, int& nMsgIndex);

	virtual int								GetCheckCount();

	VErrorMsg&								GetNpcInfoErrorMsg();
};
