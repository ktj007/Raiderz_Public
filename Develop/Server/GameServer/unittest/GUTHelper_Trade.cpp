#include "StdAfx.h"
#include "GUTHelper_Trade.h"
#include "GTradeSystem.h"
#include "GTradeRequester.h"
#include "GTradeAcceptor.h"
#include "GTradeChecker.h"
#include "GTradeCanceler.h"
#include "GTradeConfirmer.h"
#include "GEntityPlayer.h"
#include "GPlayerTrade.h"

void GUTHelper_Trade::SetRequestState(GEntityPlayer* pPlayer1, GEntityPlayer* pPlayer2)
{
	gsys.pTradeSystem->GetRequester().Request(pPlayer1, pPlayer2);
}

void GUTHelper_Trade::SetTradeState(GEntityPlayer* pPlayer1, GEntityPlayer* pPlayer2)
{
	VALID(pPlayer1);
	VALID(pPlayer2);

	gsys.pTradeSystem->GetRequester().Request(pPlayer1, pPlayer2);
	gsys.pTradeSystem->GetAcceptor().Accept(pPlayer2, pPlayer1, true);
}

void GUTHelper_Trade::SetConfirmState( GEntityPlayer* pPlayer1, GEntityPlayer* pPlayer2 )
{
	VALID(pPlayer1);
	VALID(pPlayer2);

	pPlayer1->GetPlayerTrade().Confirm();
	pPlayer2->GetPlayerTrade().Confirm();
}

void GUTHelper_Trade::FullTradeItem(GEntityPlayer* pPlayer)
{
	VALID(pPlayer);

	for (int i=0; i<MAX_TRADE_ITEM_SPACE; i++)
	{
		pPlayer->GetPlayerTrade().AddItem(i, 1, 1);
	}
}

void GUTHelper_Trade::AddTradeItem(GEntityPlayer* pPlayer, GItem* pItem, int nAmount)
{
	VALID(pPlayer);
	VALID(pItem);

	if (0 == nAmount)
	{
		nAmount = pItem->GetAmount();
	}

	pPlayer->GetPlayerTrade().AddItem(pItem->m_nSlotID, nAmount, nAmount);
}