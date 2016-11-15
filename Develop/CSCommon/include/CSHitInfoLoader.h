#pragma once

#include "MXml.h"
#include "CSTalentInfo.h"
#include "CSCommonLib.h"

class CSTalentInfoMgr;

class CSCOMMON_API CSHitInfoLoader
{
private:
public:
	void ParseHitInfo( CSTalentInfoMgr* pTalentInfoMgr, MXml* pXml, MXmlElement* pElement);
	bool LoadHitInfo(CSTalentInfoMgr* pTalentInfoMgr, const TCHAR* szFileName);
};