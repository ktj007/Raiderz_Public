#include "stdafx.h"
#include "GTalentInfoLoader.h"
#include "GTalentInfoParser.h"
#include "GTalentInfoExtParser.h"
#include "CSTalentInfoDef.h"
#include "GTalentInfoMgr.h"
#include "CSTalentPosInfoLoader.h"
#include "GGlobal.h"
#include "GValidateLogger.h"


bool GTalentInfoLoader::Load( GTalentInfoMgr* pTalentInfoMgr, const GTalentInfoPathParam& pathParam)
{
	_VLI(pathParam.strTalentInfo.c_str());
	if (LoadTalent(pTalentInfoMgr, pathParam.strTalentInfo.c_str()) == false) return false;
	
	_VLI(pathParam.strTalentInfoExt.c_str());
	if (LoadTalentExt(pTalentInfoMgr, pathParam.strTalentInfoExt.c_str()) == false) return false;
	
	_VLI(pathParam.strTalentPosInfo.c_str());
	if (LoadPosInfo(pTalentInfoMgr, pathParam.strTalentPosInfo.c_str()) == false) return false;
	
	_VLI(pathParam.strTalentHitInfo.c_str());
	if (LoadHitInfo(pTalentInfoMgr, pathParam.strTalentHitInfo.c_str()) == false) return false;

	return true;
}

bool GTalentInfoLoader::LoadTalentExt( GTalentInfoMgr* pTalentInfoMgr, const wchar_t* szFileName )
{
	GTalentInfoExtParser talentInfoExtParser;

	MXml xml;

	if(!xml.LoadFile(MLocale::ConvUTF16ToAnsi(szFileName).c_str())) 
	{		
		_VLOGGER->LogFailedLoad(szFileName);
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

bool GTalentInfoLoader::LoadPosInfo( GTalentInfoMgr* pTalentInfoMgr, const wchar_t* szFileName )
{
	CSTalentPosInfoLoader loader;
	return loader.Load(pTalentInfoMgr, szFileName);
}

bool GTalentInfoLoader::LoadTalent( GTalentInfoMgr* pTalentInfoMgr, const wchar_t* szFileName)
{
	GTalentInfoParser talentInfoParser;
	MXml xml;

	if (!xml.LoadFile(MLocale::ConvUTF16ToAnsi(szFileName).c_str())) 
	{
		_VLOGGER->LogFailedLoad(szFileName);
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

