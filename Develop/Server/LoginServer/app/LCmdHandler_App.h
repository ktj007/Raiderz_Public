#pragma once

#include "MCommandHandler.h"
using namespace minet;

class LCmdHandler_App : public MCommandHandler
{
public:
	LCmdHandler_App(MCommandCommunicator* pCC);

	DECL_CMD_HANDLER(OnOutPutLog);

};

