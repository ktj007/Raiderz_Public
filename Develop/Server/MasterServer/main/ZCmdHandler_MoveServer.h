#pragma once

#include "MCommandHandler.h"
using namespace minet;

class ZCmdHandler_MoveServer : public MCommandHandler
{
public:
	ZCmdHandler_MoveServer(MCommandCommunicator* pCC);

	DECL_CMD_HANDLER(OnRequestLoginGameServer);
	DECL_CMD_HANDLER(OnResponseLoginGameServer);

	DECL_CMD_HANDLER(OnRequestReserveMoveGameServer);
	DECL_CMD_HANDLER(OnResponseReadyEnterMovePlayer);

	DECL_CMD_HANDLER(OnCancelMoveGameServer);

	DECL_CMD_HANDLER(OnCommReserveMoveLoginServerReq);
	DECL_CMD_HANDLER(OnCommCancelMoveLoginServer);
	DECL_CMD_HANDLER(OnCommReadyEnterLoginServerRes);

};
