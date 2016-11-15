#include "stdafx.h"
#include "CSFieldInfo.h"
#include "MTypes.h"
#include "MDebug.h"
#include "MLocale.h"
#include "CSFormatString.h"


//////////////////////////////////////////////////////////////////////////////////////////
// Tag
const char* ZONELIST_XML_TAG_MAIET		= "maiet";
const char* ZONELIST_XML_TAG_ZONELIST	= "ZONELIST";
const char* ZONELIST_XML_TAG_ZONE		= "ZONE";

const char* ZONELIST_XML_ATTR_ID		= "id";
const char* ZONELIST_XML_ATTR_NAME		= "name";
const char* ZONELIST_XML_ATTR_FILE		= "file";


CSZoneList::CSZoneList()
{

}

CSZoneList::~CSZoneList()
{
	Clear();
}

void CSZoneList::Clear()
{
	while (!empty())
	{
		iterator itor = begin();
		delete (*itor).second;
		erase(itor);
	}
}

bool CSZoneList::Load(const TCHAR* szFileName)
{
	MXml xml;
	if (!xml.LoadFile(MLocale::ConvTCHARToAnsi(szFileName).c_str()))
	{
		_ASSERT(0);
		return false;
	}

	MXmlHandle docHandle = xml.DocHandle();
	MXmlElement* pElement = docHandle.FirstChild(ZONELIST_XML_TAG_MAIET).FirstChildElement().Element();

	for( pElement; pElement != NULL; pElement=pElement->NextSiblingElement() )
	{
		if (!_stricmp(pElement->Value(), ZONELIST_XML_TAG_ZONELIST))
		{
			MXmlElement* pZoneElement = pElement->FirstChildElement(ZONELIST_XML_TAG_ZONE);
			for( pZoneElement; pZoneElement != NULL; pZoneElement=pZoneElement->NextSiblingElement() )
			{
				if (!_stricmp(pZoneElement->Value(), ZONELIST_XML_TAG_ZONE))
				{
					ParseZone(pZoneElement, &xml);
				}
			}
		}
	}

	return true;
}

CSZoneFileInfo* CSZoneList::Find(int nID)
{
	iterator itor = find(nID);
	if (itor != end())
	{
		return static_cast<CSZoneFileInfo*> ( (*itor).second );
	}

	return NULL;
}

void CSZoneList::ParseZone(MXmlElement* pElement, MXml* pXml)
{
	CSZoneFileInfo* pNewInfo = new CSZoneFileInfo();

	_Attribute(&pNewInfo->nZoneID, pElement, ZONELIST_XML_ATTR_ID);
	_Attribute(pNewInfo->strName, pElement, ZONELIST_XML_ATTR_NAME, pXml);
	_Attribute(pNewInfo->strFile, pElement, ZONELIST_XML_ATTR_FILE, pXml);

	// 중복 체크
	bool bOverlap = CheckOverlapZoneID(pNewInfo->nZoneID);
	if(bOverlap)
	{
		SAFE_DELETE(pNewInfo);
		_ASSERT(bOverlap == false && "Zone ID 중복입니다.");
		return;
	}

	insert(value_type(pNewInfo->nZoneID, pNewInfo));
}

bool CSZoneList::CheckOverlapZoneID( int nZoneID )
{
	CSZoneFileInfo * pZoneInfo = Find(nZoneID);
	if(pZoneInfo != NULL)
	{
		return true;
	}

	return false;
}

//////////////////////////////////////////////////////////////////////////////////////////
// Tag
const char* FIELDLIST_XML_TAG_MAIET				= "maiet";
const char* FIELDLIST_XML_TAG_FIELDLIST			= "FIELDLIST";
const char* FIELDLIST_XML_TAG_FIELDLIST_DEVELOP	= "FIELDLIST_DEVELOP";
const char* FIELDLIST_XML_TAG_FIELD				= "FIELD";
const char* FIELDLIST_XML_TAG_FIELDGROUP		= "FIELDGROUP";

const char* FIELDLIST_XML_ATTR_ID		= "id";
const char* FIELDLIST_XML_ATTR_NAME		= "name";
const char* FIELDLIST_XML_ATTR_DEFAULT_CHANNEL	= "default_channel";
const char* FIELDLIST_XML_ATTR_CAPACITY	= "capacity";
const char* FIELDLIST_XML_ATTR_VALIDATION	= "validation";
const char* FIELDLIST_XML_ATTR_AILOD_ENABLE	= "ailod_enable";
const char* FIELDLIST_XML_ATTR_SHOW_MAP		= "show_map";

CSFieldList::CSFieldList()
{

}

CSFieldList::~CSFieldList()
{
	Clear();
}

void CSFieldList::Clear()
{
	while (!empty())
	{
		iterator itor = begin();
		delete (*itor).second;
		erase(itor);
	}
}

