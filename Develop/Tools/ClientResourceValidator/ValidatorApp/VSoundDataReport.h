#pragma once

#include "VSoundDataValidator.h"
#include "VDataReport.h"

class CProgressCheck;

class VSoundDataReport : public VDataReport
{
private:
	VSoundDataValidator						m_SoundDataValidator;

public:
	VSoundDataReport();
	virtual ~VSoundDataReport()	{}

	virtual void							Init(VValidatorInfoMgr* vInfoMgr);
	virtual void							Start(VValidatorInfoMgr* vInfoMgr, BackgroundWorker^ worker, int nFullCompleteCount, int& nCurrCompleteCount, int& nMsgIndex);

	virtual int								GetCheckCount();

	VErrorMsg&								GetSoundInfoErrorMsg();
};
