#include "stdafx.h"
#include "GCmdHandler_App_Msg.h"
#include "CCommandTable_App.h"
#include "GGlobal.h"
#include "GMsgCommandFacade.h"
#include "STypes.h"
#include "CTransData.h"
#include "GPlayerObjectManager.h"
#include "GEntityPlayer.h"
#include "GClientChatChannelRouter.h"
#include "STransData_A2G.h"
#include "GProxyChatChannel.h"
#include "GAppServerFacade.h"


GCmdHandler_App_Msg::GCmdHandler_App_Msg(MCommandCommunicator* pCC) : MCommandHandler(pCC)
{
	SetCmdHandler(MPC_MSG_RES,							OnMsgRes);
	SetCmdHandler(MPC_MSG_ENTER_CHANNEL,				OnMsgEnterChannel);
	SetCmdHandler(MPC_MSG_ENTER_CHANNEL_PLAYER_INFO,	OnMsgEnterChannelPlayerInfo);
	SetCmdHandler(MPC_MSG_LEAVE_CHANNEL,				OnMsgLeaveChannel);
	SetCmdHandler(MPC_MSG_LEAVE_CHANNEL_PLAYER_INFO,	OnMsgLeaveChannelPlayerInfo);
	SetCmdHandler(MPC_MSG_KICK,							OnMsgKick);
	SetCmdHandler(MPC_MSG_KICK_PLAYER_INFO,				OnMsgKickPlayerInfo);
	SetCmdHandler(MPC_MSG_BAN,							OnMsgBan);
	SetCmdHandler(MPC_MSG_BAN_PLAYER_INFO,				OnMsgBanPlayerInfo);
	SetCmdHandler(MPC_MSG_UNBAN_RES,					OnMsgUnbanRes);
	SetCmdHandler(MPC_MSG_CHANGE_HOST_INFO,				OnMsgChangeHostInfo);

	SetCmdHandler(MPC_MSG_ADDED_CHANNEL_INFO,			OnMsgAddedChannelInfo);
	SetCmdHandler(MPC_MSG_DELETED_CHANNEL_INFO,			OnMsgDeletedChannelInfo);
	SetCmdHandler(MPC_MSG_ADDED_PLAYER_INFO,			OnMsgAddedPlayerInfo);
	SetCmdHandler(MPC_MSG_DELETED_PLAYER_INFO,			OnMsgDeletedPlayerInfo);
	SetCmdHandler(MPC_MSG_PLAYER_GAMESERVER_INFO,		OnMsgPlayerGameServerInfo);
}

MCommandResult GCmdHandler_App_Msg::OnMsgRes(MCommand* pCommand, MCommandHandler* pHandler)
{
	// 파싱 및 채팅 처리 모두 담당
	if (!gsys.pMsgCommandFacade->ResponseMessage(pCommand))	return CR_ERROR;

	return CR_TRUE;
}

MCommandResult GCmdHandler_App_Msg::OnMsgEnterChannel(MCommand* pCommand, MCommandHandler* pHandler)
{
	CID cidResPlayer;
	TD_CHAT_CHANNEL_INFO tdChannelInfo;

	if (!pCommand->GetParameter(&cidResPlayer,		0, MPT_INT))			return CR_ERROR;
	if (!pCommand->GetParameter(&tdChannelInfo,	1, MPT_SINGLE_BLOB))	return CR_ERROR;

	// 수신자 확인	
	GEntityPlayer* pPlayer = gmgr.pPlayerObjectManager->GetEntity(cidResPlayer);
	if (pPlayer == NULL)	return CR_TRUE;

	// 전달
	MUID uidPlayer = pPlayer->GetUID();
	GClientChatChannelRouter router;
	router.EnterChannelRes(uidPlayer, tdChannelInfo);

	return CR_TRUE;
}

MCommandResult GCmdHandler_App_Msg::OnMsgEnterChannelPlayerInfo(MCommand* pCommand, MCommandHandler* pHandler)
{
	MUID uidChannel;
	wstring strPlayerName;

	if (!pCommand->GetParameter(&uidChannel,	0, MPT_UID))	return CR_ERROR;
	if (!pCommand->GetParameter(strPlayerName,	1, MPT_WSTR))	return CR_ERROR;

	// 전달
	GClientChatChannelRouter router;
	router.EnterPlayerInfo(uidChannel, strPlayerName);

	return CR_TRUE;
}

