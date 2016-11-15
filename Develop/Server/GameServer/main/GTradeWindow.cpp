#include "stdafx.h"
#include "GTradeWindow.h"
#include "CTransData.h"
#include "GEntityPlayer.h"
#include "GField.h"
#include "GItemManager.h"

GTradeWindow::GTradeWindow()
: m_uidOtherTrader(MUID::ZERO)
, m_bMyConfirm(false)
, m_pOwner(NULL)
, m_pOtherItemContainer(NULL)
, m_pOtherConfirm(NULL)
{	
}

GTradeWindow::~GTradeWindow()
{
	while (0 < m_myItemContainer.GetSize())
	{
		GItem* pItem = m_myItemContainer.FirstItem();
		m_myItemContainer.PopItem(pItem);
		delete pItem;
	}

	m_bMyConfirm = false;	
	m_pOtherItemContainer = NULL;
	m_pOtherConfirm = NULL;	


	GField* pOwnerPlayerField = m_pOwner->GetField();
	if (NULL == pOwnerPlayerField) return;

	MUID uidOtherTrader = m_pOwner->GetOtherTrader();
	GEntityPlayer* pEntityOtherTrader = pOwnerPlayerField->FindPlayer(uidOtherTrader);
	m_uidOtherTrader = MUID::ZERO;
	if (NULL == pEntityOtherTrader) return;

	pEntityOtherTrader->SetOtherTrader(MUID::ZERO);
	pEntityOtherTrader->TradeReset();

}

void GTradeWindow::Init(GEntityPlayer* pOwnerPlayer)
{	
	_ASSERT(NULL != pOwnerPlayer && "GTradingWindow::GTradingWindow - pOwnerPlayer is NULL");
	if (NULL == pOwnerPlayer) return;

	m_pOwner = pOwnerPlayer;
	m_myItemContainer.Init(m_pOwner->GetUID());
}

bool GTradeWindow::Start(GEntityPlayer* pReqPlayer)
{
	_ASSERT(NULL != pReqPlayer && "GTradeWindow::Start - pReqPlayer is NULL");
	if (NULL == pReqPlayer) return false;

	MUID uidReqPlayer = pReqPlayer->GetUID();
	MUID uidOwner = m_pOwner->GetUID();

	// 자기 자신과 거래 하려는지
	if (uidReqPlayer == uidOwner) return false;

	// 이미 같은 필드에서 거래중인지
	if (true == m_pOwner->IsValidTrade()) return false;
	if (true == pReqPlayer->IsValidTrade()) return false;

	// 같은 필드에 없는지
	if (false == m_pOwner->IsSameField(pReqPlayer)) return false;
	
	GItemContainer* pItemContainer = NULL;
	bool* pConfirm = NULL;
	pReqPlayer->GetTradeMyItemContainerAndConfirm(pItemContainer, pConfirm);
	_ASSERT(NULL != pItemContainer && "GTradeWindow::Start - pItemContainer is NULL");
	if (NULL == pItemContainer) return false;
	_ASSERT(NULL != pConfirm && "GTradeWindow::Start - pConfirm is NULL");
	if (NULL == pConfirm) return false;

	SetOtherItemContainerAndConfirm(pItemContainer, pConfirm);
	pReqPlayer->SetTradeOtherItemContainerAndConfirm(&m_myItemContainer, &m_bMyConfirm);

	SetOtherTrader(uidReqPlayer);
	pReqPlayer->SetOtherTrader(uidOwner);

	return true;
}

