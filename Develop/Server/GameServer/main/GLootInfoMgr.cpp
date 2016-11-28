#include "StdAfx.h"
#include "GLootInfoMgr.h"
#include "GLootInfo.h"
#include "GSTLUtil.h"
#include "GStrings.h"
#include "GGlobal.h"
#include "GValidateLogger.h"


GLootInfoMgr::GLootInfoMgr()
{
	
}

GLootInfoMgr::~GLootInfoMgr()
{
	Clear();
}

void GLootInfoMgr::Clear()
{
	for_each(m_mapLootInfo.begin(), m_mapLootInfo.end(), DPSO());
	m_mapLootInfo.clear();
}

bool GLootInfoMgr::Load( const wchar_t* szFileName )
{
	if (NULL == szFileName) return false;

	MXml xml;

	if (!xml.LoadFile(MLocale::ConvUTF16ToAnsi(szFileName).c_str())) return false;

	_VLI(szFileName);

	MXmlHandle docHandle = xml.DocHandle();
	MXmlElement* pElement = docHandle.FirstChild(LOOT_XML_TAG_MAIET).FirstChildElement().Element();

	for( pElement; pElement != NULL; pElement=pElement->NextSiblingElement() )
	{
		if (!_stricmp(pElement->Value(), LOOT_XML_TAG_LOOT))
		{
			ParseLOOT(pElement, &xml);
		}
	}

	return true;
}

void GLootInfoMgr::ParseLOOT(MXmlElement* pElement, MXml* pXml)
{	
	int nLootID;
	_Attribute(&nLootID, pElement, LOOT_XML_ATTR_ID);

	_VLE(_T(LOOT_XML_TAG_LOOT));	
	_VLA(_T(LOOT_XML_ATTR_ID), nLootID);
	_VLP;

	if (IsExist(nLootID))
	{
		_VLOGGER->Log(_T(LOOT_XML_ATTR_ID) IS_ALREADY_EXIST);
		return;
	}

	GLootInfo* pNewLootINfo = new GLootInfo();
	pNewLootINfo->m_nID = nLootID;
	_Attribute(&pNewLootINfo->m_nLimitDropQuantity, pElement, LOOT_XML_ATTR_MAX);

	_Attribute(&pNewLootINfo->m_bValidation, pElement, LOOT_XML_ATTR_VALIDATION);
	_Attribute(&pNewLootINfo->m_bBPartLoot, pElement, LOOT_XML_ATTR_BPARTLOOT);

	MXmlElement* pChild = pElement->FirstChildElement();
	for (pChild; pChild != NULL; pChild=pChild->NextSiblingElement() )
	{
		if (!_stricmp(pChild->Value(), LOOT_XML_TAG_MONEY))
		{
			ParseMONEY(pChild, pXml, pNewLootINfo);
		}
		else if (!_stricmp(pChild->Value(), LOOT_XML_TAG_ITEM))
		{
			ParseITEM(pChild, pXml, pNewLootINfo);
		}
		else if (!_stricmp(pChild->Value(), LOOT_XML_TAG_SOUL))
		{
			ParseSOUL(pChild, pXml, pNewLootINfo);
		}
		else if (!_stricmp(pChild->Value(), LOOT_XML_TAG_BUFF))
		{
			ParseBUFF(pChild, pXml, pNewLootINfo);
		}
	}
	m_mapLootInfo.insert(LOOT_INFO_MAP::value_type(pNewLootINfo->m_nID, pNewLootINfo));
}

void GLootInfoMgr::ParseMONEY( MXmlElement* pElement, MXml* pXml, GLootInfo* pLootInfo )
{
	LOOT_MONEY_INFO* pNewLootMoneyInfo = new LOOT_MONEY_INFO();

	_Attribute(&pNewLootMoneyInfo->m_fRate,	pElement,	LOOT_XML_ATTR_RATE);
	_Attribute(&pNewLootMoneyInfo->m_nMin,	pElement,	LOOT_XML_ATTR_MIN);
	_Attribute(&pNewLootMoneyInfo->m_nMax,	pElement,	LOOT_XML_ATTR_MAX);

	if (pNewLootMoneyInfo->m_nMax < pNewLootMoneyInfo->m_nMin)
	{
		pNewLootMoneyInfo->m_nMax = pNewLootMoneyInfo->m_nMin;
	}

	pLootInfo->m_vecLootMoneys.push_back(pNewLootMoneyInfo);
}

