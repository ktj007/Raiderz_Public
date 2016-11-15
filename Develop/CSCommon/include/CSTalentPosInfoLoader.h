#pragma once

#include "MXml.h"
#include "CSTalentInfo.h"
#include "CSCommonLib.h"

class CSTalentInfoMgr;

class CSCOMMON_API CSTalentPosInfoLoader
{
private:
	void Parse( CSTalentInfoMgr* pTalentInfoMgr, MXml* pXml, MXmlElement* pElement );
	void ParseExtraMoveSegment( MXmlElement* pMoveSegElement, vec3& vExtraPos, float& fExtraRot, tstring strMoveSegElement );

public:
	bool Load(CSTalentInfoMgr* pTalentInfoMgr, const TCHAR* szFileName);
};