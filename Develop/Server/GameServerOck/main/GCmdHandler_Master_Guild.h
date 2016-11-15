#pragma once

#include "MCommandHandler.h"
using namespace minet;

class GCmdHandler_Master_Guild : public MCommandHandler, public MTestMemPool<GCmdHandler_Master_Guild>
{
public:
	GCmdHandler_Master_Guild(MCommandCommunicator* pCC);

	DECL_CMD_HANDLER(OnMemberOnlineInfo);
	DECL_CMD_HANDLER(OnDestroySync);
	DECL_CMD_HANDLER(OnJoinSync);
	DECL_CMD_HANDLER(OnLeaveSync);
	DECL_CMD_HANDLER(OnKickSync);
	DECL_CMD_HANDLER(OnOnLineSync);
	DECL_CMD_HANDLER(OnOffLineSync);
	DECL_CMD_HANDLER(OnMoveFieldSync);
	DECL_CMD_HANDLER(OnChangeMasterSync);
	DECL_CMD_HANDLER(OnDepositStorageMoneySync);
	DECL_CMD_HANDLER(OnWithdrawStorageMoneySync);
	DECL_CMD_HANDLER(OnMoveStorageItemSync);
};
