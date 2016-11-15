#pragma once

#include "CSCommonLib.h"

class CSTalentInfoMgr;

class CSCOMMON_API CSTalentInfoLoader
{
protected:
	bool LoadHitInfo(CSTalentInfoMgr* pTalentInfoMgr, const TCHAR* szFileName);
public:
	CSTalentInfoLoader() {}
	virtual ~CSTalentInfoLoader() {}
};