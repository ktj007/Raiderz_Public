#include "stdafx.h"
#include "GTalentInfoExtParser.h"

void GTalentInfoExtParser::_Parse( MXmlElement* pElement, MXml* pXml, CSTalentInfo* pTalentInfo )
{
	CSTalentInfoExtParser::_Parse(pElement, pXml, pTalentInfo);

	_ParseExt_Server(pElement, pXml, pTalentInfo);
}