#include "stdafx.h"
#include "XItem.h"
#include "CSItemManager.h"

XItem::XItem() : 
m_nSlotID(INVALID_ID),
m_pItemData(NULL), 
m_nAmount(1),						// 갯수는 기본으로 1개.
m_nDurability(0),
m_nDyedColor(-1),
m_bClaimed(false),
m_nSoulCount(0),
m_bNewItem(false),
m_nEXP(-1),
m_nNextAttuneXP(-1),
m_nAttuneLvl(-1)
{
	for (int i=0; i<ENCHANT_MAX_COUNT; ++i)
	{
		m_nEnchants[i] = ENCHANT_UNUSED_SLOT; 
	}
}

XItem::~XItem()
{
}

void XItem::Set(const MUID& id, XItemData* dat)
{	
	m_pItemData = dat;
	m_nDurability = m_pItemData->m_nMaxDurability;
	m_nDyedColor = m_pItemData->m_nTexColor;
}

void XItem::ApplyTDItem(const TD_ITEM& tdItem)
{
	m_nDurability	= tdItem.m_nDurability;
	m_nAmount		= tdItem.m_nQuantity;		
	m_nSlotID		= tdItem.m_nSlotID;
	m_bClaimed		= tdItem.m_bBind;
	m_nSoulCount	= tdItem.m_nSoulQuantity;
	m_nDyedColor	= tdItem.m_nDyedColor;
	m_nEXP			= tdItem.m_nXP;
	m_nNextAttuneXP	= tdItem.m_nNextAttuneXP;
	m_nAttuneLvl	= tdItem.m_nAttuneLevel;

	for (int i=0; i<ENCHANT_MAX_COUNT; ++i)
	{
		m_nEnchants[i] = tdItem.m_nEnchants[i]; 
	}	
}

int XItem::GetTalentID()
{
	if(	m_pItemData->m_nUsableType == USABLE_TALENT_USE &&		// 탤런트 사용으로 제한
		m_pItemData->m_vecUsableParam.size() > 0)
	{
		// 아이템에 탤런트가 있으면... 이 ID로 리턴 
		// 이렇게 해야 아이템 쿨타임과 Active가 작동된다.
		return m_pItemData->m_vecUsableParam.front();
	}

	return 0;
}

bool XItem::IsUsableItem()
{
	if (m_pItemData != NULL && m_pItemData->m_ItemType == ITEMTYPE_USABLE )
		return true;

	return false;
}

bool XItem::IsQuestRelated()
{
	if (m_pItemData != NULL)
		return m_pItemData->m_bQuestRelated;

	return false;
}

int XItem::GetID()
{
	if (NULL == m_pItemData) return INVALID_ID;

	return m_pItemData->m_nID;
}
