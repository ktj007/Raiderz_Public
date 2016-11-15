#include "StdAfx.h"
#include "XItemRelatedNPCInfoMgr.h"
#include "XItemRelatedNPCInfo.h"

XItemRelatedNPCInfoMgr::XItemRelatedNPCInfoMgr(void)
{
}

XItemRelatedNPCInfoMgr::~XItemRelatedNPCInfoMgr(void)
{
	Clear();
}

void XItemRelatedNPCInfoMgr::Clear()
{
	SAFE_DELETE_MAP(m_mapInfo);
}

bool XItemRelatedNPCInfoMgr::Load( const wchar_t* szFileName )
{
	_ASSERT(NULL != szFileName);
	if (NULL == szFileName) return false;

	MXml xml;
	if (!xml.LoadFile(MLocale::ConvUTF16ToAnsi(szFileName).c_str())) return false;

	MXmlHandle docHandle = xml.DocHandle();
	MXmlElement* pElement = docHandle.FirstChild(ITEMRELATEDNPC_XML_TAG_MAIET).FirstChildElement().Element();

	for (pElement; pElement != NULL; pElement = pElement->NextSiblingElement())
	{
		if (!_stricmp(pElement->Value(), ITEMRELATEDNPC_XML_TAG_ITEM))
		{
			XItemRelatedNPCInfo* pNewInfo = new XItemRelatedNPCInfo;
			if (true == ParseItem(pElement, &xml, pNewInfo))
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

bool XItemRelatedNPCInfoMgr::ParseItem(MXmlElement* pElement, MXml* pXml, XItemRelatedNPCInfo* pInfo)
{
	int nItemID;
	_Attribute(&nItemID, pElement, ITEMRELATEDNPC_XML_ATTR_ID);

	XItemRelatedNPCInfo* pExistInfo = Get(nItemID);
	_ASSERT(NULL == pExistInfo);
	if (NULL != pExistInfo)
	{
		return false;
	}

	pInfo->m_nItemID = nItemID;

	MXmlElement* pChild = pElement->FirstChildElement();
	for (pChild; pChild != NULL; pChild = pChild->NextSiblingElement())
	{
		if (!_stricmp(pChild->Value(), ITEMRELATEDNPC_XML_TAG_NPC))
		{
			ParseNPC(pChild, pXml, pInfo);
		}
	}

	return true;
}

void XItemRelatedNPCInfoMgr::ParseNPC( MXmlElement* pElement, MXml* pXml, XItemRelatedNPCInfo* pInfo )
{
	int nNPCID;
	_Attribute(&nNPCID, pElement, ITEMRELATEDNPC_XML_ATTR_ID);

	pInfo->m_setNPCID.insert(nNPCID);
}

void XItemRelatedNPCInfoMgr::Insert(XItemRelatedNPCInfo* pInfo)
{
	if (NULL == pInfo) return;

	if (NULL != Get(pInfo->m_nItemID))
	{
		_ASSERT(0);
		return;
	}

	m_mapInfo.insert(MAP_ITEMRELATEDNPCINFO::value_type(pInfo->m_nItemID, pInfo));
}

XItemRelatedNPCInfo* XItemRelatedNPCInfoMgr::Get( int nItemID )
{
	MAP_ITEMRELATEDNPCINFO::iterator itor = m_mapInfo.find(nItemID);
	if (m_mapInfo.end() == itor) return NULL;

	return (*itor).second;
}

bool XItemRelatedNPCInfoMgr::IsExist( int nItemID )
{
	if (NULL == Get(nItemID)) return false;

	return true;
}