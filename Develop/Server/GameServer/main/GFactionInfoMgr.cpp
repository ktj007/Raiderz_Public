#include "StdAfx.h"
#include "GFactionInfoMgr.h"
#include "GFactionInfo.h"
#include "GSTLUtil.h"
#include "GDef.h"
#include "GValidateLogger.h"
#include "GGlobal.h"


GFactionInfoMgr::~GFactionInfoMgr()
{
	Clear();
}

void GFactionInfoMgr::Clear()
{
	SAFE_DELETE_MAP(m_mapInfo);
}

bool GFactionInfoMgr::Load( const wchar_t* szFileName )
{	
	VALID_RET(NULL != szFileName, false);

	_VLI(szFileName);

	MXml xml;
	if (!xml.LoadFile(MLocale::ConvUTF16ToAnsi(szFileName).c_str())) return false;

	MXmlHandle docHandle = xml.DocHandle();
	MXmlElement* pElement = docHandle.FirstChild(FACTION_XML_TAG_MAIET).FirstChildElement().Element();

	for (pElement; pElement != NULL; pElement = pElement->NextSiblingElement())
	{
		if (!_stricmp(pElement->Value(), FACTION_XML_TAG_FACTION))
		{
			ParseFaction(pElement, &xml);
		}
	}

	return true;
}

void GFactionInfoMgr::ParseFaction(MXmlElement* pElement, MXml* pXml)
{
	uint8 nID;
	_Attribute(&nID, pElement, FACTION_XML_ATTR_ID);

	_VLE(_T(FACTION_XML_TAG_FACTION));
	_VLA(_T(FACTION_XML_ATTR_ID), nID);
	_VLP;

	if (true ==IsExist(nID))
	{
		_VLOGGER->Log(_T(FACTION_XML_ATTR_ID) IS_ALREADY_EXIST);
		return;
	}


	uint16 nDefaultFaction;
	_Attribute(&nDefaultFaction, pElement, FACTION_XML_ATTR_DEFAULT);

	GFactionInfo* pNewInfo = New(nID, nDefaultFaction);
	Insert(pNewInfo);
}

GFactionInfo* GFactionInfoMgr::New(uint8 nID, uint16 nDefaultQuantity)
{
	return new GFactionInfo(nID, nDefaultQuantity);
}

void GFactionInfoMgr::Insert(GFactionInfo* pInfo)
{
	VALID(pInfo);
	VALID(false == IsExist(pInfo->m_nID));

	m_mapInfo.insert(MAP_FACTIONINFO::value_type(pInfo->m_nID, pInfo));
}

bool GFactionInfoMgr::IsExist( uint8 nID )
{
	if (NULL == Get(nID)) return false;

	return true;
}

GFactionInfo* GFactionInfoMgr::Get(uint8 nID)
{
	if (true == m_mapInfo.empty()) return NULL;

	MAP_FACTIONINFO::iterator itor = m_mapInfo.find(nID);
	if (m_mapInfo.end() == itor) return NULL;

	return (*itor).second;
}