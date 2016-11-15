#include "stdafx.h"
#include "XCmdHandler_ChatChannel.h"
#include "XChatChannelManager.h"
#include "XChatChannel.h"
#include "XStrings.h"

XCmdHandler_ChatChannel::XCmdHandler_ChatChannel(MCommandCommunicator* pCC)
: MCommandHandler(pCC)
{	
	SetCmdHandler(MC_MSG_ENTER_CHANNEL,					OnEnterChannel);
	SetCmdHandler(MC_MSG_LEAVE_CHANNEL,					OnLeaveChannel);
	SetCmdHandler(MC_MSG_KICK_PLAYER,					OnKickPlayer);
	SetCmdHandler(MC_MSG_BAN_PLAYER,					OnBanPlayer);
	SetCmdHandler(MC_MSG_UNBAN_PLAYER,					OnUnbanPlayer);
	SetCmdHandler(MC_MSG_CHANGE_HOST,					OnChangeHost);
	SetCmdHandler(MC_MSG_ENTER_CHANNEL_PLAYER_INFO,		OnPlayerEnterInfo);
	SetCmdHandler(MC_MSG_LEAVE_CHANNEL_PLAYER_INFO,		OnPlayerLeaveInfo);
	SetCmdHandler(MC_MSG_KICK_PLAYER_INFO,				OnPlayerKickInfo);
}

MCommandResult XCmdHandler_ChatChannel::OnEnterChannel(MCommand* pCommand, MCommandHandler* pHandler)
{
	// 커멘드 확인
	TD_CHAT_CHANNEL_INFO channelInfo;
	if (!pCommand->GetParameter(&channelInfo, 0, MPT_SINGLE_BLOB))	return CR_ERROR;	

	// 채널 입장
	MUID uidChannel = channelInfo.uidChannel;
	wstring strChannelName = channelInfo.strChannelName;
	bool isOwner = channelInfo.isOwner;

	if (info.chat_channel->Join(uidChannel, strChannelName, isOwner))
	{
		if (!strChannelName.empty())
		{
			global.ui->OnSystemMsg( XGetStr( L"CHATTING_CHANNELENTERED"), FSParam(strChannelName));
		}
	}

	return CR_TRUE;
}

MCommandResult XCmdHandler_ChatChannel::OnLeaveChannel(MCommand* pCommand, MCommandHandler* pHandler)
{
	// 커멘드 확인
	MUID uidChannel;
	if (!pCommand->GetParameter(&uidChannel, 0, MPT_UID))	return CR_ERROR;

	// 채널 퇴장
	info.chat_channel->Leave(uidChannel);
	
	global.ui->OnSystemMsg( XGetStr( L"CHATTING_CHANNELLEFT"));

	return CR_TRUE;
}

MCommandResult XCmdHandler_ChatChannel::OnKickPlayer(MCommand* pCommand, MCommandHandler* pHandler)
{
	// 커멘드 확인
	MUID uidChannel;
	if (!pCommand->GetParameter(&uidChannel, 0, MPT_UID))	return CR_ERROR;

	// 채널 퇴장
	XChatChannel* pChannel = info.chat_channel->Find(uidChannel);
	if (pChannel == NULL)	return CR_TRUE;
	
	global.ui->OnSystemMsg( XGetStr( L"CHATTING_CHANNELBANISHED"), FSParam(pChannel->GetName()));

	info.chat_channel->Leave(uidChannel);

	return CR_TRUE;
}

MCommandResult XCmdHandler_ChatChannel::OnBanPlayer(MCommand* pCommand, MCommandHandler* pHandler)
{
	// 커멘드 확인
	MUID uidChannel;
	if (!pCommand->GetParameter(&uidChannel, 0, MPT_UID))	return CR_ERROR;

	// 채널 영구추방
	XChatChannel* pChannel = info.chat_channel->Find(uidChannel);
	if (pChannel == NULL)	return CR_TRUE;

	global.ui->OnSystemMsg( XGetStr( L"CHATTING_CHANNELBANISHED"), FSParam(pChannel->GetName()));

	info.chat_channel->Leave(uidChannel);

	return CR_TRUE;
}

