#pragma once

#include "MTime.h"
#include "ServerCommonLib.h"

class SSystem;

class SCOMMON_API SFrameCounter : public MTestMemPool<SFrameCounter>
{
private:
	SSystem*	m_pSystem;
	MTimeValue	m_nTime;
	MTimeValue	m_nFrameCounter;
	MTimeValue	m_nCurrFrame;
	MTimeValue	m_nLastTime;
public:
	SFrameCounter(SSystem* pSystem = NULL);
	int Update();
	void Clear();
	uint32 GetNowTime();
};
