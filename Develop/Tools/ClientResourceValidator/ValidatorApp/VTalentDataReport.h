#pragma once

#include "VTalentDataValidator.h"
#include "VDataReport.h"

class CProgressCheck;

class VTalentDataReport : public VDataReport
{
private:
	VTalentDataValidator				m_TalentDataValidator;

public:
	VTalentDataReport();
	virtual ~VTalentDataReport()	{}

	virtual void						Init(VValidatorInfoMgr* vInfoMgr);
	virtual void						Start(VValidatorInfoMgr* vInfoMgr, BackgroundWorker^ worker, int nFullCompleteCount, int& nCurrCompleteCount, int& nMsgIndex);

	virtual int							GetCheckCount();

	VErrorMsg&							GetTalentEventErrorMsg();
	//VErrorMsg&							GetAnimationEventErrorMsg();
	//VErrorMsg&							GetAnimationInfoErrorMsg();
};
