#pragma once

#include "MCommandHandler.h"
using namespace minet;

class GCmdHandler_App_Mail: public MCommandHandler, public MTestMemPool<GCmdHandler_App_Mail>
{
public:
	GCmdHandler_App_Mail(MCommandCommunicator* pCC);

	DECL_CMD_HANDLER(OnTranNewMailReq);
};