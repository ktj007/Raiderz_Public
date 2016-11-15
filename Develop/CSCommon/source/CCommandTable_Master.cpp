#include "stdafx.h"
#include "CCommandTable_Master.h"
#include "MCommandDesc.h"
#include "MCommand.h"

using namespace minet;

void AddMaster2GameCommandTable()
{
	BEGIN_CMD_DESC();

	C(MMC_COMM_REQUEST_LOGIN_G2M,						MCDT_SERVER2SERVER, MCF_C2S, "Request Login")
														P(MPT_WSTR,	"ServerName");
														P(MPT_INT,	"ServerType - SServerType")
														P(MPT_INT,	"WorldID")
														P(MPT_INT,	"ServerID")
														P(MPT_INT,	"CommandVersion");

	C(MMC_COMM_RESPONSE_LOGIN_M2G,						MCDT_SERVER2SERVER, MCF_S2C, "Response Login")
														P(MPT_INT,	"nResult")
														P(MPT_UID,	"AllocServerUID")
														P(MPT_UCHAR,"ServerMode")
														P(MPT_BLOB,	"AssignedSharedField - TDMG_ASSIGNED_SHARED_FIELD")

	C(MMC_COMM_REQUEST_LOGIN_GAME_SERVER,				MCDT_SERVER2SERVER, MCF_S2C, "Request Login GameServer")
														P(MPT_UID,			"RequestServer")
														P(MPT_UID,  		"RequestID")
														P(MPT_UID,			"ConnectionKey")
														P(MPT_SINGLE_BLOB,	"LoginAccountInfo")

	C(MMC_COMM_RESPONSE_LOGIN_GAME_SERVER,				MCDT_SERVER2SERVER, MCF_C2S, "Response Login GameServer")
														P(MPT_UID,			"RequestServer")
														P(MPT_UID,  		"RequestID")
														P(MPT_INT,  		"nResult")
														P(MPT_SINGLE_BLOB,	"LoginGameServerInfo")

	C(MMC_COMM_REQUEST_RESERVE_MOVE_GAME_SERVER,		MCDT_SERVER2SERVER, MCF_C2S,	"Rsquest Reserve Move GameServer")
														P(MPT_INT,	"TargetGameServerID")
														P(MPT_UID,	"RequstPlayer")
														P(MPT_BLOB, "GateInfo")
														P(MPT_BLOB, "DynamicFieldEnterPos")
														P(MPT_BLOB,	"PlayerInfoData")
														P(MPT_BLOB, "CharaInfoData")
														P(MPT_BLOB, "EquipData")
														P(MPT_BLOB, "ItemInstanceData")
														P(MPT_BLOB, "TalentData")
														P(MPT_BLOB, "PaletteData")
														P(MPT_BLOB, "TalentCollTimeSec")
														P(MPT_BLOB, "BuffRemaindSec")
														P(MPT_BLOB, "QuestInstanceData")
														P(MPT_BLOB, "QuestInstanceDontDoingData")
														P(MPT_BLOB, "FactionData")
														P(MPT_BLOB, "SawnCutsceneList")
														P(MPT_BLOB, "RecipeList")
	C(MMC_COMM_RESPONSE_RESERVE_MOVE_GAME_SERVER,		MCDT_SERVER2SERVER, MCF_S2C,	"Response Reserve Move GameServer")
														P(MPT_UID,	"RequestPlayerUID")
														P(MPT_INT,	"nResult")
														P(MPT_BLOB,	"LoginGameServerInfo")
	C(MMC_COMM_REQUEST_READY_ENTER_MOVE_PLAYER,			MCDT_SERVER2SERVER, MCF_S2C,	"Request Ready Enter Move Player")
														P(MPT_UID,	"ConnectionKey")
														P(MPT_UID,	"RequestGameServer")
														P(MPT_UID,	"RequestPlayer")
														P(MPT_BLOB, "GateInfo")
														P(MPT_BLOB, "DynamicFieldEnterPos")
														P(MPT_BLOB, "PlayerInfoData")
														P(MPT_BLOB, "CharaInfoData")
														P(MPT_BLOB, "EquipData")
														P(MPT_BLOB, "ItemInstanceData")
														P(MPT_BLOB, "TalentData")
														P(MPT_BLOB, "PaletteData")
														P(MPT_BLOB, "TalentCollTimeSec")
														P(MPT_BLOB, "BuffRemaindSec")
														P(MPT_BLOB, "QuestInstanceData")
														P(MPT_BLOB, "QuestInstanceDontDoingData")
														P(MPT_BLOB, "FactionData")
														P(MPT_BLOB, "SawnCutsceneList")
														P(MPT_BLOB, "RecipeList")
	C(MMC_COMM_RESPONSE_READY_ENTER_MOVE_PLAYER,		MCDT_SERVER2SERVER, MCF_C2S,	"Response Ready Enter Move Player")
														P(MPT_UID,	"RequestGameServer")
														P(MPT_UID,	"RequestPlayerUID")
														P(MPT_INT,	"nResult")
														P(MPT_BLOB,	"LoginGameServerInfo")

	C(MMC_COMM_REQUEST_APP_SERVER_LIST,					MCDT_SERVER2SERVER, MCF_C2S, "Request AppServer List")
														// no parameter command
	
	C(MMC_COMM_RESPONSE_APP_SERVER_LIST,				MCDT_SERVER2SERVER, MCF_S2C, "Response AppServer List")
														P(MPT_BLOB,	"AppServerList")

	C(MMC_COMM_CANCEL_MOVE_GAME_SERVER,					MCDT_SERVER2SERVER, MCF_C2S, "Cancel Move Game Server")
														P(MPT_UID,	"uidRequestPlayer")

	C(MMC_COMM_CANCEL_RESERVED_MOVE_PLAYER,				MCDT_SERVER2SERVER, MCF_S2C, "Cancel Reserved Move Player")
														P(MPT_UID,	"uidConnectionKey")

	C(MMC_COMM_RESERVE_MOVE_LOGINSERVER_REQ,			MCDT_SERVER2SERVER, MCF_C2S, "Request Reserving Move To LoginServer From GameServer")
														P(MPT_UID,	"RequstPlayer")

	C(MMC_COMM_RESERVE_MOVE_LOGINSERVER_RES,			MCDT_SERVER2SERVER, MCF_S2C, "Response Reserving Move To LoginServer From GameServer")
														P(MPT_UID,	"RequstPlayer")
														P(MPT_INT,	"nResult")
														P(MPT_UID,	"uidConnectionKey")

	C(MMC_COMM_CANCEL_MOVE_LOGINSERVER,					MCDT_SERVER2SERVER, MCF_C2S, "Request Cancel Move To LoginServer From GameServer")
														P(MPT_UID,	"RequstPlayer")

	//////////////////////////////////////////////////////////////////////////

	C(MMC_PLAYER_DELETE_PLAYER,							MCDT_SERVER2SERVER, MCF_C2S, "Delete Player")
														P(MPT_INT,	"AID")
														P(MPT_UID,	"UID")

	C(MMC_PLAYER_DUPLICATED_PLAYER_LOGIN,				MCDT_SERVER2SERVER, MCF_S2C, "Duplicated Player Login")
														P(MPT_UID,	"PlayerUID")
														P(MPT_UID,	"ReservedConnectionKey")

	C(MMC_PLAYER_STATE_IN_WORLD_REQ,					MCDT_SERVER2SERVER, MCF_C2S, "Request Player State - In World")
														P(MPT_INT,	"AID")
														P(MPT_INT,	"CID")
														P(MPT_UID,	"UID")
														P(MPT_WSTR,	"Player Name")
														P(MPT_INT,	"GID")

	C(MMC_PLAYER_STATE_IN_WORLD_RES,					MCDT_SERVER2SERVER, MCF_S2C, "Response Player State - In World")
														P(MPT_INT,	"CID")
														P(MPT_INT,	"nResult")

	C(MMC_PLAYER_IN_FIELD_NOTIFY,						MCDT_SERVER2SERVER, MCF_C2S, "Player In Field")
														P(MPT_INT,	"CID")
														P(MPT_UID,	"uidField")
														P(MPT_INT,	"FieldID")


	//////////////////////////////////////////////////////////////////////////////////

	C(MMC_FIELD_ADD_SHARED_FIELD,						MCDT_SERVER2SERVER, MCF_S2C, "Add Shared Field")
														P(MPT_BLOB,	"FieldInfo")
														P(MPT_UINT,	"nChecksum");

	C(MMC_FIELD_DEL_SHARED_FIELD,						MCDT_SERVER2SERVER, MCF_S2C, "Del Shared Field")
														P(MPT_UID,	"uidField")
														P(MPT_UINT,	"nChecksum");

	C(MMC_FIELD_SHARED_FIELD_INFO_ALL,					MCDT_SERVER2SERVER, MCF_S2C, "All Field Info")
														P(MPT_BLOB,	"FieldInfo")
														P(MPT_UINT,	"nChecksum");

	C(MMC_FIELD_REQUEST_SHARED_FIELD_INFO_ALL,			MCDT_SERVER2SERVER, MCF_C2S, "Request All Field Info")
	
	C(MMC_FIELD_ADD_DYNAMIC_FIELD_GROUP,				MCDT_SERVER2SERVER, MCF_S2C, "Add Dynamic Field Group")
														P(MPT_SINGLE_BLOB,	"DynamicFieldGroupInfo")
														P(MPT_BLOB,			"DynamicFieldInfo")
														P(MPT_UINT,			"nChecksum");

	C(MMC_FIELD_REQUEST_DEL_DYNAMIC_FIELD_GROUP,		MCDT_SERVER2SERVER, MCF_C2S, "Request Del Dynamic Field Group")
														P(MPT_UID,	"uidDynamicFieldGroup")

	C(MMC_FIELD_DEL_DYNAMIC_FIELD_GROUP,				MCDT_SERVER2SERVER, MCF_S2C, "Del Dynamic Field Group")
														P(MPT_UID,	"uidDynamicFieldGroup")
														P(MPT_UINT,	"nChecksum");

	C(MMC_FIELD_REQUEST_DYNAMIC_FIELD_GROUP_INFO_ALL,	MCDT_SERVER2SERVER, MCF_C2S, "Request All Dynamic Field Group Info")
	
	C(MMC_FIELD_DYNAMIC_FIELD_GROUP_INFO_ALL,			MCDT_SERVER2SERVER, MCF_S2C, "All Dynamic Field Group Info")
														P(MPT_BLOB,	"DynamicFieldGroupInfos")
														P(MPT_BLOB,	"DynamicFieldInfos")
														P(MPT_UINT,	"nChecksum");

	C(MMC_FIELD_UPDATE_PROXY_FIELD_INFO,				MCDT_SERVER2SERVER, MCF_S2C, "Update Field Info")
														P(MPT_UID,			"uidField")
														P(MPT_SINGLE_BLOB,	"FieldUpdatingInfo")
	
	//////////////////////////////////////////////////////////////////////////

	C(MMC_GATE_ASK_SHARED_FIELD,						MCDT_SERVER2SERVER, MCF_C2S, "Ask Gate -  Shared Field")
														P(MPT_UID,			"RequestPlayer")
														P(MPT_SINGLE_BLOB,	"GateInfo")

	C(MMC_GATE_ASK_DYNAMIC_FIELD,						MCDT_SERVER2SERVER, MCF_C2S, "Ask Gate -  Dynamic Field")
														P(MPT_UID,			"RequestPlayer")
														P(MPT_SINGLE_BLOB,	"GateFieldGroupInfo")

	C(MMC_GATE_RES,										MCDT_SERVER2SERVER, MCF_S2C, "Response Gate")
														P(MPT_UID,	"RequestPlayer")
														P(MPT_INT,	"nResponseGateType")
														P(MPT_BLOB,	"GateInfo")

	C(MMC_GATE_MOVE_SERVER,								MCDT_SERVER2SERVER, MCF_S2C, "Gate MoveServer")
														P(MPT_UID,			"RequestPlayer")
														P(MPT_INT,			"TargetGameServerID")
														P(MPT_SINGLE_BLOB,	"GateInfo")

	C(MMC_GATE_PREPARE_CHANGE_CHANNEL_REQ,				MCDT_SERVER2SERVER, MCF_C2S, "Request Prepare Change Channel")
														P(MPT_UID,	"RequestPlayer")
														P(MPT_INT,	"Dest ChannelID")

	C(MMC_GATE_PREPARE_CHANGE_CHANNEL_RES,				MCDT_SERVER2SERVER, MCF_S2C, "Response Prepare Change Channel")
														P(MPT_UID,	"RequestPlayer")
														P(MPT_INT,	"Result")
														P(MPT_INT,	"Dest ChannelID")

	C(MMC_GATE_CANCEL_CHANGE_CHANNEL,					MCDT_SERVER2SERVER, MCF_C2S, "Cancel Change Channel")
														P(MPT_UID,	"RequestPlayer")

	C(MMC_GATE_ASK_CHANGE_CHANNEL,						MCDT_SERVER2SERVER, MCF_C2S, "Ask Gate - Change Channel")
														P(MPT_UID,			"RequestPlayer")
														P(MPT_SINGLE_BLOB,	"GateInfo")

	// Party //////////////////////////////////////////////////////////////////////////

	C(MMC_PARTY_ADD,									MCDT_SERVER2SERVER, MCF_S2C, "Add Party")
														P(MPT_UID, "uidParty")
														P(MPT_UID, "uidLeader")
														P(MPT_WSTR, "strLeaderName")
														P(MPT_INT, "nLeaderCID")

	C(MMC_PARTY_REMOVE,									MCDT_SERVER2SERVER, MCF_S2C, "Remove Party")
														P(MPT_UID, "uidParty")

	C(MMC_PARTY_ADD_MEMBER,								MCDT_SERVER2SERVER, MCF_S2C, "Add Party Member")
														P(MPT_UID, "uidParty")
														P(MPT_UID, "uidMember")
														P(MPT_WSTR, "strMemberName")
														P(MPT_INT, "nMemberCID")

	C(MMC_PARTY_REMOVE_MEMBER,							MCDT_SERVER2SERVER, MCF_S2C, "Remove Party Member")
														P(MPT_UID, "uidParty")
														P(MPT_UID, "uidMember")
	
	// Party New Command /////////////////////////////////////////////////////////////////////////////////////

	C(MMC_PARTY_INVITE_REQ,								MCDT_SERVER2SERVER, MCF_C2S, "Party Invite Req")
														P(MPT_UID, "uidTargetPlayer")
														P(MPT_UID, "uidRequestPlayer")

	C(MMC_PARTY_INVITE_RES,								MCDT_SERVER2SERVER, MCF_S2C, "Party Invite Res")
														P(MPT_UID, "uidRequestPlayer")
														P(MPT_UID, "uidTargetPlayer")
														P(MPT_INT, "nResult")

	C(MMC_PARTY_ACCEPT_REQ,								MCDT_SERVER2SERVER, MCF_S2C, "Party Accept Req")
														P(MPT_UID, "uidTargetPlayer")
														P(MPT_UID, "uidRequestPlayer")
														P(MPT_WSTR, "szRequestPlayerName")

	C(MMC_PARTY_ACCEPT_RES,								MCDT_SERVER2SERVER, MCF_C2S, "Party Accept Res")
														P(MPT_UID, "uidRequestPlayer")
														P(MPT_UID, "uidTargetPlayer")
														P(MPT_INT, "nResult")

	C(MMC_PARTY_ACCEPT_CANCEL,							MCDT_SERVER2SERVER, MCF_S2C, "Party Accept Cancel")
														P(MPT_UID, "uidTargetPlayer")
														P(MPT_INT, "nResult")

	C(MMC_PARTY_UPDATE,									MCDT_SERVER2SERVER, MCF_C2S, "Party Update")
														P(MPT_UID, "uidParty")
														P(MPT_BLOB, "PartyMember")
														P(MPT_BLOB, "vecBuff")

	C(MMC_PARTY_SYNC,									MCDT_SERVER2SERVER, MCF_S2C, "Party Sync")
														P(MPT_BLOB, "Party")
														P(MPT_BLOB, "vecPartyMember")
														P(MPT_BLOB, "vecBuff1")
														P(MPT_BLOB, "vecBuff2")
														P(MPT_BLOB, "vecBuff3")
														P(MPT_BLOB, "vecBuff4")
														P(MPT_BLOB, "vecBuff5")


	C(MMC_PARTY_UPDATE_MEMBER,							MCDT_SERVER2SERVER, MCF_C2S, "Party Update Member")
														P(MPT_UID, "uidParty")
														P(MPT_BLOB, "PartyMember")
														P(MPT_BLOB, "vecBuff")

	C(MMC_PARTY_MEMBER_SYNC,							MCDT_SERVER2SERVER, MCF_S2C, "Party Member Sync")
														P(MPT_UID, "uidParty")
														P(MPT_BLOB, "PartyMember")
														P(MPT_BLOB, "vecBuff")

	C(MMC_PARTY_ADD_QUEST,								MCDT_SERVER2SERVER, MCF_C2S, "Party Add Quest")
														P(MPT_UID, "uidParty")
														P(MPT_UID, "uidMember")
														P(MPT_BLOB, "vecQuestID")

	C(MMC_PARTY_QUEST_SYNC,								MCDT_SERVER2SERVER, MCF_S2C, "Party Quest Sync")
														P(MPT_UID, "uidParty")
														P(MPT_BLOB, "vecPartyQuests")

	C(MMC_PARTY_ADD_FIELD,								MCDT_SERVER2SERVER, MCF_C2S, "Party Add Field")
														P(MPT_UID, "uidParty")
														P(MPT_UID, "uidMember")
														P(MPT_UID, "uidFieldGroup")

	C(MMC_PARTY_FIELD_SYNC,								MCDT_SERVER2SERVER, MCF_S2C, "Party Field Sync")
														P(MPT_UID, "uidParty")
														P(MPT_BLOB, "vecPartyField");

	C(MMC_PARTY_UPDATE_QUEST_RUNNER,					MCDT_SERVER2SERVER, MCF_C2S, "Party Update Quest Runner")
														P(MPT_UID, "uidParty")
														P(MPT_UID, "uidMember")
														P(MPT_UID, "uidField")
														P(MPT_BLOB, "vecQuestID")

	C(MMC_PARTY_QUEST_RUNNER_SYNC,						MCDT_SERVER2SERVER, MCF_S2C, "Party Quest Runner Sync")
														P(MPT_UID, "uidParty")
														P(MPT_UID, "uidMember")
														P(MPT_UID, "uidField")
														P(MPT_BLOB, "vecQeustID")

	C(MMC_PARTY_LEAVE_REQ,								MCDT_SERVER2SERVER, MCF_C2S, "Party Leave Req")
														P(MPT_UID, "uidParty")
														P(MPT_UID, "uidLeavePlayer")

	C(MMC_PARTY_LEAVE_RES,								MCDT_SERVER2SERVER, MCF_S2C, "Party Leave Res")
														P(MPT_UID, "uidLeavePlayer")
														P(MPT_INT, "nResult")

	C(MMC_PARTY_REMOVE_QUEST,							MCDT_SERVER2SERVER, MCF_C2S, "Party Remove Quest")
														P(MPT_UID, "uidParty")
														P(MPT_UID, "uidMember")
														P(MPT_BLOB, "vecQuestID")

	C(MMC_PARTY_REMOVE_FIELD,							MCDT_SERVER2SERVER, MCF_C2S, "Party Remove Field")
														P(MPT_UID, "uidParty")
														P(MPT_UID, "uidMember")
														P(MPT_INT, "nFieldGroupID")
													
	C(MMC_PARTY_ERASE_QUEST_RUNNER,						MCDT_SERVER2SERVER, MCF_C2S, "Party Erase Quest Runner")
														P(MPT_UID, "uidParty")
														P(MPT_UID, "uidMember")

	C(MMC_PARTY_ERASE_QUEST_RUNNER_SYNC,				MCDT_SERVER2SERVER, MCF_S2C, "Party Erase Quest Runner Sync")
														P(MPT_UID, "uidParty")
														P(MPT_UID, "uidMember")

	C(MMC_PARTY_KICK_REQ,								MCDT_SERVER2SERVER, MCF_C2S, "Party Kick Req")
														P(MPT_UID, "uidParty")
														P(MPT_UID, "uidRequestPlayer")
														P(MPT_UID, "uidTargetPlayer")

	C(MMC_PARTY_KICK_RES,								MCDT_SERVER2SERVER, MCF_S2C, "Party Kick Res")
														P(MPT_UID, "uidRequestPlayer")
														P(MPT_UID, "uidTargetPlayer")
														P(MPT_INT, "nResult")

	C(MMC_PARTY_DO_OFFLINE,								MCDT_SERVER2SERVER, MCF_C2S, "Party Do Offline")
														P(MPT_UID, "uidParty")
														P(MPT_UID, "uidMember")

	C(MMC_PARTY_DO_ONLINE,								MCDT_SERVER2SERVER, MCF_C2S, "Party Do Online")
														P(MPT_UID, "uidParty")
														P(MPT_UID, "uidMember")
														P(MPT_UID, "uidOffline")

	C(MMC_PARTY_ADD_OFFLINE_MEMBER,						MCDT_SERVER2SERVER, MCF_S2C, "Party Add Offline Meber")
														P(MPT_UID, "uidParty")
														P(MPT_UID, "uidMember")

	C(MMC_PARTY_REMOVE_OFFLINE_MEMBER,					MCDT_SERVER2SERVER, MCF_S2C, "Party Remove Offline Member")
														P(MPT_UID, "uidParty")
														P(MPT_UID, "uidMember")
														P(MPT_UID, "uidOffline")

	C(MMC_PARTY_JOIN_INVITE_REQ,						MCDT_SERVER2SERVER, MCF_C2S, "Party Join Invite Req")
														P(MPT_UID, "uidParty")
														P(MPT_UID, "uidRequestPlayer")

	C(MMC_PARTY_JOIN_INVITE_RES,						MCDT_SERVER2SERVER, MCF_S2C, "Party Join Invite Res")
														P(MPT_UID, "uidRequestPlayer")
														P(MPT_INT, "nResult")

	C(MMC_PARTY_JOIN_ACCEPT_REQ,						MCDT_SERVER2SERVER, MCF_S2C, "Party Join Accept Req")
														P(MPT_UID, "uidParty")
														P(MPT_UID, "uidLeader")
														P(MPT_UID, "uidRequestPlayer")
														P(MPT_WSTR, "strRequestPlayerName")

	C(MMC_PARTY_JOIN_ACCEPT_RES,						MCDT_SERVER2SERVER, MCF_C2S, "Party Join Accept Res")
														P(MPT_UID, "uidParty")
														P(MPT_UID, "uidLeader")
														P(MPT_UID, "uidRequestPlayer")
														P(MPT_INT, "nResult")

	C(MMC_PARTY_JOIN_ACCEPT_CANCEL,						MCDT_SERVER2SERVER, MCF_S2C, "Party Join Accept Cancel")
														P(MPT_UID, "uidLeader")
														P(MPT_INT, "nResult")

	C(MMC_PARTY_MOVE_SERVER,							MCDT_SERVER2SERVER, MCF_C2S, "Party Move Server")
														P(MPT_UID, "uidParty")
														P(MPT_UID, "uidMember")
														
	C(MMC_PARTY_MOVE_SERVER_SYNC,						MCDT_SERVER2SERVER, MCF_S2C, "Party Move Server Sync")
														P(MPT_UID, "uidParty")
														P(MPT_UID, "uidMember")
														P(MPT_UID, "uidOffline")

	C(MMC_PARTY_CREATE_SINGLE_REQ,						MCDT_SERVER2SERVER, MCF_C2S, "Party Create Single Req")
														P(MPT_UID, "uidRequestPlayer")

	C(MMC_PARTY_INFO_ALL_REQ,							MCDT_SERVER2SERVER, MCF_C2S, "Party Info All Req")
	
	C(MMC_PARTY_INFO_ALL_RES,							MCDT_SERVER2SERVER, MCF_S2C, "Party Info All Res")
														P(MPT_BLOB, "tdPartyInfo")

	C(MMC_PARTY_REMOVE_FIELD_SELF,						MCDT_SERVER2SERVER, MCF_C2S, "Party Remove Field Self")
														P(MPT_UID, "uidParty")
														P(MPT_INT, "nFieldGroupID")

	C(MMC_PARTY_CHANGE_NAME_REQ,						MCDT_SERVER2SERVER,	MCF_C2S, "Party.Change Name Req.")
														P(MPT_UID,			"uidParty")
														P(MPT_UID,			"uidLeader")
														P(MPT_WSTR,			"strName")

	C(MMC_PARTY_CHANGE_NAME,							MCDT_SERVER2SERVER, MCF_S2C, "Party.Change Name")
														P(MPT_UID,			"uidParty")
														P(MPT_WSTR,			"strName")
	
	C(MMC_PARTY_CHANGE_LEADER_REQ,						MCDT_SERVER2SERVER, MCF_C2S, "Party.Change Leader Req.")
														P(MPT_UID,			"uidParty")
														P(MPT_UID,			"uidLeader")
														P(MPT_UID,			"uidNewLeader")

	C(MMC_PARTY_CHANGE_LEADER,							MCDT_SERVER2SERVER, MCF_S2C, "Party.Change Leader")
														P(MPT_UID,			"uidParty")
														P(MPT_UID,			"uidNewLeader")

	C(MMC_PARTY_CHANGE_LOOTING_RULE_REQ,				MCDT_SERVER2SERVER, MCF_C2S, "Party.Change Looting Rule Req.")
														P(MPT_UID,			"uidParty")
														P(MPT_UID,			"uidLeader")
														P(MPT_CHAR,			"nCommon")
														P(MPT_CHAR,			"nRare")
														P(MPT_CHAR,			"nRareFor")

	C(MMC_PARTY_CHANGE_LOOTING_RULE,					MCDT_SERVER2SERVER, MCF_S2C, "Party.Change Looting Rule")
														P(MPT_UID,			"uidParty")
														P(MPT_CHAR,			"nCommon")
														P(MPT_CHAR,			"nRare")
														P(MPT_CHAR,			"nRareFor")

	C(MMC_PARTY_CHANGE_QUESTID_REQ,						MCDT_SERVER2SERVER, MCF_C2S, "Party.Change QuestID Req.")
														P(MPT_UID,			"uidParty")
														P(MPT_UID,			"uidLeader")
														P(MPT_INT,			"nQuestID")

	C(MMC_PARTY_FIXED_LOG_ON,							MCDT_SERVER2SERVER, MCF_C2S, "Fixed Party Log On")
														P(MPT_UID,			"uidParty")
														P(MPT_UID,			"uidMember")
														P(MPT_UID,			"uidOffline")

	C(MMC_PARTY_CHANGE_QUESTID,							MCDT_SERVER2SERVER, MCF_S2C, "Party.Change QuestID")
														P(MPT_UID,			"uidParty")
														P(MPT_INT,			"nQuestID")

	C(MMC_PARTY_CREATE_AUTO_PARTY_REQ,					MCDT_SERVER2SERVER, MCF_C2S, "Create Auto Party Req")
														P(MPT_BLOB,			"vecMemberUID")
														P(MPT_INT,			"nQuestID")

	C(MMC_PARTY_JOIN_ATUTO_PARTY_REQ,					MCDT_SERVER2SERVER, MCF_C2S, "Join Auto Party Req")
														P(MPT_UID,			"uidParty")
														P(MPT_UID,			"uidPlayer")

	C(MMC_PARTY_FAIL,									MCDT_SERVER2SERVER, MCF_S2C, "Fail")
														P(MPT_UID,		"uidPlayer")
														P(MPT_INT,		"nResult")

	// Loot
	C(MMC_LOOT_MASTERLOOT_NOTIFY_REQ,					MCDT_SERVER2SERVER, MCF_C2S, "MasterLoot Notify Req")
														P(MPT_BLOB,		"vecBeneficiaryCID")
														P(MPT_BLOB,		"vecTDMasterLootNotify")

	C(MMC_LOOT_MASTERLOOT_NOTIFY,						MCDT_SERVER2SERVER, MCF_S2C, "MasterLoot Notify")
														P(MPT_BLOB,		"vecBeneficiaryCID")
														P(MPT_BLOB,		"vecTDMasterLootNotify")

	C(MMC_LOOT_OTHER_GAIN_ITEM_REQ,						MCDT_SERVER2SERVER, MCF_C2S, "Other Gain Item Req")
														P(MPT_BLOB,		"vecBeneficiaryCID")
														P(MPT_UID,		"uidOther")
														P(MPT_BLOB,		"vecTDItemOtherGain")

	C(MMC_LOOT_OTHER_GAIN_ITEM,							MCDT_SERVER2SERVER, MCF_S2C, "Other Gain Item")
														P(MPT_BLOB,		"vecBeneficiaryCID")
														P(MPT_UID,		"uidOther")
														P(MPT_BLOB,		"vecTDItemOtherGain")

	C(MMC_LOOT_GETTABLE_ITEM_ADD_REQ,					MCDT_SERVER2SERVER, MCF_C2S, "Gettable Item Add Req")
														P(MPT_INT,		"nCID")
														P(MPT_BLOB,		"vecTDGettableItemAdd")

	C(MMC_LOOT_GETTABLE_ITEM_ADD,						MCDT_SERVER2SERVER, MCF_S2C, "Gettable Item Add")
														P(MPT_INT,		"nCID")
														P(MPT_BLOB,		"vecTDGettableItemAdd")

	C(MMC_LOOT_ROLLRESULT_REQ,							MCDT_SERVER2SERVER, MCF_C2S, "RollResult Req")
														P(MPT_BLOB,		"vecBeneficiaryCID")
														P(MPT_BLOB,		"vecTDRollItem")
														P(MPT_BLOB,		"vecTDRollResult")

	C(MMC_LOOT_ROLLRESULT,								MCDT_SERVER2SERVER, MCF_S2C, "RollResult")
														P(MPT_BLOB,		"vecBeneficiaryCID")
														P(MPT_BLOB,		"vecTDRollItem")
														P(MPT_BLOB,		"vecTDRollResult")

	// Guild
	C(MMC_GUILD_MEMBER_ONLINEINFO_REQ,					MCDT_SERVER2SERVER, MCF_C2S, "Member OnlineInfo Req")
														P(MPT_UID,		"uiPlayer")
														P(MPT_BLOB,		"vecMemberCID")

	C(MMC_GUILD_MEMBER_ONLINEINFO,						MCDT_SERVER2SERVER, MCF_S2C, "Member OnlineInfo Req")
														P(MPT_UID,		"uiPlayer")
														P(MPT_BLOB,		"vecOnlineMemberCID")


	C(MMC_GUILD_DESTROY,								MCDT_SERVER2SERVER, MCF_C2S, "Destroy")
														P(MPT_INT,		"nGID")

	C(MMC_GUILD_DESTROY_SYNC,							MCDT_SERVER2SERVER, MCF_S2C, "Destroy Sync")
														P(MPT_INT,		"nGID")

	C(MMC_GUILD_JOIN,									MCDT_SERVER2SERVER, MCF_C2S, "Join")
														P(MPT_INT,		"nCID")
														P(MPT_INT,		"nAID")
														P(MPT_INT,		"nGID")
														P(MPT_WSTR,		"strName")
														P(MPT_INT,		"nLevel")
														P(MPT_INT,		"nFieldID")
														P(MPT_INT,		"nChannelID")

	C(MMC_GUILD_JOIN_SYNC,								MCDT_SERVER2SERVER, MCF_S2C, "Join Sync")
														P(MPT_INT,		"nCID")
														P(MPT_INT,		"nAID")
														P(MPT_INT,		"nGID")
														P(MPT_WSTR,		"strName")
														P(MPT_INT,		"nLevel")
														P(MPT_INT,		"nFieldID")
														P(MPT_INT,		"nChannelID")

	C(MMC_GUILD_LEAVE,									MCDT_SERVER2SERVER, MCF_C2S, "Leave")
														P(MPT_INT,		"nCID")
														P(MPT_INT,		"nGID")

	C(MMC_GUILD_LEAVE_SYNC,								MCDT_SERVER2SERVER, MCF_S2C, "Leave Sync")
														P(MPT_INT,		"nCID")
														P(MPT_INT,		"nGID")

	C(MMC_GUILD_KICK,									MCDT_SERVER2SERVER, MCF_C2S, "Kick")
														P(MPT_INT,		"nCID")
														P(MPT_INT,		"nGID")

	C(MMC_GUILD_KICK_SYNC,								MCDT_SERVER2SERVER, MCF_S2C, "Kick Sync")
														P(MPT_INT,		"nCID")
														P(MPT_INT,		"nGID")

	C(MMC_GUILD_ONLINE,									MCDT_SERVER2SERVER, MCF_C2S, "Online")
														P(MPT_INT,		"nCID")
														P(MPT_INT,		"nGID")
														P(MPT_INT,		"nFieldID")
														P(MPT_INT,		"nChannelID")														

	C(MMC_GUILD_ONLINE_SYNC,							MCDT_SERVER2SERVER, MCF_S2C, "Online Sync")
														P(MPT_INT,		"nCID")
														P(MPT_INT,		"nGID")
														P(MPT_INT,		"nFieldID")
														P(MPT_INT,		"nChannelID")

	C(MMC_GUILD_OFFLINE,								MCDT_SERVER2SERVER, MCF_C2S, "Offline")
														P(MPT_INT,		"nCID")
														P(MPT_INT,		"nGID")														

	C(MMC_GUILD_OFFLINE_SYNC,							MCDT_SERVER2SERVER, MCF_S2C, "Offline Sync")
														P(MPT_INT,		"nCID")
														P(MPT_INT,		"nGID")

	C(MMC_GUILD_MOVEFIELD,								MCDT_SERVER2SERVER, MCF_C2S, "MoveField")
														P(MPT_INT,		"nCID")
														P(MPT_INT,		"nGID")
														P(MPT_INT,		"nFieldID")
														P(MPT_INT,		"nChannelID")

	C(MMC_GUILD_MOVEFIELD_SYNC,							MCDT_SERVER2SERVER, MCF_S2C, "MoveField Sync")
														P(MPT_INT,		"nCID")
														P(MPT_INT,		"nGID")
														P(MPT_INT,		"nFieldID")
														P(MPT_INT,		"nChannelID")														

	C(MMC_GUILD_CHANGE_MASTER,							MCDT_SERVER2SERVER, MCF_C2S, "Change Master")
														P(MPT_INT,		"nOldCID")
														P(MPT_INT,		"nNewCID")
														P(MPT_INT,		"nGID")														

	C(MMC_GUILD_CHANGE_MASTER_SYNC,						MCDT_SERVER2SERVER, MCF_S2C, "Change Master Sync")
														P(MPT_INT,		"nOldCID")
														P(MPT_INT,		"nNewCID")
														P(MPT_INT,		"nGID")

	C(MMC_GUILD_DEPOSIT_STORAGEMONEY,					MCDT_SERVER2SERVER, MCF_C2S, "Deposit StorageMoney")
														P(MPT_INT,		"nCID")
														P(MPT_INT,		"nGID")
														P(MPT_INT,		"nDepositMoney")
														P(MPT_INT,		"nStorageMoney")

	C(MMC_GUILD_DEPOSIT_STORAGEMONEY_SYNC,				MCDT_SERVER2SERVER, MCF_S2C, "Deposit StorageMoney Sync")
														P(MPT_INT,		"nCID")
														P(MPT_INT,		"nGID")
														P(MPT_INT,		"nDepoistMoney")
														P(MPT_INT,		"nStorageMoney")

	C(MMC_GUILD_WITHDRAW_STORAGEMONEY,					MCDT_SERVER2SERVER, MCF_C2S, "Withdraw StorageMoney")
														P(MPT_INT,		"nCID")
														P(MPT_INT,		"nGID")
														P(MPT_INT,		"nWithdrawMoney")
														P(MPT_INT,		"nStorageMoney")

	C(MMC_GUILD_WITHDRAW_STORAGEMONEY_SYNC,				MCDT_SERVER2SERVER, MCF_S2C, "Withdraw StorageMoney Sync")
														P(MPT_INT,		"nCID")
														P(MPT_INT,		"nGID")
														P(MPT_INT,		"nWithdrawMoney")
														P(MPT_INT,		"nStorageMoney")

	C(MMC_GUILD_MOVE_STORAGEITEM,						MCDT_SERVER2SERVER, MCF_C2S, "Move StorageItem")
														P(MPT_INT,		"nCID")
														P(MPT_INT,		"nGID")														
														P(MPT_BLOB,		"vecItem")

	C(MMC_GUILD_MOVE_STORAGEITEM_SYNC,					MCDT_SERVER2SERVER, MCF_S2C, "Move StorageItem Sync")
														P(MPT_INT,		"nCID")
														P(MPT_INT,		"nGID")
														P(MPT_BLOB,		"vecItem")
	// Char
	C(MMC_CHAR_NPCDIE_REWARD_REQ,						MCDT_SERVER2SERVER, MCF_C2S, "NPCDie Reward Req")
														P(MPT_BLOB,		"vecBeneficiaryCID")
														P(MPT_INT,		"nAllBeneficiaryCount")
														P(MPT_INT,		"nNPCID")														

	C(MMC_CHAR_NPCDIE_REWARD,							MCDT_SERVER2SERVER, MCF_S2C, "NPCDie Reward Exp")
														P(MPT_BLOB,		"vecOtherServerBeneficiaryCID")
														P(MPT_INT,		"nAllBeneficiaryCount")
														P(MPT_INT,		"nNPCID")									

	C(MMC_MSG_ANNOUNCE,									MCDT_SERVER2SERVER, MCF_S2C, "Msg Announce")
														P(MPT_STR,		"strMsg")
	
	// Admin
	C(MMC_ADMIN_CHANGE_SERVER_MODE_REQ,					MCDT_SERVER2SERVER, MCF_C2S, "Request Change ServerMode")
														P(MPT_INT,		"ServerMode")

	C(MMC_ADMIN_CHANGE_SERVER_MODE,						MCDT_SERVER2SERVER, MCF_S2C, "Change ServerMode")
														P(MPT_INT,		"ServerMode")

	// Debug
	C(MMC_DEBUG_DUMP,									MCDT_SERVER2SERVER, MCF_C2S, "Request Dump Master Server Info")
														P(MPT_INT,		"AID")

	// GM
	C(MMC_GM_SUMMON_REQ,								MCDT_SERVER2SERVER, MCF_C2S, "Request Summon")
														P(MPT_WSTR,		"strTarName")
														P(MPT_UID,		"uidFieldGroup")
														P(MPT_INT,		"nFieldID")
														P(MPT_INT,		"nChannelID")
														P(MPT_VEC,		"vPos")

	C(MMC_GM_SUMMON,									MCDT_SERVER2SERVER, MCF_S2C, "Summon")
														P(MPT_UID,		"uidTarPlayer")
														P(MPT_UID,		"uidFieldGroup")
														P(MPT_INT,		"nFieldID")
														P(MPT_INT,		"nChannelID")
														P(MPT_VEC,		"vPos")
														
	

	END_CMD_DESC();
}

