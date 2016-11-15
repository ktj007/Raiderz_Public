#include "StdAfx.h"
#include "XFactionInfoMgr.h"
#include "XFactionInfo.h"
#include "XSTLUtil.h"

XFactionInfoMgr::~XFactionInfoMgr()
{
	Clear();
}

void XFactionInfoMgr::Clear()
{
	SAFE_DELETE_MAP(m_mapInfo);
}

bool XFactionInfoMgr::Load(const wchar_t* szFileName)
{
	if (NULL == szFileName) return false;

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

void XFactionInfoMgr::ParseFaction(MXmlElement* pElement, MXml* pXml)
{
	uint8 nID;
	_Attribute(&nID, pElement, FACTION_XML_ATTR_ID);

	if (true ==IsExist(nID))
	{
		return;
	}

	uint16 nDefaultQuantity;
	_Attribute(&nDefaultQuantity, pElement, FACTION_XML_ATTR_DEFAULT);

	wstring strTitle = CSFormatString::Format(L"FACTION_TITLE_{0}", FSParam(nID));
	wstring strDesc = CSFormatString::Format(L"FACTION_DESC_{0}", FSParam(nID));

	XFactionInfo* pNewInfo = New(nID, nDefaultQuantity, strTitle, strDesc);
	Insert(pNewInfo);
}

XFactionInfo* XFactionInfoMgr::New(uint8 nID, uint16 nDefaultFaction, wstring strTitle, wstring strDesc)
{
	return new XFactionInfo(nID, nDefaultFaction, strTitle, strDesc);
}

void XFactionInfoMgr::Insert( XFactionInfo* pInfo )
{
	if (NULL == pInfo) return;
	if (true == IsExist(pInfo->m_nID)) return;

	m_mapInfo.insert(MAP_FACTIONINFO::value_type(pInfo->m_nID, pInfo));
}

bool XFactionInfoMgr::IsExist(uint8 nID)
{
	if (NULL == Get(nID)) return false;

	return true;
}

XFactionInfo* XFactionInfoMgr::Get(uint8 nID)
{
	if (true == m_mapInfo.empty()) return NULL;

	MAP_FACTIONINFO::iterator itor = m_mapInfo.find(nID);
	if (m_mapInfo.end() == itor) return NULL;

	return (*itor).second;
}