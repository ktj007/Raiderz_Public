#pragma once

#include "VInfoLoader.h"
#include "MXml.h"

class VItemInfoLoader : public VInfoLoader
{
private:
	int										m_nItemCount;
	MXml									m_xmlItem;

public:
	VItemInfoLoader();
	virtual ~VItemInfoLoader();

	virtual void							Init();
	virtual void							Load(BackgroundWorker^ worker, VValidatorInfoMgr * pValidatorInfoMgr, int nFullCompleteCount, int& nCurrCompleteCount, int& nMsgIndex);

	virtual int								GetLoadingCount() { return m_nItemCount; }
	virtual LOADING_STATE					GetID() { return LS_SOUND_INFO; }
};
