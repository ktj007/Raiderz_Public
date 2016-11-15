#include "StdAfx.h"
#include "GCmdHandler_TradeMarket.h"
#include "CCommandTable.h"
#include "GGlobal.h"
#include "GPlayerObjectManager.h"
#include "GStorageSystem.h"
#include "GInteractionSystem.h"
#include "GNPCInteractor.h"
#include "CSDef_Interaction.h"

GCmdHandler_TradeMarket::GCmdHandler_TradeMarket(MCommandCommunicator* pCC) : MCommandHandler(pCC)
{
	SetCmdHandler(MC_CS_TRADEMARKET_PUT_ON,			OnTradeMarketPutOn);
	SetCmdHandler(MC_CS_TRADEMARKET_BUY,			OnTradeMarketBuy);
	SetCmdHandler(MC_CS_TRADEMARKET_SEARCH,			OnTradeMarketSearch);
	SetCmdHandler(MC_CS_TRADEMARKET_SEARCH_NEXT,	OnTradeMarketSearchNext);
	SetCmdHandler(MC_CS_TRADEMARKET_REFRESH,		OnTradeMarketRefresh);
}

MCommandResult GCmdHandler_TradeMarket::OnTradeMarketPutOn(MCommand* pCmd, MCommandHandler* pHandler)
{
	return CR_TRUE;
}

MCommandResult GCmdHandler_TradeMarket::OnTradeMarketBuy(MCommand* pCmd, MCommandHandler* pHandler)
{
	return CR_TRUE;
}

MCommandResult GCmdHandler_TradeMarket::OnTradeMarketSearch(MCommand* pCmd, MCommandHandler* pHandler)
{
	return CR_TRUE;
}

MCommandResult GCmdHandler_TradeMarket::OnTradeMarketSearchNext(MCommand* pCmd, MCommandHandler* pHandler)
{
	return CR_TRUE;
}

MCommandResult GCmdHandler_TradeMarket::OnTradeMarketRefresh(MCommand* pCmd, MCommandHandler* pHandler)
{
	return CR_TRUE;
}