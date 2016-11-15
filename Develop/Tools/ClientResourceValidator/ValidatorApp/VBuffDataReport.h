#pragma once

#include "VBuffDataValidator.h"
#include "VDataReport.h"

class CProgressCheck;

class VBuffDataReport : public VDataReport
{
private:
	VBuffDataValidator					m_BuffDataValidator;

public:
	VBuffDataReport();
	virtual ~VBuffDataReport()	{}

	virtual void						Init(VValidatorInfoMgr* vInfoMgr);
	virtual void						Start(VValidatorInfoMgr* vInfoMgr, BackgroundWorker^ worker, int nFullCompleteCount, int& nCurrCompleteCount, int& nMsgIndex);

	virtual int							GetCheckCount();

	//VErrorMsg&							GetAnimationListErrorMsg();
	//VErrorMsg&							GetAnimationEventErrorMsg();
	//VErrorMsg&							GetAnimationInfoErrorMsg();
};
