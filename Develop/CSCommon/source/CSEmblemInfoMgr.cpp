#include "StdAfx.h"
#include "CSEmblemInfoMgr.h"
#include "CSEmblemInfo.h"
#include "MLocale.h"

CSEmblemInfoMgr::CSEmblemInfoMgr(void)
{
}

CSEmblemInfoMgr::~CSEmblemInfoMgr(void)
{
	Clear();
}

void CSEmblemInfoMgr::Clear()
{
	for each(EmblemMap::value_type pair in m_mapEmblems)
	{
		delete pair.second;
	}

	m_mapEmblems.clear();
}

bool CSEmblemInfoMgr::Load(const TCHAR* szFileName)
{
	_ASSERT(NULL != szFileName);
	if (NULL == szFileName) return false;

	
	MXml xml;
	if (!xml.LoadFile(MLocale::ConvTCHARToAnsi(szFileName).c_str())) return false;

	MXmlHandle docHandle = xml.DocHandle();
	MXmlElement* pElement = docHandle.FirstChild(EMBLEM_XML_TAG_MAIET).FirstChildElement().Element();

	for (pElement; pElement != NULL; pElement = pElement->NextSiblingElement())
	{
		if (!_stricmp(pElement->Value(), EMBLEMS_XML_TAG_EMBLEMS))
		{
			ParseEmblems(pElement, &xml);
		}
	}

	OnLoadCompleted();

	return true;
}

void CSEmblemInfoMgr::ParseEmblems( MXmlElement* pElement, MXml* pXml )
{
	MXmlElement* pChild = pElement->FirstChildElement();
	for (pChild; pChild != NULL; pChild=pChild->NextSiblingElement() )
	{
		if (!_stricmp(pChild->Value(), EMBLEMS_XML_TAG_EMBLEM))
		{
			ParseEmblem(pChild, pXml);
		}
	}
}

void CSEmblemInfoMgr::ParseEmblem( MXmlElement* pElement, MXml* pXml )
{
	int nID;
	_Attribute(&nID, pElement, EMBLEM_XML_ATTR_ID);

	_ASSERT(!Get(nID));

	CSEmblemInfo* pNewInfo = new CSEmblemInfo();
	pNewInfo->nID = nID;

	_Attribute(&pNewInfo->nConditionID, pElement, EMBLEM_XML_ATTR_CONDITION);
	

	Insert(pNewInfo);
}

void CSEmblemInfoMgr::Insert( CSEmblemInfo* pEmblem )
{
	_ASSERT(NULL != pEmblem);
	if (NULL == pEmblem) return;
	
	m_mapEmblems.insert(make_pair(pEmblem->nID, pEmblem));
}

CSEmblemInfo* CSEmblemInfoMgr::Get( int nID )
{
	EmblemMap::iterator itor = m_mapEmblems.find(nID);
	if (m_mapEmblems.end() == itor) 
		return NULL;

	return (*itor).second;
}