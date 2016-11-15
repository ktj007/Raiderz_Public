#pragma once

#include "MCommandHandler.h"
using namespace minet;

class XCmdHandler_ChatChannel: public MCommandHandler
{
public:
	XCmdHandler_ChatChannel(MCommandCommunicator* pCC);

	DECL_CMD_HANDLER(OnCreateChannel);
	DECL_CMD_HANDLER(OnEnterChannel);
	DECL_CMD_HANDLER(OnLeaveChannel);
	DECL_CMD_HANDLER(OnKickPlayer);
	DECL_CMD_HANDLER(OnBanPlayer);
	DECL_CMD_HANDLER(OnUnbanPlayer);
	DECL_CMD_HANDLER(OnChangeHost);
	DECL_CMD_HANDLER(OnPlayerEnterInfo);
	DECL_CMD_HANDLER(OnPlayerLeaveInfo);
	DECL_CMD_HANDLER(OnPlayerKickInfo);
};
