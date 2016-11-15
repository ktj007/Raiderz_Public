#pragma once

#include "MCommandHandler.h"
using namespace minet;

class PCmdHandler_App : public MCommandHandler
{
public:
	PCmdHandler_App(MCommandCommunicator* pCC);

	DECL_CMD_HANDLER(OnOutPutLog);
};
