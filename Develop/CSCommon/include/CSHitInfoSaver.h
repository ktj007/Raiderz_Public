#pragma once

#include "MXml.h"
#include "CSTalentInfo.h"
#include "CSCommonLib.h"

class CSTalentInfoMgr;

class CSCOMMON_API CSHitInfoSaver
{
protected:
	void SetTalentElement( CSTalentInfo* pTalentInfo, MXmlElement * pMaietElement );
public:
	bool SaveHitInfo(CSTalentInfoMgr* pTalentInfoMgr, const TCHAR* szFileName);
};