#pragma once

#include "MCommandHandler.h"
using namespace minet;

class LCmdHandler_Master_Global : public MCommandHandler
{
public:
	LCmdHandler_Master_Global(MCommandCommunicator* pCC);

	DECL_CMD_HANDLER(OnResponseLogin);
	DECL_CMD_HANDLER(OnResponseAddPlayer);
	DECL_CMD_HANDLER(OnDuplicatedPlayerLogin);
	
	DECL_CMD_HANDLER(OnPmangResponseAddPlayer);

	DECL_CMD_HANDLER(OnAdminChangeServerMode);
};

