#pragma once

class GEntityPlayer;
struct TD_CHAT_CHANNEL_INFO;

class GClientChatChannelRouter
{
public:
	void EnterChannelRes(MUID uidPlayer, const TD_CHAT_CHANNEL_INFO& tdInfo);
	void LeaveChannelRes(MUID uidPlayer, MUID uidChannel);
	void KickPlayerRes(MUID uidPlayer, MUID uidChannel);
	void BanPlayerRes(MUID uidPlayer, MUID uidChannel);	
	void UnbanPlayerRes(MUID uidPlayer, MUID uidChannel, wstring strUnbanedPlayerName);	
	void ChangeHostRes(MUID uidChannel, wstring strNewHostName);
	void EnterPlayerInfo(MUID uidChannel, wstring strEnterPlayerName);
	void LeavePlayerInfo(MUID uidChannel, wstring strLeavePlayerName);
	void KickedPlayerInfo(MUID uidChannel, wstring strKickedPlayerName);
	void LeaveChannelAllPlayer(MUID uidChannel);
};
