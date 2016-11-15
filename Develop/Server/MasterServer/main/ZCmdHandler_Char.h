#pragma once

#include "MCommandHandler.h"
using namespace minet;

class ZCmdHandler_Char : public MCommandHandler
{
public:
	ZCmdHandler_Char(MCommandCommunicator* pCC);

	DECL_CMD_HANDLER(OnNPCDieRewardReq);
};
