#pragma once

#include "VInfoLoader.h"
#include "MXml.h"

class VNpcInfoLoader : public VInfoLoader
{
private:
	int										m_nNpcCount;

	MXml									m_xmlNpcInfo;

public:
	VNpcInfoLoader();
	virtual ~VNpcInfoLoader();

	virtual void							Init();
	virtual void							Load(BackgroundWorker^ worker, VValidatorInfoMgr * pValidatorInfoMgr, int nFullCompleteCount, int& nCurrCompleteCount, int& nMsgIndex);

	virtual int								GetLoadingCount() { return m_nNpcCount; }
	virtual LOADING_STATE					GetID() { return LS_NPC_INFO; }
};
