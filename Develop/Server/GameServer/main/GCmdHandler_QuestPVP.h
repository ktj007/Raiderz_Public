#pragma once

#include "MCommandHandler.h"
using namespace minet;

class GEntityPlayer;

class GCmdHandler_QuestPVP : public MCommandHandler, public MTestMemPool<GCmdHandler_QuestPVP>
{
public:
	GCmdHandler_QuestPVP(MCommandCommunicator* pCC);

	DECL_CMD_HANDLER(OnEnterReq);
};
