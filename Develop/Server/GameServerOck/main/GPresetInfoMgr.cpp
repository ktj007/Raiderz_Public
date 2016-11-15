#include "StdAfx.h"
#include "GPresetInfoMgr.h"
#include "AStlUtil.h"
#include "GPresetInfo.h"
#include "CSStrings.h"

GPresetInfoMgr::GPresetInfoMgr(void)
{
}

GPresetInfoMgr::~GPresetInfoMgr(void)
{
	Clear();
}

void GPresetInfoMgr::Clear()
{
	SAFE_DELETE_MAP(m_mapInfo);
}

bool GPresetInfoMgr::Load( const TCHAR* szFileName )
{
	_ASSERT(NULL != szFileName);
	if (NULL == szFileName) return false;

	MXml xml;
	if (!xml.LoadFile(MLocale::ConvTCHARToAnsi(szFileName).c_str())) return false;

	MXmlHandle docHandle = xml.DocHandle();
	MXmlElement* pElement = docHandle.FirstChild(PRESET_XML_TAG_MAIET).FirstChildElement().Element();

	for (pElement; pElement != NULL; pElement = pElement->NextSiblingElement())
	{
		if (!_stricmp(pElement->Value(), PRESET_XML_TAG_PRESET))
		{
			GPresetInfo* pNewInfo = new GPresetInfo();
			if (true == ParsePreset(pElement, &xml, pNewInfo))
			{
				Insert(pNewInfo);
			}
			else
			{
				SAFE_DELETE(pNewInfo);
			}
		}
	}

	return true;
}

void GPresetInfoMgr::Insert( GPresetInfo* pInfo )
{
	if (NULL == pInfo) return;

	m_mapInfo.insert(MAP_PRESETINFO::value_type(pInfo->strID, pInfo));
}

GPresetInfo* GPresetInfoMgr::Get( wstring strID )
{
	MAP_PRESETINFO::iterator itor = m_mapInfo.find(strID);
	if (m_mapInfo.end() == itor) return NULL;

	return (*itor).second;
}

bool GPresetInfoMgr::ParsePreset( MXmlElement* pElement, MXml* pXml, GPresetInfo* pInfo )
{	
	_Attribute(pInfo->strID, pElement, PRESET_XML_ATTR_ID);
	VALID_RET(!Get(pInfo->strID), false);

	_Attribute(&pInfo->nLevel, pElement, PRESET_XML_ATTR_LEVEL);
	_Attribute(&pInfo->nTP, pElement, PRESET_XML_ATTR_TP);

	MXmlElement* pChild = pElement->FirstChildElement();
	for (pChild; pChild != NULL; pChild=pChild->NextSiblingElement() )
	{		
		if (!_stricmp(pChild->Value(), PRESET_XML_TAG_TALENT))
		{
			ParseTalent(pChild, pXml, pInfo);
		}
		else if (!_stricmp(pChild->Value(), PRESET_XML_TAG_ITEM))
		{
			ParseItem(pChild, pXml, pInfo);
		}
	}

	return true;
}

void GPresetInfoMgr::ParseTalent( MXmlElement* pElement, MXml* pXml, GPresetInfo* pInfo )
{
	wstring strValue;
	if (true == _Attribute(strValue, pElement, PRESET_XML_ATTR_ID, pXml))
	{
		vector<int> vecTalentID;
		CSStrings::StringNumbersToIntVector(strValue, vecTalentID);

		for each (int nTalentID in vecTalentID)
		{
			pInfo->setTalentID.insert(nTalentID);
		}
	}
}

void GPresetInfoMgr::ParseItem( MXmlElement* pElement, MXml* pXml, GPresetInfo* pInfo )
{
	wstring strValue;
	if (true == _Attribute(strValue, pElement, PRESET_XML_ATTR_ID, pXml))
	{
		vector<int> vecItemID;
		CSStrings::StringNumbersToIntVector(strValue, vecItemID);

		for each (int nItemID in vecItemID)
		{
			pInfo->setItemID.insert(nItemID);
		}
	}
}
