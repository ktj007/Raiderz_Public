#ifndef _GCMD_HANDLER_TRADE_H
#define _GCMD_HANDLER_TRADE_H

#include "MCommandHandler.h"
using namespace minet;

class GCmdHandler_Trade : public MCommandHandler, public MTestMemPool<GCmdHandler_Trade>
{
public:
	GCmdHandler_Trade(MCommandCommunicator* pCC);

	DECL_CMD_HANDLER(OnRequestTradeStart);
	DECL_CMD_HANDLER(OnRequestTradeStartAccept);
	DECL_CMD_HANDLER(OnRequestTradePutUpItem);
	DECL_CMD_HANDLER(OnRequestTradePutDownItem);
	DECL_CMD_HANDLER(OnRequestTradePutUpMoney);
	DECL_CMD_HANDLER(OnRequestTradeCancel);
	DECL_CMD_HANDLER(OnRequestTradeConfirm);
};

#endif//_GCMD_HANDLER_TRADE_H