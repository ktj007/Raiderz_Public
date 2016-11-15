#pragma once

#include "VInfoLoader.h"
#include "MXml.h"

class VSoundInfoLoader : public VInfoLoader
{
private:
	int										m_nSoundCount;
	MXml									m_xmlSound;

public:
	VSoundInfoLoader();
	virtual ~VSoundInfoLoader();

	virtual void							Init();
	virtual void							Load(BackgroundWorker^ worker, VValidatorInfoMgr * pValidatorInfoMgr, int nFullCompleteCount, int& nCurrCompleteCount, int& nMsgIndex);

	virtual int								GetLoadingCount() { return m_nSoundCount; }
	virtual LOADING_STATE					GetID() { return LS_SOUND_INFO; }
};
