#pragma once

#include "MCommandHandler.h"
using namespace minet;

class PCmdHandler_Mail: public MCommandHandler
{
public:
	PCmdHandler_Mail(MCommandCommunicator* pCC);

	DECL_CMD_HANDLER(RouteNewMailToReceiver);
	DECL_CMD_HANDLER(TransNewMailRes);
};
