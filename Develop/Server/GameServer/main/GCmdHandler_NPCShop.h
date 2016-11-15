#ifndef _GCMD_HANDLER_NPC_TRADE_H
#define _GCMD_HANDLER_NPC_TRADE_H

#include "MCommandHandler.h"
using namespace minet;

class GCmdHandler_NPCShop : public MCommandHandler, public MTestMemPool<GCmdHandler_NPCShop>
{
public:
	GCmdHandler_NPCShop(MCommandCommunicator* pCC);

	DECL_CMD_HANDLER(OnRequestBuy);
	DECL_CMD_HANDLER(OnRequestSell);
	DECL_CMD_HANDLER(OnRequestSellVeryCommon);
	DECL_CMD_HANDLER(OnRequestRepairOne);
	DECL_CMD_HANDLER(OnRequestRepairAll);
};

#endif//_GCMD_HANDLER_NPC_TRADE_H
