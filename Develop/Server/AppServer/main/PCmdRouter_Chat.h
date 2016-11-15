#pragma once

#include "STypes.h"

namespace minet
{
	class MCommand;
}

class PChatChannel;
class PChatPlayer;
class SProxyPlayer;

class PCmdRouter_Chat
{
public:
	void PostEnterChannel(MUID uidTargetGameServer, CID cidPlayer, PChatChannel* pChannel);
	void RouteEnterChannelPlayerInfo(PChatChannel* pChannel, std::wstring& strEnterPlayerName, CID cidIgnoreToRoute);

	void PostLeaveChannel(MUID uidTargetGameServer, CID cidPlayer, PChatChannel* pChannel);
	void RouteLeaveChannelPlayerInfo(PChatChannel* pChannel, std::wstring& strEnterPlayerName);

	void PostKick(MUID uidTargetGameServer, CID cidKickedPlayer, MUID uidChannel);
	void RouteKickPlayerInfo(PChatChannel* pChannel, std::wstring& strKickedPlayerName);
	void PostBan(MUID uidTargetGameServer, CID cidBanedPlayer, MUID uidChannel);
	void RouteBanPlayerInfo(PChatChannel* pChannel, std::wstring& strBanedPlayerName);
	void PostUnbanRes(MUID uidTargetGameServer, CID cidPlayer, MUID uidChannel, std::wstring& strUnbanedPlayerName);
	void RouteChangeHostInfo(PChatChannel* pChannel, std::wstring& strNewHostPlayerName);

	void RouteAllChannelInfo(MUID uidTargetServer);
	void RouteAddedChannelInfo(PChatChannel* pChannel);
	void RouteDeletedChannelInfo(MUID uidChannel);
	void RouteAllChatPlayerInfo(MUID uidTargetServer);
	void RouteAddedChatPlayerInfo(MUID uidChannel, CID cidPlayer);
	void RouteDeletedChatPlayerInfo(MUID uidChannel, CID cidPlayer);
	void RouteChatPlayerGameServerInfo(CID cidPlayer, int nGameServerID);

private:
	minet::MCommand* MakeCmd_EnterChannel(CID cidPlayer, PChatChannel* pChannel);
	minet::MCommand* MakeCmd_EnterChannelPlayerInfo(MUID uidChannel, std::wstring strName);
	minet::MCommand* MakeCmd_LeaveChannel(CID cidPlayer, PChatChannel* pChannel);
	minet::MCommand* MakeCmd_LeaveChannelPlayerInfo(MUID uidChannel, std::wstring strName);

	minet::MCommand* MakeCmd_Kick(CID cidKickedPlayer, MUID uidChannel);
	minet::MCommand* MakeCmd_KickPlayerInfo(MUID uidChannel, std::wstring& strKickedPlayerName);
	minet::MCommand* MakeCmd_Ban(CID cidBanedPlayer, MUID uidChannel);
	minet::MCommand* MakeCmd_BanPlayerInfo(MUID uidChannel, std::wstring& strBanedPlayerName);
	minet::MCommand* MakeCmd_UnbanRes(CID cidPlayer, MUID uidChannel, std::wstring& strUnbanedPlayerName);
	minet::MCommand* MakeCmd_ChangeHostInfo(MUID uidChannel, std::wstring& strNewHostPlayerName);

	minet::MCommand* MakeCmd_AllChannelInfo();
	minet::MCommand* MakeCmd_AddedChannelInfo(PChatChannel* pChannel);
	minet::MCommand* MakeCmd_DeletedChannelInfo(MUID uidChannel);
	minet::MCommand* MakeCmd_AllChatPlayerInfo();
	minet::MCommand* MakeCmd_AddedChatPlayerInfo(MUID uidChannel, CID cidPlayer);
	minet::MCommand* MakeCmd_DeletedChatPlayerInfo(MUID uidChannel, CID cidPlayer);
	minet::MCommand* MakeCmd_ChatPlayerGameServerInfo(CID cidPlayer, int nGameServerID);

};
