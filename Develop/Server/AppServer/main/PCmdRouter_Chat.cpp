#include "StdAfx.h"
#include "PCmdRouter_Chat.h"
#include "MCommand.h"
#include "PChatChannel.h"
#include "PGlobal.h"
#include "PCommandCenter.h"
#include "CCommandTable_App.h"
#include "STransData_A2G.h"
#include "CTransData.h"
#include "STypes.h"
#include "SProxyPlayer.h"
#include "SProxyPlayerManager.h"
#include "PServer.h"
#include "PChatChannelManager.h"
#include "PChatPlayer.h"
#include "PGameServerObjectManager.h"


void PCmdRouter_Chat::PostEnterChannel(MUID uidTargetGameServer, CID cidPlayer, PChatChannel* pChannel)
{
	minet::MCommand* pNewCommand = MakeCmd_EnterChannel(cidPlayer, pChannel);
	VALID(pNewCommand);
	gsys.pCommandCenter->SendCommand(uidTargetGameServer, pNewCommand);
}
void PCmdRouter_Chat::RouteEnterChannelPlayerInfo(PChatChannel* pChannel, std::wstring& strEnterPlayerName, CID cidIgnoreToRoute)
{
	minet::MCommand* pNewCommand = MakeCmd_EnterChannelPlayerInfo(pChannel->GetUID(), strEnterPlayerName);
	VALID(pNewCommand);
	gsys.pCommandCenter->RouteToChannel(pChannel, pNewCommand, cidIgnoreToRoute);
}

void PCmdRouter_Chat::PostLeaveChannel(MUID uidTargetGameServer, CID cidPlayer, PChatChannel* pChannel)
{
	minet::MCommand* pNewCommand = MakeCmd_LeaveChannel(cidPlayer, pChannel);
	VALID(pNewCommand);
	gsys.pCommandCenter->SendCommand(uidTargetGameServer, pNewCommand);
}
void PCmdRouter_Chat::RouteLeaveChannelPlayerInfo(PChatChannel* pChannel, std::wstring& strEnterPlayerName)
{
	minet::MCommand* pNewCommand = MakeCmd_LeaveChannelPlayerInfo(pChannel->GetUID(), strEnterPlayerName);
	VALID(pNewCommand);
	gsys.pCommandCenter->RouteToChannel(pChannel, pNewCommand);
}

void PCmdRouter_Chat::PostKick(MUID uidTargetGameServer, CID cidKickedPlayer, MUID uidChannel)
{
	minet::MCommand* pNewCommand = MakeCmd_Kick(cidKickedPlayer, uidChannel);
	VALID(pNewCommand);
	gsys.pCommandCenter->SendCommand(uidTargetGameServer, pNewCommand);
}
void PCmdRouter_Chat::RouteKickPlayerInfo(PChatChannel* pChannel, std::wstring& strKickedPlayerName)
{
	minet::MCommand* pNewCommand = MakeCmd_KickPlayerInfo(pChannel->GetUID(), strKickedPlayerName);
	VALID(pNewCommand);
	gsys.pCommandCenter->RouteToChannel(pChannel, pNewCommand);
}
void PCmdRouter_Chat::PostBan(MUID uidTargetGameServer, CID cidBanedPlayer, MUID uidChannel)
{
	minet::MCommand* pNewCommand = MakeCmd_Ban(cidBanedPlayer, uidChannel);
	VALID(pNewCommand);
	gsys.pCommandCenter->SendCommand(uidTargetGameServer, pNewCommand);
}
void PCmdRouter_Chat::RouteBanPlayerInfo(PChatChannel* pChannel, std::wstring& strBanedPlayerName)
{
	minet::MCommand* pNewCommand = MakeCmd_BanPlayerInfo(pChannel->GetUID(), strBanedPlayerName);
	VALID(pNewCommand);
	gsys.pCommandCenter->RouteToChannel(pChannel, pNewCommand);
}
void PCmdRouter_Chat::PostUnbanRes(MUID uidTargetGameServer, CID cidPlayer, MUID uidChannel, std::wstring& strUnbanedPlayerName)
{
	minet::MCommand* pNewCommand = MakeCmd_UnbanRes(cidPlayer, uidChannel, strUnbanedPlayerName);
	VALID(pNewCommand);
	gsys.pCommandCenter->SendCommand(uidTargetGameServer, pNewCommand);
}
void PCmdRouter_Chat::RouteChangeHostInfo(PChatChannel* pChannel, std::wstring& strNewHostPlayerName)
{
	minet::MCommand* pNewCommand = MakeCmd_ChangeHostInfo(pChannel->GetUID(), strNewHostPlayerName);
	VALID(pNewCommand);
	gsys.pCommandCenter->RouteToChannel(pChannel, pNewCommand);
}


