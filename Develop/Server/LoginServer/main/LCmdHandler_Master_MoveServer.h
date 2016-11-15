#pragma once

#include "MCommandHandler.h"
using namespace minet;

class LCmdHandler_Master_MoveServer : public MCommandHandler
{
public:
	LCmdHandler_Master_MoveServer(MCommandCommunicator* pCC);

	DECL_CMD_HANDLER(OnResponseLoginGameServer);
	DECL_CMD_HANDLER(OnReadyEnterLoginServerReq);
	DECL_CMD_HANDLER(OnCancelMoveLoginServer);
};
