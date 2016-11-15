#pragma once

#include "VInfoLoader.h"

class VBuffInfoLoader : public VInfoLoader
{
private:
	int										m_nBuffCount;
	MXml									m_xmlBuff;

public:
	VBuffInfoLoader();
	virtual ~VBuffInfoLoader();

	virtual void							Init();
	virtual void							Load(BackgroundWorker^ worker, VValidatorInfoMgr * pValidatorInfoMgr, int nFullCompleteCount, int& nCurrCompleteCount, int& nMsgIndex);

	virtual int								GetLoadingCount() { return m_nBuffCount; }
	virtual LOADING_STATE					GetID() { return LS_BUFF_INFO; }
};
