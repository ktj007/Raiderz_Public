#pragma once

#include "MCommandHandler.h"
using namespace minet;

class ZCmdHandler_App : public MCommandHandler
{
public:
	ZCmdHandler_App(MCommandCommunicator* pCC);

	DECL_CMD_HANDLER(OnOutPutLog);

};

