#pragma once

#include "GTestWrapper.h"
#include "SUnitTestUtil.h"
#include "GTalentInfo.h"
#include "GTalentInfoMgr.h"

class FBaseTalent
{
public:
	FBaseTalent()
	{

	}
	virtual ~FBaseTalent()
	{

	}

	GTalentInfo* NewTalentInfo(int nID=INVALID_ID, int nTalentLine=0, int8 nRank=0);

	GTestMgrWrapper<GTalentInfoMgr>				m_TalentInfoMgrWrapper;
};

