#pragma once

#include "MCommandHandler.h"
using namespace minet;

class GCmdHandler_App_Msg: public MCommandHandler, public MTestMemPool<GCmdHandler_App_Msg>
{
public:
	GCmdHandler_App_Msg(MCommandCommunicator* pCC);

	DECL_CMD_HANDLER(OnMsgRes);
	DECL_CMD_HANDLER(OnMsgEnterChannel);
	DECL_CMD_HANDLER(OnMsgEnterChannelPlayerInfo);
	DECL_CMD_HANDLER(OnMsgLeaveChannel);
	DECL_CMD_HANDLER(OnMsgLeaveChannelPlayerInfo);
	DECL_CMD_HANDLER(OnMsgKick);
	DECL_CMD_HANDLER(OnMsgKickPlayerInfo);
	DECL_CMD_HANDLER(OnMsgBan);
	DECL_CMD_HANDLER(OnMsgBanPlayerInfo);
	DECL_CMD_HANDLER(OnMsgUnbanRes);
	DECL_CMD_HANDLER(OnMsgChangeHostInfo);

	DECL_CMD_HANDLER(OnMsgAddedChannelInfo);
	DECL_CMD_HANDLER(OnMsgDeletedChannelInfo);
	DECL_CMD_HANDLER(OnMsgAddedPlayerInfo);
	DECL_CMD_HANDLER(OnMsgDeletedPlayerInfo);
	DECL_CMD_HANDLER(OnMsgPlayerGameServerInfo);
};
