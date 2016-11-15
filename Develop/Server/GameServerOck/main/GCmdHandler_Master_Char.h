#pragma once

#include "MCommandHandler.h"
using namespace minet;

class GCmdHandler_Master_Char : public MCommandHandler, public MTestMemPool<GCmdHandler_Master_Char>
{
public:
	GCmdHandler_Master_Char(MCommandCommunicator* pCC);

	DECL_CMD_HANDLER(OnNPCDieReward);
};
