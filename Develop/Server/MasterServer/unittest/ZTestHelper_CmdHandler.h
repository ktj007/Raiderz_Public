#pragma once

#include "CCommandTable_Master.h"
#include "CTransData.h"
#include "STransData.h"

class ZTestHelper_CmdHandler
{
public:
	static MCommandResult OnRecv_MLC_PLAYER_ADD_PLAYER_REQ(MockLink* pLoginServerLink, MUID uidPlayer, AID nAID, const wstring& strUserID)
	{
		return pLoginServerLink->OnRecv(MLC_PLAYER_ADD_PLAYER_REQ, 3, 
			NEW_UID(uidPlayer),
			NEW_INT(nAID),
			NEW_WSTR(strUserID.c_str()));
	}

	static MCommandResult OnRecv_MLC_PLAYER_PMANG_ADD_PLAYER_REQ(MockLink* pLoginServerLink, MUID uidPlayer, AID nAID, const wstring& strUserID, TD_PMANG_USER_DATA* pTdPmangUserData)
	{
		return pLoginServerLink->OnRecv(MLC_PLAYER_PMANG_ADD_PLAYER_REQ
			, 4
			, NEW_UID(uidPlayer)
			, NEW_INT(nAID)
			, NEW_WSTR(strUserID.c_str())
			, NEW_SINGLE_BLOB(*pTdPmangUserData));
	}

	static MCommandResult OnRecv_MLC_PLAYER_DELETE_PLAYER(MockLink* pLoginServerLink, AID nAID, const MUID& uidPlayer)
	{
		return pLoginServerLink->OnRecv(MLC_PLAYER_DELETE_PLAYER, 2, 
			NEW_INT(nAID),
			NEW_UID(uidPlayer));
	}

	static MCommandResult OnRecv_PlayerStateInWorldReq(MockLink* pGameServerLink, AID nAID, CID nCID, const MUID& uidPlayer, const wstring& strPlayerName, int nGID)
	{
		return pGameServerLink->OnRecv(MMC_PLAYER_STATE_IN_WORLD_REQ, 5, 
			NEW_INT(nAID),
			NEW_INT(nCID),
			NEW_UID(uidPlayer),
			NEW_WSTR(strPlayerName.c_str()),
			NEW_INT(nGID));
	}

	static MCommandResult OnRecv_MMC_PLAYER_IN_FIELD_NOTIFY(MockLink* pGameServerLink, CID nCID, const MUID& uidField, const int nFieldID)
	{
		return pGameServerLink->OnRecv(MMC_PLAYER_IN_FIELD_NOTIFY, 3, 
			NEW_INT(nCID),
			NEW_UID(uidField),
			NEW_INT(nFieldID));
	}


	// Gate 관련
	static MCommandResult OnRecv_MMC_GATE_ASK(MockLink* pGameServerLink, MUID uidRequester, TD_GATE_INFO tdGateInfo)
	{
		return pGameServerLink->OnRecv(MMC_GATE_ASK_SHARED_FIELD, 2
			, NEW_UID(uidRequester)
			, NEW_SINGLE_BLOB(&tdGateInfo, sizeof(TD_GATE_INFO)));
	}

	static MCommandResult OnRecv_MMC_GATE_ASK_DYNAMIC_FIELD(MockLink* pGameServerLink, MUID uidRequestPlayer, TD_GATE_FIELD_GROUP_INFO GateFieldGroupInfo)
	{
		return pGameServerLink->OnRecv(MMC_GATE_ASK_DYNAMIC_FIELD, 2,
			NEW_UID(uidRequestPlayer),
			NEW_SINGLE_BLOB(&GateFieldGroupInfo, sizeof(GateFieldGroupInfo)));
	}

	static MCommandResult OnRecv_MMC_GATE_PREPARE_CHANGE_CHANNEL_REQ(MockLink* pGameServerLink, MUID uidRequestPlayer, int nChannelID)
	{
		return pGameServerLink->OnRecv(MMC_GATE_PREPARE_CHANGE_CHANNEL_REQ, 2,
			NEW_UID(uidRequestPlayer),
			NEW_INT(nChannelID));
	}

	static MCommandResult OnRecv_MMC_GATE_CANCEL_CHANGE_CHANNEL(MockLink* pGameServerLink, MUID uidRequestPlayer)
	{
		return pGameServerLink->OnRecv(MMC_GATE_CANCEL_CHANGE_CHANNEL, 1,
			NEW_UID(uidRequestPlayer));
	}

	static MCommandResult OnRecv_MMC_GATE_ASK_CHANGE_CHANNEL(MockLink* pGameServerLink, MUID uidRequestPlayer, TD_GATE_INFO tdGateInfo)
	{
		return pGameServerLink->OnRecv(MMC_GATE_ASK_CHANGE_CHANNEL, 2,
			NEW_UID(uidRequestPlayer),
			NEW_SINGLE_BLOB(&tdGateInfo, sizeof(TD_GATE_INFO)));
	}

	// 네트웍 통신
	static MCommandResult OnRecv_MLC_COMM_REQUEST_LOGIN_GAME_SERVER(MockLink* pLoginServerLink, MUID uidRequestPlayer, TD_LOGIN_ACCOUNT_INFO LoginAccountInfo, TD_LOGIN_FIELD_INFO LoginFieldInfo)
	{
		return pLoginServerLink->OnRecv(MLC_COMM_REQUEST_LOGIN_GAME_SERVER,
			3,
			NEW_UID(uidRequestPlayer),
			NEW_SINGLE_BLOB(&LoginAccountInfo, sizeof(LoginAccountInfo)),
			NEW_SINGLE_BLOB(&LoginFieldInfo, sizeof(LoginFieldInfo)));
	}

	static MCommandResult OnRecv_MMC_COMM_RESPONSE_LOGIN_GAME_SERVER(MockLink* pGameServerLink, MUID uidRequestServer, MUID uidRequestPlayer, int nResult, TD_LOGIN_GAME_SERVER_INFO LoginGameServerInfo)
	{
		return pGameServerLink->OnRecv(MMC_COMM_RESPONSE_LOGIN_GAME_SERVER, 
			4,
			NEW_UID(uidRequestServer),
			NEW_UID(uidRequestPlayer), 
			NEW_INT(nResult), 
			NEW_SINGLE_BLOB(&LoginGameServerInfo, sizeof(LoginGameServerInfo)));
	}
};