#pragma once

#include "GTalentInfo.h"
#include "CSTalentInfoParser.h"

class GTalentInfoMgr;

class GTalentInfoParser : public CSTalentInfoParser
{
protected:
	virtual void ParseTalentServer(CSTalentInfo* pTalentInfo, MXml* pXml, MXmlElement* pElement) override;

public:
	GTalentInfoParser() {}
	virtual ~GTalentInfoParser() {}

	// talent.xml
	void ParseTalent(GTalentInfoMgr* pTalentInfoMgr, MXmlElement* pElement, MXml* pXml);

};


