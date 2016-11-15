#pragma once

#include "MCommandHandler.h"
using namespace minet;

class ZServerAcceptor;


class ZCmdHandler_Guild : public MCommandHandler
{
public:
	ZCmdHandler_Guild(MCommandCommunicator* pCC);

	DECL_CMD_HANDLER(OnMemberOnlineInfoReq);
	DECL_CMD_HANDLER(OnDestroy);
	DECL_CMD_HANDLER(OnJoin);
	DECL_CMD_HANDLER(OnLeave);
	DECL_CMD_HANDLER(OnKick);
	DECL_CMD_HANDLER(OnOnLine);
	DECL_CMD_HANDLER(OnOffLine);
	DECL_CMD_HANDLER(OnMoveField);
	DECL_CMD_HANDLER(OnChaneMaster);

	DECL_CMD_HANDLER(OnDepositStorageMoney);
	DECL_CMD_HANDLER(OnWithdrawStorageMoney);
	DECL_CMD_HANDLER(OnMoveStorageItem);
};
