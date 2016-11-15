#include "StdAfx.h"
#include "GTradeExchanger.h"
#include "GEntityPlayer.h"
#include "GGlobal.h"
#include "GDBManager.h"
#include "GItemManager.h"
#include "CCommandTable.h"
#include "GCommand.h"
#include "GPlayerNPCIconSender.h"
#include "GItemHolder.h"
#include "GPlayerDoing.h"
#include "GPlayerTrade.h"
#include "GInteractionSystem.h"
#include "GConditionInfo.h"
#include "GCmdHandler_Trade.h"

bool GTradeExchanger::Exchange(GEntityPlayer* pPlayer1, GEntityPlayer* pPlayer2)
{
	VALID_RET(pPlayer1, false);
	VALID_RET(pPlayer2, false);

	const int nP1GiveMoney = pPlayer1->GetPlayerTrade().GetMoney();	
	const int nP2GiveMoney = pPlayer2->GetPlayerTrade().GetMoney();	

	vector<GTradeItem*> vecP1GiveItem = pPlayer1->GetPlayerTrade().GetAllItem();
	vector<GTradeItem*> vecP2GiveItem = pPlayer2->GetPlayerTrade().GetAllItem();
	
	if (false == Check(	pPlayer1, vecP1GiveItem, nP1GiveMoney, 
						pPlayer2, vecP2GiveItem, nP2GiveMoney)) return false;

	if (false == DB(	pPlayer1, vecP1GiveItem, nP1GiveMoney, 
						pPlayer2, vecP2GiveItem, nP2GiveMoney)) return false;

	return true;
}

bool GTradeExchanger::Check(GEntityPlayer* pPlayer1, const vector<GTradeItem*>& vecP1GiveItem, int nP1GiveMoney, GEntityPlayer* pPlayer2, vector<GTradeItem*> vecP2GiveItem, int nP2GiveMoney)
{
	VALID_RET(pPlayer1, false);
	VALID_RET(pPlayer2, false);

	if (false == CheckMoney(pPlayer1->GetMoney(), nP1GiveMoney, pPlayer2->GetMoney(), nP2GiveMoney)) return false;
	if (false == CheckItem(pPlayer1, vecP1GiveItem)) return false;
	if (false == CheckItem(pPlayer2, vecP2GiveItem)) return false;
	

	return true;
}

bool GTradeExchanger::CheckMoney(int nP1OwnMoney, int nP1GiveMoney, int nP2OwnMoney, int nP2GiveMoney)
{
	if (0 > nP1GiveMoney) return false;
	if (0 > nP2GiveMoney) return false;

	if (nP1OwnMoney < nP1GiveMoney) return false;
	if (nP2OwnMoney < nP2GiveMoney) return false;

	if (INT_MAX - nP2GiveMoney < nP1OwnMoney) return false;
	if (INT_MAX - nP1GiveMoney < nP2OwnMoney) return false;

	return true;
}

bool GTradeExchanger::CheckItem(GEntityPlayer* pPlayer, const vector<GTradeItem*>& vecTradeItem)
{
	VALID_RET(pPlayer, false);

	if (MAX_TRADE_ITEM_SPACE < vecTradeItem.size()) return false;

	for each(GTradeItem* pTradeItem in vecTradeItem)
	{
		GItemHolder* pItemHolder = pPlayer->GetItemHolder();
		GItem* pInvenItem = pItemHolder->GetItem(SLOTTYPE_INVENTORY, pTradeItem->GetSlotID());
		if (NULL == pInvenItem) return false;

		if (pInvenItem->GetAmount() < pTradeItem->GetAmount()) return false;
		if (true == pInvenItem->m_bBind) return false;
	}

	return true;
}