void PCmdRouter_Chat::RouteAllChannelInfo(MUID uidTargetServer)
{
	minet::MCommand* pNewCommand = MakeCmd_AllChannelInfo();
	if (NULL == pNewCommand) return;
	gsys.pCommandCenter->SendCommand(uidTargetServer, pNewCommand);
}
void PCmdRouter_Chat::RouteAddedChannelInfo(PChatChannel* pChannel)
{
	minet::MCommand* pNewCommand = MakeCmd_AddedChannelInfo(pChannel);
	VALID(pNewCommand);
	gsys.pCommandCenter->RouteToAll(pNewCommand);
}
void PCmdRouter_Chat::RouteDeletedChannelInfo(MUID uidChannel)
{
	minet::MCommand* pNewCommand = MakeCmd_DeletedChannelInfo(uidChannel);
	VALID(pNewCommand);
	gsys.pCommandCenter->RouteToAll(pNewCommand);
}
void PCmdRouter_Chat::RouteAllChatPlayerInfo(MUID uidTargetServer)
{
	minet::MCommand* pNewCommand = MakeCmd_AllChatPlayerInfo();
	if (NULL == pNewCommand) return;
	gsys.pCommandCenter->SendCommand(uidTargetServer, pNewCommand);
}
void PCmdRouter_Chat::RouteAddedChatPlayerInfo(MUID uidChannel, CID cidPlayer)
{
	minet::MCommand* pNewCommand = MakeCmd_AddedChatPlayerInfo(uidChannel, cidPlayer);
	VALID(pNewCommand);
	gsys.pCommandCenter->RouteToAll(pNewCommand);
}
void PCmdRouter_Chat::RouteDeletedChatPlayerInfo(MUID uidChannel, CID cidPlayer)
{
	minet::MCommand* pNewCommand = MakeCmd_DeletedChatPlayerInfo(uidChannel, cidPlayer);
	VALID(pNewCommand);
	gsys.pCommandCenter->RouteToAll(pNewCommand);
}
void PCmdRouter_Chat::RouteChatPlayerGameServerInfo(CID cidPlayer, int nGameServerID)
{
	minet::MCommand* pNewCommand = MakeCmd_ChatPlayerGameServerInfo(cidPlayer, nGameServerID);
	VALID(pNewCommand);
	gsys.pCommandCenter->RouteToAll(pNewCommand);
}


minet::MCommand* PCmdRouter_Chat::MakeCmd_EnterChannel(CID cidPlayer, PChatChannel* pChannel)
{
	TD_CHAT_CHANNEL_INFO tdCreatedChatChannelInfo;
	tdCreatedChatChannelInfo.uidChannel = pChannel->GetUID();
	tdCreatedChatChannelInfo.nChannelType = (uint8)pChannel->GetType();
	wcsncpy_s(tdCreatedChatChannelInfo.strChannelName, pChannel->GetName().c_str(), _TRUNCATE);
	tdCreatedChatChannelInfo.isOwner = cidPlayer == pChannel->GetOwnerCID();

	minet::MCommand* pNewCommand = gsys.pCommandCenter->MakeNewCommand(MPC_MSG_ENTER_CHANNEL
		, 2
		, NEW_INT(cidPlayer)
		, NEW_SINGLE_BLOB(&tdCreatedChatChannelInfo, sizeof(tdCreatedChatChannelInfo))
		);
	return pNewCommand;
}
minet::MCommand* PCmdRouter_Chat::MakeCmd_EnterChannelPlayerInfo(MUID uidChannel, std::wstring strName)
{
	minet::MCommand* pNewCommand = gsys.pCommandCenter->MakeNewCommand(MPC_MSG_ENTER_CHANNEL_PLAYER_INFO
		, 2
		, NEW_UID(uidChannel)
		, NEW_WSTR(strName.c_str())
		);
	return pNewCommand;
}

