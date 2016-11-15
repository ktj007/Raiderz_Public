#pragma once

#include "MCommandHandler.h"
using namespace minet;

class XCmdHandler_Storage : public MCommandHandler
{
public:
	XCmdHandler_Storage(MCommandCommunicator* pCC);

	DECL_CMD_HANDLER(OnShowUI);
	DECL_CMD_HANDLER(OnDepositMoney);
	DECL_CMD_HANDLER(OnWithdrawMoney);
};