void GLootInfoMgr::ParseITEM( MXmlElement* pElement, MXml* pXml, GLootInfo* pLootInfo )
{
	LOOT_ITEM_INFO* pNewLootItemInfo = new LOOT_ITEM_INFO();

	_Attribute(&pNewLootItemInfo->m_nID,				pElement,	LOOT_XML_ATTR_ID);
	_Attribute(&pNewLootItemInfo->m_fRate,				pElement,	LOOT_XML_ATTR_RATE);
	_Attribute(&pNewLootItemInfo->m_nMin,				pElement,	LOOT_XML_ATTR_MIN);
	_Attribute(&pNewLootItemInfo->m_nMax,				pElement,	LOOT_XML_ATTR_MAX);
	_Attribute(&pNewLootItemInfo->m_nBreakPart,			pElement,	LOOT_XML_ATTR_BPART);
	_Attribute(&pNewLootItemInfo->m_nQuestPVPFieldID,	pElement,	LOOT_XML_ATTR_QUESTPVP_FIELD);
	wstring strValue;
	if (true == _Attribute(strValue,		pElement, LOOT_XML_ATTR_QUEST, pXml))
	{
		GStrings::StringNumbersToIntVector(strValue, pNewLootItemInfo->m_vecQuestID);
	}

	if (pNewLootItemInfo->m_nMax < pNewLootItemInfo->m_nMin)
	{
		pNewLootItemInfo->m_nMax = pNewLootItemInfo->m_nMin;
	}

	pLootInfo->m_vecLootItems.push_back(pNewLootItemInfo);
}

void GLootInfoMgr::ParseSOUL( MXmlElement* pElement, MXml* pXml, GLootInfo* pLootInfo )
{
	LOOT_SOUL_INFO* pNewLootSoulInfo = new LOOT_SOUL_INFO();

	wstring strValue;
	if (true == _Attribute(strValue, pElement, LOOT_XML_ATTR_TYPE, pXml))
	{
		pNewLootSoulInfo->m_nType = static_cast<SOUL_TYPE>(GStrings::StringToSoulType(strValue.c_str()));
	}	

	_Attribute(&pNewLootSoulInfo->m_fRate,			pElement,	LOOT_XML_ATTR_RATE);
	_Attribute(&pNewLootSoulInfo->m_fBPart1Rate,	pElement,	LOOT_XML_ATTR_BPART1);
	_Attribute(&pNewLootSoulInfo->m_fBPart2Rate,	pElement,	LOOT_XML_ATTR_BPART2);
	_Attribute(&pNewLootSoulInfo->m_fBPart3Rate,	pElement,	LOOT_XML_ATTR_BPART3);
	_Attribute(&pNewLootSoulInfo->m_fBPart4Rate,	pElement,	LOOT_XML_ATTR_BPART4);
	_Attribute(&pNewLootSoulInfo->m_nMin,			pElement,	LOOT_XML_ATTR_MIN);
	_Attribute(&pNewLootSoulInfo->m_nMax,			pElement,	LOOT_XML_ATTR_MAX);

	if (pNewLootSoulInfo->m_nMax < pNewLootSoulInfo->m_nMin)
	{
		pNewLootSoulInfo->m_nMax = pNewLootSoulInfo->m_nMin;
	}

	pLootInfo->m_vecLootSouls.push_back(pNewLootSoulInfo);	
}

void GLootInfoMgr::ParseBUFF( MXmlElement* pElement, MXml* pXml, GLootInfo* pLootInfo )
{
	int nBuffID  = INVALID_BUFF_ID;
	_Attribute(&nBuffID,			pElement,	LOOT_XML_ATTR_ID);
	
	pLootInfo->m_vecLootBuffs.push_back(nBuffID);	
}


void GLootInfoMgr::Insert( GLootInfo* pLootInfo )
{
	if (NULL == pLootInfo) return;

	m_mapLootInfo.insert(LOOT_INFO_MAP::value_type(pLootInfo->m_nID, pLootInfo));
}

GLootInfo* GLootInfoMgr::Get( int nLootID )
{
	LOOT_INFO_MAP::iterator itor = m_mapLootInfo.find(nLootID);
	if (m_mapLootInfo.end() == itor) return NULL;

	return (*itor).second;
}

bool GLootInfoMgr::IsExist(int nID)
{
	if (NULL == Get(nID)) return false;

	return true;
}

LOOT_INFO_MAP& GLootInfoMgr::GetContainer()
{
	return m_mapLootInfo;
}