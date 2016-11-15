#include "StdAfx.h"
#include "XItem_RecipeProductInfoMgr.h"
#include "XItemData.h"
#include "XItemInfoLoader.h"
#include "XItemManager.h"
#include "XRecipeInfoMgr.h"
#include "CSRecipeInfo.h"
#include "XConditionsInfoMgr.h"

XItem_RecipeProductInfoMgr::XItem_RecipeProductInfoMgr(void)
{
}

XItem_RecipeProductInfoMgr::~XItem_RecipeProductInfoMgr(void)
{
}

void XItem_RecipeProductInfoMgr::Clear()
{
	m_mapInfo.clear();
}

void XItem_RecipeProductInfoMgr::Load( XItemManager* pItemManager, XRecipeInfoMgr* pRecipeInfoMgr )
{
	XItemDataMap& itemDataMap = pItemManager->GetItemDataMap();
	MAP_CSRECIPEINFO& recipeDataMap = pRecipeInfoMgr->GetContainer();

	for each (const XItemDataMap::value_type& data in itemDataMap)
	{
		XItemData* pItemData = data.second;

		for each (const MAP_CSRECIPEINFO::value_type& data in recipeDataMap)
		{
			CSRecipeInfo* pRecipeInfo = data.second;
			if (false == IsExist(pRecipeInfo->m_setMaterialItemID, pItemData->m_nID)) continue;

			MapSecondSetInserter(m_mapInfo, pItemData->m_nID, pRecipeInfo->m_nProductItemID);
		}		
	}
}

int XItem_RecipeProductInfoMgr::GetProductCount( int nMaterialItemID )
{
	return Get(nMaterialItemID).size();
}

int XItem_RecipeProductInfoMgr::GetProductItemID( int nMaterialItemID, int i )
{
	const set<int>& setProductItemtID = Get(nMaterialItemID);
	int nSize = static_cast<int>(setProductItemtID.size()); 
	if (0 == nSize) return 0;
	if (i >= nSize) return 0;

	vector<int> vecProductItemID = SetToVector(setProductItemtID);

	return vecProductItemID[i];
}

int XItem_RecipeProductInfoMgr::GetProductMinLevel( int nMaterialItemID )
{
	const set<int>& setProductItemtID = Get(nMaterialItemID);

	int nMinLevel = INT_MAX;
	for each (int nProductItemID in setProductItemtID)
	{
		XItemData* pItemData = info.item->GetItemData(nProductItemID);
		if (NULL == pItemData) continue;

		int nLevel = info.cond_item->GetMinLevel_For_CT_LEVEL(pItemData->m_nConditionsInfoID);
		
		if (nMinLevel > nLevel)
		{
			nMinLevel = nLevel;
		}
	}

	return nMinLevel;
}

int XItem_RecipeProductInfoMgr::GetProductMaxLevel( int nMaterialItemID )
{
	const set<int>& setProductItemtID = Get(nMaterialItemID);

	int nMaxLevel = 0;
	for each (int nProductItemID in setProductItemtID)
	{
		XItemData* pItemData = info.item->GetItemData(nProductItemID);
		if (NULL == pItemData) continue;

		int nLevel = info.cond_item->GetMinLevel_For_CT_LEVEL(pItemData->m_nConditionsInfoID);

		if (nMaxLevel < nLevel)
		{
			nMaxLevel = nLevel;
		}
	}

	return nMaxLevel;
}

bool XItem_RecipeProductInfoMgr::IsWeaponProductMaterial( int nMaterialItemID )
{
	const set<int>& setProductItemtID = Get(nMaterialItemID);

	for each (int nProductItemID in setProductItemtID)
	{
		XItemData* pItemData = info.item->GetItemData(nProductItemID);
		if (NULL == pItemData) continue;

		if (WEAPON_NONE != pItemData->m_nWeaponType)
		{
			return true;
		}
	}

	return false;
}

bool XItem_RecipeProductInfoMgr::IsShieldProductMaterial( int nMaterialItemID )
{
	const set<int>& setProductItemtID = Get(nMaterialItemID);

	for each (int nProductItemID in setProductItemtID)
	{
		XItemData* pItemData = info.item->GetItemData(nProductItemID);
		if (NULL == pItemData) continue;

		if (ARMOR_SHIELD == pItemData->m_nArmorType)
		{
			return true;
		}
	}

	return false;
}

bool XItem_RecipeProductInfoMgr::IsAccessoryProductMaterial( int nMaterialItemID )
{
	const set<int>& setProductItemtID = Get(nMaterialItemID);

	for each (int nProductItemID in setProductItemtID)
	{
		XItemData* pItemData = info.item->GetItemData(nProductItemID);
		if (NULL == pItemData) continue;

		if (ARMOR_ACCESSORY == pItemData->m_nArmorType)
		{
			return true;
		}
	}

	return false;
}

bool XItem_RecipeProductInfoMgr::IsArmorProductMaterial( int nMaterialItemID )
{
	const set<int>& setProductItemtID = Get(nMaterialItemID);

	for each (int nProductItemID in setProductItemtID)
	{
		XItemData* pItemData = info.item->GetItemData(nProductItemID);
		if (NULL == pItemData) continue;

		if (ARMOR_SHIELD != pItemData->m_nArmorType && ARMOR_NONE != pItemData->m_nArmorType)
		{
			return true;
		}
	}

	return false;
}

bool XItem_RecipeProductInfoMgr::IsUsableProductMaterial( int nMaterialItemID )
{
	const set<int>& setProductItemtID = Get(nMaterialItemID);

	for each (int nProductItemID in setProductItemtID)
	{
		XItemData* pItemData = info.item->GetItemData(nProductItemID);
		if (NULL == pItemData) continue;

		if (ITEMTYPE_USABLE == pItemData->m_ItemType)
		{
			return true;
		}
	}

	return false;
}

bool XItem_RecipeProductInfoMgr::IsHousingProductMaterial( int nMaterialItemID )
{
	const set<int>& setProductItemtID = Get(nMaterialItemID);

	for each (int nProductItemID in setProductItemtID)
	{
		XItemData* pItemData = info.item->GetItemData(nProductItemID);
		if (NULL == pItemData) continue;

		if (ITEMTYPE_HOUSING == pItemData->m_ItemType)
		{
			return true;
		}
	}

	return false;

}

bool XItem_RecipeProductInfoMgr::IsEnchantProductMaterial( int nMaterialItemID )
{
	const set<int>& setProductItemtID = Get(nMaterialItemID);	

	return false;
}

const set<int>& XItem_RecipeProductInfoMgr::Get( int nMaterialItemID )
{
	MAP_ITEMRECIPEPRODUCTINFO::iterator it = m_mapInfo.find(nMaterialItemID);
	if (m_mapInfo.end() == it) return m_dummuySet;

	return it->second;
}