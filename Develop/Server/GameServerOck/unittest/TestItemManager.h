#ifndef _TEST_ITEM_MANAGER_H_
#define _TEST_ITEM_MANAGER_H_

#include "GItemManager.h"
#include "GItemData.h"

/// 테스트 목적으로 GItemManager를 확장시킴
class TestItemManager : public GItemManager 
{	
public:
	TestItemManager() : GItemManager() {}

	virtual ~TestItemManager() {}

	virtual GItemData*		NewItemData()
	{
		return GItemManager::NewItemData();
	}

	virtual void InsertItemData(GItemData* pItemData)
	{
		GItemManager::InsertItemData(pItemData);
	}

	GItem* MakeInvalidIDItem()
	{
		return new GItem();
	}
};

#endif //_MOCK_ITEM_MANAGER_H_