bool GTradeExchanger::DB(GEntityPlayer* pPlayer1, const vector<GTradeItem*> &vecP1GiveItem, int nP1GiveMoney,
						 GEntityPlayer* pPlayer2, const vector<GTradeItem*> &vecP2GiveItem, int nP2GiveMoney)
{
	VALID_RET(pPlayer1, false);
	VALID_RET(pPlayer2, false);

	GTRADE_CHAR_VEC vTradeChar;
	vTradeChar.push_back(GTRADE_CHAR(pPlayer1->GetAID(), pPlayer1->GetUID(), pPlayer1->GetCID(), pPlayer1->GetPlayerInfo()->nXP
		, pPlayer1->GetMoney() + (nP2GiveMoney - nP1GiveMoney), nP1GiveMoney
		, pPlayer1->GetPlayerInfo()->nPlayTimeSec, pPlayer1->GetPlayerInfo()->GetDeltaPlayTime(), pPlayer1->GetLevel()
		, pPlayer1->GetFieldID(), pPlayer1->GetPos().x, pPlayer1->GetPos().y, pPlayer1->GetPos().z));
	vTradeChar.push_back(GTRADE_CHAR(pPlayer2->GetAID(), pPlayer2->GetUID(), pPlayer2->GetCID(), pPlayer2->GetPlayerInfo()->nXP
		, pPlayer2->GetMoney() + (nP1GiveMoney - nP2GiveMoney), nP2GiveMoney
		, pPlayer2->GetPlayerInfo()->nPlayTimeSec, pPlayer2->GetPlayerInfo()->GetDeltaPlayTime(), pPlayer2->GetLevel()
		, pPlayer2->GetFieldID(), pPlayer2->GetPos().x, pPlayer2->GetPos().y, pPlayer2->GetPos().z));


	if (!MakeTradeItemInfo(pPlayer1, vecP1GiveItem, vTradeChar[0], vTradeChar[1], vecP2GiveItem.size()))
	{
		// 실패 시, SlotID 예약취소
		CancelReservedSlotID(pPlayer1, vTradeChar[1], MAX_TRADE_ITEM_SPACE);
		CancelReservedSlotID(pPlayer2, vTradeChar[0], MAX_TRADE_ITEM_SPACE);

		return false;
	}

	if (!MakeTradeItemInfo(pPlayer2, vecP2GiveItem, vTradeChar[1], vTradeChar[0], vecP1GiveItem.size()))
	{
		// 실패 시, SlotID 예약취소
		CancelReservedSlotID(pPlayer1, vTradeChar[1], MAX_TRADE_ITEM_SPACE);
		CancelReservedSlotID(pPlayer2, vTradeChar[0], MAX_TRADE_ITEM_SPACE);

		return false;
	}

	if (!gsys.pDBManager->Trade(vTradeChar, (vecP1GiveItem.size() * 2) + (vecP2GiveItem.size() * 2)))
	{
		// 실패 시, SlotID 예약취소
		CancelReservedSlotID(pPlayer1, vTradeChar[1], MAX_TRADE_ITEM_SPACE);
		CancelReservedSlotID(pPlayer2, vTradeChar[0], MAX_TRADE_ITEM_SPACE);

		return false;
	}

	return true;
}