minet::MCommand* PCmdRouter_Chat::MakeCmd_LeaveChannel(CID cidPlayer, PChatChannel* pChannel)
{
	VALID_RET(pChannel != NULL, NULL);

	MUID uidChannel = pChannel->GetUID();

	minet::MCommand* pNewCommand = gsys.pCommandCenter->MakeNewCommand(MPC_MSG_LEAVE_CHANNEL
		, 2
		, NEW_INT(cidPlayer)
		, NEW_UID(uidChannel)
		);
	return pNewCommand;
}
minet::MCommand* PCmdRouter_Chat::MakeCmd_LeaveChannelPlayerInfo(MUID uidChannel, std::wstring strName)
{
	minet::MCommand* pNewCommand = gsys.pCommandCenter->MakeNewCommand(MPC_MSG_LEAVE_CHANNEL_PLAYER_INFO
		, 2
		, NEW_UID(uidChannel)
		, NEW_WSTR(strName.c_str())
		);
	return pNewCommand;
}

minet::MCommand* PCmdRouter_Chat::MakeCmd_Kick(CID cidKickedPlayer, MUID uidChannel)
{
	minet::MCommand* pNewCommand = gsys.pCommandCenter->MakeNewCommand(MPC_MSG_KICK
		, 2
		, NEW_INT(cidKickedPlayer)
		, NEW_UID(uidChannel)
		);
	return pNewCommand;
}
minet::MCommand* PCmdRouter_Chat::MakeCmd_KickPlayerInfo(MUID uidChannel, std::wstring& strKickedPlayerName)
{
	minet::MCommand* pNewCommand = gsys.pCommandCenter->MakeNewCommand(MPC_MSG_KICK_PLAYER_INFO
		, 2
		, NEW_UID(uidChannel)
		, NEW_WSTR(strKickedPlayerName.c_str())
		);
	return pNewCommand;
}
minet::MCommand* PCmdRouter_Chat::MakeCmd_Ban(CID cidBanedPlayer, MUID uidChannel)
{
	minet::MCommand* pNewCommand = gsys.pCommandCenter->MakeNewCommand(MPC_MSG_BAN
		, 2
		, NEW_INT(cidBanedPlayer)
		, NEW_UID(uidChannel)
		);
	return pNewCommand;
}
minet::MCommand* PCmdRouter_Chat::MakeCmd_BanPlayerInfo(MUID uidChannel, std::wstring& strBanedPlayerName)
{
	minet::MCommand* pNewCommand = gsys.pCommandCenter->MakeNewCommand(MPC_MSG_BAN_PLAYER_INFO
		, 2
		, NEW_UID(uidChannel)
		, NEW_WSTR(strBanedPlayerName.c_str())
		);
	return pNewCommand;
}
minet::MCommand* PCmdRouter_Chat::MakeCmd_UnbanRes(CID cidPlayer, MUID uidChannel, std::wstring& strUnbanedPlayerName)
{
	minet::MCommand* pNewCommand = gsys.pCommandCenter->MakeNewCommand(MPC_MSG_UNBAN_RES
		, 3
		, NEW_INT(cidPlayer)
		, NEW_UID(uidChannel)
		, NEW_WSTR(strUnbanedPlayerName.c_str())
		);
	return pNewCommand;
}
minet::MCommand* PCmdRouter_Chat::MakeCmd_ChangeHostInfo(MUID uidChannel, std::wstring& strNewHostPlayerName)
{
	minet::MCommand* pNewCommand = gsys.pCommandCenter->MakeNewCommand(MPC_MSG_CHANGE_HOST_INFO
		, 2
		, NEW_UID(uidChannel)
		, NEW_WSTR(strNewHostPlayerName.c_str())
		);
	return pNewCommand;
}


