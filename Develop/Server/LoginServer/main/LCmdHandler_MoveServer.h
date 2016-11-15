#pragma once

#include "MCommandHandler.h"
using namespace minet;

class LCmdHandler_MoveServer: public MCommandHandler
{
public:
	LCmdHandler_MoveServer(MCommandCommunicator* pCC);

	DECL_CMD_HANDLER(OnRequestMoveLoginServer);
	DECL_CMD_HANDLER(OnRequestMoveLoginServerOnPmang);
};
