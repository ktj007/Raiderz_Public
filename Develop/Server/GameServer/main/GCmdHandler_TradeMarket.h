#pragma once

#include "MCommandHandler.h"
using namespace minet;

class GCmdHandler_TradeMarket : public MCommandHandler, public MTestMemPool<GCmdHandler_TradeMarket>
{
public:
	GCmdHandler_TradeMarket(MCommandCommunicator* pCC);

	DECL_CMD_HANDLER(OnTradeMarketPutOn);
	DECL_CMD_HANDLER(OnTradeMarketBuy);
	DECL_CMD_HANDLER(OnTradeMarketSearch);
	DECL_CMD_HANDLER(OnTradeMarketSearchNext);
	DECL_CMD_HANDLER(OnTradeMarketRefresh);
};
