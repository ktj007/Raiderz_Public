#pragma once

#include "MCommandHandler.h"
using namespace minet;

class GCmdHandler_BattleArena : public MCommandHandler, public MTestMemPool<GCmdHandler_BattleArena>
{
public:
	GCmdHandler_BattleArena(MCommandCommunicator* pCC);

	DECL_CMD_HANDLER(OnRegister);
	DECL_CMD_HANDLER(OnDeregister);
};
