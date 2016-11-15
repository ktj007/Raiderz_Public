#pragma once

#include "MCommandHandler.h"
using namespace minet;

class PCmdHandler_TradeMarket: public MCommandHandler
{
public:
	PCmdHandler_TradeMarket(MCommandCommunicator* pCC);

	DECL_CMD_HANDLER(AddedItem);
	DECL_CMD_HANDLER(DeletedItem);
	DECL_CMD_HANDLER(Search);
	DECL_CMD_HANDLER(SearchNext);
};