bool GTradeWindow::PutUpItem(TD_ITEM* pItem)
{
	_ASSERT(NULL != pItem && "GTradeWindow::PutUpItem - pItem is NULL");
	if (NULL == pItem) return false;

	// 이미 확인을 누른 플레이어는 아이템을 옮길수 없다.
	if (true == m_bMyConfirm) return false;

	// 같은 필드에 없거나, 거래 중이 아닌지
	if (false == m_pOwner->IsValidTrade())
	{
		Reset();
		return false;
	}

	GItem* pInvenItem = m_pOwner->GetItemInventory().GetItem(pItem->m_uidID);
	if (NULL == pInvenItem) return false;

	GItem* pTradingItem = m_myItemContainer.GetItem(pItem->m_uidID);
	if (NULL == pTradingItem)
	{
		if (pItem->m_nQuantity > pInvenItem->m_nQuantity) return false;

		GItem* pExistTradingItem = GetServer()->GetItemMgr()->MakeExistItem(pItem->m_nItemID, pItem->m_uidID);
		pExistTradingItem->m_nQuantity = pItem->m_nQuantity;
		pExistTradingItem->m_fDurability = pItem->m_fDurability;
		m_myItemContainer.PushItem(pExistTradingItem);		
	}
	else
	{
		if (pItem->m_nQuantity > pInvenItem->m_nQuantity - pTradingItem->m_nQuantity) return false;
		pTradingItem->m_nQuantity += pItem->m_nQuantity;
	}

	_ASSERT(NULL != m_pOtherConfirm && "GTradeWindow::PutDownItem - m_pOtherConfirm is NULL");
	if (NULL == m_pOtherConfirm) return false;

	(*m_pOtherConfirm) = false;

	return true;
}

bool GTradeWindow::PutDownItem(TD_ITEM* pItem)
{
	_ASSERT(NULL != pItem && "GTradeWindow::PutDownItem - pItem is NULL");
	if (NULL == pItem) return false;

	// 이미 확인을 누른 플레이어는 아이템을 옮길수 없다.
	if (true == m_bMyConfirm) return false;

	// 같은 필드에 없거나, 거래 중이 아닌지
	if (false == m_pOwner->IsValidTrade())
	{
		Reset();
		return false;
	}

	GItem* pTradingtem = m_myItemContainer.GetItem(pItem->m_uidID);
	if (NULL == pTradingtem) return false;

	// 거래창에 있는것보다 더 많이 내릴수 없다.
	if (pItem->m_nQuantity > pTradingtem->m_nQuantity) return false;

	pTradingtem->m_nQuantity -= pItem->m_nQuantity;
	if (0 == pTradingtem->m_nQuantity)
	{
		m_myItemContainer.PopItem(pTradingtem);
	}

	_ASSERT(NULL != m_pOtherConfirm && "GTradeWindow::PutDownItem - m_pOtherConfirm is NULL");
	if (NULL == m_pOtherConfirm) return false;

	(*m_pOtherConfirm) = false;

	return true;
}

