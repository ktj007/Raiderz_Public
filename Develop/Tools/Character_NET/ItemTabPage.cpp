#include "StdAfx.h"
#include "ItemTabPage.h"
#include "TItemData.h"

//아이템을 슬롯 인덱스대로 리스트를 만들어야되나...
// TODO: 모델에서 종족을 고르면 종족에 맞는 아이템리스트만 출력하게
// 로드는 다해도 상관 없지 않나???
//종족별로 분류할 수 있게 수정해야할듯..
void Character_NET::ItemTabPage::InitItemList()
{
	ItemtreeView->Nodes->Clear();

	for( int i = 0;i < ITEMSLOT_MAX; ++i)
	{
		String^ str = gcnew String(CSItemSlotStr[i]);
		TreeNode^ node = ItemtreeView->Nodes->Add(str);

		SlotList* pSlotList = g_pMainApp->m_TabItem.SelectItem(i);
		if (pSlotList)
		{
			int size = pSlotList->m_Index.size();
			for(int j = 0; j < size; j++)
			{
				if (j == 0)
				{
					TreeNode^ tagnode0 = node->Nodes->Add("None");
					tagnode0->Tag = nullptr;
					continue;
				}

				String^ Name = gcnew String(pSlotList->m_Index[j]->m_strName.c_str());
				Name = Name  + "(" + pSlotList->m_Index[j]->m_nID + ")";
				TreeNode^ tagnode = node->Nodes->Add(Name);
				tagnode->Tag = pSlotList->m_Index[j]->m_nID; //아이템 아이디추가
			}
		}
	}
}
