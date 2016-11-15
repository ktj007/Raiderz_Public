#pragma once

#include "MXml.h"
#include "CSTalentInfo.h"
#include "CSCommonLib.h"

class CSTalentInfoMgr;

class CSCOMMON_API CSTalentPosInfoSaver
{
private:
public:
	bool Save(CSTalentInfoMgr* pTalentInfoMgr, const TCHAR* szFileName);
};