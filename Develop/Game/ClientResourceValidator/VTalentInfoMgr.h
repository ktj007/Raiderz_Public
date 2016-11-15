#pragma once

#include "XTalentInfoMgr.h"
#include "XTalentEffectInfo.h"
#include "VErrorMsg.h"

class VTalentInfoMgr
{
public:
	XTalentInfoMgr						m_TalentInfoMgr;
	map<int, XTalentEffectBase *>		m_mapEffectBase;

	VErrorMsg							m_ErrorMsg;

public:
	VTalentInfoMgr();
	virtual ~VTalentInfoMgr();
};

