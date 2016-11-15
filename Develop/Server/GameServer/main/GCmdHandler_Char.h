#pragma once

#include "MCommandHandler.h"
using namespace minet;

class GCmdHandler_Char : public MCommandHandler, public MTestMemPool<GCmdHandler_Char>
{
public:
	GCmdHandler_Char(MCommandCommunicator* pCC);

	DECL_CMD_HANDLER(OnRequestRebirth);
	DECL_CMD_HANDLER(OnRequestTargetInfo);
	DECL_CMD_HANDLER(OnRequestDie);
};
