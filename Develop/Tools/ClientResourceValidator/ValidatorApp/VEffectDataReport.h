#pragma once

#include "VEffectDataValidator.h"
#include "VDataReport.h"

class CProgressCheck;

class VEffectDataReport : public VDataReport
{
private:
	VEffectDataValidator					m_EffectDataValidator;

public:
	VEffectDataReport();
	virtual ~VEffectDataReport()	{}

	virtual void							Init(VValidatorInfoMgr* vInfoMgr);
	virtual void							Start(VValidatorInfoMgr* vInfoMgr, BackgroundWorker^ worker, int nFullCompleteCount, int& nCurrCompleteCount, int& nMsgIndex);

	virtual int								GetCheckCount();

	VErrorMsg&								GetEffectInfoErrorMsg();
	VErrorMsg&								GetTalentEffectInfoErrorMsg();
	VErrorMsg&								GetBuffEffectInfoErrorMsg();
};
