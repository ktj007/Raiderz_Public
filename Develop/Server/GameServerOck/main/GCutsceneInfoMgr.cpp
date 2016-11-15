#include "StdAfx.h"
#include "GCutsceneInfoMgr.h"
#include "GValidateLogger.h"
#include "GGlobal.h"


GCutsceneInfoMgr::GCutsceneInfoMgr()
{
}

GCutsceneInfoMgr::~GCutsceneInfoMgr()
{
	Clear();
}

void GCutsceneInfoMgr::Clear()
{
	MAP_CUTSCENE::iterator iter = m_mapCutsceneInfo.begin();
	for (; iter != m_mapCutsceneInfo.end(); ++iter)
	{
		SAFE_DELETE(iter->second);
	}
}
void GCutsceneInfoMgr::Insert(GCutsceneInfo* pCutsceneInfo)
{
	m_mapCutsceneInfo.insert(MAP_CUTSCENE::value_type(pCutsceneInfo->nID, pCutsceneInfo));
}
GCutsceneInfo* GCutsceneInfoMgr::Find(int nID)
{
	MAP_CUTSCENE::iterator iter = m_mapCutsceneInfo.find(nID);
	if (iter == m_mapCutsceneInfo.end())
		return NULL;
	return iter->second;
}
bool GCutsceneInfoMgr::IsExist(int nID)
{
	MAP_CUTSCENE::iterator iter = m_mapCutsceneInfo.find(nID);
	if (iter == m_mapCutsceneInfo.end())
		return false;
	return true;
}

bool GCutsceneInfoMgr::Load(const wchar_t* szFileName)
{
	VALID_RET(NULL != szFileName, false);

	_VLI(szFileName);

	MXml xml;
	if (!xml.LoadFile(MLocale::ConvUTF16ToAnsi(szFileName).c_str())) return false;

	MXmlHandle docHandle = xml.DocHandle();
	MXmlElement* pElement = docHandle.FirstChild(CUTSCENE_XML_TAG_MAIET).FirstChildElement().Element();

	for (pElement; pElement != NULL; pElement = pElement->NextSiblingElement())
	{
		if (!_stricmp(pElement->Value(), CUTSCENE_XML_TAG_CUTSCENE))
		{
			Parse_Cutscene(pElement, &xml);
		}
	}
	return true;
}

void GCutsceneInfoMgr::Parse_Cutscene(MXmlElement* pElement, MXml* pXml)
{
	int nID;
	_Attribute(&nID, pElement, CUTSCENE_XML_ATTR_ID);

	_VLE(_T(CUTSCENE_XML_TAG_CUTSCENE));
	_VLA(_T(CUTSCENE_XML_ATTR_ID), nID);
	_VLP;

	if (true ==IsExist(nID))
	{
		_VLOGGER->Log(_T(CUTSCENE_XML_ATTR_ID) IS_ALREADY_EXIST);
		return;
	}

	GCutsceneInfo* pNewCutsceneInfo = new GCutsceneInfo();
	pNewCutsceneInfo->nID = nID;

	_Attribute(&pNewCutsceneInfo->bRepeatable, pElement, CUTSCENE_XML_ATTR_REPEATABLE, false);
	_Attribute(&pNewCutsceneInfo->nFieldIntro, pElement, CUTSCENE_XML_ATTR_FIELDINTRO, false);

	Insert(pNewCutsceneInfo);
}

int GCutsceneInfoMgr::GetFieldIntroCutscneneID( int nFieldID )
{
	MAP_CUTSCENE::iterator iter = m_mapCutsceneInfo.begin();
	for (; iter != m_mapCutsceneInfo.end(); ++iter)
	{
		if (iter->second->nFieldIntro == nFieldID)
			return iter->second->nID;
	}

	return INVALID_ID;
}
