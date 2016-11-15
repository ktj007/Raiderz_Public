#include "StdAfx.h"
#include "PCmdHandler_Chat.h"
#include "MCommandCommunicator.h"
#include "CCommandTable_App.h"
#include "PChattingLogic.h"
#include "PGlobal.h"
#include "PMsgCommandFacade.h"


PCmdHandler_Chat::PCmdHandler_Chat(MCommandCommunicator* pCC) : MCommandHandler(pCC)
{
	SetCmdHandler(MPC_MSG_REQ,							OnChatMsgReq);

	SetCmdHandler(MPC_MSG_CREATE_PRIVATE_CHANNEL_REQ,	OnChatCreatePrivateChannelReq);

	SetCmdHandler(MPC_MSG_ENTER_CHANNEL_REQ,			OnChatEnterChannelReq);
	SetCmdHandler(MPC_MSG_LEAVE_CHANNEL_REQ,			OnChatLeaveChannelReq);
	SetCmdHandler(MPC_MSG_KICK_REQ,						OnChatKickPlayerReq);
	SetCmdHandler(MPC_MSG_BAN_REQ,						OnChatBanPlayerReq);
	SetCmdHandler(MPC_MSG_UNBAN_REQ,					OnChatUnbanPlayerReq);
	SetCmdHandler(MPC_MSG_CHANGE_HOST_REQ,				OnChatChangeHostReq);
}

MCommandResult PCmdHandler_Chat::OnChatMsgReq(MCommand* pCommand, MCommandHandler* pHandler)
{
	// 커멘드 파싱 및 처리 모두 담당
	if (!gsys.pMsgCommandFacade->RequestMessage(pCommand))	return CR_ERROR;

	return CR_TRUE;
}


MCommandResult PCmdHandler_Chat::OnChatCreatePrivateChannelReq(MCommand* pCommand, MCommandHandler* pHandler)
{
	CID		cidRequestPlayer;
	wstring	strChannelName;
	if (pCommand->GetParameter(&cidRequestPlayer,	0, MPT_INT)==false) return CR_ERROR;
	if (pCommand->GetParameter(strChannelName,		1, MPT_WSTR)==false) return CR_ERROR;

	PChattingLogic logicChatting;
	logicChatting.CreateChannel_Private(cidRequestPlayer, strChannelName);
	return CR_TRUE;
}

MCommandResult PCmdHandler_Chat::OnChatEnterChannelReq(MCommand* pCommand, MCommandHandler* pHandler)
{
	CID		cidRequestPlayer;
	wstring	strChannelName;
	if (pCommand->GetParameter(&cidRequestPlayer,	0, MPT_INT)==false) return CR_ERROR;
	if (pCommand->GetParameter(strChannelName,		1, MPT_WSTR)==false) return CR_ERROR;

	PChattingLogic logicChatting;
	logicChatting.RunChatModule(pCommand, CMT_ENTER_CHANNEL, cidRequestPlayer, strChannelName);
	return CR_TRUE;
}
MCommandResult PCmdHandler_Chat::OnChatLeaveChannelReq(MCommand* pCommand, MCommandHandler* pHandler)
{
	CID		cidRequestPlayer;
	MUID	uidChannel;
	if (pCommand->GetParameter(&cidRequestPlayer,	0, MPT_INT)==false) return CR_ERROR;
	if (pCommand->GetParameter(&uidChannel,			1, MPT_UID)==false) return CR_ERROR;

	PChattingLogic logicChatting;
	logicChatting.RunChatModule(pCommand, CMT_LEAVE_CHANNEL, cidRequestPlayer, uidChannel);
	return CR_TRUE;
}

MCommandResult PCmdHandler_Chat::OnChatKickPlayerReq(MCommand* pCommand, MCommandHandler* pHandler)
{
	CID		cidRequestPlayer;
	MUID	uidChannel;
	if (pCommand->GetParameter(&cidRequestPlayer,	0, MPT_INT)==false) return CR_ERROR;
	if (pCommand->GetParameter(&uidChannel,			1, MPT_UID)==false) return CR_ERROR;

	PChattingLogic logicChatting;
	logicChatting.RunChatModule(pCommand, CMT_KICK, cidRequestPlayer, uidChannel);
	return CR_TRUE;
}
MCommandResult PCmdHandler_Chat::OnChatBanPlayerReq(MCommand* pCommand, MCommandHandler* pHandler)
{
	CID		cidRequestPlayer;
	MUID	uidChannel;
	if (pCommand->GetParameter(&cidRequestPlayer,	0, MPT_INT)==false) return CR_ERROR;
	if (pCommand->GetParameter(&uidChannel,			1, MPT_UID)==false) return CR_ERROR;

	PChattingLogic logicChatting;
	logicChatting.RunChatModule(pCommand, CMT_BAN, cidRequestPlayer, uidChannel);
	return CR_TRUE;
}
MCommandResult PCmdHandler_Chat::OnChatUnbanPlayerReq(MCommand* pCommand, MCommandHandler* pHandler)
{
	CID		cidRequestPlayer;
	MUID	uidChannel;
	if (pCommand->GetParameter(&cidRequestPlayer,	0, MPT_INT)==false) return CR_ERROR;
	if (pCommand->GetParameter(&uidChannel,			1, MPT_UID)==false) return CR_ERROR;

	PChattingLogic logicChatting;
	logicChatting.RunChatModule(pCommand, CMT_UNBAN, cidRequestPlayer, uidChannel);
	return CR_TRUE;
}
MCommandResult PCmdHandler_Chat::OnChatChangeHostReq(MCommand* pCommand, MCommandHandler* pHandler)
{
	CID		cidRequestPlayer;
	MUID	uidChannel;
	if (pCommand->GetParameter(&cidRequestPlayer,	0, MPT_INT)==false) return CR_ERROR;
	if (pCommand->GetParameter(&uidChannel,			1, MPT_UID)==false) return CR_ERROR;

	PChattingLogic logicChatting;
	logicChatting.RunChatModule(pCommand, CMT_CHANGE_HOST, cidRequestPlayer, uidChannel);
	return CR_TRUE;
}
