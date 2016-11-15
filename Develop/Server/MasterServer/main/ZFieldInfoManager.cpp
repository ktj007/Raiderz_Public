#include "stdafx.h"
#include "ZFieldInfoManager.h"
#include "MDebug.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Tag
#define FIELD_XML_TAG_MAIET			"maiet"
#define FIELD_XML_TAG_FIELD			"FIELD"
#define FIELD_XML_TAG_FIELDGROUP	"FIELDGROUP"
#define FIELD_XML_TAG_FIELDLIST		"FIELDLIST"


// Attribute
#define FIELD_XML_ATTR_ID			"id"
#define FIELD_XML_ATTR_ZONE			"zone"
#define FIELD_XML_ATTR_START_FIELD	"start_field"
#define FIELD_XML_ATTR_START_MARKER	"start_marker"

#define FIELD_XML_ATTR_VALIDATION	"validation"


ZFieldInfoManager::ZFieldInfoManager()
{

}

ZFieldInfoManager::~ZFieldInfoManager()
{
	Clear();
}

void ZFieldInfoManager::Insert(ZFieldInfo* pFieldInfo)
{
	if (NULL == pFieldInfo) return;

	m_FieldInfoMap.insert(ZFieldInfoMap::value_type(pFieldInfo->m_nFieldID, pFieldInfo));
}

bool ZFieldInfoManager::Load(const wchar_t* szFieldList, const wchar_t* szFieldPath, bool bIncludeDevFieldList )
{
	m_strFieldPath = szFieldPath;

	if (m_FieldList.Load(szFieldList, bIncludeDevFieldList) == false) return false;

	// 모든 필드 로딩
	for (CSFieldList::iterator iter=m_FieldList.begin();
		iter !=m_FieldList.end();
		++iter)
	{
		NewFieldInfo((*iter).first, (*iter).second);
	}

	if (LoadFieldGroupInfo(szFieldList) == false) return false;

	return true;
}

ZFieldInfo* ZFieldInfoManager::NewFieldInfo( int nFieldID, CSFieldFileInfo* pFileInfo )
{
	ZFieldInfo* pFieldInfo = new ZFieldInfo();
	pFieldInfo->Assign(pFileInfo);

	m_FieldInfoMap.insert(ZFieldInfoMap::value_type(pFieldInfo->m_nFieldID, pFieldInfo));

	wstring strFieldFileName = pFieldInfo->MakeFieldFileName(m_strFieldPath.c_str());

	if (LoadField(strFieldFileName.c_str(), pFieldInfo) == false)
	{
		ZFieldInfoMap::iterator itor = m_FieldInfoMap.find(pFieldInfo->m_nFieldID);
		if (itor != m_FieldInfoMap.end()) m_FieldInfoMap.erase(itor);
		delete pFieldInfo;
		return NULL;
	}

	return pFieldInfo;
}

bool ZFieldInfoManager::LoadFieldGroupInfo(const wchar_t* szFileName)
{
	USES_CONVERSION_EX;

	mlog0("LoadFieldGroupInfo(%s)...Begin\n", W2A_EX(szFileName, 0));

	MXml xml;
	if (!xml.LoadFile(W2A_EX(szFileName, 0))) 
	{
		_ASSERT(0);
		return false;
	}

	MXmlHandle docHandle = xml.DocHandle();
	MXmlElement* pElement = docHandle.FirstChild(FIELD_XML_TAG_MAIET).FirstChildElement().Element();

	for( pElement; pElement != NULL; pElement=pElement->NextSiblingElement() )
	{
		if (0 != _stricmp(pElement->Value(), FIELD_XML_TAG_FIELDLIST)) continue;
		
		MXmlElement* pFieldElement = pElement->FirstChildElement(FIELD_XML_TAG_FIELD);
		for (pFieldElement; pFieldElement != NULL; pFieldElement = pFieldElement->NextSiblingElement())
		{
			if (!_stricmp(pFieldElement->Value(), FIELD_XML_TAG_FIELDGROUP))
			{
				int nFieldGroupID;
				_Attribute(&nFieldGroupID,				pFieldElement, FIELD_XML_ATTR_ID);
				
				int nStartFieldID;
				_Attribute(&nStartFieldID,				pFieldElement, FIELD_XML_ATTR_START_FIELD);
				
				FIELD_GROUP_INFO* info = new FIELD_GROUP_INFO;
				info->nID = nFieldGroupID;
				
				MXmlElement* pChild = pFieldElement->FirstChildElement();
				for (pChild; pChild != NULL; pChild=pChild->NextSiblingElement() )
				{
					if (!_stricmp(pChild->Value(), FIELD_XML_TAG_FIELD))
					{
						int nFieldID;
						_Attribute(&nFieldID,			pChild, FIELD_XML_ATTR_ID);
						
						ZFieldInfo* pFieldInfo = gmgr.pFieldInfoManager->Find(nFieldID);
						_ASSERT(NULL != pFieldInfo);
						if (NULL == pFieldInfo)
						{
							continue;
						}
						
						info->vecFieldInfo.push_back(pFieldInfo);
					}
				}
				
				ZFieldInfo* pStartFieldInfo = gmgr.pFieldInfoManager->Find(nStartFieldID);
				info->pStartFieldInfo = pStartFieldInfo;
				
				InsertFieldGroup(info);
			}
		}
	}

	mlog0("LoadFieldGroupInfo()...Complete\n");
	return true;
}

void ZFieldInfoManager::InsertFieldGroup(FIELD_GROUP_INFO* pFieldGroupInfo)
{
	if (NULL == pFieldGroupInfo) return;

	m_FieldGroupInfoMap.insert(ZFieldGroupInfoMap::value_type(pFieldGroupInfo->nID, pFieldGroupInfo));
}

void ZFieldInfoManager::Clear()
{
	while (!m_FieldInfoMap.empty())
	{
		ZFieldInfoMap::iterator itor = m_FieldInfoMap.begin();
		delete (*itor).second;
		m_FieldInfoMap.erase(itor);
	}

	while (!m_FieldGroupInfoMap.empty())
	{
		ZFieldGroupInfoMap::iterator itor = m_FieldGroupInfoMap.begin();
		delete (*itor).second;
		m_FieldGroupInfoMap.erase(itor);
	}

}

bool ZFieldInfoManager::LoadField( const wchar_t* szFileName, ZFieldInfo* pFieldInfo )
{
	USES_CONVERSION_EX;

	MXml xml;
	if (!xml.LoadFile(W2A_EX(szFileName, 0))) 
	{
		mlog("Field Loading Failed : (%s)\n", W2A_EX(szFileName, 0));
		return false;
	}

	MXmlHandle docHandle = xml.DocHandle();
	MXmlElement* pElement = docHandle.FirstChild(FIELD_XML_TAG_MAIET).FirstChildElement().Element();

	_Attribute(&pFieldInfo->m_nZoneID,		pElement, FIELD_XML_ATTR_ZONE);

	return true;
}

ZFieldInfo* ZFieldInfoManager::Find( int nFieldID )
{
	ZFieldInfoMap::iterator itor = m_FieldInfoMap.find(nFieldID);
	if (itor != m_FieldInfoMap.end())
	{
		return (*itor).second;
	}
	return NULL;
}

FIELD_GROUP_INFO* ZFieldInfoManager::FindGroupInfo(int nFieldGroupInfo)
{
	ZFieldGroupInfoMap::iterator itor = m_FieldGroupInfoMap.find(nFieldGroupInfo);
	if (itor != m_FieldGroupInfoMap.end())
	{
		return (*itor).second;
	}
	return NULL;
}