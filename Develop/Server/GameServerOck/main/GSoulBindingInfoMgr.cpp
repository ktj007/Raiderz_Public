#include "StdAfx.h"
#include "GSoulBindingInfoMgr.h"
#include "GSoulBindingInfo.h"
#include "GValidateLogger.h"
#include "GSTLUtil.h"
#include "GGlobal.h"
#include "GFieldMgr.h"
#include "SDef.h"
#include "GFieldInfo.h"
#include "GFieldInfoMgr.h"
#include "GDef.h"

class GFieldInfo;
struct MARKER_INFO;


GSoulBindingInfoMgr::GSoulBindingInfoMgr()
{
}

GSoulBindingInfoMgr::~GSoulBindingInfoMgr()
{
	Clear();
}

void GSoulBindingInfoMgr::Clear()
{
	SAFE_DELETE_MAP(m_mapSoulBindingInfo)
	m_mapSoulBindingField.clear();
}

bool GSoulBindingInfoMgr::Load(const wchar_t* szFileName)
{
	_ASSERT(NULL != szFileName);
	if (NULL == szFileName) return false;

	_VLI(szFileName);

	MXml xml;
	if (!xml.LoadFile(MLocale::ConvUTF16ToAnsi(szFileName).c_str())) return false;

	MXmlHandle docHandle = xml.DocHandle();
	MXmlElement* pElement = docHandle.FirstChild(SOULBINDING_XML_TAG_MAIET).FirstChildElement().Element();

	for (pElement; pElement != NULL; pElement = pElement->NextSiblingElement())
	{
		if (!_stricmp(pElement->Value(), SOULBINDING_XML_TAG_SOULBINDING))
		{
			ParserSoulBinding(pElement, &xml);
		}
	}

	Cooking();

	return true;
}

void GSoulBindingInfoMgr::ParserSoulBinding(MXmlElement* pElement, MXml* pXml)
{
	int nSoulBindingID;
	_Attribute(&nSoulBindingID, pElement, SOULBINDING_XML_ATTR_ID);

	_VLE(_T(SOULBINDING_XML_TAG_SOULBINDING));	
	_VLA(_T(SOULBINDING_XML_ATTR_ID), nSoulBindingID);
	_VLP;

	const GSoulBindingInfo* pExistInfo = Get(nSoulBindingID);
	_ASSERT(NULL == pExistInfo);
	if (NULL != pExistInfo)
	{
		_VLOGGER->Log(_T(SOULBINDING_XML_ATTR_ID) IS_ALREADY_EXIST);
		return;
	}	
	int nFieldID;
	_Attribute(&nFieldID, pElement, SOULBINDING_XML_ATTR_FIELD);
	GFieldInfo* pFieldInfo = gmgr.pFieldInfoMgr->Find(nFieldID);	
	if (NULL == pFieldInfo)
	{
		_VLOGGER->Log(_T(SOULBINDING_XML_ATTR_FIELD) IS_NOT_IN_FIELDLIST);
		return;
	}

	int nMarkerID;
	_Attribute(&nMarkerID, pElement, SOULBINDING_XML_ATTR_MARKER);
	const MARKER_INFO* pMarkerInfo = pFieldInfo->FindMarker(nMarkerID);	
	if (NULL == pMarkerInfo)
	{
		_VLOGGER->Log(_T(SOULBINDING_XML_ATTR_MARKER) IS_NOT_IN_FIELDLIST);
		return;
	}

	GSoulBindingInfo* pNewSoulBinding = new GSoulBindingInfo();
	pNewSoulBinding->m_nID = static_cast<uint8>(nSoulBindingID);
	pNewSoulBinding->m_pMarkerInfo = pMarkerInfo;

	_Attribute(&pNewSoulBinding->m_nDialogID, pElement, SOULBINDING_XML_ATTR_DIALOG);
	_Attribute(&pNewSoulBinding->m_nFactionID, pElement, SOULBINDING_XML_ATTR_FACTION);

	Insert(pNewSoulBinding);
}

void GSoulBindingInfoMgr::Insert(GSoulBindingInfo* pSoulBindingInfo)
{
	_ASSERT(NULL != pSoulBindingInfo);
	if (NULL == pSoulBindingInfo) return;

	m_mapSoulBindingInfo.insert(MAP_SOULBINDINGINFO::value_type(pSoulBindingInfo->m_nID, pSoulBindingInfo));
}

GSoulBindingInfo* GSoulBindingInfoMgr::Get(SBID nID)
{
	if (true == m_mapSoulBindingInfo.empty()) return NULL;

	MAP_SOULBINDINGINFO::iterator itor = m_mapSoulBindingInfo.find(nID);
	if (m_mapSoulBindingInfo.end() == itor) return NULL;

	return (*itor).second;
}

bool GSoulBindingInfoMgr::IsSoulBindingID(SBID nID)
{
	if (NULL == Get(nID)) return false;

	return true;
}

MAP_SOULBINDINGINFO& GSoulBindingInfoMgr::GetContainer()
{
	return m_mapSoulBindingInfo;
}

void GSoulBindingInfoMgr::Cooking()
{
	for each (const MAP_SOULBINDINGINFO::value_type& val in  m_mapSoulBindingInfo)
	{
		GSoulBindingInfo* pSoulBindingInfo = val.second;
		if (NULL == pSoulBindingInfo) continue;
		const MARKER_INFO* pMarkerInfo = pSoulBindingInfo->m_pMarkerInfo;
		if (NULL == pMarkerInfo) continue;

		int nFieldID = pMarkerInfo->nFieldID;
		
		MapSecondVectorInserter(m_mapSoulBindingField, nFieldID, pSoulBindingInfo);
	}
}

vector<GSoulBindingInfo*> GSoulBindingInfoMgr::CollectSoulBindingInfo( int nFieldID )
{
	MAP_SOULBINDINFIELD::iterator itor = m_mapSoulBindingField.find(nFieldID);
	if (m_mapSoulBindingField.end() == itor) return vector<GSoulBindingInfo*>();

	return itor->second;
}


bool GSoulBindingInfoMgr::IsSoulBindingField( int nFieldID )
{
	return m_mapSoulBindingField.end() != m_mapSoulBindingField.find(nFieldID);
}
