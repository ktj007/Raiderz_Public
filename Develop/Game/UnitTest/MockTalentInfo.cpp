#include "stdafx.h"
#include "MockTalentInfo.h"
#include "XTalentInfoParser.h"
#include "XTalentInfoExtParser.h"

bool MockTalentInfoMgr::CreateTalent( const std::wstring& _strXmlData )
{
	return SetTalent(NULL, _strXmlData);
}

bool MockTalentInfoMgr::SetTalent( XTalentInfo* pTalentInfo, const std::wstring& _strXmlData )
{
	USES_CONVERSION_EX;

	MXmlDocument doc;
	doc.Parse(W2A_EX(_strXmlData.c_str(), 0));
	MXmlElement* pRootElement = doc.FirstChildElement();
	if(pRootElement == NULL)
		return false;

	XTalentInfoParser talentInfoParser;
	talentInfoParser.ParseTalent(this, pRootElement, NULL, pTalentInfo);
	Cooking();

	return true;
}

bool MockTalentInfoMgr::SetTalentExt( XTalentInfo* pTalentInfo, const std::wstring& _strXmlData )
{
	USES_CONVERSION_EX;

	MXmlDocument doc;
	doc.Parse(W2A_EX(_strXmlData.c_str(), 0));
	MXmlElement* pRootElement = doc.FirstChildElement();
	if(pRootElement == NULL)
		return false;

	XTalentInfoExtParser talentInfoExtParser;
	talentInfoExtParser.Parse(this, NULL, pRootElement, pTalentInfo);
	Cooking();

	return true;
}