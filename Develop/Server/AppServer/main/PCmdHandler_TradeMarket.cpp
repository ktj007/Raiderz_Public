#include "stdafx.h"
#include "PCmdHandler_TradeMarket.h"
#include "CCommandTable_App.h"
#include "STypes.h"
#include "CTransData.h"
#include "PGlobal.h"
#include "SProxyPlayerManager.h"
#include "PServer.h"
#include "PTradeMarketServiceConsumer.h"

PCmdHandler_TradeMarket::PCmdHandler_TradeMarket(MCommandCommunicator* pCC) : MCommandHandler(pCC)
{
	SetCmdHandler(MPC_GA_TRADEMARKET_ADD,			AddedItem);
	SetCmdHandler(MPC_GA_TRADEMARKET_DEL,			DeletedItem);
	SetCmdHandler(MPC_GA_TRADEMARKET_SEARCH,		Search);
	SetCmdHandler(MPC_GA_TRADEMARKET_SEARCH_NEXT,	SearchNext);
}

MCommandResult PCmdHandler_TradeMarket::AddedItem(MCommand* pCommand, MCommandHandler* pHandler)
{
	gsys.pTradeMarketServiceConsumer->NotifyAddItem(pCommand);

	return CR_TRUE;
}

MCommandResult PCmdHandler_TradeMarket::DeletedItem(MCommand* pCommand, MCommandHandler* pHandler)
{
	gsys.pTradeMarketServiceConsumer->NotifyDelItem(pCommand);

	return CR_TRUE;
}

MCommandResult PCmdHandler_TradeMarket::Search(MCommand* pCommand, MCommandHandler* pHandler)
{
	gsys.pTradeMarketServiceConsumer->RequestSearch(pCommand);

	return CR_TRUE;
}

MCommandResult PCmdHandler_TradeMarket::SearchNext(MCommand* pCommand, MCommandHandler* pHandler)
{
	gsys.pTradeMarketServiceConsumer->RequestSearchNext(pCommand);

	return CR_TRUE;
}