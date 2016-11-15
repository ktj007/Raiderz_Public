#include "StdAfx.h"
#include "GInnRoomInfoMgr.h"
#include "GInnRoomInfo.h"
#include "GSTLUtil.h"
#include "GStrings.h"
#include "GGlobal.h"
#include "GValidateLogger.h"

GInnRoomInfoMgr::GInnRoomInfoMgr()
{
}

GInnRoomInfoMgr::~GInnRoomInfoMgr()
{
	Clear();
}

void GInnRoomInfoMgr::Clear()
{
	SAFE_DELETE_MAP(m_mapInnRoomInfo);
}

bool GInnRoomInfoMgr::Load(const wchar_t* szFileName)
{
	if (NULL == szFileName) return false;

	_VLI(szFileName);

	MXml xml;
	if (false == xml.LoadFile(MLocale::ConvUTF16ToAnsi(szFileName).c_str())) return false;

	MXmlHandle docHandler = xml.DocHandle();
	MXmlElement* pElement = docHandler.FirstChild(INNROOM_XML_TAG_MAIET).FirstChildElement().Element();

	for (pElement; pElement != NULL; pElement = pElement->NextSiblingElement())
	{
		if (!_stricmp(pElement->Value(), INNROOM_XML_TAG_INN))
		{
			ParseInnRoom(pElement, &xml);
		}
	}

	return true;
}

void GInnRoomInfoMgr::ParseInnRoom(MXmlElement* pElement, MXml* pXml)
{
	int nID;
	_Attribute(&nID, pElement, INNROOM_XML_ATTR_ID);

	_VLE(_T(INNROOM_XML_TAG_INN));	
	_VLA(_T(INNROOM_XML_ATTR_ID), nID);
	_VLP;

	GInnRoomInfo* pExistInfo = Get(nID);
	if (NULL != pExistInfo)
	{
		_VLOGGER->Log(_T(INNROOM_XML_ATTR_ID) IS_ALREADY_EXIST);
		_ASSERT(0);
		return;
	}

	GInnRoomInfo* pNewInfo = NewInfo();
	pNewInfo->m_nID = nID;

	wstring strValue;
	if (true == _Attribute(strValue, pElement, INNROOM_XML_ATTR_TYPE))
	{
		pNewInfo->m_nType = (INN_ROOM_TYPE) GStrings::StringToInnRoomType(strValue.c_str());
	}
	_Attribute(&pNewInfo->m_nFieldGroupID, pElement, INNROOM_XML_ATTR_FIELDGROUP);
	_Attribute(&pNewInfo->m_nDefaultBedNPCID, pElement, INNROOM_XML_ATTR_DEFAULT_BED);
	_Attribute(&pNewInfo->m_nDefaultStorageNPCID, pElement, INNROOM_XML_ATTR_DEFAULT_STORAGE);
	_Attribute(&pNewInfo->m_nDefaultGuildStorageNPCID, pElement, INNROOM_XML_ATTR_DEFAULT_GUILDSTORAGE);

	_Attribute(&pNewInfo->m_nCraftingMarkerID, pElement, INNROOM_XML_ATTR_CRAFTING);
	_Attribute(&pNewInfo->m_nBedMarkerID, pElement, INNROOM_XML_ATTR_BED);
	_Attribute(&pNewInfo->m_nCarpetMarkerID, pElement, INNROOM_XML_ATTR_CARPET);
	_Attribute(&pNewInfo->m_nFirePlaceMarkerID, pElement, INNROOM_XML_ATTR_FIREPLACE);
	_Attribute(&pNewInfo->m_nMuralMarkerID, pElement, INNROOM_XML_ATTR_MURAL);
	_Attribute(&pNewInfo->m_nFunitureMarkerID, pElement, INNROOM_XML_ATTR_FUNITURE);
	_Attribute(&pNewInfo->m_nTableMarkerID, pElement, INNROOM_XML_ATTR_TABLE);
	_Attribute(&pNewInfo->m_nStewardMarkerID, pElement, INNROOM_XML_ATTR_STEWARD);
	_Attribute(&pNewInfo->m_nStorageMarkerID, pElement, INNROOM_XML_ATTR_STORAGE);
	_Attribute(&pNewInfo->m_nGuildStorageMarkerID, pElement, INNROOM_XML_ATTR_GUILDSTORAGE);

	Insert(pNewInfo);
}

GInnRoomInfo* GInnRoomInfoMgr::NewInfo()
{
	return new GInnRoomInfo();
}

void GInnRoomInfoMgr::Insert(GInnRoomInfo* pInfo)
{
	if (NULL == pInfo) return;

	m_mapInnRoomInfo.insert(MAP_INNROOMINFO::value_type(pInfo->m_nID, pInfo));
}

GInnRoomInfo* GInnRoomInfoMgr::Get(uint8 nID)
{
	MAP_INNROOMINFO::iterator itor = m_mapInnRoomInfo.find(nID);
	if (m_mapInnRoomInfo.end() == itor) return NULL;

	return (*itor).second;
}

