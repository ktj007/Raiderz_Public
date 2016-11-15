#pragma once

#include "CSTalentInfoExtParser.h"

class XTalentInfoExtParser : public CSTalentInfoExtParser
{
protected:
	virtual void _Parse(MXmlElement* pElement, MXml* pXml, CSTalentInfo* pTalentInfo);
public:
	XTalentInfoExtParser() {}
	virtual ~XTalentInfoExtParser() {}
};