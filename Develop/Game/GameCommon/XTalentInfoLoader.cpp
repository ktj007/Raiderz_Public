#include "stdafx.h"
#include "XTalentInfoLoader.h"
#include "XTalentInfoParser.h"
#include "XTalentInfoExtParser.h"
#include "XTalentInfoMgr.h"
#include "MLocale.h"

bool XTalentInfoLoader::Load( XTalentInfoMgr* pTalentInfoMgr, const TCHAR* szFileName, const TCHAR* szExtFileName, const TCHAR* szHitFileName )
{
	if (LoadTalent(pTalentInfoMgr, szFileName) == false) return false;
	if (LoadExt(pTalentInfoMgr, szExtFileName) == false) return false;
	if (LoadHitInfo(pTalentInfoMgr, szHitFileName) == false) return false;

	return true;
}

bool XTalentInfoLoader::LoadExt( XTalentInfoMgr* pTalentInfoMgr, const TCHAR* szFileName )
{
	XTalentInfoExtParser talentInfoExtParser;
	MXml xml;

	if(!xml.LoadFile(MLocale::ConvTCHARToAnsi(szFileName).c_str())) 
	{
		mlog("Failed To Load %s\n", MLocale::ConvTCHARToAnsi(szFileName).c_str());
		return false;
	}

	MXmlHandle docHandle = xml.DocHandle();
	MXmlElement* pElement = docHandle.FirstChild(TALENT_XML_TAG_MAIET).FirstChildElement().Element();

	for(pElement; pElement != NULL; pElement=pElement->NextSiblingElement())
	{
		if(!_stricmp(pElement->Value(), TALENT_XML_TAG_TALENT))
		{
			talentInfoExtParser.Parse(pTalentInfoMgr, &xml, pElement);
		}
	}

	// 레퍼런스 데이터를 복사한다.
	talentInfoExtParser.CopyTalentEventInfoByReference(pTalentInfoMgr);

	return true;
}

bool XTalentInfoLoader::LoadTalent( XTalentInfoMgr* pTalentInfoMgr, const TCHAR* szFileName )
{
	XTalentInfoParser talentInfoParser;
	MXml xml;

	if (!xml.LoadFile(MLocale::ConvTCHARToAnsi(szFileName, 0).c_str())) 
	{
		mlog("Failed To Load %s\n", MLocale::ConvTCHARToAnsi(szFileName).c_str());
		return false;
	}

	MXmlHandle docHandle = xml.DocHandle();
	MXmlElement* pElement = docHandle.FirstChild(TALENT_XML_TAG_MAIET).FirstChildElement().Element();

	for( pElement; pElement != NULL; pElement=pElement->NextSiblingElement() )
	{
		if (!_stricmp(pElement->Value(), TALENT_XML_TAG_TALENT))
		{
			talentInfoParser.ParseTalent(pTalentInfoMgr, pElement, &xml);
		}
	}

	return true;
}