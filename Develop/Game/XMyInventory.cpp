#include "stdafx.h"
#include "XMyInventory.h"
#include "XItemManager.h"
#include "XItemHelper.h"

XMyInventory::XMyInventory()
: XItemContainer(MAX_INVENTORY_SIZE)
, m_bExpended(false)
{
}


XMyInventory::~XMyInventory()
{
}

void XMyInventory::Use( int nSlotID, CHR_STATUS* chr_status, int nAmount )
{
	XItem* pItem = GetItem(nSlotID);
	if (NULL == pItem)
	{
		_ASSERT(0);
		return;
	}

	if (true == pItem->m_pItemData->m_bUsableConsume)		// 소모성 아이템이면
	{
		pItem->m_nAmount = nAmount;

		if(nAmount == 0)
		{
			PopItem(nSlotID);
			return;
		}
	}
}

bool XMyInventory::IsHaveGatherTool( GATHER_TYPE nType )
{
	for (int i=0; i<GetSlotSize(); i++)
	{
		if(m_vecItem[i])  
		{
			if(m_vecItem[i]->m_pItemData->m_nGatherType == nType)
				return true;
		}
	}

	return false;
}

XItem* XMyInventory::GetUsableSelfRebirthItem()
{
	for (int i=0; i<GetSlotSize(); i++)
	{
		XItem* pItem = m_vecItem[i];
		if (NULL == pItem) continue;

		if (false == XItemHelper::IsUsableSelfRebirthItem(pItem->m_pItemData->m_nID)) continue;

		return pItem;
	}

	return NULL;
}

XItem* XMyInventory::GetRandomItem()
{
	vector<XItem*> vecItem = GetAllItem();
	if (vecItem.empty()) return NULL;

	int nIndex = RandomNumber(0, vecItem.size()-1);
	XItem* pItem = vecItem[nIndex];	

	return pItem;
}

XItem* XMyInventory::GetRandomSellableItem()
{
	vector<XItem*> vecItem;

	for (int i=0; i<GetSlotSize(); i++)
	{
		XItem* pItem = m_vecItem[i];
		if (NULL == pItem) continue;
		if (false == pItem->m_pItemData->m_bSellable) continue;
		
		vecItem.push_back(pItem);	
	}

	return XMath::RandomPickUp(vecItem) ;
}


vector<XItem*> XMyInventory::GetSortedAllItem()
{
	set<XItem*> setItem = VectorToSet(GetAllItem());
	vector<XItem*> vecEquipItem = SelectEquipItemForSort(setItem);
	vector<XItem*> vecUsableItem = SelectUsableItemForSort(setItem);
	vector<XItem*> vecQuestItem = SelectQuestItemForSort(setItem);
	vector<XItem*> vecMaterialItem = SelectMaterialItemForSort(setItem);
	vector<XItem*> vecEtcItem = SelectEtcItemForSort(setItem);

	vector<XItem*> vecSortedItem;
	vecSortedItem.insert(vecSortedItem.end(), vecEquipItem.begin(), vecEquipItem.end());
	vecSortedItem.insert(vecSortedItem.end(), vecUsableItem.begin(), vecUsableItem.end());
	vecSortedItem.insert(vecSortedItem.end(), vecQuestItem.begin(), vecQuestItem.end());
	vecSortedItem.insert(vecSortedItem.end(), vecMaterialItem.begin(), vecMaterialItem.end());
	vecSortedItem.insert(vecSortedItem.end(), vecEtcItem.begin(), vecEtcItem.end());

	return vecSortedItem;
}

