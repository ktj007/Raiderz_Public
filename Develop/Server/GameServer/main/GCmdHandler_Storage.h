#pragma once

#include "MCommandHandler.h"
using namespace minet;

class GCmdHandler_Storage : public MCommandHandler, public MTestMemPool<GCmdHandler_Storage>
{
public:
	GCmdHandler_Storage(MCommandCommunicator* pCC);

	DECL_CMD_HANDLER(OnDepositMoney);
	DECL_CMD_HANDLER(OnWithdrawMoney);
};
