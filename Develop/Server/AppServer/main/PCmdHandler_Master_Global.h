#pragma once

#include "MCommandHandler.h"
using namespace minet;

class PCmdHandler_Master_Global : public MCommandHandler
{
private:

public:
	PCmdHandler_Master_Global(MCommandCommunicator* pCC);

	DECL_CMD_HANDLER(OnResponseLogin);
	DECL_CMD_HANDLER(OnConnect);
	DECL_CMD_HANDLER(OnDisconnect);
	DECL_CMD_HANDLER(OnError);
};

