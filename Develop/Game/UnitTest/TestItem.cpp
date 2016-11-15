#include "stdafx.h"
#include "XItemContainer.h"
#include "XEquipmentSlot.h"
#include "CTransData.h"
#include "XItemManager.h"
#include "XStrings.h"
#include "XUnitTestList.h"

SUITE(Item)
{
	TEST(TestItemDataCook_NodeNames)
	{
		// 장비 아이템인 경우 Node 이름에 '_item' 을 붙인 Node를 Cook할 때 자동적으로 추가해준다.
		{
			CSItemData itemData;
			itemData.m_strNodeNames.push_back(wstring(L"hat"));
			itemData.Cooking();

			CHECK_EQUAL(2, itemData.m_strNodeNames.size());
			CHECK(wstring(L"hat") == itemData.m_strNodeNames[0]);
			CHECK(wstring(L"hat_item") == itemData.m_strNodeNames[1]);
		}

		// 대소문자 구분없음
		{
			CSItemData itemData;
			itemData.m_strNodeNames.push_back(wstring(L"Chest"));		
			itemData.Cooking();

			CHECK_EQUAL(2, itemData.m_strNodeNames.size());
			CHECK(wstring(L"Chest") == itemData.m_strNodeNames[0]);
			CHECK(wstring(L"Chest_item") == itemData.m_strNodeNames[1]);
		}


		// 이미 _item 이 있으면 추가하지 않음
		{
			CSItemData itemData;
			itemData.m_strNodeNames.push_back(wstring(L"feet_item"));
			itemData.m_strNodeNames.push_back(wstring(L"feet"));
			
			itemData.Cooking();

			CHECK_EQUAL(2, itemData.m_strNodeNames.size());
			CHECK(wstring(L"feet_item") == itemData.m_strNodeNames[0]);
			CHECK(wstring(L"feet") == itemData.m_strNodeNames[1]);
		}

	}
}
