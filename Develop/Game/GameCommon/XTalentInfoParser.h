#pragma once

#include "XTalentInfo.h"
#include "CSTalentInfoParser.h"

class XTalentInfoMgr;

class XTalentInfoParser : public CSTalentInfoParser
{
private:
	TALENT_SHOVE_TYPE ParseTalentShoveType(MXmlElement* pElement, string strParseName);

protected:
	void ParseTalentClient( XTalentInfo* pNewInfo, MXml* pXml, MXmlElement* pElement );
public:
	XTalentInfoParser() {}
	virtual ~XTalentInfoParser() {}

	void ParseTalent(XTalentInfoMgr* pTalentInfoMgr, MXmlElement* pElement, MXml* pXml, XTalentInfo* pTalentInfo = NULL);
};


