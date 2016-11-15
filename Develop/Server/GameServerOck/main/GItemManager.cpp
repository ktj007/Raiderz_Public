#include "stdafx.h"
#include "GItemManager.h"
#include "CSStrings.h"
#include "GEntityPlayer.h"
#include "GGlobal.h"
#include "GDBManager.h"
#include "GValidateLogger.h"
#include "GRecipeInfoMgr.h"

#define ITEM_XML_ATTR_REPAIR_GRADE			L"repairgrade"


GItemManager::GItemManager() : m_uidNowItemID(0,0)
{
	;
}


GItemManager::~GItemManager()
{
	Clear();
}

GItemData* GItemManager::NewItemData()
{
	return (new GItemData());
}

GItem* GItemManager::MakeNewItem(int id, int nAmount, bool bBind)
{
	GItemData* pItemData = GetItemData(id);

	if (pItemData == NULL)
	{
		dlog("GItemManager::MakeNewItem(), invalid item id. (id: %d)\n", id);

#ifdef _DEBUG
		gsys.pDBManager->DeleteItemInstanceForDebug(id);
		dlog("DB에서 %d ID의 Item을 모두 삭제하였습니다.\n", id);
#endif

		//_ASSERT(0 && L"no item id");
		return NULL;
	}

	GItem* pNewItem = new GItem();
	

	if (pNewItem == NULL)
	{
		_ASSERT(0 && L"new alloc failed!");
		return NULL;
	}

	pNewItem->Set(GetNewItemID(), pItemData);
	pNewItem->m_nDurability = pItemData->m_nMaxDurability;
	pNewItem->m_nAmount = nAmount;
	pNewItem->m_bBind = bBind;
	if (!pItemData->m_bTradable)
	{
		pNewItem->m_bBind = true;
	}

	return pNewItem;
}

GItem* GItemManager::MakeNewItem(GItem* pItem, int64 nIUID, int nAmount)
{
	if (NULL == pItem) return NULL;
	if (0 >= nIUID) return NULL;

	GItem* pNewItem = MakeNewItem(pItem->GetID());
	if (NULL == pNewItem) return NULL;

	pNewItem->m_nIUID = nIUID;
	pNewItem->m_nAmount = nAmount;
	pNewItem->m_nDurability = pItem->m_nDurability;
	pNewItem->m_nDyedColor = pItem->m_nDyedColor;
	pNewItem->m_bBind = pItem->m_bBind;
	pNewItem->m_UsagePeriod = pItem->m_UsagePeriod;
	pNewItem->m_ExpirationPeriod = pItem->m_ExpirationPeriod;
	pNewItem->m_strExpiDt = pItem->m_strExpiDt;
	pNewItem->m_nSoulCount = pItem->m_nSoulCount;	

	for (int i=0; i<ENCHANT_MAX_COUNT; i++)
	{
		pNewItem->m_nEnchants[i] = pItem->m_nEnchants[i];
	}

	return pNewItem;
}

GItem* GItemManager::MakeNewItem(const TD_PLAYER_GAME_DATA_ITEM_INSTANCE& tdItem)
{
	GItem* pItem = gmgr.pItemManager->MakeNewItem(tdItem.nItemID, tdItem.nQuantity);		
	if (pItem == NULL)	return NULL;

	pItem->m_nIUID			= tdItem.nIUID;			
	pItem->m_nDurability	= tdItem.nDurability;
	pItem->m_bBind			= tdItem.bBind;
	pItem->m_nSoulCount		= tdItem.nSoulQuantity;
	pItem->m_nDyedColor		= tdItem.nDyedColor;
	pItem->m_nSlotID		= tdItem.nSlotID;
	pItem->m_strExpiDt		= tdItem.szExpiDt;
	pItem->m_nAmount		= tdItem.nQuantity;

	// 강화 정보
	for (int i=0; i<ENCHANT_MAX_COUNT; ++i)
	{
		pItem->m_nEnchants[i] = tdItem.nEnchants[i];
	}

	pItem->m_UsagePeriod.Set(tdItem.bUseUsagePeriod, 
		tdItem.bStartCalculatingUsagePeriod,
		tdItem.nUsagePeriodSec,
		tdItem.nCharPlayTimeSec);

	pItem->m_ExpirationPeriod.Set(tdItem.bUseExpirationTime,
		true,
		tdItem.nExpirationTimeSec,
		tdItem.nCharPlayTimeSec);

	return pItem;
}

bool GItemManager::ParseItem(MXmlElement* pElement, CSItemData* pItemData, MXml* pXml)
{
	GItemData* pGItemData = static_cast<GItemData*>(pItemData);

	CSItemManager::ParseItem(pElement, pGItemData, pXml);

	_VLE(_T(ITEM_XML_TAG_ITEM));	
	_VLA(_T(ITEM_XML_ATTR_ID), pGItemData->m_nID);
	_VLP;


	if (0 == pGItemData->m_nID)
		return false;

	InsertItemData(pGItemData);
	return true;	
}

MUID GItemManager::GetNewItemID()
{
	m_uidNowItemID.Increase();
	return m_uidNowItemID;
}


void GItemManager::Clear()
{
	for (GItemDataMap::iterator itor = m_ItemDataMap.begin(); itor != m_ItemDataMap.end(); ++itor)
	{
		delete (*itor).second;
	}

	m_ItemDataMap.clear();
}

GItemData* GItemManager::GetItemData(int id)
{
	GItemDataMap::iterator i = m_ItemDataMap.find(id);
	if (i != m_ItemDataMap.end())
	{
		return ((*i).second);
	}
	return NULL;
}

void GItemManager::InsertItemData( GItemData* pItemData )
{	
	if (NULL == pItemData)
	{
		_ASSERT(0);
		return;
	}
	
	GItemData* pLastItemData = GetItemData(pItemData->m_nID);
	if (NULL != pLastItemData)
	{
		_VLOGGER->Log(_T(ITEM_XML_ATTR_ID) IS_ALREADY_EXIST);
		return;
	}

	m_ItemDataMap.insert(GItemDataMap::value_type(pItemData->m_nID, pItemData));
}

bool GItemManager::IsExist(int nID)
{
	if (NULL == GetItemData(nID)) return false;

	return true;
}

void GItemManager::Cooking()
{
	for each (const GItemDataMap::value_type& data in m_ItemDataMap)
	{
		GItemData* pItemData = data.second;
		if (NULL == pItemData) continue;

		pItemData->Cooking();

		if (true == gmgr.pRecipeInfoMgr->IsMaterialItemID(pItemData->m_nID))
		{
			pItemData->m_bRecipeMaterial = true;
		}
	}
}

void GItemManager::InsertNotExistItemIDInXML(int nID)
{
	m_setNotExistItemIDInXML.insert(nID);
}

bool GItemManager::IsNotExistItemIDInXML( int nID )
{
	return m_setNotExistItemIDInXML.end() != m_setNotExistItemIDInXML.find(nID);
}