MCommandResult GCmdHandler_App_Msg::OnMsgLeaveChannel(MCommand* pCommand, MCommandHandler* pHandler)
{
	CID cidResPlayer;
	MUID uidChannel;

	if (!pCommand->GetParameter(&cidResPlayer,	0, MPT_INT))	return CR_ERROR;
	if (!pCommand->GetParameter(&uidChannel,	1, MPT_UID))	return CR_ERROR;

	// 수신자 확인
	GEntityPlayer* pPlayer = gmgr.pPlayerObjectManager->GetEntity(cidResPlayer);
	if (pPlayer == NULL)	return CR_TRUE;

	// 전달
	MUID uidPlayer = pPlayer->GetUID();
	GClientChatChannelRouter router;
	router.LeaveChannelRes(uidPlayer, uidChannel);

	return CR_TRUE;
}
MCommandResult GCmdHandler_App_Msg::OnMsgLeaveChannelPlayerInfo(MCommand* pCommand, MCommandHandler* pHandler)
{
	MUID uidChannel;
	wstring strPlayerName;

	if (!pCommand->GetParameter(&uidChannel,	0, MPT_UID))	return CR_ERROR;
	if (!pCommand->GetParameter(strPlayerName,	1, MPT_WSTR))	return CR_ERROR;

	// 전달
	GClientChatChannelRouter router;
	router.LeavePlayerInfo(uidChannel, strPlayerName);

	return CR_TRUE;
}

MCommandResult GCmdHandler_App_Msg::OnMsgKick(MCommand* pCommand, MCommandHandler* pHandler)
{
	CID cidResPlayer;
	MUID uidChannel;

	if (!pCommand->GetParameter(&cidResPlayer,	0, MPT_INT))	return CR_ERROR;
	if (!pCommand->GetParameter(&uidChannel,	1, MPT_UID))	return CR_ERROR;

	// 수신자 확인
	GEntityPlayer* pPlayer = gmgr.pPlayerObjectManager->GetEntity(cidResPlayer);
	if (pPlayer == NULL)	return CR_TRUE;

	// 전달
	MUID uidPlayer = pPlayer->GetUID();
	GClientChatChannelRouter router;
	router.KickPlayerRes(uidPlayer, uidChannel);

	return CR_TRUE;
}
MCommandResult GCmdHandler_App_Msg::OnMsgKickPlayerInfo(MCommand* pCommand, MCommandHandler* pHandler)
{
	MUID uidChannel;
	wstring strPlayerName;

	if (!pCommand->GetParameter(&uidChannel,	0, MPT_UID))	return CR_ERROR;
	if (!pCommand->GetParameter(strPlayerName,	1, MPT_WSTR))	return CR_ERROR;

	// 전달
	GClientChatChannelRouter router;
	router.KickedPlayerInfo(uidChannel, strPlayerName);

	return CR_TRUE;
}

MCommandResult GCmdHandler_App_Msg::OnMsgBan(MCommand* pCommand, MCommandHandler* pHandler)
{
	CID cidResPlayer;
	MUID uidChannel;

	if (!pCommand->GetParameter(&cidResPlayer,	0, MPT_INT))	return CR_ERROR;
	if (!pCommand->GetParameter(&uidChannel,	1, MPT_UID))	return CR_ERROR;

	// 수신자 확인
	GEntityPlayer* pPlayer = gmgr.pPlayerObjectManager->GetEntity(cidResPlayer);
	if (pPlayer == NULL)	return CR_TRUE;

	// 전달
	MUID uidPlayer = pPlayer->GetUID();
	GClientChatChannelRouter router;
	router.BanPlayerRes(uidPlayer, uidChannel);

	return CR_TRUE;
}
MCommandResult GCmdHandler_App_Msg::OnMsgBanPlayerInfo(MCommand* pCommand, MCommandHandler* pHandler)
{
	MUID uidChannel;
	wstring strPlayerName;

	if (!pCommand->GetParameter(&uidChannel,	0, MPT_UID))	return CR_ERROR;
	if (!pCommand->GetParameter(strPlayerName,	1, MPT_WSTR))	return CR_ERROR;

	// 전달
	GClientChatChannelRouter router;
	router.KickedPlayerInfo(uidChannel, strPlayerName);

	return CR_TRUE;
}

MCommandResult GCmdHandler_App_Msg::OnMsgUnbanRes(MCommand* pCommand, MCommandHandler* pHandler)
{
	CID cidResPlayer;
	MUID uidChannel;
	wstring strTargetPlayerName;

	if (!pCommand->GetParameter(&cidResPlayer,			0, MPT_INT))	return CR_ERROR;
	if (!pCommand->GetParameter(&uidChannel,			1, MPT_UID))	return CR_ERROR;
	if (!pCommand->GetParameter(strTargetPlayerName,	2, MPT_WSTR))	return CR_ERROR;

	// 수신자 확인
	GEntityPlayer* pPlayer = gmgr.pPlayerObjectManager->GetEntity(cidResPlayer);
	if (pPlayer == NULL)	return CR_TRUE;

	// 전달
	MUID uidPlayer = pPlayer->GetUID();
	GClientChatChannelRouter router;
	router.UnbanPlayerRes(uidPlayer, uidChannel, strTargetPlayerName);

	return CR_TRUE;
}

