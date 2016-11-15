#pragma once

#include "VInfoLoader.h"

class VCameraInfoLoader : public VInfoLoader
{
private:
	int										m_nCameraCount;

public:
	VCameraInfoLoader();
	virtual ~VCameraInfoLoader();

	virtual void							Init();
	virtual void							Load(BackgroundWorker^ worker, VValidatorInfoMgr * pValidatorInfoMgr, int nFullCompleteCount, int& nCurrCompleteCount, int& nMsgIndex);

	virtual int								GetLoadingCount() { return m_nCameraCount; }
	virtual LOADING_STATE					GetID() { return LS_CAMERA_INFO; }
};
