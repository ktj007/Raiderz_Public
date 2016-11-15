#pragma once

#include "CSTalentInfoExtParser.h"

class GTalentInfoExtParser : public CSTalentInfoExtParser
{
protected:
	virtual void _Parse(MXmlElement* pElement, MXml* pXml, CSTalentInfo* pTalentInfo);
public:
	GTalentInfoExtParser() {}
	virtual ~GTalentInfoExtParser() {}
};