MCommandResult GCmdHandler_App_Msg::OnMsgChangeHostInfo(MCommand* pCommand, MCommandHandler* pHandler)
{
	MUID uidChannel;
	wstring strPlayerName;

	if (!pCommand->GetParameter(&uidChannel,	0, MPT_UID))	return CR_ERROR;
	if (!pCommand->GetParameter(strPlayerName,	1, MPT_WSTR))	return CR_ERROR;

	// 전달
	GClientChatChannelRouter router;
	router.ChangeHostRes(uidChannel, strPlayerName);

	return CR_TRUE;
}

MCommandResult GCmdHandler_App_Msg::OnMsgAddedChannelInfo(MCommand* pCommand, MCommandHandler* pHandler)
{
	// 커맨드 확인
	int nCount;
	TDAG_CHAT_CHANNEL_INFO* pChannelInfo;
	if (!pCommand->GetBlob(pChannelInfo, nCount, 0))	return CR_ERROR;

	// 채널 등록	
	GProxyChatChannelManager* pManager = gsys.pAppServerFacade->GetChatChannelManager();
	for (int i = 0; i < nCount; ++i)
	{
		pManager->AddChannel(pChannelInfo[i].uidChannel, pChannelInfo[i].strChannelName);	
	}

	return CR_TRUE;
}

MCommandResult GCmdHandler_App_Msg::OnMsgDeletedChannelInfo(MCommand* pCommand, MCommandHandler* pHandler)
{
	// 커맨드 확인
	MUID uidChannel;
	if (!pCommand->GetParameter(&uidChannel, 0, MPT_UID))	return CR_ERROR;

	// 채널 제거
	GProxyChatChannelManager* pManager = gsys.pAppServerFacade->GetChatChannelManager();
	pManager->RemoveChannel(uidChannel);

	return CR_TRUE;
}

MCommandResult GCmdHandler_App_Msg::OnMsgAddedPlayerInfo(MCommand* pCommand, MCommandHandler* pHandler)
{
	// 커맨드 확인
	int nCount;
	TDAG_CHAT_CHANNEL_PLAYER_INFO* pPlayerInfo;

	if (!pCommand->GetBlob(pPlayerInfo, nCount, 0))	return CR_ERROR;


	for (int i = 0; i < nCount; ++i)
	{
		// 현재 서버에 속해있는 플레이어라면, UID를 알아낸다.
		MUID uidPlayer = gmgr.pPlayerObjectManager->GetPlayerUID(pPlayerInfo[i].nCID, pPlayerInfo[i].nGameServer);

		// 플레이어 채널 입장
		GProxyChatChannelManager* pManager = gsys.pAppServerFacade->GetChatChannelManager();
		pManager->AddPlayer(pPlayerInfo[i].uidChannel, pPlayerInfo[i].nCID, pPlayerInfo[i].nGameServer, uidPlayer);
	}

	return CR_TRUE;
}

MCommandResult GCmdHandler_App_Msg::OnMsgDeletedPlayerInfo(MCommand* pCommand, MCommandHandler* pHandler)
{
	// 커맨드 확인
	MUID uidChannel;
	int nCID;

	if (!pCommand->GetParameter(&uidChannel, 0, MPT_UID))	return CR_ERROR;
	if (!pCommand->GetParameter(&nCID, 1, MPT_INT))			return CR_ERROR;


	// 플레이어 채널 퇴장
	GProxyChatChannelManager* pManager = gsys.pAppServerFacade->GetChatChannelManager();
	pManager->RemovePlayer(uidChannel, nCID);


	return CR_TRUE;
}

MCommandResult GCmdHandler_App_Msg::OnMsgPlayerGameServerInfo(MCommand* pCommand, MCommandHandler* pHandler)
{
	// 커맨드 확인
	int nCID;
	int nGameServerID;
	if (!pCommand->GetParameter(&nCID,			0, MPT_INT))	return CR_ERROR;
	if (!pCommand->GetParameter(&nGameServerID,	1, MPT_INT))	return CR_ERROR;

	// 현재 서버에 속해있는 플레이어라면, UID를 알아낸다.
	MUID uidPlayer = gmgr.pPlayerObjectManager->GetPlayerUID(nCID, nGameServerID);

	// 플레이어 정보 갱신
	GProxyChatChannelManager* pManager = gsys.pAppServerFacade->GetChatChannelManager();
	pManager->UpdatePlayerInLocal(nCID, nGameServerID, uidPlayer);

	return CR_TRUE;
}