bool GTradeExchanger::MakeTradeItemInfo( GEntityPlayer* pPlayer
										, const vector<GTradeItem*>& vecGiveItem
										, GTRADE_CHAR& TradeCharFrom
										, GTRADE_CHAR& TradeCharTo										
										, const size_t nReqSlotCnt)
{
	VALID_RET(pPlayer, false);

	// 기간제 아이템의 거래를 못하게 하는 기획이 아직 없기 때문에 우선 가능하게 함.
	// 대신 사용 가능한 시간이 1시간 이상 남아야 거래 가능 하도록 했음. - sunge.

	int16 nFreeSlotID = 0;
	for (size_t j = 0; j < MAX_TRADE_ITEM_SPACE; ++j)
	{
		if (j < nReqSlotCnt)
		{
			nFreeSlotID = pPlayer->GetItemHolder()->ReserveEmptySlot(SLOTTYPE_INVENTORY, nFreeSlotID);
			
			// 인벤토리에 거래를 위한 공간이 부족하다.
			if (nFreeSlotID == INVALID_ID)
			{
				return false;
			}

			TradeCharTo.m_vItem[j].m_nSlotIDTo = nFreeSlotID;			
		}
		else
		{
			TradeCharTo.m_vItem[j].m_nSlotIDTo = INVALID_ID;
		}
	}

	#define TRADE_MIN_REMAIN_TIME_SEC 3600 // 기획이 정해진건 아니고 우선 필요해서 1시간이상 남아야 거래 가능.

	for (size_t i = 0; i < vecGiveItem.size(); ++i)
	{
		GItemHolder* pItemHolder = pPlayer->GetItemHolder();		
		GItem* pItem = pItemHolder->GetItem(SLOTTYPE_INVENTORY, vecGiveItem[i]->GetSlotID());
		if (NULL == pItem)
			return false;

		if (pItem->m_UsagePeriod.IsPeriod())
		{
			if (TRADE_MIN_REMAIN_TIME_SEC > pItem->m_UsagePeriod.GetRemainTimeSec(pPlayer->GetPlayerInfo()->nPlayTimeSec))
				return false;
		}

		if (pItem->m_ExpirationPeriod.IsPeriod())
		{
			if (TRADE_MIN_REMAIN_TIME_SEC > pItem->m_ExpirationPeriod.GetRemainTimeSec(pPlayer->GetPlayerInfo()->nPlayTimeSec))
				return false;
		}

		if (pItem->GetAmount() < vecGiveItem[i]->GetAmount())
			return false;

		TradeCharFrom.m_vItem[i].m_nItemID				= pItem->m_pItemData->m_nID;
		TradeCharFrom.m_vItem[i].m_nIUID				= pItem->m_nIUID;
		TradeCharFrom.m_vItem[i].m_nSlotID				= pItem->m_nSlotID;
		TradeCharFrom.m_vItem[i].m_nStackAmt			= pItem->GetAmount() - vecGiveItem[i]->GetAmount();
		TradeCharFrom.m_vItem[i].m_nTradeAmt			= vecGiveItem[i]->GetAmount();
		TradeCharFrom.m_vItem[i].m_nDura				= pItem->m_nDurability;
		TradeCharFrom.m_vItem[i].m_nMaxDura				= pItem->m_pItemData->m_nMaxDurability;
		TradeCharFrom.m_vItem[i].m_nEnchCnt				= pItem->GetEnchantCount();
		TradeCharFrom.m_vItem[i].m_nColor				= pItem->m_nDyedColor;
		TradeCharFrom.m_vItem[i].m_nSoulCnt				= pItem->m_nSoulCount;

		if (pItem->m_UsagePeriod.IsPeriod())
			TradeCharFrom.m_vItem[i].m_nRemainUsagePeriod = pItem->m_UsagePeriod.GetRemainTimeSec(pPlayer->GetPlayerInfo()->nPlayTimeSec);
		else 
			TradeCharFrom.m_vItem[i].m_nRemainUsagePeriod = 0;

		if (pItem->m_ExpirationPeriod.IsPeriod())
			TradeCharFrom.m_vItem[i].m_nAddExpiSec = pItem->m_ExpirationPeriod.GetRemainTimeSec(pPlayer->GetPlayerInfo()->nPlayTimeSec);
		else
			TradeCharFrom.m_vItem[i].m_nAddExpiSec = -1;
	}

	return true;
}

void GTradeExchanger::CancelReservedSlotID( GEntityPlayer* pPlayer, GTRADE_CHAR& TradeChar, size_t nSlotIDCount )
{
	VALID(pPlayer);

	for(size_t i = 0; i < nSlotIDCount; i++)
	{
		if (TradeChar.m_vItem[i].m_nSlotIDTo != INVALID_ID)
		{
			GItemHolder* pItemHolder = pPlayer->GetItemHolder();
			pItemHolder->CancelReserveSlot(SLOTTYPE_INVENTORY, TradeChar.m_vItem[i].m_nSlotIDTo);
		}
	}
}