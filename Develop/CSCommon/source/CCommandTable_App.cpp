#include "stdafx.h"
#include "CCommandTable_App.h"
#include "MCommandDesc.h"
#include "MCommand.h"

using namespace minet;

void AddApp2GameCommandTable()
{
	BEGIN_CMD_DESC();

	C(MPC_COMM_REQUEST_LOGIN,					MCDT_SERVER2SERVER, MCF_C2S, "Request Login")
												P(MPT_WSTR,	"ServerName");
												P(MPT_INT,	"ServerType - SServerType")
												P(MPT_INT,	"WorldID")
												P(MPT_INT,	"ServerID")
												P(MPT_INT,	"CommandVersion");

	C(MPC_COMM_RESPONSE_LOGIN,					MCDT_SERVER2SERVER, MCF_S2C, "Response Login")
												P(MPT_INT,	"nResult")
												P(MPT_UID,	"AllocServerUID")

	C(MPC_MSG_REQ,								MCDT_SERVER2SERVER, MCF_C2S, "Request Chatting Message")
												P(MPT_INT,	"eMsgType")
												P(MPT_WSTR,	"msg")
												P(MPT_BLOB,	"SenderInfo")
												P(MPT_BLOB, "ReceiverInfo")
												P(MPT_INT,	"nAdditional")

	C(MPC_MSG_RES,								MCDT_SERVER2SERVER, MCF_S2C, "Chatting Message")
												P(MPT_INT,	"eMsgType")
												P(MPT_WSTR,	"msg")
												P(MPT_BLOB,	"SenderInfo")
												P(MPT_BLOB, "ReceiverInfo")
												P(MPT_INT,	"nAdditional")


	C(MPC_MSG_CREATE_PRIVATE_CHANNEL_REQ,		MCDT_SERVER2SERVER, MCF_C2S, "Request Create Private Chatting Channel")
												P(MPT_INT,	"RequestPlayerCID");
												P(MPT_WSTR,	"ChannelName");

	//////////////////////////////////////////////////////////////////////////
	/// 동기화 정보
	C(MPC_MSG_ADDED_CHANNEL_INFO,				MCDT_SERVER2SERVER, MCF_S2C, "Added Chatting Channel Info")
												P(MPT_BLOB,	"TD_CHAT_CHANNEL_INFO");

	C(MPC_MSG_DELETED_CHANNEL_INFO,				MCDT_SERVER2SERVER, MCF_S2C, "Deleted Chatting Channel Info")
												P(MPT_UID,	"ChannelUID");

	C(MPC_MSG_ADDED_PLAYER_INFO,				MCDT_SERVER2SERVER, MCF_S2C, "Chatting Channel Added Player Info")
												P(MPT_BLOB,	"TDAG_CHAT_CHANNEL_PLAYER_INFO");

	C(MPC_MSG_DELETED_PLAYER_INFO,				MCDT_SERVER2SERVER, MCF_S2C, "Chatting Channel Deleted Player Info")
												P(MPT_UID,	"ChannelUID");
												P(MPT_INT,	"PlayerCID");

	C(MPC_MSG_PLAYER_GAMESERVER_INFO,			MCDT_SERVER2SERVER, MCF_S2C, "Chatting Channel Player GameServer Info")
												P(MPT_INT,	"CID");
												P(MPT_INT,	"GameServerID");
	//////////////////////////////////////////////////////////////////////////


	//////////////////////////////////////////////////////////////////////////
	/// 채팅 기능
	C(MPC_MSG_ENTER_CHANNEL_REQ,				MCDT_SERVER2SERVER, MCF_C2S, "Request Enter Chatting Channel")
												P(MPT_INT,	"RequestPlayerCID");
												P(MPT_WSTR,	"ChannelName");

	C(MPC_MSG_ENTER_CHANNEL,					MCDT_SERVER2SERVER, MCF_S2C, "Send Enter Chatting Channel")
												P(MPT_INT,			"PlayerCID");
												P(MPT_SINGLE_BLOB,	"TD_CHAT_CHANNEL_INFO");

	C(MPC_MSG_ENTER_CHANNEL_PLAYER_INFO,		MCDT_SERVER2SERVER, MCF_S2C, "Notify Enter Chatting Channel Player Info")
												P(MPT_UID,	"TargetChannelUID");
												P(MPT_WSTR,	"EnterPlayerName");

	C(MPC_MSG_LEAVE_CHANNEL_REQ,				MCDT_SERVER2SERVER, MCF_C2S, "Request Leave Chatting Channel")
												P(MPT_INT,	"RequestPlayerCID");
												P(MPT_UID,	"ChannelUID");

	C(MPC_MSG_LEAVE_CHANNEL,					MCDT_SERVER2SERVER, MCF_S2C, "Send Leave Chatting Channel")
												P(MPT_INT,	"PlayerCID");
												P(MPT_UID,	"ChannelUID");

	C(MPC_MSG_LEAVE_CHANNEL_PLAYER_INFO,		MCDT_SERVER2SERVER, MCF_S2C, "Notify Leave Chatting Channel Player Info")
												P(MPT_UID,	"TargetChannelUID");
												P(MPT_WSTR,	"LeavePlayerName");


	C(MPC_MSG_KICK_REQ,							MCDT_SERVER2SERVER, MCF_C2S, "Request Kick Player")
												P(MPT_INT,	"RequestPlayerCID");
												P(MPT_UID,	"ChannelUID");
												P(MPT_WSTR,	"TargetPlayerName");

	C(MPC_MSG_KICK,								MCDT_SERVER2SERVER, MCF_S2C, "Send Kick Player")
												P(MPT_INT,	"TargetPlayerCID");
												P(MPT_UID,	"ChannelUID");

	C(MPC_MSG_KICK_PLAYER_INFO,					MCDT_SERVER2SERVER, MCF_S2C, "Notify Kick Player Info")
												P(MPT_UID,	"TargetChannelUID");
												P(MPT_WSTR,	"KickedPlayerName");


	C(MPC_MSG_BAN_REQ,							MCDT_SERVER2SERVER, MCF_C2S, "Request Ban Player")
												P(MPT_INT,	"RequestPlayerCID");
												P(MPT_UID,	"ChannelUID");
												P(MPT_WSTR,	"TargetPlayerName");

	C(MPC_MSG_BAN,								MCDT_SERVER2SERVER, MCF_S2C, "Send Ban Player")
												P(MPT_INT,	"TargetPlayerCID");
												P(MPT_UID,	"ChannelUID");

	C(MPC_MSG_BAN_PLAYER_INFO,					MCDT_SERVER2SERVER, MCF_S2C, "Notify Ban Player Info")
												P(MPT_UID,	"TargetChannelUID");
												P(MPT_WSTR,	"BanedPlayerName");


	C(MPC_MSG_UNBAN_REQ,						MCDT_SERVER2SERVER, MCF_C2S, "Request Unban Player")
												P(MPT_INT,	"RequestPlayerCID");
												P(MPT_UID,	"ChannelUID");
												P(MPT_WSTR,	"TargetPlayerName");

	C(MPC_MSG_UNBAN_RES,						MCDT_SERVER2SERVER, MCF_S2C, "Send Unban Player")
												P(MPT_INT,	"PlayerCID");
												P(MPT_UID,	"ChannelUID");
												P(MPT_WSTR,	"UnbanedPlayerName");


	C(MPC_MSG_CHANGE_HOST_REQ,					MCDT_SERVER2SERVER, MCF_C2S, "Request Change Host")
												P(MPT_INT,	"RequestPlayerCID");
												P(MPT_UID,	"ChannelUID");
												P(MPT_WSTR,	"NewHostName");

	C(MPC_MSG_CHANGE_HOST_INFO,					MCDT_SERVER2SERVER, MCF_S2C, "Notify Change Host Info")
												P(MPT_UID,	"TargetChannelUID");
												P(MPT_WSTR,	"NewHostName");
	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////
	/// 편지 시스템
	C(MPC_MAIL_ROUTE_NEW_MAIL_TO_RECEIVER,		MCDT_SERVER2SERVER, MCF_C2S, "Route New Mail To Receiver")
												P(MPT_INT,	"CID")
												P(MPT_SINGLE_BLOB,	"TD_MAIL_MAILBOX_MAIL_INFO")

	C(MPC_MAIL_TRANS_NEW_MAIL,					MCDT_SERVER2SERVER, MCF_S2C, "Trans New Mail Req")
												P(MPT_INT,	"CID")
												P(MPT_SINGLE_BLOB,	"TD_MAIL_MAILBOX_MAIL_INFO")

	C(MPC_MAIL_RETRY_TRANS_NEW_MAIL,			MCDT_SERVER2SERVER, MCF_C2S, "Trans New Mail Res")
												P(MPT_INT,	"CID")
												P(MPT_SINGLE_BLOB,	"TD_MAIL_MAILBOX_MAIL_INFO")
	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////
	/// 물품거래소
	C(MPC_GA_TRADEMARKET_ADD,					MCDT_SERVER2SERVER, MCF_C2S, "market item added")
												P(MPT_SINGLE_BLOB,	"TD_TRADEMARKETITEM")
	C(MPC_GA_TRADEMARKET_DEL,					MCDT_SERVER2SERVER, MCF_C2S, "market item deleted")
												P(MPT_INT,			"market item type")
												P(MPT_INT64,		"market item id")
	C(MPC_GA_TRADEMARKET_SEARCH,				MCDT_SERVER2SERVER, MCF_C2S, "requested search market item")
												P(MPT_INT,			"cid")
												P(MPT_BLOB,			"vector<int>")
	C(MPC_GA_TRADEMARKET_SEARCH_NEXT,			MCDT_SERVER2SERVER, MCF_C2S, "requested search next page")
												P(MPT_INT,			"cid")
	C(MPC_AG_TRADEMARKET_SEARCH_RESULT,			MCDT_SERVER2SERVER, MCF_S2C, "market item search result")
												P(MPT_INT,			"cid")
												P(MPT_BLOB,			"vector<TD_TRADEMARKETITEM>")
	//////////////////////////////////////////////////////////////////////////


	END_CMD_DESC();
};
