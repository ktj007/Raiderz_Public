#pragma once

#include "MCommandHandler.h"
using namespace minet;

class GCmdHandler_Master_Login: public MCommandHandler, public MTestMemPool<GCmdHandler_Master_Login>
{
public:
	GCmdHandler_Master_Login(MCommandCommunicator* pCC);

	DECL_CMD_HANDLER(OnRequestLoginGameServer);
	DECL_CMD_HANDLER(OnPlayerStateInWorldRes);
	DECL_CMD_HANDLER(OnRequestReadyEnterMovePlayer);
	DECL_CMD_HANDLER(OnResponseReserveMoveGameServer);
	DECL_CMD_HANDLER(OnCancelReservedMovePlayer);
};