vector<XItem*> XMyInventory::SelectEquipItemForSort(set<XItem*>& setItem)
{
	map<int, vector<XItem*>> mapSelectItem;
	for each (XItem* pItem in setItem)
	{
		if (ITEMTYPE_WEAPON != pItem->m_pItemData->m_ItemType &&
			ITEMTYPE_ARMOR != pItem->m_pItemData->m_ItemType) continue;

		MapSecondVectorInserter(mapSelectItem, pItem->m_pItemData->m_nID, pItem);
	}

	vector<XItem*> vecSelectItem;
	for each (const map<int, vector<XItem*>>::value_type& data in mapSelectItem)
	{
		const vector<XItem*> vecItem = data.second;
		for each(XItem* pItem in vecItem)
		{
			setItem.erase(pItem);
			vecSelectItem.push_back(pItem);
		}		
	}

	return vecSelectItem;
}

vector<XItem*> XMyInventory::SelectUsableItemForSort( set<XItem*>& setItem )
{
	map<int, vector<XItem*>> mapSelectItem;	
	for each (XItem* pItem in setItem)
	{
		if (ITEMTYPE_USABLE != pItem->m_pItemData->m_ItemType) continue;

		MapSecondVectorInserter(mapSelectItem, pItem->m_pItemData->m_nID, pItem);
	}

	vector<XItem*> vecSelectItem;
	for each (const map<int, vector<XItem*>>::value_type& data in mapSelectItem)
	{
		const vector<XItem*> vecItem = data.second;
		for each(XItem* pItem in vecItem)
		{
			setItem.erase(pItem);
			vecSelectItem.push_back(pItem);
		}		
	}

	return vecSelectItem;
}

vector<XItem*> XMyInventory::SelectQuestItemForSort( set<XItem*>& setItem )
{
	map<int, vector<XItem*>> mapSelectItem;	
	for each (XItem* pItem in setItem)
	{
		if (false == pItem->m_pItemData->m_bQuestRelated) continue;

		MapSecondVectorInserter(mapSelectItem, pItem->m_pItemData->m_nID, pItem);
	}

	vector<XItem*> vecSelectItem;
	for each (const map<int, vector<XItem*>>::value_type& data in mapSelectItem)
	{
		const vector<XItem*> vecItem = data.second;
		for each(XItem* pItem in vecItem)
		{
			setItem.erase(pItem);
			vecSelectItem.push_back(pItem);
		}		
	}

	return vecSelectItem;
}

vector<XItem*> XMyInventory::SelectMaterialItemForSort( set<XItem*>& setItem )
{
	map<int, vector<XItem*>> mapSelectItem;	
	for each (XItem* pItem in setItem)
	{
		if (false == pItem->m_pItemData->m_bRecipeMaterial) continue;

		MapSecondVectorInserter(mapSelectItem, pItem->m_pItemData->m_nID, pItem);
	}

	vector<XItem*> vecSelectItem;
	for each (const map<int, vector<XItem*>>::value_type& data in mapSelectItem)
	{
		const vector<XItem*> vecItem = data.second;
		for each(XItem* pItem in vecItem)
		{
			setItem.erase(pItem);
			vecSelectItem.push_back(pItem);
		}		
	}

	return vecSelectItem;
}

vector<XItem*> XMyInventory::SelectEtcItemForSort( set<XItem*>& setItem )
{
	map<int, vector<XItem*>> mapSelectItem;	
	for each (XItem* pItem in setItem)
	{
		MapSecondVectorInserter(mapSelectItem, pItem->m_pItemData->m_nID, pItem);
	}

	vector<XItem*> vecSelectItem;
	for each (const map<int, vector<XItem*>>::value_type& data in mapSelectItem)
	{
		const vector<XItem*> vecItem = data.second;
		for each(XItem* pItem in vecItem)
		{
			setItem.erase(pItem);
			vecSelectItem.push_back(pItem);
		}		
	}

	return vecSelectItem;
}

void XMyInventory::SortSlot()
{
	vector<XItem*> vecSortedAllItem = GetSortedAllItem();
	Clear();

	int nSlotID=0;
	for each (XItem* pItem in vecSortedAllItem)
	{		
		PushItem(nSlotID, pItem);
		nSlotID++;
	}
}