minet::MCommand* PCmdRouter_Chat::MakeCmd_AllChannelInfo()
{
	if (0 == gmgr.pChatChannelManager->GetChannelCount()) return NULL;

	vector<TDAG_CHAT_CHANNEL_INFO> vecTDChannel;
	PChatChannelManager::PChatChannelMapByUID::iterator itr = gmgr.pChatChannelManager->GetChannelMap().begin();
	for (; itr != gmgr.pChatChannelManager->GetChannelMap().end(); ++itr)
	{
		PChatChannel* pChatChannel = (*itr).second;

		TDAG_CHAT_CHANNEL_INFO tdChannelInfo;
		tdChannelInfo.uidChannel = pChatChannel->GetUID();
		wcsncpy_s(tdChannelInfo.strChannelName, pChatChannel->GetName().c_str(), _TRUNCATE);

		vecTDChannel.push_back(tdChannelInfo);
	}

	minet::MCommand* pNewCommand = gsys.pCommandCenter->MakeNewCommand(MPC_MSG_ADDED_CHANNEL_INFO
		, 1
		, NEW_BLOB(vecTDChannel)
		);
	return pNewCommand;
}
minet::MCommand* PCmdRouter_Chat::MakeCmd_AddedChannelInfo(PChatChannel* pChannel)
{
	VALID_RET(pChannel, NULL);

	TDAG_CHAT_CHANNEL_INFO tdChannelInfo;

	tdChannelInfo.uidChannel = pChannel->GetUID();
	wcsncpy_s(tdChannelInfo.strChannelName, pChannel->GetName().c_str(), _TRUNCATE);

	minet::MCommand* pNewCommand = gsys.pCommandCenter->MakeNewCommand(MPC_MSG_ADDED_CHANNEL_INFO
		, 1
		, NEW_BLOB(&tdChannelInfo, sizeof(tdChannelInfo), 1)
		);
	return pNewCommand;
}
minet::MCommand* PCmdRouter_Chat::MakeCmd_DeletedChannelInfo(MUID uidChannel)
{
	minet::MCommand* pNewCommand = gsys.pCommandCenter->MakeNewCommand(MPC_MSG_DELETED_CHANNEL_INFO
		, 1
		, NEW_UID(uidChannel)
		);
	return pNewCommand;
}

minet::MCommand* PCmdRouter_Chat::MakeCmd_AllChatPlayerInfo()
{
	if (0 == gmgr.pChatChannelManager->GetChatPlayerMap().size()) return NULL;

	vector<TDAG_CHAT_CHANNEL_PLAYER_INFO> vecTDChatPlayer;
	PChatChannelManager::PChatPlayerMapByCID::iterator itr = gmgr.pChatChannelManager->GetChatPlayerMap().begin();
	for (; itr != gmgr.pChatChannelManager->GetChatPlayerMap().end(); ++itr)
	{
		PChatPlayer* pChatPlayer = (*itr).second;

		vector<MUID> vecChannelUID;
		pChatPlayer->GetChannelList(vecChannelUID);
		for (int i = 0; i < (int)vecChannelUID.size(); ++i)
		{
			TDAG_CHAT_CHANNEL_PLAYER_INFO tdPlayerInfo;
			tdPlayerInfo.uidChannel = vecChannelUID[i];
			tdPlayerInfo.nCID = pChatPlayer->GetCID();
			tdPlayerInfo.nGameServer = pChatPlayer->GetGameServerID();

			vecTDChatPlayer.push_back(tdPlayerInfo);
		}
	}

	minet::MCommand* pNewCommand = gsys.pCommandCenter->MakeNewCommand(MPC_MSG_ADDED_PLAYER_INFO
		, 1
		, NEW_BLOB(vecTDChatPlayer)
		);
	return pNewCommand;
}
minet::MCommand* PCmdRouter_Chat::MakeCmd_AddedChatPlayerInfo(MUID uidChannel, CID cidPlayer)
{
	TDAG_CHAT_CHANNEL_PLAYER_INFO tdChannelPlayerInfo;

	SProxyPlayer* pPlayer = gsys.pServer->GetPlayerManager()->FindPlayer(cidPlayer);
	VALID_RET(pPlayer, NULL);

	tdChannelPlayerInfo.uidChannel = uidChannel;
	tdChannelPlayerInfo.nCID = pPlayer->GetCID();
	tdChannelPlayerInfo.nGameServer = pPlayer->GetGameServerID();

	minet::MCommand* pNewCommand = gsys.pCommandCenter->MakeNewCommand(MPC_MSG_ADDED_PLAYER_INFO
		, 1
		, NEW_BLOB(&tdChannelPlayerInfo, sizeof(tdChannelPlayerInfo), 1)
		);
	return pNewCommand;
}
minet::MCommand* PCmdRouter_Chat::MakeCmd_DeletedChatPlayerInfo(MUID uidChannel, CID cidPlayer)
{
	minet::MCommand* pNewCommand = gsys.pCommandCenter->MakeNewCommand(MPC_MSG_DELETED_PLAYER_INFO
		, 2
		, NEW_UID(uidChannel)
		, NEW_INT(cidPlayer)
		);
	return pNewCommand;
}
minet::MCommand* PCmdRouter_Chat::MakeCmd_ChatPlayerGameServerInfo(CID cidPlayer, int nGameServerID)
{
	minet::MCommand* pNewCommand = gsys.pCommandCenter->MakeNewCommand(MPC_MSG_PLAYER_GAMESERVER_INFO
		, 2
		, NEW_INT(cidPlayer)
		, NEW_INT(nGameServerID)
		);
	return pNewCommand;
}