#include "StdAfx.h"
#include "GNPCShopInfoMgr.h"
#include "GNPCShopInfo.h"
#include "GItemManager.h"
#include "GConditionsInfoMgr.h"
#include "GDialogInfoMgr.h"
#include "GValidateLogger.h"
#include "GSTLUtil.h"
#include "GGlobal.h"
#include "GDef.h"
#include "GStrings.h"

GNPCShopInfoMgr::GNPCShopInfoMgr(void)
{
}

GNPCShopInfoMgr::~GNPCShopInfoMgr(void)
{
	Clear();
}

void GNPCShopInfoMgr::Clear()
{
	for_each(m_mapNPCShopInfo.begin(), m_mapNPCShopInfo.end(), DPSO());
	m_mapNPCShopInfo.clear();	
}

bool GNPCShopInfoMgr::Load(const wchar_t* szFileName)
{
	_ASSERT(NULL != szFileName);
	if (NULL == szFileName) return false;

	_VLI(szFileName);

	MXml xml;
	if (!xml.LoadFile(MLocale::ConvUTF16ToAnsi(szFileName).c_str())) return false;

	MXmlHandle docHandle = xml.DocHandle();
	MXmlElement* pElement = docHandle.FirstChild(TRADE_XML_TAG_MAIET).FirstChildElement().Element();

	for (pElement; pElement != NULL; pElement = pElement->NextSiblingElement())
	{
		if (!_stricmp(pElement->Value(), TRADE_XML_TAG_NPCSHOP))
		{
			ParseNPCShop(pElement, &xml);
		}
	}

	return true;
}

void GNPCShopInfoMgr::ParseNPCShop(MXmlElement* pElement, MXml* pXml)
{
	int nID;
	_Attribute(&nID, pElement, TRADE_XML_ATTR_ID);

	_VLE(_T(TRADE_XML_TAG_NPCSHOP));	
	_VLA(_T(TRADE_XML_ATTR_ID), nID);
	_VLP;

	GNPCShopInfo* pExistInfo = Get(nID);
	_ASSERT(NULL == pExistInfo);
	if (NULL != pExistInfo)
	{
		_VLOGGER->Log(_T(TRADE_XML_ATTR_ID) IS_ALREADY_EXIST);
		return;
	}

	GNPCShopInfo* pNewInfo = new GNPCShopInfo();
	pNewInfo->m_nID = nID;

	int nDialogID;
	_Attribute(&nDialogID,	pElement, TRADE_XML_ATTR_DIALOG);
	pNewInfo->m_pDialogInfo = gmgr.pDialogInfoMgr->Get(nDialogID);
	_ASSERT(NULL != pNewInfo->m_pDialogInfo);
	if (NULL == pNewInfo->m_pDialogInfo)
	{
		_VLOGGER->Log(_T(TRADE_XML_ATTR_DIALOG) IS_NOT_IN_DIALOG);
	}

	_Attribute(&pNewInfo->m_fBaseBuyMod,	pElement, TRADE_XML_ATTR_BUYMOD);
	_Attribute(&pNewInfo->m_fBaseSellMod,	pElement, TRADE_XML_ATTR_SELLMOD);
	_Attribute(&pNewInfo->m_nFactionID,	pElement, TRADE_XML_ATTR_FACTION);
	_Attribute(&pNewInfo->m_bRepairable,pElement, TRADE_XML_ATTR_REPAIRABLE);

	MXmlElement* pChild = pElement->FirstChildElement();
	for (pChild; pChild != NULL; pChild = pChild->NextSiblingElement())
	{
		if (!_stricmp(pChild->Value(), TRADE_XML_TAG_ITEM))
		{
			ParseItem(pChild, pXml, pNewInfo);
		}
	}

	Insert(pNewInfo);
}

void GNPCShopInfoMgr::ParseItem(MXmlElement* pElement, MXml* pXml, GNPCShopInfo* pNPCShopInfo)
{
	wstring strValue;
	int nConditionsID;
	vector<int> vecItemID;
	if (false == _Attribute(strValue,		pElement, TRADE_XML_ATTR_ID)) return;

	_VLE(_T(TRADE_XML_TAG_ITEM));	
	_VLA(_T(TRADE_XML_ATTR_ID), strValue);
	_VLP;

	GStrings::StringNumbersToIntVector(strValue, vecItemID);

	if (true == _Attribute(&nConditionsID,	pElement, TRADE_XML_ATTR_CONDITION))
	{
		GConditionsInfo* pConditionsInfo = gmgr.pCondtionsInfoMgr->Get(nConditionsID);
		if (NULL == pConditionsInfo)
		{			
			_VLOGGER->Log(_T(TRADE_XML_ATTR_CONDITION) IS_NOT_IN_CONDITIONS);
			return;
		}

		for each (int nItemID in vecItemID)
		{
			GItemData* pItemData = gmgr.pItemManager->GetItemData(nItemID);
			_ASSERT(NULL != pItemData);
			if (NULL == pItemData)
			{
				_VLOGGER->ValidateItemID(nItemID, _T(TRADE_XML_ATTR_ID));
				continue;
			}

			pNPCShopInfo->m_mapConditionsItem.insert(map<GItemData*, GConditionsInfo*>::value_type(pItemData, pConditionsInfo));
		}
	}
	else
	{
		for each (int nItemID in vecItemID)
		{
			GItemData* pItemData = gmgr.pItemManager->GetItemData(nItemID);
			if (NULL == pItemData)
			{				
				_VLOGGER->ValidateItemID(nItemID, _T(TRADE_XML_ATTR_ID));
				continue;
			}

			pNPCShopInfo->m_vecNormalItem.push_back(pItemData);
		}
	}	
}

void GNPCShopInfoMgr::Insert( GNPCShopInfo* pNPCShopInfo )
{
	_ASSERT(NULL != pNPCShopInfo);
	if (NULL == pNPCShopInfo) return;

	GNPCShopInfo* pExistInfo = Get(pNPCShopInfo->m_nID);
	_ASSERT(NULL == pExistInfo);
	if (NULL != pExistInfo) return;

	m_mapNPCShopInfo.insert(MAP_NPCSHOPINFO::value_type(pNPCShopInfo->m_nID, pNPCShopInfo));
}

GNPCShopInfo* GNPCShopInfoMgr::Get( int nID )
{
	if (true == m_mapNPCShopInfo.empty()) return NULL;

	MAP_NPCSHOPINFO::iterator itor = m_mapNPCShopInfo.find(nID);
	if (m_mapNPCShopInfo.end() == itor) return NULL;

	return (*itor).second;
}

bool GNPCShopInfoMgr::IsRepairable(int nID)
{
	GNPCShopInfo* pInfo = Get(nID);
	if (NULL == pInfo) return false;

	return pInfo->m_bRepairable;	
}