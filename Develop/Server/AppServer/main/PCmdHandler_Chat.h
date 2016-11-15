#pragma once

#include "MCommandHandler.h"
using namespace minet;

class PCmdHandler_Chat : public MCommandHandler
{
public:
	PCmdHandler_Chat(MCommandCommunicator* pCC);

	DECL_CMD_HANDLER(OnChatMsgReq);

	DECL_CMD_HANDLER(OnChatCreatePrivateChannelReq);

	DECL_CMD_HANDLER(OnChatEnterChannelReq);
	DECL_CMD_HANDLER(OnChatLeaveChannelReq);
	DECL_CMD_HANDLER(OnChatKickPlayerReq);
	DECL_CMD_HANDLER(OnChatBanPlayerReq);
	DECL_CMD_HANDLER(OnChatUnbanPlayerReq);
	DECL_CMD_HANDLER(OnChatChangeHostReq);

};
