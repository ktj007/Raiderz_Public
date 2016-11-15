#pragma once

#include "GServerChatChannelRouter.h"

class GAppServerChatChannelRouter : public GServerChatChannelRouter
{
public:
	bool CreateChannelReq(CID cidPlayer, wstring strChannelName) override;
	bool EnterChannelReq(CID cidPlayer, wstring strChannelName) override;
	bool LeaveChannelReq(CID cidPlayer, MUID uidChannel) override;
	bool KickPlayerReq(CID cidPlayer, MUID uidChannel, wstring strTargetPlayerName) override;
	bool BanPlayerReq(CID cidPlayer, MUID uidChannel, wstring strTargetPlayerName) override;
	bool UnbanPlayerReq(CID cidPlayer, MUID uidChannel, wstring strBanedPlayerName) override;
	bool ChangeHostReq(CID cidPlayer, MUID uidChannel, wstring strTargetPlayerName) override;
};
