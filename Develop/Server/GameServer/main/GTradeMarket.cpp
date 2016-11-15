#include "stdafx.h"
#include "GTradeMarket.h"
#include "GGlobal.h"
#include "GPlayerObjectManager.h"
#include "GItemHolder.h"
#include "GEntityPlayer.h"
#include "CCommandTable.h"
#include "GCommand.h"
#include "GDBManager.h"
#include "GDBTaskTradeMarketData.h"

namespace {
	void RouteError(GEntityPlayer* const player,const TRADEMARKET_EXCEPTION& errCode)
	{
		VALID(NULL!=player);
		MCommand* const cmd = MakeNewCommand(MC_SC_TRADEMARKET_EXCEPTION, player->GetUID(), 1, NEW_UCHAR(errCode));
		VALID(NULL != cmd);

		player->RouteToMe(cmd);
	}
}

bool GTradeMarket::PutOn(const MUID& playerUID,const int& slot,const int& price,const int& quantity,const int& expiryDate)
{
	GEntityPlayer* const player = gmgr.pPlayerObjectManager->GetEntityInWorld(playerUID);
	VALID_RET(NULL != player, false);

	GItemHolder* const itemHolder = player->GetItemHolder();
	VALID_RET(NULL != itemHolder, false);

	GItem* const item = itemHolder->GetItem(SLOTTYPE_INVENTORY, slot);
	if (NULL == item)
	{
		RouteError(player, TRADEMARKET_EXCEPTION_PUTON_INVALID_SLOT_NUM);
		return false;
	}

	if (quantity > item->GetAmount())
	{
		RouteError(player, TRADEMARKET_EXCEPTION_PUTON_INVALID_ITEM_COUNT);
		return false;
	}

	if ( false == gsys.pDBManager->TradeMarketPutOn(GDBT_TRADEMARKET_PUT_ON(playerUID, player->GetCID(), slot, price, quantity, expiryDate, item->m_nIUID, item->GetAmount() - quantity)) )
	{
		RouteError(player, TRADEMARKET_EXCEPTION_PUTON_DB_ERROR);
		return false;
	}

	return true;
}

void GTradeMarket::Search(const MUID& playerUID,const int& itemUID)
{
	// 
}