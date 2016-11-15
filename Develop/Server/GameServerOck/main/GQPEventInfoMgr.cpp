#include "stdafx.h"
#include "GQPEventInfoMgr.h"
#include "GQPEventInfo.h"
#include "AStlUtil.h"
#include "GStrings.h"

GQPEventInfoMgr::GQPEventInfoMgr()
{

}

GQPEventInfoMgr::~GQPEventInfoMgr()
{
	Clear();
}

void GQPEventInfoMgr::Clear()
{
	SAFE_DELETE_MAP(m_mapInfo);
}

bool GQPEventInfoMgr::Load( const wchar_t* szFileName )
{
	VALID_RET(szFileName, false);

	MXml xml;
	if (!xml.LoadFile(MLocale::ConvUTF16ToAnsi(szFileName).c_str())) return false;

	MXmlHandle docHandle = xml.DocHandle();
	MXmlElement* pElement = docHandle.FirstChild(QPEVENT_XML_TAG_MAIET).FirstChildElement().Element();

	for (pElement; pElement != NULL; pElement = pElement->NextSiblingElement())
	{
		if (!_stricmp(pElement->Value(), QPEVENT_XML_TAG_QPEVENT))
		{			
			ParseQPEvent(pElement, &xml);
		}
	}

	return true;
}

void GQPEventInfoMgr::Insert( GQPEventInfo* pInfo )
{
	m_mapInfo.insert(MAP_QPEVENTINFO::value_type(pInfo->m_nID, pInfo));
}

GQPEventInfo* GQPEventInfoMgr::Get( int nID )
{
	if (true == m_mapInfo.empty()) return NULL;

	MAP_QPEVENTINFO::iterator itor = m_mapInfo.find(nID);
	if (m_mapInfo.end() == itor) return NULL;

	return (*itor).second;
}

void GQPEventInfoMgr::ParseQPEvent( MXmlElement* pElement, MXml* pXml )
{
	int nID;
	_Attribute(nID,		pElement, QPEVENT_XML_ATTR_ID);

	GQPEventInfo* pExistInfo = Get(nID);
	VALID(!pExistInfo);

	GQPEventInfo* pNewInfo = new GQPEventInfo();
	pNewInfo->m_nID = nID;
	
	_Attribute(pNewInfo->m_strTitle,	pElement, QPEVENT_XML_ATTR_TITLE, pXml);

	MXmlElement* pChild = pElement->FirstChildElement();
	for (pChild; pChild != NULL; pChild = pChild->NextSiblingElement())
	{
		if (!_stricmp(pChild->Value(), QPEVENT_XML_TAG_BEGIN))
		{
			ParseBegin(pChild, pXml, pNewInfo);
		}
		else if (!_stricmp(pChild->Value(), QPEVENT_XML_TAG_END))
		{
			ParseEnd(pChild, pXml, pNewInfo);
		}
	}

	Insert(pNewInfo);
}

void GQPEventInfoMgr::ParseBegin( MXmlElement* pElement, MXml* pXml, GQPEventInfo* pInfo )
{
	MXmlElement* pChild = pElement->FirstChildElement();
	for (pChild; pChild != NULL; pChild=pChild->NextSiblingElement() )
	{
		if (!_stricmp(pChild->Value(), QPEVENT_XML_TAG_NOTICE))
		{			
			ParseNotice(pChild, pXml, pInfo->m_BeginInfo.m_vecStrNotice);
		}
		else if (!_stricmp(pChild->Value(), QPEVENT_XML_TAG_SPAWN))
		{
			ParseSpawn(pChild, pXml, pInfo->m_BeginInfo.m_vecSpawnInfo);
		}
	}
}

void GQPEventInfoMgr::ParseEnd( MXmlElement* pElement, MXml* pXml, GQPEventInfo* pInfo )
{
	GQPEEndInfo* pNewInfo = new GQPEEndInfo();

	int nWinnerTeam;
	_Attribute(nWinnerTeam, pElement, QPEVENT_XML_ATTR_WINNERTEAM);
	pNewInfo->m_nWinnerTeam = static_cast<QUESTPVP_TEAM>(nWinnerTeam);

	MXmlElement* pChild = pElement->FirstChildElement();
	for (pChild; pChild != NULL; pChild=pChild->NextSiblingElement() )
	{
		if (!_stricmp(pChild->Value(), QPEVENT_XML_TAG_NOTICE))
		{			
			ParseNotice(pChild, pXml, pNewInfo->m_vecStrNotice);
		}
		else if (!_stricmp(pChild->Value(), QPEVENT_XML_TAG_SPAWN))
		{
			ParseSpawn(pChild, pXml, pNewInfo->m_vecSpawnInfo);
		}
		else if (!_stricmp(pChild->Value(), QPEVENT_XML_TAG_DESPAWN))
		{
			ParseDespawn(pChild, pXml, pNewInfo->m_vecDespawnNPCID);
		}
		else if (!_stricmp(pChild->Value(), QPEVENT_XML_TAG_REWARD))
		{
			ParseReward(pChild, pXml, pNewInfo);
		}
	}

	pInfo->m_vecEndInfo.push_back(pNewInfo);
}

