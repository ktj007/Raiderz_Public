#include "StdAfx.h"
#include "TTabItem.h"
#include "TItemData.h"

TTabItem::TTabItem(void)
{
	Init();
}

TTabItem::~TTabItem(void)
{
}

void TTabItem::Init()
{
	m_nSelSlot = -1;
}

//아이템을 슬롯 인덱스대로 리스트를 만들어야되나...
// TODO: 모델에서 종족을 고르면 종족에 맞는 아이템리스트만 출력하게
// 로드는 다해도 상관 없지 않나???
//종족별로 분류할 수 있게 수정해야할듯..
void TTabItem::InitItemMap()
{
	TItemManager* pItemMgr = g_pMainApp->GetItemMgr();

	if ( !pItemMgr->LoadFromFile(FILENAME_XITEM_TOOL) )
		assert(!"pItemMgr->LoadFromFile() - 아이템 테이블 불러오기 실패");

	if ( !pItemMgr->LoadFromFile_SubMtrl(FILENAME_ITEM_CHANGEMTRL_TOOL) )
		mlog("에러 : pItemMgr->LoadFromFile() - 아이템 재질 테이블 불러오기 실패");

	TItemManager::CSItemDataMap& dataMap = pItemMgr->GetDataMap();

	//none 빈 아이템 추가.
	for(int i=0;i<ITEMSLOT_MAX;i++)
		m_ItemMap[i].m_Index.push_back(NULL);

	for(TItemManager::CSItemDataMap::iterator itr = dataMap.begin();itr!=dataMap.end();itr++)
	{
		TItemData* data = (TItemData*)itr->second;
		int slot = (int)data->m_nItemSlot;
		m_ItemMap[slot].m_Index.push_back(data);

		// 왼쪽에도 장착이 되는가?
		if(data->m_bLeftWeaponEquipable)
		{
			m_ItemMap[ITEMSLOT_LWEAPON].m_Index.push_back(data);
		}
	}

	pItemMgr->Cooking();
}

SlotList* TTabItem::SelectItem( int nSlot )
{
	ITEMMAP::iterator it = m_ItemMap.find(nSlot);
	if (it != m_ItemMap.end())
	{
		m_nSelSlot = nSlot;
		m_SelSlotList = (*it).second;

		return &m_SelSlotList;
	}

	return NULL;
}

CSItemData* TTabItem::GetItemData( int index )
{
	return m_SelSlotList.m_Index[index];
}

CSItemData* TTabItem::GetItemDataByID(int nID)
{
	for( int i = 1; i < (int)m_SelSlotList.m_Index.size(); ++i )
	{
		if( m_SelSlotList.m_Index[i]->m_nID == nID )
		{
			return m_SelSlotList.m_Index[i];
		}
	}

	return NULL;
}

//void TTabItem::SelectItemEquip( int nSelSlot, int nID )
//{
//	SelectItem(nSelSlot);
//
//	CSItemData* pItemData = GetItemDataByID(nID);
//
//	g_pMainApp->GetActor()->EquipItem(*pItemData);
//}