#pragma once

#include "VItemDataValidator.h"
#include "VDataReport.h"

class CProgressCheck;

class VItemDataReport : public VDataReport
{
private:
	VItemDataValidator					m_ItemDataValidator;

public:
	VItemDataReport();
	virtual ~VItemDataReport()	{}

	virtual void						Init(VValidatorInfoMgr* vInfoMgr);
	virtual void						Start(VValidatorInfoMgr* vInfoMgr, BackgroundWorker^ worker, int nFullCompleteCount, int& nCurrCompleteCount, int& nMsgIndex);

	virtual int							GetCheckCount();

	VErrorMsg&							GetItemInfoErrorMsg();
};
