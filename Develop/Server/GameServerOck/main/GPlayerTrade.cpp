#include "stdafx.h"
#include "GPlayerTrade.h"
#include "CTransData.h"
#include "GEntityPlayer.h"
#include "GPlayerDoing.h"
#include "GField.h"
#include "GItem.h"
#include "AStlUtil.h"

GTradeItem::GTradeItem(int nSlotID, int nAmount, int nID)
{	
	m_nSlotID = nSlotID;
	m_nAmount = nAmount;
	m_nID = nID;
}

GTradeItem::~GTradeItem()
{
}

void GTradeItem::Increse( int nAmount )
{
	m_nAmount += nAmount;
}

void GTradeItem::Decrease( int nAmount )
{
	m_nAmount -= nAmount;
}

int GTradeItem::GetSlotID()
{
	return m_nSlotID;
}

int GTradeItem::GetAmount()
{
	return m_nAmount;
}

int GTradeItem::GetID()
{
	return m_nID;
}

GPlayerTrade::GPlayerTrade(GEntityPlayer* pOwner)
: m_pOwner(pOwner)
, m_bConfirm(false)
, m_nMoney(0)
, m_uidTrader(MUID::ZERO)
{

}

GPlayerTrade::~GPlayerTrade()
{
	Reset();
}

void GPlayerTrade::Reset()
{
	SAFE_DELETE_MAP(m_mapTradeItem);

	m_bConfirm = false;	
	m_nMoney = 0;
	m_uidTrader = MUID::ZERO;
	m_nTradeRequesterUID = MUID::ZERO;
}

void GPlayerTrade::TradeRequested(MUID nRequesterUID)
{
	m_nTradeRequesterUID = nRequesterUID;
}

void GPlayerTrade::TradeStart(MUID nTraderUID)
{
	Reset();
	m_uidTrader = nTraderUID;
}

void GPlayerTrade::TradeReject()
{
	m_nTradeRequesterUID = MUID::ZERO;
}

void GPlayerTrade::AddItem(int nSlotID, int nAmount, int nID)
{
	GTradeItem* pTradeItem = GetItem(nSlotID);
	if (NULL == pTradeItem)
	{
		pTradeItem = new GTradeItem(nSlotID, nAmount, nID);
		m_mapTradeItem.insert(map<int, GTradeItem*>::value_type(nSlotID, pTradeItem));
	}
	else
	{
		pTradeItem->Increse(nAmount);
	}
}

void GPlayerTrade::RemoveItem(int nSlotID, int nAmount)
{
	GTradeItem* pTradeItem = GetItem(nSlotID);
	if (NULL == pTradeItem) return;

	if (nAmount == pTradeItem->GetAmount())
	{
		m_mapTradeItem.erase(nSlotID);
		SAFE_DELETE(pTradeItem);

	}
	else
	{
		pTradeItem->Decrease(nAmount);
	}
}

void GPlayerTrade::SetMoney( int nMoney )
{
	m_nMoney = nMoney;
}

void GPlayerTrade::Confirm()
{
	m_bConfirm = true;
}

void GPlayerTrade::Unconfirm()
{
	m_bConfirm = false;
}

bool GPlayerTrade::IsTradeRequester(MUID nPlayerUID)
{
	return m_nTradeRequesterUID == nPlayerUID;
}

bool GPlayerTrade::IsTrader( MUID nPlayerUID )
{
	return nPlayerUID == m_uidTrader;
}

bool GPlayerTrade::IsExistItemByID(int nItemID)
{
	for each (const map<int, GTradeItem*>::value_type& val in m_mapTradeItem)
	{
		GTradeItem* pTradeItem = val.second;
		if (nItemID == pTradeItem->GetID()) return true;
	}

	return false;
}

bool GPlayerTrade::IsExistItemBySlotID( int nSlotID )
{
	return NULL != GetItem(nSlotID);
}

bool GPlayerTrade::IsItemFull()
{
	return MAX_TRADE_ITEM_SPACE <= m_mapTradeItem.size();
}

bool GPlayerTrade::IsConfirm()
{
	return m_bConfirm;
}

vector<GTradeItem*> GPlayerTrade::GetAllItem()
{
	vector<GTradeItem*> vecTradeItem;

	for each (const map<int, GTradeItem*>::value_type& val in m_mapTradeItem)
	{
		vecTradeItem.push_back(val.second);
	}

	return vecTradeItem;
}

GTradeItem* GPlayerTrade::GetItem(int nSlotID)
{
	map<int, GTradeItem*>::iterator itor = m_mapTradeItem.find(nSlotID);
	if (m_mapTradeItem.end() == itor) return NULL;

	GTradeItem* pTradeItem = itor->second;

	return pTradeItem;
}

int GPlayerTrade::GetItemAmount(int nSlotID)
{
	GTradeItem* pTradeItem = GetItem(nSlotID);
	if (NULL == pTradeItem) return 0;

	return pTradeItem->GetAmount();
}

int GPlayerTrade::GetItemCount()
{
	return static_cast<int>(m_mapTradeItem.size());
}

int GPlayerTrade::GetMoney()
{
	return m_nMoney;
}

MUID GPlayerTrade::GetTrader()
{
	return m_uidTrader;
}

MUID GPlayerTrade::GetTradeRequesterUID()
{
	return m_nTradeRequesterUID;
}