MCommandResult XCmdHandler_ChatChannel::OnUnbanPlayer(MCommand* pCommand, MCommandHandler* pHandler)
{
	// 커멘드 확인
	MUID uidChannel;
	wstring strUnbanedPlayerName;

	if (!pCommand->GetParameter(&uidChannel, 0, MPT_UID))	return CR_ERROR;
	if (!pCommand->GetParameter(strUnbanedPlayerName, 1, MPT_WSTR))	return CR_ERROR;

	// 추방해제	
	global.ui->OnSystemMsg( XGetStr( L"CHATTING_CHANNELUNBANISHED"), FSParam(strUnbanedPlayerName));

	return CR_TRUE;
}

MCommandResult XCmdHandler_ChatChannel::OnChangeHost(MCommand* pCommand, MCommandHandler* pHandler)
{
	// 커멘드 확인
	MUID uidChannel;
	wstring strNewHostName;

	if (!pCommand->GetParameter(&uidChannel, 0, MPT_UID))		return CR_ERROR;
	if (!pCommand->GetParameter(strNewHostName, 1, MPT_WSTR))	return CR_ERROR;


	// 방장 변경
	XChatChannel* pChannel = info.chat_channel->Find(uidChannel);
	if (pChannel == NULL)	return CR_ERROR;

	// 자신이면 true
	pChannel->SetOwner(strNewHostName == gvar.MyInfo.ChrInfo.szName);

	global.ui->OnSystemMsg( XGetStr( L"CHATTING_CHANNELCHANGEDHOST"), FSParam(pChannel->GetName(), strNewHostName));

	return CR_TRUE;
}

MCommandResult XCmdHandler_ChatChannel::OnPlayerEnterInfo(MCommand* pCommand, MCommandHandler* pHandler)
{
	// 커멘드 확인
	MUID uidChannel;
	wstring strEnterPlayerName;

	if (!pCommand->GetParameter(&uidChannel, 0, MPT_UID))			return CR_ERROR;
	if (!pCommand->GetParameter(strEnterPlayerName, 1, MPT_WSTR))	return CR_ERROR;


	// 입장 플레이어 정보 출력
	XChatChannel* pChannel = info.chat_channel->Find(uidChannel);
	if (pChannel == NULL)	return CR_ERROR;

	global.ui->OnSystemMsg( XGetStr( L"CHATTING_CHANNELENTEREDMEMBER"), FSParam(pChannel->GetName(), strEnterPlayerName));

	return CR_TRUE;
}

MCommandResult XCmdHandler_ChatChannel::OnPlayerLeaveInfo(MCommand* pCommand, MCommandHandler* pHandler)
{
	// 커멘드 확인
	MUID uidChannel;
	wstring strLeavePlayerName;

	if (!pCommand->GetParameter(&uidChannel, 0, MPT_UID))			return CR_ERROR;
	if (!pCommand->GetParameter(strLeavePlayerName, 1, MPT_WSTR))	return CR_ERROR; 


	// 퇴장 플레이어 정보 출력
	XChatChannel* pChannel = info.chat_channel->Find(uidChannel);
	if (pChannel == NULL)	return CR_ERROR;

	global.ui->OnSystemMsg( XGetStr( L"CHATTING_CHANNELLEFTMEMBER"), FSParam(pChannel->GetName(), strLeavePlayerName));

	return CR_TRUE;
}

MCommandResult XCmdHandler_ChatChannel::OnPlayerKickInfo(MCommand* pCommand, MCommandHandler* pHandler)
{
	// 커멘드 확인
	MUID uidChannel;
	wstring strKickedPlayerName;

	if (!pCommand->GetParameter(&uidChannel, 0, MPT_UID))			return CR_ERROR;
	if (!pCommand->GetParameter(strKickedPlayerName, 1, MPT_WSTR))	return CR_ERROR;

	
	// 추방 플레이어 정보 출력
	XChatChannel* pChannel = info.chat_channel->Find(uidChannel);
	if (pChannel == NULL)	return CR_ERROR;

	global.ui->OnSystemMsg( XGetStr( L"CHATTING_CHANNELBANISHEDMBER"), FSParam(pChannel->GetName(), strKickedPlayerName));

	return CR_TRUE;
}
