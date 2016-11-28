#include "StdAfx.h"
#include "GFactionRelationInfoMgr.h"
#include "GFactionRelationInfo.h"
#include "GSTLUtil.h"
#include "GValidateLogger.h"
#include "GDef.h"
#include "GGlobal.h"
#include "GStrings.h"
#include "CSFactionInfo.h"



GFactionRelationInfoMgr::~GFactionRelationInfoMgr(void)
{
	Clear();
}

void GFactionRelationInfoMgr::Clear()
{
	for each (MAP_MAP_FACTIONRELATIONINFO::value_type val in m_mapmapInfo)
	{
		SAFE_DELETE_MAP(val.second);
	}

	m_mapmapInfo.clear();
}

bool GFactionRelationInfoMgr::Load( const wchar_t* szFileName )
{
	VALID_RET(NULL != szFileName, false);

	_VLI(szFileName);

	MXml xml;
	if (!xml.LoadFile(MLocale::ConvUTF16ToAnsi(szFileName).c_str())) return false;

	MXmlHandle docHandle = xml.DocHandle();
	MXmlElement* pElement = docHandle.FirstChild(FACTIONREL_XML_TAG_MAIET).FirstChildElement().Element();

	for (pElement; pElement != NULL; pElement = pElement->NextSiblingElement())
	{
		if (!_stricmp(pElement->Value(), FACTIONREL_XML_TAG_FACTIONREL))
		{
			ParseFactionRelation(pElement, &xml);
		}
	}

	return true;
}

void GFactionRelationInfoMgr::ParseFactionRelation(MXmlElement* pElement, MXml* pXml)
{
	uint8 nID1, nID2;
	_Attribute(&nID1, pElement, FACTIONREL_XML_ATTR_ID1);
	_Attribute(&nID2, pElement, FACTIONREL_XML_ATTR_ID2);

	_VLE(_T(FACTIONREL_XML_TAG_FACTIONREL));
	_VLA(_T(FACTIONREL_XML_ATTR_ID1), nID1);
	_VLA(_T(FACTIONREL_XML_ATTR_ID2), nID2);
	_VLP;

	if (true ==IsExist(nID1, nID2))
	{
		_VLOGGER->Log(_T(FACTIONREL_XML_ATTR_ID1) IS_ALREADY_EXIST);
		_VLOGGER->Log(_T(FACTIONREL_XML_ATTR_ID2) IS_ALREADY_EXIST);
		return;
	}

	FACTION_RELATION_TYPE nFRT;
	wstring strValue;
	if (true == _Attribute(strValue, pElement, FACTIONREL_XML_ATTR_RELATION, pXml))
	{
		nFRT = static_cast<FACTION_RELATION_TYPE>(GStrings::StringToFactionRelationType(strValue.c_str()));
	}

	GFactionRelationInfo* pNewInfo = New(nID1, nID2, nFRT);
	Insert(pNewInfo);
}

GFactionRelationInfo* GFactionRelationInfoMgr::New(uint8 nID1, uint8 nID2, FACTION_RELATION_TYPE nFRT)
{
	return new GFactionRelationInfo(nID1, nID2, nFRT);
}

void GFactionRelationInfoMgr::Insert(GFactionRelationInfo* pInfo)
{
	VALID(pInfo);
	VALID(false == IsExist(pInfo->m_nID1, pInfo->m_nID2));

	MAP_MAP_FACTIONRELATIONINFO::iterator itor =  m_mapmapInfo.find(pInfo->m_nID1);
	if (m_mapmapInfo.end() == itor)
	{
		MAP_FACTIONRELATIONINFO mapInfo;
		mapInfo.insert(MAP_FACTIONRELATIONINFO::value_type(pInfo->m_nID2, pInfo));
		
		m_mapmapInfo.insert(MAP_MAP_FACTIONRELATIONINFO::value_type(pInfo->m_nID1, mapInfo));
	}
	else
	{
		MAP_FACTIONRELATIONINFO& mapInfo = (*itor).second;
		mapInfo.insert(MAP_FACTIONRELATIONINFO::value_type(pInfo->m_nID2, pInfo));
	}
}

bool GFactionRelationInfoMgr::IsExist(uint8 nID1, uint8 nID2)
{
	if (NULL == Get(nID1, nID2)) return false;

	return true;
}

GFactionRelationInfo* GFactionRelationInfoMgr::Get(uint8 nID1, uint8 nID2)
{
	if (true == m_mapmapInfo.empty()) return NULL;

	MAP_MAP_FACTIONRELATIONINFO::iterator itor1 = m_mapmapInfo.find(nID1);
	if (m_mapmapInfo.end() == itor1) return NULL;

	MAP_FACTIONRELATIONINFO& mapInfo = (*itor1).second;
	MAP_FACTIONRELATIONINFO::iterator itor2 = mapInfo.find(nID2);
	if (mapInfo.end() == itor2) return NULL;

	return (*itor2).second;
}

FACTION_RELATION_TYPE GFactionRelationInfoMgr::GetRelation(uint8 nID1, uint8 nID2)
{
	if (0 == nID1 || 0 == nID2) return FRT_NONE;

	if (nID1 == nID2)
	{
		return FRT_EXCELLENT;
	}

	GFactionRelationInfo* pFactionRelationInfo = Get(nID1, nID2);
	if (NULL == pFactionRelationInfo) return FRT_NONE;

	return pFactionRelationInfo->m_nFRT;
}