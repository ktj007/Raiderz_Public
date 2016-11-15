#include "stdafx.h"
#include "XItemManager.h"
#include "XRecipeInfoMgr.h"

XItemManager::XItemManager() : 
CSItemManager()
{
}

XItemManager::~XItemManager()
{
	Clear();
}

CSItemData* XItemManager::NewItemData()
{
	return new XItemData();
}

XItem* XItemManager::MakeNewItem(int id)
{
	XItemData* pItemData = GetItemData(id);
	if (pItemData == NULL) return NULL;

	XItem* ret = new XItem();
	if (ret == NULL)
	{
		_ASSERT(0 && "new alloc failed!");
		return NULL;
	}
	
	ret->Set(MUID::ZERO, pItemData);
	ret->m_bClaimed = !pItemData->m_bTradable;

	m_ItemVec.push_back(ret);

	
	return ret;
}

XItem* XItemManager::MakeNewItem( XItem* pItem, int nAmount )
{
	XItem* pNewItem = MakeNewItem(pItem->GetID());
	if (NULL == pNewItem) return NULL;

	pNewItem->m_nAmount = nAmount;
	pNewItem->m_nDurability = pItem->m_nDurability;
	pNewItem->m_nDyedColor = pItem->m_nDyedColor;
	pNewItem->m_bClaimed = pItem->m_bClaimed;
	pNewItem->m_nSoulCount = pItem->m_nSoulCount;
	pNewItem->m_bNewItem = pItem->m_bNewItem;
	//SoulHunterZ
	pNewItem->m_nEXP = pItem->m_nEXP;
	pNewItem->m_nNextAttuneXP = pItem->m_nNextAttuneXP;
	pNewItem->m_nAttuneLvl = pItem->m_nAttuneLvl;

	for (int i=0; i<ENCHANT_MAX_COUNT; i++)
	{
		pNewItem->m_nEnchants[i] = pItem->m_nEnchants[i];
	}

	return pNewItem;
}

void XItemManager::Clear()
{
	for (XItemDataMap::iterator itor = m_ItemDataMap.begin(); itor != m_ItemDataMap.end(); ++itor)
	{
		SAFE_DELETE( (*itor).second );
	}
	m_ItemDataMap.clear();

	for(XItemVec::iterator itItem = m_ItemVec.begin(); itItem != m_ItemVec.end(); ++itItem)
	{
		SAFE_DELETE((*itItem));
	}
	m_ItemVec.clear();
}

XItemData* XItemManager::GetItemData(int id)
{
	XItemDataMap::iterator i = m_ItemDataMap.find(id);
	if (i != m_ItemDataMap.end())
	{
		return (XItemData*)((*i).second);
	}
	return NULL;
}

bool XItemManager::IsItemID(int id)
{
	if (NULL == GetItemData(id)) return false;

	return true;
}

bool XItemManager::ParseItem(MXmlElement* pElement, CSItemData* pItemData, MXml* pXml)
{
	CSItemManager::ParseItem(pElement, pItemData, pXml);

	XItemInfoLoader itemInfoLoader;

	return itemInfoLoader.ParseItem(pElement, pItemData, pXml, m_ItemDataMap);
}

void XItemManager::Cooking()
{
	for each (const XItemDataMap::value_type& data in m_ItemDataMap)
	{
		CSItemData* pItemData = data.second;
		if (NULL == pItemData) continue;

		pItemData->Cooking();

		if (true == info.recipe->IsMaterialItemID(pItemData->m_nID))
		{
			pItemData->m_bRecipeMaterial = true;
		}
	}
}

int XItemManager::GetEnchantItemBuffID( XItem* pItem, int nEnchantSlot )
{
	if(pItem == NULL)
		return 0;

	XItemData* pEnchantStone = GetItemData(pItem->m_nEnchants[nEnchantSlot]);
	if(pEnchantStone == NULL)
		return pItem->m_nEnchants[nEnchantSlot];

	return pEnchantStone->m_EnchantStone.nLinkedBuffID;
}

void XItemManager::DelItem( XItem* pItem )
{
	for(XItemVec::iterator itItem = m_ItemVec.begin(); itItem != m_ItemVec.end(); ++itItem)
	{
		if(*itItem == pItem)
		{
			SAFE_DELETE((*itItem));
			m_ItemVec.erase(itItem);
			return;
		}
	}
}
