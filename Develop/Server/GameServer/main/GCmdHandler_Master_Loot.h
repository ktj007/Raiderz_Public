#pragma once

#include "MCommandHandler.h"
using namespace minet;

class GCmdHandler_Master_Loot : public MCommandHandler, public MTestMemPool<GCmdHandler_Master_Loot>
{
public:
	GCmdHandler_Master_Loot(MCommandCommunicator* pCC);

	// Loot
	DECL_CMD_HANDLER(OnMasterLootNotify);
	DECL_CMD_HANDLER(OnOtherGainItem);
	DECL_CMD_HANDLER(OnGettableItemAdd);
};