bool GTradeWindow::Confirm(TD_ITEM* pGiveItems, int nGiveItemCount, TD_ITEM* pTakeItems, int nTakeItemCount)
{
	_ASSERT(0 >= nGiveItemCount || NULL != pGiveItems && "GTradeWindow::Confirm - pGiveItems is NULL");
	if (0 < nGiveItemCount && NULL == pGiveItems) return false;
	_ASSERT(0 >= nTakeItemCount || NULL != pTakeItems && "GTradeWindow::Confirm - pTakeItems is NULL");
	if (0 < nTakeItemCount && NULL == pTakeItems) return false;

	// 이미 확인을 눌렀는지 확인
	if (true == m_bMyConfirm) return false;

	// 같은 필드에 없거나, 거래 중이 아닌지
	if (false == m_pOwner->IsValidTrade())
	{
		Reset();
		return false;
	}

	int nGiveTradingItemCount = static_cast<int>(m_myItemContainer.GetSize());
	if (nGiveTradingItemCount !=  nGiveItemCount) return false;

	_ASSERT(NULL != m_pOtherItemContainer && "GTradeWindow::Confirm - m_pOtherItemContainer is NULL");
	if (NULL == m_pOtherItemContainer) return false;

	int nTakeTradingItemCount = static_cast<int>(m_pOtherItemContainer->GetSize());
	if (nTakeTradingItemCount !=  nTakeItemCount) return false;

	for (int i = 0; nGiveItemCount > i; i++)
	{
		GItem* pReqTradingItem = m_myItemContainer.GetItem(pGiveItems[i].m_uidID);
		if (NULL == pReqTradingItem) return false;
		if (pGiveItems[i].m_nItemID != pReqTradingItem->m_pItemData->m_nID) return false;
		if (pGiveItems[i].m_uidID != pReqTradingItem->m_uidID) return false;
		if (pGiveItems[i].m_nQuantity != pReqTradingItem->m_nQuantity) return false;
		if (pGiveItems[i].m_fDurability != pReqTradingItem->m_fDurability) return false;
	}	

	for (int i = 0; nTakeTradingItemCount > i; i++)
	{
		_ASSERT(NULL != m_pOtherItemContainer && "GTradeWindow::Confirm - m_pOtherItemContainer is NULL");
		if (NULL == m_pOtherItemContainer) return false;

		GItem* pOtherTradingItem = m_pOtherItemContainer->GetItem(pTakeItems[i].m_uidID);
		if (NULL == pOtherTradingItem) return false;
		if (pTakeItems[i].m_nItemID != pOtherTradingItem->m_pItemData->m_nID) return false;
		if (pTakeItems[i].m_uidID != pOtherTradingItem->m_uidID) return false;
		if (pTakeItems[i].m_nQuantity != pOtherTradingItem->m_nQuantity) return false;
		if (pTakeItems[i].m_fDurability != pOtherTradingItem->m_fDurability) return false;
	}

	m_bMyConfirm = true;

	return true;
}

bool GTradeWindow::Reset()
{
	while (0 < m_myItemContainer.GetSize())
	{
		GItem* pItem = m_myItemContainer.FirstItem();
		m_myItemContainer.PopItem(pItem);
		delete pItem;
	}

	m_bMyConfirm = false;	
	m_pOtherItemContainer = NULL;
	m_pOtherConfirm = NULL;	
	m_uidOtherTrader = MUID::ZERO;

	return true;
}

void GTradeWindow::SetOtherItemContainerAndConfirm(GItemContainer* pItemContainer, bool* pConfirm)
{
	m_pOtherItemContainer = pItemContainer;
	m_pOtherConfirm = pConfirm;
}

void GTradeWindow::GetMyItemContainerAndConfirm(GItemContainer*& pOutItemContainer, bool*& pOutConfirm)
{
	pOutItemContainer = &m_myItemContainer;
	pOutConfirm = &m_bMyConfirm;
}

bool GTradeWindow::IsValidTrade()
{
	GField* pOwnerField = m_pOwner->GetField();
	_ASSERT(NULL != pOwnerField && "GTradingWindow::IsTrading pOwnerField is NULL");
	if (NULL == pOwnerField) return false;

	GEntityPlayer* pOtherTrader = pOwnerField->FindPlayer(m_uidOtherTrader);
	if (NULL == pOtherTrader) return false;

	if (m_pOwner->GetUID() != pOtherTrader->GetOtherTrader()) return false;

	return true;
}

bool GTradeWindow::IsTradeConclusion()
{
	_ASSERT(NULL != m_pOtherConfirm && "GTradeWindow::IsTradeConclusion - m_pOtherConfirm is NULL");
	if (NULL == m_pOtherConfirm) return false;
	
	if (false ==  m_bMyConfirm || false == (*m_pOtherConfirm)) return false;

	return true;
}

void GTradeWindow::MakeTD_Item(TD_ITEM* pItems)
{
	int i = 0;
	for(GItem* item = m_myItemContainer.FirstItem();
		item != NULL;
		item = m_myItemContainer.NextItem())
	{
		pItems[i].m_fDurability = item->m_fDurability;
		pItems[i].m_nQuantity = item->m_nQuantity;
		pItems[i].m_uidID = item->m_uidID;
		pItems[i].m_nItemID = item->m_pItemData->m_nID;

		i++;
	}
}
