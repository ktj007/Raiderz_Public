#pragma once

#include "MCommandHandler.h"
using namespace minet;

class GCmdHandler_App_Common: public MCommandHandler, public MTestMemPool<GCmdHandler_App_Common>
{
public:
	GCmdHandler_App_Common(MCommandCommunicator* pCC);

	DECL_CMD_HANDLER(OnConnect);
	DECL_CMD_HANDLER(OnDisconnect);
	DECL_CMD_HANDLER(OnError);

	DECL_CMD_HANDLER(OnResponseLogin);
};