void GQPEventInfoMgr::ParseNotice( MXmlElement* pElement, MXml* pXml, vector<wstring>& vecStrNotice )
{
	wstring strNotice;
	_Attribute(strNotice, pElement, QPEVENT_XML_ATTR_TEXT, pXml);
	vecStrNotice.push_back(strNotice);
}

void GQPEventInfoMgr::ParseSpawn( MXmlElement* pElement, MXml* pXml, vector<GQPESpawnInfo*>& vecSpawnInfo )
{
	vector<int> vecSpawnID;

	wstring strValue;
	if (true == _Attribute(strValue,		pElement, QPEVENT_XML_ATTR_ID))
	{
		CSStrings::StringNumbersToIntVector(strValue, vecSpawnID);
	}

	if (true == vecSpawnID.empty()) return;

	if (1 == vecSpawnID.size())
	{
		GQPESpawnInfo* pNewInfo = new GQPESpawnInfo();
		pNewInfo->m_nSpawnID = static_cast<SPAWN_ID>(vecSpawnID.front());
		_Attribute(pNewInfo->m_fDelayTime,		pElement, QPEVENT_XML_ATTR_DELAYTIME);
		_Attribute(pNewInfo->m_fDespawnTime,	pElement, QPEVENT_XML_ATTR_DESPAWNTIME);
		vecSpawnInfo.push_back(pNewInfo);
	}
	else if (1 < vecSpawnID.size())
	{
		for each (int nSpawnID in vecSpawnID)
		{
			GQPESpawnInfo* pNewInfo = new GQPESpawnInfo();
			pNewInfo->m_nSpawnID = static_cast<SPAWN_ID>(nSpawnID);
			vecSpawnInfo.push_back(pNewInfo);
		}		
	}
}

void GQPEventInfoMgr::ParseDespawn( MXmlElement* pElement, MXml* pXml, vector<int>& vecDespawnNPCID )
{
	wstring strValue;
	if (true == _Attribute(strValue,		pElement, QPEVENT_XML_ATTR_NPCID))
	{
		CSStrings::StringNumbersToIntVector(strValue, vecDespawnNPCID);
	}
}


void GQPEventInfoMgr::ParseReward( MXmlElement* pElement, MXml* pXml, GQPEEndInfo* pEndInfo)
{
	_Attribute(pEndInfo->m_nMoney,	pElement, QPEVENT_XML_ATTR_MONEY);
	_Attribute(pEndInfo->m_nXP,		pElement, QPEVENT_XML_ATTR_XP);

	MXmlElement* pChild = pElement->FirstChildElement();
	for (pChild; pChild != NULL; pChild=pChild->NextSiblingElement() )
	{
		if (!_stricmp(pChild->Value(), QPEVENT_XML_TAG_ITEM))
		{			
			ParseItem(pChild, pXml, pEndInfo->m_vecItemInfo);
		}
	}
}

void GQPEventInfoMgr::ParseItem( MXmlElement* pElement, MXml* pXml, vector<GQPEItemInfo*>& vecItemInfo)
{
	GQPEItemInfo* pNewInfo = new GQPEItemInfo();

	_Attribute(pNewInfo->m_nItemID,	pElement, QPEVENT_XML_ATTR_ID);
	_Attribute(pNewInfo->m_nAmount,	pElement, QPEVENT_XML_ATTR_AMOUNT);

	vecItemInfo.push_back(pNewInfo);
}

vector<GQPEventInfo*> GQPEventInfoMgr::GetAll()
{
	vector<GQPEventInfo*> vecInfo;

	for each (const MAP_QPEVENTINFO::value_type& val in m_mapInfo)
	{
		vecInfo.push_back(val.second);
	}

	return vecInfo;
}
