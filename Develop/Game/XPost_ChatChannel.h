#pragma once

#include "XNetClient.h"
#include "XCommandTable.h"

inline void XPostChatChannel_Join(wstring strChannelName)
{
	XPOSTCMD1(MC_MSG_ENTER_CHANNEL_REQ, MCommandParameterWideString(strChannelName.c_str()));
}

inline void XPostChatChannel_Leave(MUID uidChannel)
{
	XPOSTCMD1(MC_MSG_LEAVE_CHANNEL_REQ, MCommandParameterUID(uidChannel));
}

inline void XPostChatChannel_Kick(MUID uidChannel, wstring strTargetPlayerName)
{
	XPOSTCMD2(MC_MSG_KICK_PLAYER_REQ, MCommandParameterUID(uidChannel), MCommandParameterWideString(strTargetPlayerName.c_str()));
}

inline void XPostChatChannel_Ban(MUID uidChannel, wstring strTargetPlayerName)
{
	XPOSTCMD2(MC_MSG_BAN_PLAYER_REQ, MCommandParameterUID(uidChannel), MCommandParameterWideString(strTargetPlayerName.c_str()));
}

inline void XPostChatChannel_Unban(MUID uidChannel, wstring strBanedPlayerName)
{
	XPOSTCMD2(MC_MSG_UNBAN_PLAYER_REQ, MCommandParameterUID(uidChannel), MCommandParameterWideString(strBanedPlayerName.c_str()));
}

inline void XPostChatChannel_Owner(MUID uidChannel, wstring strTargetPlayerName)
{
	XPOSTCMD2(MC_MSG_CHANGE_HOST_REQ, MCommandParameterUID(uidChannel), MCommandParameterWideString(strTargetPlayerName.c_str()));
}

inline void XPostChatChannel_Create(wstring strChannelName)
{
	XPOSTCMD1(MC_MSG_CREATE_CHANNEL_REQ, MCommandParameterWideString(strChannelName.c_str()));
}
