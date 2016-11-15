#ifndef _GCMD_HANDLER_MSG_H
#define _GCMD_HANDLER_MSG_H

#include "MCommandHandler.h"
using namespace minet;

class GCmdHandler_MSG : public MCommandHandler, public MTestMemPool<GCmdHandler_MSG>
{
public:
	GCmdHandler_MSG(MCommandCommunicator* pCC);

	DECL_CMD_HANDLER(OnMsgReq);
	DECL_CMD_HANDLER(OnMsgCreateChannel);
	DECL_CMD_HANDLER(OnMsgEnterChannelReq);
	DECL_CMD_HANDLER(OnMsgLeaveChannelReq);
	DECL_CMD_HANDLER(OnMsgKickPlayerReq);
	DECL_CMD_HANDLER(OnMsgBanPlayerReq);
	DECL_CMD_HANDLER(OnMsgUnbanPlayerReq);
	DECL_CMD_HANDLER(OnMsgChangeHostReq);
	
};

#endif//_GCMD_HANDLER_MSG_H
