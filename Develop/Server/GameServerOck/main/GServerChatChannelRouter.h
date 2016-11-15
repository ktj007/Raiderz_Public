#pragma once

#include "STypes.h"

class GServerChatChannelRouter
{
public:
	virtual bool CreateChannelReq(CID cidPlayer, wstring strChannelName) = 0;
	virtual bool EnterChannelReq(CID cidPlayer, wstring strChannelName) = 0;
	virtual bool LeaveChannelReq(CID cidPlayer, MUID uidChannel) = 0;
	virtual bool KickPlayerReq(CID cidPlayer, MUID uidChannel, wstring strTargetPlayerName) = 0;
	virtual bool BanPlayerReq(CID cidPlayer, MUID uidChannel, wstring strTargetPlayerName) = 0;
	virtual bool UnbanPlayerReq(CID cidPlayer, MUID uidChannel, wstring strBanedPlayerName) = 0;
	virtual bool ChangeHostReq(CID cidPlayer, MUID uidChannel, wstring strTargetPlayerName) = 0;
};
