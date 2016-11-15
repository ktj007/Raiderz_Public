#pragma once

#include "MCommandHandler.h"
using namespace minet;

class GCmdHandler_Master_MoveServer: public MCommandHandler, public MTestMemPool<GCmdHandler_Master_MoveServer>
{
public:
	GCmdHandler_Master_MoveServer(MCommandCommunicator* pCC);

	DECL_CMD_HANDLER(OnReserveMoveLoginServerRes);
};
