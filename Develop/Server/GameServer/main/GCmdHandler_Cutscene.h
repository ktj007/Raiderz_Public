#pragma once

#include "MCommandHandler.h"
using namespace minet;

class GCmdHandler_Cutscene : public MCommandHandler, public MTestMemPool<GCmdHandler_Cutscene>
{
public:
	GCmdHandler_Cutscene(MCommandCommunicator* pCC);

	DECL_CMD_HANDLER(OnBeginReq);
	DECL_CMD_HANDLER(OnEndReq);
};