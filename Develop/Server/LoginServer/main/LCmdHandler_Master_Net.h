#pragma once

#include "MCommandHandler.h"
using namespace minet;

class LCmdHandler_Master_Net : public MCommandHandler
{
public:
	LCmdHandler_Master_Net(MCommandCommunicator* pCC);

	DECL_CMD_HANDLER(OnConnect);
	DECL_CMD_HANDLER(OnDisconnect);
	DECL_CMD_HANDLER(OnError);
};

