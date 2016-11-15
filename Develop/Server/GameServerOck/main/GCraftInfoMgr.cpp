#include "StdAfx.h"
#include "GCraftInfoMgr.h"
#include "GCraftInfo.h"
#include "GSTLUtil.h"
#include "GStrings.h"
#include "GGlobal.h"
#include "GValidateLogger.h"

GCraftInfoMgr::GCraftInfoMgr(void)
{
}

GCraftInfoMgr::~GCraftInfoMgr(void)
{
	Clear();
}

void GCraftInfoMgr::Clear()
{
	SAFE_DELETE_MAP(m_mapInfo);
}

bool GCraftInfoMgr::Load(const wchar_t* szFileName)
{
	_ASSERT(NULL != szFileName);
	if (NULL == szFileName) return false;

	_VLI(szFileName);

	MXml xml;
	if (!xml.LoadFile(MLocale::ConvUTF16ToAnsi(szFileName).c_str())) return false;

	MXmlHandle docHandle = xml.DocHandle();
	MXmlElement* pElement = docHandle.FirstChild(CRAFT_XML_TAG_MAIET).FirstChildElement().Element();

	for (pElement; pElement != NULL; pElement = pElement->NextSiblingElement())
	{
		if (!_stricmp(pElement->Value(), CRAFT_XML_TAG_CRAFT))
		{
			ParseCraft(pElement, &xml);
		}
	}

	return true;
}

GCraftInfo* GCraftInfoMgr::New()
{
	return new GCraftInfo();
}

void GCraftInfoMgr::ParseCraft(MXmlElement* pElement, MXml* pXml)
{
	int nID;
	_Attribute(&nID, pElement, CRAFT_XML_ATTR_ID);

	GCraftInfo* pExistInfo = Get(nID);
	_ASSERT(NULL == pExistInfo);
	
	_VLE(_T(CRAFT_XML_TAG_CRAFT));
	_VLA(_T(CRAFT_XML_ATTR_ID), nID);
	_VLP;
	
	if (NULL != pExistInfo)
	{
		_VLOGGER->Log(_T(CRAFT_XML_ATTR_ID) IS_ALREADY_EXIST);
		return;
	}

	GCraftInfo* pNewInfo = New();
	pNewInfo->m_nID = nID;

	_Attribute(&pNewInfo->m_nDialogID,		pElement, CRAFT_XML_ATTR_DIALOG);
	_Attribute(&pNewInfo->m_fBaseMakeMod,	pElement, CRAFT_XML_ATTR_MAKEMOD);
	_Attribute(&pNewInfo->m_nFactionID,		pElement, CRAFT_XML_ATTR_FACTIONID);

	MXmlElement* pChild = pElement->FirstChildElement();
	for (pChild; pChild != NULL; pChild = pChild->NextSiblingElement())
	{
		if (!_stricmp(pChild->Value(), CRAFT_XML_TAG_RECIPE))
		{
			ParseRecipe(pChild, pXml, pNewInfo);
		}
	}

	Insert(pNewInfo);
}

void GCraftInfoMgr::ParseRecipe(MXmlElement* pElement, MXml* pXml, GCraftInfo* pCraftInfo)
{
	wstring strValue;	
	if (false == _Attribute(strValue,		pElement, CRAFT_XML_ATTR_ID)) return;

	vector<int> vecRecipeID;
	GStrings::StringNumbersToIntVector(strValue, vecRecipeID);

	for each (int nRecipeID in vecRecipeID)
	{
		pCraftInfo->m_setRecipeID.insert(nRecipeID);
	}
}

void GCraftInfoMgr::Insert(GCraftInfo* pInfo)
{
	_ASSERT(NULL != pInfo);
	if (NULL == pInfo) return;

	GCraftInfo* pExistInfo = Get(pInfo->m_nID);
	_ASSERT(NULL == pExistInfo);
	if (NULL != pExistInfo) return;

	m_mapInfo.insert(MAP_CRAFTINFO::value_type(pInfo->m_nID, pInfo));
}

GCraftInfo* GCraftInfoMgr::Get(int nID)
{
	if (true == m_mapInfo.empty()) return NULL;

	MAP_CRAFTINFO::iterator itor = m_mapInfo.find(nID);
	if (m_mapInfo.end() == itor) return NULL;

	return (*itor).second;
}