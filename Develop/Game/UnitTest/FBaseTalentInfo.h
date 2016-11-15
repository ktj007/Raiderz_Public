#pragma once

#include "XTalentInfo.h"
#include "XTalentInfoMgr.h"
#include "XTestWrapper.h"
#include "MockTalentInfo.h"

class FBaseTalentInfo
{
public:
	FBaseTalentInfo()
	{

	}
	virtual ~FBaseTalentInfo()
	{

	}

	XTestInfoWrapper2<XTalentInfoMgr, MockTalentInfoMgr>	m_TalentInfoWrapper;
};