//////////////////////////////////////////////////////////////////////////

void AddMaster2LoginCommandTable()
{
	BEGIN_CMD_DESC();

	C(MLC_COMM_REQUEST_LOGIN,							MCDT_SERVER2SERVER, MCF_C2S, "Request Login")
														P(MPT_WSTR,	"ServerName");
														P(MPT_INT,	"ServerType - SServerType")
														P(MPT_INT,	"WorldID")
														P(MPT_INT,	"ServerID")
														P(MPT_INT,	"CommandVersion");

	C(MLC_COMM_RESPONSE_LOGIN,							MCDT_SERVER2SERVER, MCF_S2C, "Response Login")
														P(MPT_INT,	"nResult")
														P(MPT_UID,	"AllocServerUID")
														P(MPT_UCHAR,"ServerMode")
														P(MPT_BLOB,	"TD_SERVER_LIST_NODE")

	C(MLC_COMM_REQUEST_LOGIN_GAME_SERVER,				MCDT_SERVER2SERVER, MCF_C2S, "Request Login GameServer")
														P(MPT_UID,  "RequestID")											
														P(MPT_SINGLE_BLOB, "LoginAccountInfo")
														P(MPT_SINGLE_BLOB, "LoginFieldInfo")

	C(MLC_COMM_RESPONSE_LOGIN_GAME_SERVER,				MCDT_SERVER2SERVER, MCF_S2C, "Response Login GameServer")
														P(MPT_UID,  "RequestID")
														P(MPT_INT,	"nResult")
														P(MPT_BLOB, "LoginGameServerInfo")

	C(MLC_COMM_READY_ENTER_LOGINSERVER_REQ,				MCDT_SERVER2SERVER, MCF_S2C, "Request Ready Enter LoginServer From GameServer")
														P(MPT_UID,	"RequestGameServer")
														P(MPT_UID,	"RequestPlayer")
														P(MPT_UID,	"ConnectionKey")
														P(MPT_INT,	"AID")
														P(MPT_WSTR,	"UserID")

	C(MLC_COMM_READY_ENTER_LOGINSERVER_RES,				MCDT_SERVER2SERVER, MCF_C2S, "Response Ready Enter LoginServer From GameServer")
														P(MPT_UID,	"RequestGameServer")
														P(MPT_UID,	"RequestPlayer")
														P(MPT_INT,	"nResult")
														P(MPT_UID,	"ConnectionKey")

	C(MLC_COMM_CANCEL_MOVE_LOGINSERVER,					MCDT_SERVER2SERVER, MCF_S2C, "Request Cancel Reserved Move LoginServer")
														P(MPT_UID,	"ConnectionKey")

	// Player ฐüทร
	C(MLC_PLAYER_ADD_PLAYER_REQ,						MCDT_SERVER2SERVER, MCF_C2S, "Request Add Player")
														P(MPT_UID,	"PlayerUID")
														P(MPT_INT,	"AID")
														P(MPT_WSTR,	"UserID")

	C(MLC_PLAYER_ADD_PLAYER_RES,						MCDT_SERVER2SERVER, MCF_S2C, "Response Add Player")
														P(MPT_UID,	"PlayerUID")
														P(MPT_INT,	"nResult")

	C(MLC_PLAYER_DELETE_PLAYER,							MCDT_SERVER2SERVER, MCF_C2S, "Delete Player")
														P(MPT_INT,	"AID")
														P(MPT_UID,	"UID")

	C(MLC_PLAYER_DUPLICATED_PLAYER_LOGIN,				MCDT_SERVER2SERVER, MCF_S2C, "Duplicated Player Login")
														P(MPT_UID,	"PlayerUID")
														P(MPT_UID,	"ConnectionKey")

	C(MLC_PLAYER_STATE_SELECT_CHAR,						MCDT_SERVER2SERVER, MCF_C2S, "Request Player State - Select Char")
														P(MPT_INT,	"AID")
														P(MPT_UID,	"PlayerUID")

	C(MLC_PLAYER_REQUEST_SET_MYCHAR,					MCDT_SERVER2SERVER, MCF_C2S, "Request Set MyChar")
														P(MPT_INT,	"CID")
														P(MPT_WSTR,	"Player Name")

	C(MLC_PLAYER_RESPONSE_SET_MYCHAR,					MCDT_SERVER2SERVER, MCF_S2C, "Response Set MyChar")
														P(MPT_INT,	"CID");

	C(MLC_PLAYER_PMANG_ADD_PLAYER_REQ,					MCDT_SERVER2SERVER, MCF_C2S, "Request Pmang Add Player")
														P(MPT_UID,			"PlayerUID")
														P(MPT_INT,			"AID")
														P(MPT_WSTR,			"UserID")
														P(MPT_SINGLE_BLOB,	"tdPmangUserData")

	C(MLC_PLAYER_PMANG_ADD_PLAYER_RES,					MCDT_SERVER2SERVER, MCF_S2C, "Response Pmang Add Player")
														P(MPT_UID,	"PlayerUID")
														P(MPT_INT,	"nResult")

	C(MLC_ADMIN_CHANGE_SERVER_MODE,						MCDT_SERVER2SERVER, MCF_S2C, "Change ServerMode")
														P(MPT_INT,	"ServerMode")

	END_CMD_DESC();
}

//////////////////////////////////////////////////////////////////////////

void AddMaster2AppCommandTable()
{
	BEGIN_CMD_DESC();

	C(MAC_COMM_REQUEST_LOGIN,							MCDT_SERVER2SERVER, MCF_C2S, "Request Login")
														P(MPT_WSTR,	"ServerName")
														P(MPT_INT,	"ServerType - SServerType")
														P(MPT_INT,	"WorldID")
														P(MPT_INT,	"ServerID")
														P(MPT_INT,	"CommandVersion")
														P(MPT_BLOB,	"vecAppServerInfoList")

	C(MAC_COMM_RESPONSE_LOGIN,							MCDT_SERVER2SERVER, MCF_S2C, "Response Login")
														P(MPT_INT,	"nResult")
														P(MPT_UID,	"AllocServerUID")
														P(MPT_UCHAR,"ServerMode")


	END_CMD_DESC();
}
