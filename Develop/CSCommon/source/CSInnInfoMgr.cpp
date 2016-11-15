#include "StdAfx.h"
#include "CSInnInfoMgr.h"
#include "CSInnInfo.h"
#include "ASTLUtil.h"
#include "MLocale.h"

CSInnInfoMgr::CSInnInfoMgr()
{
}

CSInnInfoMgr::~CSInnInfoMgr()
{
	Clear();
}

void CSInnInfoMgr::Clear()
{
	SAFE_DELETE_MAP(m_mapInnInfo);
}

bool CSInnInfoMgr::Load(const TCHAR* szFileName)
{
	if (NULL == szFileName) return false;

	
	MXml xml;
	if (!xml.LoadFile(MLocale::ConvTCHARToAnsi(szFileName).c_str())) return false;

	MXmlHandle docHandle = xml.DocHandle();
	MXmlElement* pElement = docHandle.FirstChild(INN_XML_TAG_MAIET).FirstChildElement().Element();

	for (pElement; pElement != NULL; pElement = pElement->NextSiblingElement())
	{
		if (!_stricmp(pElement->Value(), INN_XML_TAG_INN))
		{
			CSInnInfo* pNewInfo = NewInfo();
			if (true == ParseInn(pElement, &xml, pNewInfo))
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

bool CSInnInfoMgr::ParseInn(MXmlElement* pElement, MXml* pXml, CSInnInfo* pInfo)
{
	int nID;
	_Attribute(&nID, pElement, INN_XML_ATTR_ID);

	CSInnInfo* pExistInfo = Get(nID);
	if (NULL != pExistInfo)
	{
		_ASSERT(0);
		return false;
	}

	pInfo->m_nID = nID;

	return true;
}

void CSInnInfoMgr::Insert(CSInnInfo* pInfo)
{
	if (NULL == pInfo) return;

	m_mapInnInfo.insert(MAP_INNINFO::value_type(pInfo->m_nID, pInfo));
}

CSInnInfo* CSInnInfoMgr::Get(uint8 nID)
{
	MAP_INNINFO::iterator itor = m_mapInnInfo.find(nID);
	if (m_mapInnInfo.end() == itor) return NULL;

	return (*itor).second;
}