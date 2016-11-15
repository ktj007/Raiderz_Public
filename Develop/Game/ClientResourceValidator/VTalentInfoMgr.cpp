#include "stdafx.h"
#include "VTalentInfoMgr.h"

VTalentInfoMgr::VTalentInfoMgr()
{

}

VTalentInfoMgr::~VTalentInfoMgr()
{
	map<int, XTalentEffectBase *>::iterator itBase = m_mapEffectBase.begin();
	while(itBase != m_mapEffectBase.end())
	{
		XTalentEffectBase * pBase = itBase->second;
		SAFE_DELETE(pBase);
		itBase++;
	}

	m_mapEffectBase.clear();
}