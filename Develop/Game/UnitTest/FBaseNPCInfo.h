#pragma once

#include "MockNPCInfo.h"

class FBaseNPCInfo
{
public:
	FBaseNPCInfo()
	{

	}
	virtual ~FBaseNPCInfo()
	{

	}

	XTestInfoWrapper2<XNPCInfoMgr, MockNPCInfoMgr>	m_NPCInfoWrapper;
};