bool CSFieldList::Load(const TCHAR* szFileName, bool bIncludeDevFieldList)
{
	MXml xml;
	if (!xml.LoadFile(MLocale::ConvTCHARToAnsi(szFileName).c_str())) 
	{
		assert(!"invalid xml file format\n");
		return false;
	}

	MXmlHandle docHandle = xml.DocHandle();
	MXmlElement* pElement = docHandle.FirstChild(FIELDLIST_XML_TAG_MAIET).FirstChildElement().Element();

	for( pElement; pElement != NULL; pElement=pElement->NextSiblingElement() )
	{
		if (!_stricmp(pElement->Value(), FIELDLIST_XML_TAG_FIELDLIST))
		{
			ParseFieldList(pElement, &xml);
		}
		else if (!_stricmp(pElement->Value(), FIELDLIST_XML_TAG_FIELDLIST_DEVELOP))
		{
			if (bIncludeDevFieldList)
			{
				ParseFieldList(pElement, &xml);
			}
		}
	}

	return true;
}

void CSFieldList::ParseField(MXmlElement* pElement, MXml* pXml, CSFieldFileInfo*& pNewInfo)
{
	int nFieldID = 0;
	_Attribute(&nFieldID, pElement, FIELDLIST_XML_ATTR_ID);

	// 중복 체크(메모리릭 생기는 원인이 된다)
	if(Find(nFieldID))
	{
		// 이미 등록되어 있다.
		mlog("Parse Field Error [%d]- (중복)", nFieldID);
		_ASSERT(FALSE && "필드 ID 중복");
		return;
	}

	pNewInfo = NULL;
	pNewInfo = new CSFieldFileInfo();

	pNewInfo->nFieldID = nFieldID;	
	
	pNewInfo->strName = CSFormatString::Format(_T("FIELD_NAME_{0}"), FSParam(pNewInfo->nFieldID));
	
	_Attribute(&pNewInfo->nDefaultChannelCount, pElement, FIELDLIST_XML_ATTR_DEFAULT_CHANNEL);
	_Attribute(&pNewInfo->nCapacity, pElement, FIELDLIST_XML_ATTR_CAPACITY);
	_Attribute(&pNewInfo->bValidation, pElement, FIELDLIST_XML_ATTR_VALIDATION);
	_Attribute(&pNewInfo->bAILODEnable, pElement, FIELDLIST_XML_ATTR_AILOD_ENABLE);
	_Attribute(&pNewInfo->bShowMap, pElement, FIELDLIST_XML_ATTR_SHOW_MAP);

	TCHAR buff[512];
	_stprintf_s(buff, _T("%d"), pNewInfo->nFieldID);
	pNewInfo->strFile = buff;

	Insert(pNewInfo);
}

void CSFieldList::ParseFieldGroup(MXmlElement* pElement, MXml* pXml)
{
	MXmlElement* pChildElement = pElement->FirstChildElement(FIELDLIST_XML_TAG_FIELD);
	for( pChildElement; pChildElement != NULL; pChildElement=pChildElement->NextSiblingElement() )
	{
		if (!_stricmp(pChildElement->Value(), FIELDLIST_XML_TAG_FIELD))
		{
			CSFieldFileInfo* pNewInfo=NULL;
			ParseField(pChildElement, pXml, pNewInfo);
			if (pNewInfo)
			{
				pNewInfo->bDynamic = true;
			}
		}
	}
}

void CSFieldList::Insert(CSFieldFileInfo* pFieldFileInfo)
{
	insert(value_type(pFieldFileInfo->nFieldID, pFieldFileInfo));
}

CSFieldFileInfo* CSFieldList::Find(int nID)
{
	iterator itor = find(nID);
	if (itor != end())
	{
		return static_cast<CSFieldFileInfo*> ( (*itor).second );
	}

	return NULL;
}

void CSFieldList::ParseFieldList( MXmlElement* pElement, MXml* pXml )
{
	MXmlElement* pFieldElement = pElement->FirstChildElement(FIELDLIST_XML_TAG_FIELD);
	if (!pFieldElement)
		pFieldElement = pElement->FirstChildElement(FIELDLIST_XML_TAG_FIELDGROUP);

	for( pFieldElement; pFieldElement != NULL; pFieldElement=pFieldElement->NextSiblingElement() )
	{
		if (!_stricmp(pFieldElement->Value(), FIELDLIST_XML_TAG_FIELD))
		{
			CSFieldFileInfo* pNewInfo=NULL;
			ParseField(pFieldElement, pXml, pNewInfo);
		}
		if (!_stricmp(pFieldElement->Value(), FIELDLIST_XML_TAG_FIELDGROUP))
		{
			ParseFieldGroup(pFieldElement, pXml);
		}
	}
}

bool CSFieldList::IsExist( int nID )
{
	if (NULL == Find(nID)) return false;

	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////
