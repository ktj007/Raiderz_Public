#pragma once

#include "MCommandHandler.h"
using namespace minet;

class GCmdHandler_App : public MCommandHandler
{
public:
	GCmdHandler_App(MCommandCommunicator* pCC);

	DECL_CMD_HANDLER(OnOutPutLog);

};
