#pragma once

#include "VMeshDataValidator.h"
#include "VDataReport.h"

class CProgressCheck;

class VMeshDataReport : public VDataReport
{
private:
	VMeshDataValidator					m_MeshDataValidator;

public:
	VMeshDataReport();
	virtual ~VMeshDataReport()	{}

	virtual void						Init(VValidatorInfoMgr* vInfoMgr);
	virtual void						Start(VValidatorInfoMgr* vInfoMgr, BackgroundWorker^ worker, int nFullCompleteCount, int& nCurrCompleteCount, int& nMsgIndex);

	virtual int							GetCheckCount();

	VErrorMsg&							GetAnimationListErrorMsg();
	VErrorMsg&							GetAnimationEventErrorMsg();
	VErrorMsg&							GetAnimationInfoErrorMsg();
};
