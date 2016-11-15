#include "stdafx.h"
#include "GClientChatChannelRouter.h"
#include "GCommand.h"
#include "CTransData.h"
#include "GGlobal.h"
#include "GCommandCenter.h"
#include "CCommandTable.h"

void GClientChatChannelRouter::EnterChannelRes(MUID uidPlayer, const TD_CHAT_CHANNEL_INFO& tdInfo)
{
	MCommand* pNewCmd = MakeNewCommand(MC_MSG_ENTER_CHANNEL,
										uidPlayer,
										1,
										NEW_SINGLE_BLOB(&tdInfo, sizeof(tdInfo))
										);

	gsys.pCommandCenter->PostCommand(pNewCmd);
}

void GClientChatChannelRouter::LeaveChannelRes(MUID uidPlayer, MUID uidChannel)
{
	MCommand* pNewCmd = MakeNewCommand(MC_MSG_LEAVE_CHANNEL,
										uidPlayer,
										1,
										NEW_UID(uidChannel)
										);

	gsys.pCommandCenter->PostCommand(pNewCmd);
}

void GClientChatChannelRouter::KickPlayerRes(MUID uidPlayer, MUID uidChannel)
{
	MCommand* pNewCmd = MakeNewCommand(MC_MSG_KICK_PLAYER,
										uidPlayer,
										1,
										NEW_UID(uidChannel)
										);

	gsys.pCommandCenter->PostCommand(pNewCmd);
}

void GClientChatChannelRouter::BanPlayerRes(MUID uidPlayer, MUID uidChannel)
{
	MCommand* pNewCmd = MakeNewCommand(MC_MSG_BAN_PLAYER,
										uidPlayer,
										1,
										NEW_UID(uidChannel)
										);

	gsys.pCommandCenter->PostCommand(pNewCmd);
}

void GClientChatChannelRouter::UnbanPlayerRes(MUID uidPlayer, MUID uidChannel, wstring strUnbanedPlayerName)
{
	MCommand* pNewCmd = MakeNewCommand(MC_MSG_UNBAN_PLAYER,
										uidPlayer,
										2,
										NEW_UID(uidChannel),
										NEW_WSTR(strUnbanedPlayerName.c_str())
										);

	gsys.pCommandCenter->PostCommand(pNewCmd);
}

void GClientChatChannelRouter::ChangeHostRes(MUID uidChannel, wstring strNewHostName)
{
	MCommand* pNewCmd = MakeNewCommand(MC_MSG_CHANGE_HOST,
										2,
										NEW_UID(uidChannel),
										NEW_WSTR(strNewHostName.c_str())
										);

	gsys.pCommandCenter->RouteToChatChannel(uidChannel, pNewCmd);
}

void GClientChatChannelRouter::EnterPlayerInfo(MUID uidChannel, wstring strEnterPlayerName)
{
	MCommand* pNewCmd = MakeNewCommand(MC_MSG_ENTER_CHANNEL_PLAYER_INFO,
										2,
										NEW_UID(uidChannel),
										NEW_WSTR(strEnterPlayerName.c_str())
										);

	gsys.pCommandCenter->RouteToChatChannel(uidChannel, pNewCmd);
}

void GClientChatChannelRouter::LeavePlayerInfo(MUID uidChannel, wstring strLeavePlayerName)
{
	MCommand* pNewCmd = MakeNewCommand(MC_MSG_LEAVE_CHANNEL_PLAYER_INFO,
										2,
										NEW_UID(uidChannel),
										NEW_WSTR(strLeavePlayerName.c_str())
										);

	gsys.pCommandCenter->RouteToChatChannel(uidChannel, pNewCmd);
}

void GClientChatChannelRouter::KickedPlayerInfo(MUID uidChannel, wstring strKickedPlayerName)
{
	MCommand* pNewCmd = MakeNewCommand(MC_MSG_KICK_PLAYER_INFO,
										2,
										NEW_UID(uidChannel),
										NEW_WSTR(strKickedPlayerName.c_str())
										);

	gsys.pCommandCenter->RouteToChatChannel(uidChannel, pNewCmd);
}
void GClientChatChannelRouter::LeaveChannelAllPlayer(MUID uidChannel)
{
	MCommand* pNewCmd = MakeNewCommand(MC_MSG_LEAVE_CHANNEL,
										1,
										NEW_UID(uidChannel)
										);

	gsys.pCommandCenter->RouteToChatChannel(uidChannel, pNewCmd);
}