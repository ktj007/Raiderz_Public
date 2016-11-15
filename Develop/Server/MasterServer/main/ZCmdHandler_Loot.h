#pragma once

#include "MCommandHandler.h"
using namespace minet;

class ZCmdHandler_Loot : public MCommandHandler
{
public:
	ZCmdHandler_Loot(MCommandCommunicator* pCC);

	DECL_CMD_HANDLER(OnMasterLootNotifyReq);
	DECL_CMD_HANDLER(OnOtherGainItemReq);
	DECL_CMD_HANDLER(OnGettableItemAddReq);
	DECL_CMD_HANDLER(OnRollResultReq);
};
