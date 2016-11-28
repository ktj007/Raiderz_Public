#include "stdafx.h"
#include "CCommandTable.h"
#include "MCommandDesc.h"
#include "MCommand.h"

using namespace minet;

void AddCommonCommandTable()
{
	BEGIN_CMD_DESC();

	C(MC_COMM_REQUEST_ACCOUNT_CHAR_LIST,	MCDT_MACHINE2MACHINE, MCF_C2S, "Request Account Char List")
	C(MC_COMM_RESPONSE_ACCOUNT_CHAR_LIST,	MCDT_MACHINE2MACHINE, MCF_S2C, "Response Account Char List")
											P(MPT_INT,	"Result")
											P(MPT_BLOB,	"CharInfo")	// 캐릭터 정보

	C(MC_COMM_MOVE_TO_GAME_SERVER,			MCDT_MACHINE2MACHINE, MCF_S2C, "Move to GameServer")
											P(MPT_BLOB,	"LoginGameServerInfo")

	C(MC_COMM_START_MOVE_TO_LOGIN_SERVER_REQ,	MCDT_MACHINE2MACHINE, MCF_C2S, "Request Move To LoginServer")

	C(MC_COMM_START_MOVE_TO_LOGIN_SERVER_RES,	MCDT_MACHINE2MACHINE, MCF_S2C, "Response Move To LoginServer")
												P(MPT_INT,	"nResult")
												P(MPT_UID,	"ConnectionKey")

	C(MC_COMM_NOTIFY_KICK,					MCDT_MACHINE2MACHINE, MCF_S2C, "Notify Kick")
											P(MPT_INT,	"nResult")

	C(MC_COMM_NOTIFY_REASON_KICK,			MCDT_MACHINE2MACHINE, MCF_S2C, "Notify Reason Kick")
											P(MPT_WSTR,	"wReason")

	C(MC_CHAR_REQUEST_INSERT_CHAR,			MCDT_MACHINE2MACHINE, MCF_C2S, "Request Insert Character")
											P(MPT_SINGLE_BLOB,	"TD_INSERT_CHARACTER_INFO")

	C(MC_CHAR_RESPONSE_INSERT_CHAR,			MCDT_MACHINE2MACHINE, MCF_S2C, "Response Insert Character")
											P(MPT_INT,	"nResult")			// 결과값

	C(MC_CHAR_REQUEST_DELETE_CHAR,			MCDT_MACHINE2MACHINE, MCF_C2S, "Request Delete Character")
											P(MPT_CHAR,	"nCharIndex")	// 캐릭터 인덱스
	C(MC_CHAR_RESPONSE_DELETE_CHAR,			MCDT_MACHINE2MACHINE, MCF_S2C, "Response Delete Character")
											P(MPT_INT,	"nResult")		// 결과값
											P(MPT_CHAR, "nCharIndex")	// 삭제 캐릭터 인덱스

	C(MC_CHAR_REQUEST_SEL_MYCHAR,			MCDT_MACHINE2MACHINE, MCF_C2S, "Request Sel MyChar")
											P(MPT_INT,	"nCharIndex")	// 캐릭터 인덱스

	C(MC_CHAR_RESPONSE_SEL_MYCHAR,			MCDT_MACHINE2MACHINE, MCF_S2C, "Response Sel MyChar")
											P(MPT_INT,	"nResult")		// 결과값											

	C(MC_CHAR_GAMESTART,					MCDT_MACHINE2MACHINE, MCF_C2S, "Game Start")
											P(MPT_INT,	"nFieldID")		// 처음 시작하기 원하는 필드 ID	<- 서비스되면 삭제되어야 한다.
											P(MPT_VEC,	"vPos")			// 처음 시작하기 원하는 위치 <- 서비스되면 삭제되어야 한다.
											
	C(MC_CHAR_MYINFO,						MCDT_MACHINE2MACHINE, MCF_S2C, "My Info")
											P(MPT_SINGLE_BLOB,	"MyInfo")
											P(MPT_BLOB,	"MyInventory")
											P(MPT_BLOB,	"MyEquipment")
											P(MPT_BLOB,	"MyReservedDummySlotInfo")
											P(MPT_BLOB,	"MyTalent")
											P(MPT_BLOB,	"MyQuest")
											P(MPT_BLOB,	"MyPalette")
											P(MPT_BLOB,	"MyFaction")
											P(MPT_BLOB,	"MyRecipe")
											P(MPT_BLOB, "MyGuideBook")

	C(MC_CHAR_MYTIMEINFO,					MCDT_MACHINE2MACHINE, MCF_S2C, "My Time Info")
											P(MPT_BLOB,	"MyBuffRemainTime")
											P(MPT_BLOB,	"MyTalentRemainCoolTime")
											P(MPT_FLOAT,"MyAutoRebirthRemainTime")

	C(MC_CHAR_REBIRTH_REQ,					MCDT_MACHINE2MACHINE, MCF_C2S, "Rebirth Req")
											P(MPT_INT,	"nRebirthType")
											

	C(MC_CHAR_REBIRTH_MYPLAYER,				MCDT_MACHINE2MACHINE, MCF_S2C, "Rebirth Char")
											P(MPT_INT,			"nFieldID")
											P(MPT_VEC,			"MyPos")
											P(MPT_SVEC2,		"MyDir")
											P(MPT_SINGLE_BLOB,	"SimpleStatus")


	C(MC_CHAR_REBIRTH_NETPLAYER,			MCDT_MACHINE2MACHINE, MCF_S2C, "Rebirth")
											P(MPT_UID,			"uidEntity")
											P(MPT_VEC,			"MyPos")
											P(MPT_SVEC2,		"MyDir")


	C(MC_CHAR_REWARD_EXP,					MCDT_MACHINE2MACHINE, MCF_S2C, "Reward Exp")
											P(MPT_INT,		"nRewardCause")
											P(MPT_INT,		"RewardExp")
											P(MPT_FLOAT,	"ExpPercent")
											P(MPT_INT,		"PlayerExp")

	C(MC_CHAR_LEVEL_UP,						MCDT_MACHINE2MACHINE, MCF_S2C, "LevelUp")
											P(MPT_UID,		"uidPlayer")
											P(MPT_UCHAR,	"Level")
											P(MPT_USHORT,	"NewHP")
											P(MPT_USHORT,	"NewEN")
											P(MPT_USHORT,	"NewSTA")
											P(MPT_USHORT,	"NewTP_first")
											P(MPT_USHORT,	"NewTP_second")
											P(MPT_USHORT,	"NewTP_PVP")
											P(MPT_USHORT,	"NewStr")
											P(MPT_USHORT,	"NewDex")
											P(MPT_USHORT,	"NewInt")
											P(MPT_USHORT,	"NewChr")
											P(MPT_USHORT,	"NewCon")
											
	C(MC_CHAR_UPDATE_SIMPLE_STATUS,			MCDT_MACHINE2MACHINE, MCF_S2C, "Update Simple Status")
											P(MPT_SINGLE_BLOB,		"SimpleStatus")

	/*
	C(MC_CHAR_SAVE_SOULBINDING,				MCDT_MACHINE2MACHINE, MCF_S2C, "SaveSoulBlinding")
											P(MPT_INT,		"nFieldID")
	*/

	C(MC_CHAR_SAVE_CHECKPOINT,				MCDT_MACHINE2MACHINE, MCF_S2C, "SaveCheckPoint")
											P(MPT_INT,		"nFieldID")

	C(MC_CHAR_UPDATE_MONEY,					MCDT_MACHINE2MACHINE, MCF_S2C, "Update Money")
											P(MPT_INT,		"nRewardCause")
											P(MPT_INT,		"nMoney")
	C(MC_CHAR_ENEMY_INFO,					MCDT_MACHINE2MACHINE, MCF_S2C, "Char.EmepyHPInfo")
											P(MPT_BLOB,		"EmepyHPInfo")
	C(MC_CHAR_ENEMY_INFO_CLEAR,				MCDT_MACHINE2MACHINE, MCF_S2C, "Char.EmepyHPInfo")

	C(MC_CHAR_UPDATE_TP,					MCDT_MACHINE2MACHINE, MCF_S2C, "Update TP")
											P(MPT_INT,		"nTP")

	C(MC_CHAR_TIP,							MCDT_MACHINE2MACHINE, MCF_S2C, "TIP")
											P(MPT_WSTR,			"strText")

	C(MC_CHAR_ADD_ENEMYNPC,					MCDT_MACHINE2MACHINE, MCF_S2C, "Add EnemyNPC")
											P(MPT_UID,		"nNPCUID")

	C(MC_CHAR_DEL_ENEMYNPC,					MCDT_MACHINE2MACHINE, MCF_S2C, "Del EnemyNPC")
											P(MPT_UID,		"nNPCUID")	

	C(MC_CHAR_TARGET_INFO_REQ,				MCDT_MACHINE2MACHINE, MCF_C2S, "TargetInfo Req")
											P(MPT_BLOB,		"vecUIID")

	C(MC_CHAR_TARGET_INFO,					MCDT_MACHINE2MACHINE, MCF_S2C, "TargetInfo")
											P(MPT_BLOB,		"vecTargetInfo")

	C(MC_CHAR_DIE_REQ,						MCDT_MACHINE2MACHINE, MCF_C2S, "Request Die")											

	C(MC_MSG_REQ,							MCDT_MACHINE2MACHINE, MCF_C2S, "Send Msg")
											P(MPT_INT,	"eMsgType")
											P(MPT_WSTR,	"msg")
											P(MPT_BLOB,	"SenderInfo")
											P(MPT_BLOB, "ReceiverInfo")
											P(MPT_INT,	"nAdditional")

	C(MC_MSG_RES,							MCDT_MACHINE2MACHINE, MCF_S2C, "Receive Msg")
											P(MPT_INT,	"eMsgType")
											P(MPT_WSTR,	"msg")
											P(MPT_BLOB,	"SenderInfo")
											P(MPT_BLOB, "ReceiverInfo")
											P(MPT_INT,	"nAdditional")

	C(MC_MSG_CREATE_CHANNEL_REQ,			MCDT_MACHINE2MACHINE, MCF_C2S, "Request Create Chatting Channel")
											P(MPT_WSTR,	"ChannelName")												

	C(MC_MSG_ENTER_CHANNEL_REQ,				MCDT_MACHINE2MACHINE, MCF_C2S, "Request Enter Chatting Channel")
											P(MPT_WSTR,	"ChannelName")

	C(MC_MSG_ENTER_CHANNEL,					MCDT_MACHINE2MACHINE, MCF_S2C, "Enter Chatting Channel")
											P(MPT_SINGLE_BLOB,	"TD_CHAT_CHANNEL_INFO")

	C(MC_MSG_LEAVE_CHANNEL_REQ,				MCDT_MACHINE2MACHINE, MCF_C2S, "Request Leave Chatting Channel")
											P(MPT_UID,	"ChannelUID")

	C(MC_MSG_LEAVE_CHANNEL,					MCDT_MACHINE2MACHINE, MCF_S2C, "Leave Chatting Channel")
											P(MPT_UID,	"ChannelUID")

	C(MC_MSG_KICK_PLAYER_REQ,				MCDT_MACHINE2MACHINE, MCF_C2S, "Request Kick Player")
											P(MPT_UID,	"ChannelUID")
											P(MPT_WSTR,	"TargetPlayerName")

	C(MC_MSG_KICK_PLAYER,					MCDT_MACHINE2MACHINE, MCF_S2C, "Kick Player")
											P(MPT_UID,	"ChannelUID")

	C(MC_MSG_BAN_PLAYER_REQ,				MCDT_MACHINE2MACHINE, MCF_C2S, "Request Ban Player")
											P(MPT_UID,	"ChannelUID")
											P(MPT_WSTR,	"TargetPlayerName")

	C(MC_MSG_BAN_PLAYER,					MCDT_MACHINE2MACHINE, MCF_S2C, "Ban Player")
											P(MPT_UID,	"ChannelUID")									
	
	C(MC_MSG_UNBAN_PLAYER_REQ,				MCDT_MACHINE2MACHINE, MCF_C2S, "Request Unban Player")
											P(MPT_UID,	"ChannelUID")
											P(MPT_WSTR,	"BanedPlayerName")

	C(MC_MSG_UNBAN_PLAYER,					MCDT_MACHINE2MACHINE, MCF_S2C, "Unban Player")
											P(MPT_UID,	"ChannelUID")
											P(MPT_WSTR,	"UnbanedPlayerName")

	C(MC_MSG_CHANGE_HOST_REQ,				MCDT_MACHINE2MACHINE, MCF_C2S, "Request Change Host")
											P(MPT_UID,	"ChannelUID")
											P(MPT_WSTR,	"TargetPlayerName")

	C(MC_MSG_CHANGE_HOST,					MCDT_MACHINE2MACHINE, MCF_S2C, "Change Host")
											P(MPT_UID,	"ChannelUID")
											P(MPT_WSTR,	"NewHostName")

	C(MC_MSG_ENTER_CHANNEL_PLAYER_INFO,		MCDT_MACHINE2MACHINE, MCF_S2C, "Enter Channel Player Info")
											P(MPT_UID,	"ChatChannelUID");
											P(MPT_WSTR,	"EnterPlayerName");

	C(MC_MSG_LEAVE_CHANNEL_PLAYER_INFO,		MCDT_MACHINE2MACHINE, MCF_S2C, "Leave Channel Player Info")
											P(MPT_UID,	"ChatChannelUID");
											P(MPT_WSTR,	"LeavePlayerName");

	C(MC_MSG_KICK_PLAYER_INFO,				MCDT_MACHINE2MACHINE, MCF_S2C, "Kick Player Info")
											P(MPT_UID,	"ChatChannelUID");
											P(MPT_WSTR,	"KickedPlayerName");

								
											
	C(MC_FIELD_PREPARING_TO_LOAD,			MCDT_MACHINE2MACHINE, MCF_S2C, "Field Preparing To Gate")
											P(MPT_INT,	"DestFieldID")

	C(MC_FIELD_LOADING_COMPLETE,			MCDT_MACHINE2MACHINE, MCF_C2S, "Field Loading Complete")

	C(MC_FIELD_OBJECT_LOADING,				MCDT_MACHINE2MACHINE, MCF_S2C, "Field Object Loading")

	C(MC_FIELD_OBJECT_LOADING_COMPLETE,		MCDT_MACHINE2MACHINE, MCF_C2S, "Field Object Loading Complete")

	C(MC_FIELD_START_GAME,					MCDT_MACHINE2MACHINE, MCF_S2C, "Start Game")

	C(MC_FIELD_CHANGE_FIELD,				MCDT_MACHINE2MACHINE, MCF_S2C, "Change Field")
											P(MPT_INT,	"FieldID")
											P(MPT_INT,	"ChannelID")
											P(MPT_VEC,	"MyPos")
											P(MPT_VEC,	"MyDir")
											P(MPT_INT,	"IntroCutsceneID")
											P(MPT_UCHAR, "InitWeather")
											P(MPT_UCHAR, "InitTime")

	C(MC_FIELD_PREPARE_CHANGE_CHANNEL_REQ,	MCDT_MACHINE2MACHINE, MCF_C2S, "Request Prepare Change Channel")
											P(MPT_INT,	"ChannelID")

	C(MC_FIELD_PREPARE_CHANGE_CHANNEL_RES,	MCDT_MACHINE2MACHINE, MCF_S2C, "Response Prepare Change Channel")
											P(MPT_INT,	"nResult")

	C(MC_FIELD_CANCEL_CHANGE_CHANNEL,		MCDT_MACHINE2MACHINE, MCF_C2S, "Cancel Change Channel")
	
	C(MC_FIELD_REQUEST_CHANGE_CHANNEL,		MCDT_MACHINE2MACHINE, MCF_C2S, "Request Change Channel")
	
	C(MC_FIELD_REQUEST_CHANNEL_LIST,		MCDT_MACHINE2MACHINE, MCF_C2S, "Request Channel List")

	C(MC_FIELD_CHANNEL_LIST,				MCDT_MACHINE2MACHINE, MCF_S2C, "Channel List")
											P(MPT_BLOB,	"ChannelIDs")
											P(MPT_BLOB,	"ChannelStatuses")

	C(MC_FIELD_CANCEL_CHANGE_FIELD,			MCDT_MACHINE2MACHINE, MCF_S2C, "Cancel Change Field")


	C(MC_FIELD_IN_PLAYER,					MCDT_MACHINE2MACHINE, MCF_S2C, "Player Come in")
											P(MPT_SINGLE_BLOB,		"PlayerInfo")			// 플레이어 정보
											P(MPT_SINGLE_BLOB,		"ExtraFeatureInfo")		// 외형 부가정보
											P(MPT_BLOB,				"vector<int>")			// Buff IDs


	C(MC_FIELD_OUT_PLAYER,					MCDT_MACHINE2MACHINE, MCF_S2C, "Player Go out")
											P(MPT_USHORT,			"nPlayerUIID")	// 플레이어 UIID
	C(MC_FIELD_IN_NPC,						MCDT_MACHINE2MACHINE, MCF_S2C, "NPC Come in")
											P(MPT_SINGLE_BLOB,		"NPCInfo")	// NPC 정보
	C(MC_FIELD_OUT_NPC,						MCDT_MACHINE2MACHINE, MCF_S2C, "NPC Go out")
											P(MPT_USHORT,			"nNPCUIID")	// NPC UIID
	C(MC_FIELD_IN_BUFFENTITY,				MCDT_MACHINE2MACHINE, MCF_S2C, "Magicarea Come in")
											P(MPT_SINGLE_BLOB,		"MagicareaInfo")	// NPC 정보
	C(MC_FIELD_OUT_BUFFENTITY,				MCDT_MACHINE2MACHINE, MCF_S2C, "Magicarea Go out")
											P(MPT_UID,		"uidMagicarea")	// Magicarea UID
	C(MC_FIELD_TRAP_TRIGGERED,				MCDT_MACHINE2MACHINE, MCF_S2C, "Trap Go out")
											P(MPT_UID,		"uidMagicarea")	// Trap UID

	/*
	C(MC_FIELD_SECTOR_ENTITY_INFO,			MCDT_MACHINE2MACHINE, MCF_S2C, "Sector Entity Info")
											P(MPT_BLOB,		"PlayerInfo")
											P(MPT_BLOB,		"PlayerExtraInfo")
											P(MPT_BLOB,		"NPCInfo")
											P(MPT_BLOB,		"BPartInfo")
											P(MPT_BLOB,		"BuffEntityInfo")
											P(MPT_BLOB,		"UseableSensorInfo")
	*/

	C(MC_FIELD_SET_TIME,					MCDT_MACHINE2MACHINE, MCF_S2C, "Set Time")
											P(MPT_CHAR,		"nGameTime")	// 시간대
	C(MC_FIELD_SET_WEATHER,					MCDT_MACHINE2MACHINE, MCF_S2C, "Set Weather")
											P(MPT_CHAR,		"nWeatherType")
	C(MC_FIELD_SET_TIME_WEATHER,			MCDT_MACHINE2MACHINE, MCF_S2C, "Set Time and Weather")
											P(MPT_CHAR,		"nTimeType")
											P(MPT_CHAR,		"nWeatherType")
	C(MC_FIELD_WHO_IS_REQ,					MCDT_MACHINE2MACHINE, MCF_C2S, "Who is")
											P(MPT_UID,		"uidObject")
	C(MC_FIELD_WHO_IS_UIID_REQ,				MCDT_MACHINE2MACHINE, MCF_C2S, "Who is")
											P(MPT_USHORT,	"ObjectUIID")

	C(MC_FIELD_PLAYER_EXTRA_INFO_REQ,		MCDT_MACHINE2MACHINE, MCF_C2S, "Extra Player Info")
											P(MPT_BLOB,		"vector<uiidPlayer>")
	C(MC_FIELD_IN_BPART,					MCDT_MACHINE2MACHINE, MCF_S2C, "BPart Come in")
											P(MPT_SINGLE_BLOB,		"infoTD")	// NPC 정보
	C(MC_FIELD_OUT_BPART,					MCDT_MACHINE2MACHINE, MCF_S2C, "BPart Go out")
											P(MPT_USHORT,			"nNPCUIID")	// NPC UIID

	C(MC_FIELD_PLAYER_EXTRA_INFO,			MCDT_MACHINE2MACHINE, MCF_S2C, "player extra info")
											P(MPT_USHORT,			"playerUIID")
											P(MPT_SINGLE_BLOB,		"TD_EXTRA_UPDATE_CACHE_PLAYER")
											P(MPT_SINGLE_BLOB,		"TD_PLAYER_FEATURE_TATTOO")

	C(MC_FIELD_IN_PLAYER_SIMPLE,			MCDT_MACHINE2MACHINE, MCF_S2C, "player simple info")
											P(MPT_SINGLE_BLOB,		"TD_SIMPLE_UPDATE_CACHE_PLAYER")
											P(MPT_BLOB,				"vector<int>")

	C(MC_FIELD_SECTOR_PLAYER_INFO,			MCDT_MACHINE2MACHINE, MCF_S2C, "Sector Player Info")
											P(MPT_BLOB,				"SimpleInfo")
											P(MPT_BLOB,				"PlayerInfo")
											P(MPT_BLOB,				"TattooInfo")
											P(MPT_BLOB,				"BuffInfo")

	C(MC_FIELD_SECTOR_NPC_INFO,				MCDT_MACHINE2MACHINE, MCF_S2C, "Sector NPC Info")
											P(MPT_BLOB,				"NpcInfo")

	C(MC_FIELD_SECTOR_BPART_INFO,			MCDT_MACHINE2MACHINE, MCF_S2C, "Sector BPart Info")
											P(MPT_BLOB,				"BPartInfo")

	C(MC_FIELD_SECTOR_MAGIC_AREA_INFO,		MCDT_MACHINE2MACHINE, MCF_S2C, "Sector Magic Area Info")
											P(MPT_BLOB,				"MagicAreaInfo")

	C(MC_FIELD_SECTOR_SENSOR_INFO,			MCDT_MACHINE2MACHINE, MCF_S2C, "Sector Sensor Info")
											P(MPT_BLOB,				"SensorInfo")
											
	C(MC_DYNAMIC_FIELD_ENTER_REQ,			MCDT_MACHINE2MACHINE, MCF_C2S, "Request Enter To DynamicField")
											P(MPT_INT,		"DynamicFieldKey")
	C(MC_DYNAMIC_FIELD_LEAVE_REQ,			MCDT_MACHINE2MACHINE, MCF_C2S, "Request Leave From DynamicField")

	C(MC_DYNAMIC_FIELD_EXPIRING,			MCDT_MACHINE2MACHINE, MCF_S2C, "DynamicField Expiring")
											P(MPT_UINT,		"nExpiredTime")

	C(MC_DYNAMIC_FIELD_CANCEL_EXPIRE,		MCDT_MACHINE2MACHINE, MCF_S2C, "DynamicField Cancel Expire")


	C(MC_ACTION_FAILED,						MCDT_MACHINE2MACHINE, MCF_S2C, "Action Failed")

	C(MC_ACTION_MOVE_REQ,					MCDT_MACHINE2MACHINE|MCDT_FILTER_CROWDED, MCF_C2S, "Request Move")
											P(MPT_SINGLE_BLOB,		"MoveInfo")

	C(MC_ACTION_MOVE,						MCDT_MACHINE2MACHINE|MCDT_FILTER_CROWDED, MCF_S2C, "Move")
											P(MPT_USHORT,			"nPlayerUIID")
											P(MPT_SINGLE_BLOB,		"MovingInfo")

	C(MC_ACTION_POS_SYNC_REQ,				MCDT_MACHINE2MACHINE, MCF_C2S, "Request Pos Sync")
											P(MPT_VEC,		"Position")
	C(MC_ACTION_MOVING_MF_POS_SYNC_REQ,		MCDT_MACHINE2MACHINE, MCF_C2S, "Request Moving MotionFactor Pos Sync")
											P(MPT_VEC,		"Position")
											
	C(MC_ACTION_ATTACK_CANCEL_REQ,			MCDT_MACHINE2MACHINE, MCF_C2S, "Request Attack Cancel")
	C(MC_ACTION_ATTACK_CANCEL,				MCDT_MACHINE2MACHINE, MCF_S2C, "Attack Cancel")
											P(MPT_USHORT,		"nPlayerUIID")

	C(MC_ACTION_PREPARE_ATTACK_REQ,			MCDT_MACHINE2MACHINE, MCF_C2S, "Request AttackPrepare")
	C(MC_ACTION_PREPARE_ATTACK,				MCDT_MACHINE2MACHINE, MCF_S2C, "Prepare Attack")
											P(MPT_USHORT,	"nPlayerUIID")
											P(MPT_INT,		"nTalentID")

	C(MC_ACTION_GUARD_REQ,					MCDT_MACHINE2MACHINE, MCF_C2S, "Request Guard")
	/*
	C(MC_ACTION_GUARD,						MCDT_MACHINE2MACHINE, MCF_S2C, "Guard")
											P(MPT_USHORT,	"nEntityUIID")
											P(MPT_SVEC2,	"Dir")
											P(MPT_VEC,		"Pos")
	*/
	C(MC_ACTION_GUARD_ME,					MCDT_MACHINE2MACHINE, MCF_S2C, "Guard Me")
	C(MC_ACTION_GUARD_OTHER,				MCDT_MACHINE2MACHINE, MCF_S2C, "Guard Other")
											P(MPT_USHORT,	"nEntityUIID")
											P(MPT_SVEC2,	"Dir")
											P(MPT_VEC,		"Pos")
	C(MC_ACTION_GUARD_FAILED,				MCDT_MACHINE2MACHINE, MCF_S2C, "Guard Failed")
											P(MPT_USHORT,	"nEntityUIID")
	C(MC_ACTION_GUARD_RELEASED_REQ,			MCDT_MACHINE2MACHINE, MCF_C2S, "Request Relased Guard")
	C(MC_ACTION_GUARD_RELEASED,				MCDT_MACHINE2MACHINE, MCF_S2C, "Relased Guard")
											P(MPT_USHORT,	"nEntityUIID")
	C(MC_ACTION_GUARD_DEFENSE,				MCDT_MACHINE2MACHINE, MCF_S2C, "Guard Defense")
											P(MPT_SINGLE_BLOB,		"GuardInfo")
	C(MC_ACTION_GUARD_DEFENSE_PARTIAL,		MCDT_MACHINE2MACHINE, MCF_S2C, "Guard Defense")
											P(MPT_SINGLE_BLOB,		"PartialGuardInfo")
											

	C(MC_ACTION_ATTACK_MELEE_REQ,			MCDT_MACHINE2MACHINE, MCF_C2S, "Request Melee Attacking")
											P(MPT_SINGLE_BLOB,		"AttackInfo")

	//C(MC_ACTION_ATTACK_MELEE,				MCDT_MACHINE2MACHINE, MCF_S2C, "Melee Attack")
	//										P(MPT_BLOB,		"AttackInfo")

	C(MC_ACTION_ATTACK_ARCHERY_REQ,			MCDT_MACHINE2MACHINE, MCF_C2S, "Request Attack Archery")
											P(MPT_VEC,		"Pos")		// 위치
											P(MPT_VEC,		"Dir")		// 방향
											P(MPT_UID,		"uidTarget")	// 목표

	C(MC_ACTION_ATTACK_ARCHERY,				MCDT_MACHINE2MACHINE, MCF_S2C, "Attack Archery")
											P(MPT_UID,		"uidEntity")
											P(MPT_INT,		"TalentID")
											P(MPT_VEC,		"Pos")		// 위치
											P(MPT_VEC,		"Dir")		// 방향
											P(MPT_UID,		"uidTarget")	// 목표

	C(MC_ACTION_ATTACK_ARCHERY_DETAIL_REQ, 	MCDT_MACHINE2MACHINE, MCF_C2S, "Request Attack Archery Detail")
											P(MPT_VEC,		"Pos")		// 위치
											P(MPT_VEC,		"Dir")		// 방향
											P(MPT_UID,		"uidTarget")	// 목표
											P(MPT_CHAR,		"nCapsuleGroupIndex")	
											P(MPT_CHAR,		"nCapsuleIndex")

	C(MC_ACTION_ATTACK_ARCHERY_DETAIL, 		MCDT_MACHINE2MACHINE, MCF_S2C, "Attack Archery Detail")
											P(MPT_UID,		"uidEntity")
											P(MPT_INT,		"TalentID")
											P(MPT_VEC,		"Pos")		// 위치
											P(MPT_VEC,		"Dir")		// 방향
											P(MPT_UID,		"uidTarget")	// 목표
											P(MPT_CHAR,		"nCapsuleGroupIndex")	
											P(MPT_CHAR,		"nCapsuleIndex")

	C(MC_ACTION_ATTACK_GUN_REQ,				MCDT_MACHINE2MACHINE, MCF_C2S, "Request Attack Gun")
											P(MPT_VEC,		"Pos")
											P(MPT_VEC,		"Dir")
											P(MPT_UID,		"uidTarget")	
	C(MC_ACTION_ATTACK_GUN,					MCDT_MACHINE2MACHINE, MCF_S2C, "Attack Gun")
											P(MPT_UID,		"uidPlayer")
											P(MPT_VEC,		"Pos")
											P(MPT_VEC,		"Dir")
											P(MPT_UID,		"uidTarget")
											P(MPT_BOOL,		"bIsHitted")

	C(MC_ACTION_JUMP_REQ,					MCDT_MACHINE2MACHINE, MCF_C2S, "Request Action")
											P(MPT_VEC,		"Pos")
											P(MPT_VEC,		"Dir")
											P(MPT_USHORT,	"MoveFlag")

	C(MC_ACTION_JUMP,						MCDT_MACHINE2MACHINE, MCF_S2C, "Player Action")
											P(MPT_USHORT,	"nPlayerUIID")
											P(MPT_VEC,		"Pos")
											P(MPT_VEC,		"Dir")
											P(MPT_USHORT,	"MoveFlag")

	C(MC_ACTION_FALLING_REQ,				MCDT_MACHINE2MACHINE, MCF_C2S, "Request Action Falling")
											P(MPT_VEC,		"Pos")
											P(MPT_SVEC,		"Dir")

	C(MC_ACTION_FALLING,					MCDT_MACHINE2MACHINE, MCF_S2C, "Player Action Falling")
											P(MPT_USHORT,	"nPlayerUIID")
											P(MPT_VEC,		"Pos")
											P(MPT_SVEC,		"Dir")

	C(MC_ACTION_END_FALLING,				MCDT_MACHINE2MACHINE, MCF_C2S, "Player End Falling")
											P(MPT_BOOL,		"bIsFallenOnWater")	// true-Water, false-Ground
											P(MPT_FLOAT,	"fFallingHeight")

	C(MC_ACTION_FALLING_DAMAGE,				MCDT_MACHINE2MACHINE, MCF_S2C, "Falling Damage")
											P(MPT_USHORT,	"nPlayerUIID")
											P(MPT_INT,		"nFallingDamage")

	C(MC_ACTION_STOP_REQ,					MCDT_MACHINE2MACHINE, MCF_C2S, "Request Stop")
											P(MPT_VEC,		"Pos")
											P(MPT_SVEC2,	"Dir")
	C(MC_ACTION_STOP,						MCDT_MACHINE2MACHINE|MCDT_FILTER_CROWDED, MCF_S2C, "Stop")
											P(MPT_USHORT,	"nPlayerUIID")
											P(MPT_VEC,		"Pos")
											P(MPT_SVEC2,	"Dir")
	C(MC_ACTION_STOP_MOVE_REQ,				MCDT_MACHINE2MACHINE, MCF_C2S, "Request Stop Move")
											P(MPT_VEC,		"Pos")
											P(MPT_SVEC2,	"Dir")
	C(MC_ACTION_STOP_MOVE,					MCDT_MACHINE2MACHINE|MCDT_FILTER_CROWDED, MCF_S2C, "Stop Move")
											P(MPT_USHORT,	"nPlayerUIID")
											P(MPT_VEC,		"Pos")
											P(MPT_SVEC2,	"Dir")
	C(MC_ACTION_ROTATE_REQ,					MCDT_MACHINE2MACHINE, MCF_C2S, "Request Rotate")
											P(MPT_SVEC2,	"Dir")
	C(MC_ACTION_ROTATE,						MCDT_MACHINE2MACHINE|MCDT_FILTER_CROWDED, MCF_S2C, "Rotate")
											P(MPT_USHORT,	"nPlayerUIID")
											P(MPT_SVEC2,	"Dir")

	C(MC_ACTION_CHANGE_STANCE_REQ,			MCDT_MACHINE2MACHINE, MCF_C2S, "Request ChangeStance")
											P(MPT_CHAR,		"Value")	// 0-Normal, 1-Battle, 2-Fast Normal, 3-Fast Battle
	C(MC_ACTION_CHANGE_STANCE,				MCDT_MACHINE2MACHINE, MCF_S2C, "ChangeStance")
											P(MPT_USHORT,	"nPlayerUIID")
											P(MPT_CHAR,		"Value")	// 0-Normal, 1-Battle, 2-Fast Normal, 3-Fast Battle

	C(MC_ACTION_DIE,						MCDT_MACHINE2MACHINE, MCF_S2C, "Die")
											P(MPT_USHORT,	"nEntityUIID")
											P(MPT_UID,		"uidKiller")

	C(MC_ACTION_DESPAWN,					MCDT_MACHINE2MACHINE, MCF_S2C, "Despawn")
											P(MPT_USHORT,	"nEntityUIID")

	C(MC_ACTION_SWIM_REQ,					MCDT_MACHINE2MACHINE, MCF_C2S, "Request Swimming")
											P(MPT_BOOL,		"IsSwimming")

	C(MC_ACTION_CANCEL_NEXT_ATTACK,			MCDT_MACHINE2MACHINE, MCF_S2C, "Cancel Next Attack")
											P(MPT_USHORT,	"nEntityUIID")
											P(MPT_INT,		"nParam")

	C(MC_ACTION_STANDUP_REQ,				MCDT_MACHINE2MACHINE, MCF_C2S, "Request Stand up")

	C(MC_ACTION_STANDUP,					MCDT_MACHINE2MACHINE, MCF_S2C, "Stand up")
											P(MPT_USHORT,		"nEntityUIID")

	C(MC_ACTION_SWITCHING_WEAPON_BEGIN_REQ,	MCDT_MACHINE2MACHINE, MCF_C2S, "Request Switching Weapon Begin")
	C(MC_ACTION_SWITCHING_WEAPON_BEGIN,		MCDT_MACHINE2MACHINE, MCF_S2C, "Switching Weapon Begin")
											P(MPT_USHORT,		"nEntityUIID")

	C(MC_ACTION_SWITCHING_WEAPON_REQ,		MCDT_MACHINE2MACHINE, MCF_C2S, "Request Switching Weapon")
											P(MPT_CHAR,		"nWeaponSet")

	C(MC_ACTION_SWITCHING_WEAPON,			MCDT_MACHINE2MACHINE, MCF_S2C, "Switching Weapon")
											P(MPT_USHORT,	"nEntityUIID")
											P(MPT_CHAR,		"nWeaponSet")

	C(MC_ACTION_SIT_REQ,					MCDT_MACHINE2MACHINE, MCF_C2S, "Sit Req")
	C(MC_ACTION_SIT,						MCDT_MACHINE2MACHINE, MCF_S2C, "Sit")
											P(MPT_USHORT,	"nPlayerUIID")

	C(MC_ACTION_SIT_RISE_REQ,				MCDT_MACHINE2MACHINE, MCF_C2S, "Sit Rise Req")
	C(MC_ACTION_SIT_RISE,					MCDT_MACHINE2MACHINE, MCF_S2C, "Sit Rise")
											P(MPT_USHORT,	"nPlayerUIID")

	C(MC_ACTION_HIDE,						MCDT_MACHINE2MACHINE, MCF_S2C, "Hide")
											P(MPT_UID,		"uidActor")
	C(MC_ACTION_AFK_BEGIN,					MCDT_MACHINE2MACHINE, MCF_S2C, "AFK Begin")
											P(MPT_UID,		"uidPlayer")
	C(MC_ACTION_AFK_END,					MCDT_MACHINE2MACHINE, MCF_S2C, "AFK End")
											P(MPT_UID,		"uidPlayer")
	C(MC_ACTION_AFK_REQ,					MCDT_MACHINE2MACHINE, MCF_C2S, "AFK Request")
	C(MC_ACTION_END_COMBAT,					MCDT_MACHINE2MACHINE, MCF_S2C, "End Combat")
											
	C(MC_NPC_MOVE,							MCDT_MACHINE2MACHINE|MCDT_FILTER_CROWDED, MCF_S2C, "Move")
											P(MPT_SINGLE_BLOB,		"MoveInfo")

	C(MC_NPC_FACE_TO,						MCDT_MACHINE2MACHINE|MCDT_FILTER_CROWDED, MCF_S2C, "FaceTo")
											P(MPT_USHORT,	"nNPCUIID")
											P(MPT_SVEC2,	"dir")
											P(MPT_VEC,		"pos")

	C(MC_NPC_RUN,							MCDT_MACHINE2MACHINE|MCDT_FILTER_CROWDED, MCF_S2C, "Run")
											P(MPT_SINGLE_BLOB,		"MoveInfo")
											
	C(MC_NPC_MOVE_STOP,						MCDT_MACHINE2MACHINE|MCDT_FILTER_CROWDED, MCF_S2C, "Stop Move")
											P(MPT_USHORT,	"nNPCUIID")
											P(MPT_VEC,		"pos")

	C(MC_NPC_CHANGE_CAPSULE,				MCDT_MACHINE2MACHINE, MCF_S2C, "Change Capsule")
											P(MPT_UID,		"uidObject")
											P(MPT_CHAR,		"nIndex")

	C(MC_NPC_SAY,							MCDT_MACHINE2MACHINE, MCF_S2C, "NPC Say")
											P(MPT_UID,		"uidSpeaker")
											P(MPT_INT,		"nSpeakerID")
											P(MPT_WSTR,		"strText")
											P(MPT_FLOAT,	"fMaintainTime")

	C(MC_NPC_BALLOON,						MCDT_MACHINE2MACHINE, MCF_S2C, "NPC Balloon")
											P(MPT_UID,		"uidSpeaker")
											P(MPT_INT,		"nSpeakerID")
											P(MPT_WSTR,		"strText")
											P(MPT_FLOAT,	"fMaintainTime")

	C(MC_NPC_SHOUT,							MCDT_MACHINE2MACHINE, MCF_S2C, "NPC Shout")
											P(MPT_UID,		"uidSpeaker")
											P(MPT_INT,		"nSpeakerID")
											P(MPT_WSTR,		"strText")
											P(MPT_FLOAT,	"fMaintainTime")

	C(MC_NPC_NARRATION,						MCDT_MACHINE2MACHINE, MCF_S2C, "NPC Narration")
											P(MPT_WSTR,			"strText")

	C(MC_NPC_SAY_AND_NARRATION,			MCDT_MACHINE2MACHINE, MCF_S2C, "NPC Say and Narration")
											P(MPT_UID,		"uidSpeaker")
											P(MPT_INT,		"nSpeakerID")
											P(MPT_WSTR,		"strText")
											P(MPT_FLOAT,	"fMaintainTime")

	C(MC_NPC_CHANGE_ATTACKABLE,				MCDT_MACHINE2MACHINE, MCF_S2C, "NPC.EanbleCombat")
											P(MPT_UID,		"uidNPC")
											P(MPT_INT,		"nAttackable")
	C(MC_NPC_CHANGE_COMBAT_TARGET,			MCDT_MACHINE2MACHINE, MCF_S2C, "NPC.ChangeCombatTarget")
											P(MPT_UID,		"uidNPC")
											P(MPT_UID,		"uidTarget")
	C(MC_NPC_SPEWUP,						MCDT_MACHINE2MACHINE, MCF_S2C, "NPC.Spewup")
											P(MPT_UID,		"uidNPC")
	C(MC_NPC_SPEWUP_INTERRUPT,				MCDT_MACHINE2MACHINE, MCF_S2C, "NPC.SpewupInterrupt")
											P(MPT_UID,		"uidNPC")	

	C(MC_NPC_START_COMBAT,					MCDT_MACHINE2MACHINE, MCF_S2C, "NPC.StartCombat")
											P(MPT_USHORT,	"nNPCUIID")
	C(MC_NPC_END_COMBAT,					MCDT_MACHINE2MACHINE, MCF_S2C, "NPC.EndCombat")
											P(MPT_USHORT,	"nNPCUIID")

	C(MC_NPC_MARK_POS,						MCDT_MACHINE2MACHINE, MCF_S2C, "NPC.MakrPos")
											P(MPT_INT,		"nNPCID")
											P(MPT_VEC,		"vecPos")
	C(MC_NPC_UNMARK_POS,					MCDT_MACHINE2MACHINE, MCF_S2C, "NPC.UnmakrPos")
											P(MPT_INT,		"nNPCID")

	C(MC_TRIGGER_TOUCH_SENSOR_ENTER,		MCDT_MACHINE2MACHINE, MCF_C2S, "Trigger.TouchSensorEnter")
											P(MPT_UINT,		"nSensorID")
											P(MPT_VEC,		"vecPos")

	C(MC_TRIGGER_TOUCH_SENSOR_LEAVE,		MCDT_MACHINE2MACHINE, MCF_C2S, "Trigger.TouchSensorLeave")
											P(MPT_UINT,		"nSensorID")
											P(MPT_VEC,		"vecPos")

	C(MC_TRIGGER_SENSOR_INTERACTION_REQ,	MCDT_MACHINE2MACHINE, MCF_C2S, "Trigger.RequestInteraction")
											P(MPT_BLOB,		"vecEnteredSensorID")											

	C(MC_TRIGGER_SENSOR_ERROR_MSG,			MCDT_MACHINE2MACHINE, MCF_S2C, "Trigger.SensorError")																						

	C(MC_TRIGGER_SENSOR_FOLLOW_PARTY_REQ,	MCDT_MACHINE2MACHINE, MCF_S2C, "Trigger.SensorFollowPartyReq")
											P(MPT_WSTR,		"strMemberName")

	C(MC_TRIGGER_SENSOR_FOLLOW_PARTY_RES,	MCDT_MACHINE2MACHINE, MCF_C2S, "Trigger.SensorFollowPartyRes")											

	C(MC_TRIGGER_UPDATE_USABLE_SENSOR,		MCDT_MACHINE2MACHINE, MCF_S2C, "Trigger.UpdateUsableSensor")										
											P(MPT_BLOB,		"UsableSensorInfo")

	C(MC_TRIGGER_SENSOR_DESTINATION_SEL_REQ,MCDT_MACHINE2MACHINE, MCF_S2C, "Trigger.Request Sensor Destination Selection")
											P(MPT_BLOB,		"UsableSensorInfo")

	C(MC_TRIGGER_SENSOR_DESTINATION_SEL,	MCDT_MACHINE2MACHINE, MCF_C2S, "Trigger.Sensor Destination Selection")
											P(MPT_INT,		"Selection")
	
	C(MC_ENTITY_WARP,						MCDT_MACHINE2MACHINE, MCF_S2C, "Warp")
											P(MPT_UID,		"uidEntity")
											P(MPT_VEC,		"vecPos")
											P(MPT_VEC,		"vecDir")

	C(MC_FIELD_REQUEST_FEATURE,				MCDT_MACHINE2MACHINE, MCF_C2S, "Request Feature Info")
											P(MPT_UID,		"uidPlayer")

	C(MC_FIELD_FEATURE,						MCDT_MACHINE2MACHINE, MCF_S2C, "Feature Info")
											P(MPT_UID,		"uidPlayer")
											P(MPT_BLOB,		"ItemListInfo")

	C(MC_ITEM_EQUIP_REQ,					MCDT_MACHINE2MACHINE, MCF_C2S, "Request Equip Item")								
											P(MPT_INT,		"nSlotTypeFrom")
											P(MPT_INT,		"nSlotIDFrom")
											P(MPT_CHAR,		"nSlotIDTo")

	C(MC_ITEM_EQUIP,						MCDT_MACHINE2MACHINE, MCF_S2C, "Equip Item")
											P(MPT_INT,		"nSlotTypeFrom")
											P(MPT_INT,		"nSlotIDFrom")
											P(MPT_CHAR,		"nSlotIDTo")

	C(MC_ITEM_EQUIP_SWAPTOINVEN,			MCDT_MACHINE2MACHINE, MCF_S2C, "Equip Item Swap To Inven")
											P(MPT_INT,		"nEquipSlotIDFrom")
											P(MPT_CHAR,		"nEquipSlotIDTo")
											P(MPT_CHAR,		"nUnequipSlotIDFrom")
											P(MPT_INT,		"nUnequipSlotIDTo")
											

	C(MC_ITEM_UNEQUIP_REQ,					MCDT_MACHINE2MACHINE, MCF_C2S, "Request Unequip Item")
											P(MPT_CHAR,		"nItemSlot")

	C(MC_ITEM_UNEQUIP,						MCDT_MACHINE2MACHINE, MCF_S2C, "Unequip Item")
											P(MPT_CHAR,		"nItemSlot")
											P(MPT_INT,		"nInvenSlotID")
	
	C(MC_ITEM_USE_REQ,						MCDT_MACHINE2MACHINE, MCF_C2S, "Request Use Item")
											P(MPT_INT,		"nInvenSlotID")
											P(MPT_WSTR,		"ChangeName")

	C(MC_ITEM_USE_FAIL,						MCDT_MACHINE2MACHINE, MCF_S2C, "Use Item Fail")
											P(MPT_INT,		"nInvenSlotID")											
											P(MPT_INT,		"nConditionType")
											P(MPT_BLOB,		"vecParam1")
											P(MPT_BLOB,		"vecParam2")
 
	/*
	C(MC_ITEM_TALENT,						MCDT_MACHINE2MACHINE, MCF_S2C, "Use Item")
											P(MPT_INT,		"nAmount")
											P(MPT_INT,		"nInvenSlotID")
											*/

	C(MC_ITEM_CHANGE_LOOK,					MCDT_MACHINE2MACHINE, MCF_S2C, "Change Look")
											P(MPT_USHORT,	"nPlayerUIID")
											P(MPT_CHAR,		"nSlotID")
											P(MPT_INT,		"nItemID")
											P(MPT_INT,		"nItemColor")
											P(MPT_INT,		"nGemEnchantBuff")
											P(MPT_CHAR,		"nItemEnchantGrade")
											
	C(MC_ITEM_REMOVE_LOOK,					MCDT_MACHINE2MACHINE, MCF_S2C, "Remove Look")
											P(MPT_USHORT,	"nPlayerUIID")
											P(MPT_CHAR,		"nItemSlot")

	C(MC_ITEM_CHANGE_AND_REMOVE_LOOK,		MCDT_MACHINE2MACHINE, MCF_S2C, "Change And Remove Look")
											P(MPT_USHORT,	"nPlayerUIID")
											P(MPT_CHAR,		"nSlotIDFrom")
											P(MPT_CHAR,		"nSlotIDTo")
											P(MPT_INT,		"nItemID")
											P(MPT_INT,		"nItemColor")
											P(MPT_INT,		"nGemEnchantBuff")
											P(MPT_CHAR,		"nItemEnchantGrade")

	C(MC_ITEM_ADD,							MCDT_MACHINE2MACHINE, MCF_S2C, "Add Item")
											P(MPT_INT,		"nRewardCause")
											P(MPT_BLOB,		"InsertItem")
											P(MPT_BLOB,		"IncreaseItem")

	C(MC_ITEM_DROP_REQ,						MCDT_MACHINE2MACHINE, MCF_C2S, "Request Drop Item")
											P(MPT_INT,		"nSlotType")
											P(MPT_INT,		"nSlotID")
											P(MPT_INT,		"nDropQuantity")

	C(MC_ITEM_REMOVE,						MCDT_MACHINE2MACHINE, MCF_S2C, "Remove Item")
											P(MPT_BLOB,		"DeleteItem")
											P(MPT_BLOB,		"DecreaseItem")

	C(MC_ITEM_REMOVE_WITH_CAUSE,			MCDT_MACHINE2MACHINE, MCF_S2C, "Remove Item with cause")
											P(MPT_INT,		"nRewardCause")
											P(MPT_BLOB,		"DeleteItem")
											P(MPT_BLOB,		"DecreaseItem")
	
	C(MC_ITEM_UPDATE_DURABILITY,			MCDT_MACHINE2MACHINE, MCF_S2C, "Change Durability")
											P(MPT_BLOB,		"TD_UPDATE_DURATION")

	C(MC_ITEM_INTERACT_NPC_REQ,				MCDT_MACHINE2MACHINE, MCF_C2S, "Request item interact to npc");
											P(MPT_INT,		"nInvenSlotID")
											P(MPT_UID,		"uidNPC")

	C(MC_ITEM_MOVE_REQ,						MCDT_MACHINE2MACHINE, MCF_C2S, "Move Item Req")
											P(MPT_INT,		"nFromSlotType")
											P(MPT_INT,		"nFromSlotID")
											P(MPT_INT,		"nFromItemID")
											P(MPT_INT,		"nToSlotType")
											P(MPT_INT,		"nToSlotID")
											P(MPT_INT,		"nToItemID")
											P(MPT_INT,		"nAmount")

	C(MC_ITEM_MOVE,							MCDT_MACHINE2MACHINE, MCF_S2C, "Move Item")
											P(MPT_CHAR,		"nFromSlotType")
											P(MPT_SHORT,	"nFromSlotID")
											P(MPT_CHAR,		"nToSlotType")
											P(MPT_SHORT,	"nToSlotID")
											P(MPT_INT,		"nAmount")

	C(MC_ITEM_SORT_REQ,						MCDT_MACHINE2MACHINE, MCF_C2S, "Sort Item Req")
											P(MPT_INT,		"nSlotType")
											P(MPT_BLOB,		"vecTDItemSort")

	C(MC_ITEM_SORT,							MCDT_MACHINE2MACHINE, MCF_S2C, "Sort Item")
											P(MPT_INT,		"nSlotType")
											P(MPT_INT,		"nInvenIndex")
											P(MPT_BLOB,		"vecItem")


	C(MC_ITEM_DYE_PREPARE_REQ,				MCDT_MACHINE2MACHINE, MCF_C2S, "Request Dye Prepare Item")
											P(MPT_INT,		"nTargetSlotType")
											P(MPT_INT,		"nTargetSlotID")
											P(MPT_INT,		"nDyeSlotID")

	C(MC_ITEM_DYE_PREPARECANCEL_REQ,		MCDT_MACHINE2MACHINE, MCF_C2S, "Request Dye Prepare Cancel Item")


	C(MC_ITEM_DYE_REQ,						MCDT_MACHINE2MACHINE, MCF_C2S, "Request Dye Item")
											P(MPT_INT,		"nTargetSlotType")
											P(MPT_INT,		"nTargetSlotID")
											P(MPT_INT,		"nDyeSlotID")

	C(MC_ITEM_DYE,							MCDT_MACHINE2MACHINE, MCF_S2C, "Dye Item")											
											P(MPT_INT,		"nSlotType")
											P(MPT_INT,		"nSlotID")
											P(MPT_INT,		"nColor")	
											

	C(MC_ACTION_USE_TALENT_REQ,				MCDT_MACHINE2MACHINE, MCF_C2S, "Request Use Talent")
											P(MPT_INT,		"TalentID")
											P(MPT_SVEC,		"CharDir")
											P(MPT_VECTOR,	"CharPos")

	C(MC_ACTION_USE_TALENT,					MCDT_MACHINE2MACHINE|MCDT_FILTER_CROWDED, MCF_S2C, "Use Talent")
											P(MPT_USHORT,	"nEntityUIID")
											P(MPT_INT,		"TalentID")
											P(MPT_SVEC,		"CharDir")
											P(MPT_VEC,		"CharPos")

	C(MC_ACTION_ACT_TALENT_REQ,				MCDT_MACHINE2MACHINE, MCF_C2S, "Request Act Spell")
											P(MPT_INT,			"TalentID")
											P(MPT_SVEC,			"Dir")	
											P(MPT_UID,			"TargetUID")
	C(MC_ACTION_ACT_TALENT_WITH_HITCAPSULE_REQ,	MCDT_MACHINE2MACHINE, MCF_C2S, "Request Act Spell With Hitcapsule")
											P(MPT_INT,			"TalentID")
											P(MPT_SVEC,			"Dir")	
											P(MPT_SINGLE_BLOB,	"TargetInfo")
	C(MC_ACTION_ACT_TALENT_WITH_GROUND_REQ,	MCDT_MACHINE2MACHINE, MCF_C2S, "Request Act Spell With GroundInfo")
											P(MPT_INT,			"TalentID")
											P(MPT_SVEC,			"Dir")	
											P(MPT_VEC,			"GroundPos")

	C(MC_ACTION_ACT_TALENT,					MCDT_MACHINE2MACHINE|MCDT_FILTER_CROWDED, MCF_S2C, "Act Talent")
											P(MPT_USHORT,	"nEntityUIID")
											P(MPT_INT,		"TalentID")
											P(MPT_SVEC,		"Dir")

	C(MC_ACTION_ACT_TALENT_WITH_GROUND,		MCDT_MACHINE2MACHINE, MCF_S2C, "Act Talent")
											P(MPT_USHORT,	"nEntityUIID")
											P(MPT_INT,		"TalentID")
											P(MPT_SVEC,		"Dir")
											P(MPT_VEC,		"TargetPos")

	C(MC_ACTION_EXTRA_ACT_TALENT,			MCDT_MACHINE2MACHINE, MCF_S2C, "Extra Act Talent")
											P(MPT_USHORT,	"uidEntity")
											P(MPT_INT,		"TalentID")
											P(MPT_VECTOR,	"Pos")
											P(MPT_SVEC,		"Dir")

	C(MC_ACTION_ACT_TALENT_MISS_REQ,		MCDT_MACHINE2MACHINE|MCDT_FILTER_CROWDED, MCF_C2S, "Act Talent")
											P(MPT_INT,		"TalentID")
											P(MPT_SVEC,		"Dir")
	C(MC_ACTION_ACT_TALENT_MISS,			MCDT_MACHINE2MACHINE|MCDT_FILTER_CROWDED, MCF_S2C, "Act Talent")
											P(MPT_USHORT,	"uidEntity")
											P(MPT_INT,		"TalentID")
											P(MPT_SVEC,		"Dir")

	C(MC_ACTION_REQUEST_FAIL_TALENT,		MCDT_MACHINE2MACHINE, MCF_S2C, "Talent Request Failed")
											P(MPT_INT,		"TalentID")

	C(MC_ACTION_CANCEL_TALENT_REQ,			MCDT_MACHINE2MACHINE, MCF_C2S, "Request Cancel Talent")

	C(MC_ACTION_END_TALENT_COOLDOWN,		MCDT_MACHINE2MACHINE, MCF_S2C, "Talent Cooldown Finish")
											P(MPT_INT,		"TalentID")

	C(MC_ACTION_CANCEL_TALENT,				MCDT_MACHINE2MACHINE, MCF_S2C, "Cancel Talent")
											P(MPT_USHORT,	"nEntityUIID")
											P(MPT_INT,		"TalentID")

	C(MC_ACTION_ACT_TALENT_PROJECTILE,		MCDT_MACHINE2MACHINE, MCF_S2C, "Act Spell(Projectile)")
											P(MPT_UID,		"uidEntity")
											P(MPT_INT,		"TalentID")
											P(MPT_VEC,		"EntityDir")
											P(MPT_VEC,		"TargetPos")
											P(MPT_UID,		"uidTarget")
											P(MPT_CHAR,		"nCapsuleGroupIndex")	
											P(MPT_CHAR,		"nCapsuleIndex")

	/*
	C(MC_ACTION_ACT_SPELL_MAGICAREA,		MCDT_MACHINE2MACHINE, MCF_S2C, "Act Spell(MagicArea)")
											P(MPT_USHORT,	"nEntityUIID")
											P(MPT_INT,		"TalentID")
											P(MPT_VEC,		"TargetPos")
	*/

	C(MC_ACTION_RECASTING_SPELL,			MCDT_MACHINE2MACHINE, MCF_S2C, "ReCasting Spell")
											P(MPT_USHORT,	"nEntityUIID")
											P(MPT_INT,		"TalentID")
											P(MPT_INT,		"TalentStep")

///////////////

											

	C(MC_ACTION_TALENT_HIT,					MCDT_MACHINE2MACHINE|MCDT_FILTER_CROWDED, MCF_S2C, "Hit from Talent")
											P(MPT_SINGLE_BLOB,		"HitInfo")

	C(MC_ACTION_TALENT_HIT_NO_MF,			MCDT_MACHINE2MACHINE|MCDT_FILTER_CROWDED, MCF_S2C, "Hit from Talent but no MotionFactor")
											P(MPT_USHORT,	"nVictimUIID")
											P(MPT_USHORT,	"nAttackerUIID")
											P(MPT_INT,		"nTalentID")

	C(MC_ACTION_TALENT_HIT_KNOCKBACK_MF,	MCDT_MACHINE2MACHINE|MCDT_FILTER_CROWDED, MCF_S2C, "Hit from Talent with knockback MotionFactor")
											P(MPT_SINGLE_BLOB,		"HitInfo")
											P(MPT_SINGLE_BLOB,		"KnockbackInfo")

	C(MC_ACTION_BUFF_HIT,					MCDT_MACHINE2MACHINE, MCF_S2C, "Hit from Buff")
											P(MPT_SINGLE_BLOB,		"HitInfo")

	C(MC_ACTION_BUFF_HIT_NO_MF,				MCDT_MACHINE2MACHINE, MCF_S2C, "Hit from Buff but no MotionFactor")
											P(MPT_USHORT,	"nVictimUIID")
											P(MPT_INT,		"nTalentID")

	C(MC_ACTION_TALENT_HIT_MISS,			MCDT_MACHINE2MACHINE, MCF_S2C, "Hit But Miss")
											P(MPT_USHORT,	"nVictimUIID")
											P(MPT_USHORT,	"nAttackerUIID")
											P(MPT_INT,		"nTalentID")

	C(MC_ACTION_TALENT_HEAL,				MCDT_MACHINE2MACHINE, MCF_S2C, "Talent Healing")
											P(MPT_USHORT,	"nUserUIID")
											P(MPT_USHORT,	"nTargetUIID")
											P(MPT_INT,		"nTalentID")
											P(MPT_INT,		"nAmount")
											P(MPT_BOOL,		"bCritical")

	C(MC_ACTION_TALENT_DRAIN,				MCDT_MACHINE2MACHINE, MCF_S2C, "Drain")
											P(MPT_INT,		"nTalentID")
											P(MPT_UCHAR,	"nDrainType")
											P(MPT_INT,		"nDrainAmount")
	
	C(MC_ACTION_BREAK_PART,					MCDT_MACHINE2MACHINE, MCF_S2C, "Break Part")
											P(MPT_UID,		"uidOwnerEntity")
											P(MPT_UID,		"uidOpponentEntity")
											P(MPT_CHAR,		"nPartID")

	C(MC_ACTION_BPART_RECOVERY,				MCDT_MACHINE2MACHINE, MCF_S2C, "BreakPart Recovery")
											P(MPT_UID,		"uidOwnerNPC")

	C(MC_ACTION_BREAK_PART_EX,				MCDT_MACHINE2MACHINE, MCF_S2C, "Break Part")
											P(MPT_UID,		"uidOwnerEntity")
											P(MPT_UID,		"uidOpponentEntity")
											P(MPT_CHAR,		"nPartID")
											P(MPT_BLOB,		"infoUsableBParts")

	// 퀘스트(QUEST) -------------------------------------------------------------------------------
	C(MC_QUEST_GIVE_REQ,					MCDT_MACHINE2MACHINE, MCF_C2S, "Request Give Quest")											
	C(MC_QUEST_GIVE,						MCDT_MACHINE2MACHINE, MCF_S2C, "Response Give Quest")
											P(MPT_INT,		"nQuestID")
											P(MPT_UINT,		"nAccepTime")
											P(MPT_UINT,		"nLimitTime")
											P(MPT_VEC,		"vRewardPos")
											P(MPT_BOOL,		"bChallengerQuest")
											
	C(MC_QUEST_UPDATE_OBJECTIVE,			MCDT_MACHINE2MACHINE, MCF_S2C, "Quest Objective Updated")
											P(MPT_SINGLE_BLOB,		"UpdatedObjective")
	C(MC_QUEST_COMPLETE,					MCDT_MACHINE2MACHINE, MCF_S2C, "Quest Complete")
											P(MPT_INT,		"nQuestID")
	C(MC_QUEST_INCOMPLETE,					MCDT_MACHINE2MACHINE, MCF_S2C, "Quest Incomplete")
											P(MPT_INT,		"nQuestID")											
	C(MC_QUEST_FAIL,						MCDT_MACHINE2MACHINE, MCF_S2C, "Quest Failure")
											P(MPT_INT,		"nQuestID")
	C(MC_QUEST_REWARD_REQ,					MCDT_MACHINE2MACHINE, MCF_C2S, "Request Reward")
											P(MPT_UID,		"uidNPC")
											P(MPT_INT,		"nSelectedReward")
	C(MC_QUEST_REWARD,						MCDT_MACHINE2MACHINE, MCF_S2C, "Response Reward")
											P(MPT_UID,		"uidNPC")
											P(MPT_INT,		"nQuestID")										
	C(MC_QUEST_CANCEL_REQ,					MCDT_MACHINE2MACHINE, MCF_C2S, "Request Cancel")
											P(MPT_INT,		"nQuestID")
	C(MC_QUEST_CANCEL,						MCDT_MACHINE2MACHINE, MCF_S2C, "Response Cancel")
											P(MPT_INT,		"nQuestID")
	C(MC_QUEST_SHOW_ACCPET_UI,				MCDT_MACHINE2MACHINE, MCF_S2C, "Show Accept UI")
											P(MPT_INT,		"nQuestID")
	C(MC_QUEST_SHOW_REWARD_UI,				MCDT_MACHINE2MACHINE, MCF_S2C, "Show Reward UI")
											P(MPT_INT,		"nQuestID")

	C(MC_QUEST_SHARE_REQ,					MCDT_MACHINE2MACHINE, MCF_C2S, "Request Quest Share")
											P(MPT_INT,		"nQuestID")

	C(MC_QUEST_SHARE_REQ_RELAY,				MCDT_MACHINE2MACHINE, MCF_S2C, "Relay Quest Share Request")
											P(MPT_SINGLE_BLOB,	"result")

	C(MC_QUEST_SHARE,						MCDT_MACHINE2MACHINE, MCF_S2C, "Quest Share")
											P(MPT_BLOB,		"vecResult")

	C(MC_QUEST_SHARE_GIVE_REQ,				MCDT_MACHINE2MACHINE, MCF_C2S, "Request Share Give Quest")
											P(MPT_INT,		"nQuestID")											

	C(MC_QUEST_INTERACT_QOBJECTIVE_BEGIN,	MCDT_MACHINE2MACHINE, MCF_S2C, "Quest Interact Begin")
											P(MPT_UID,		"uidLooter")
											P(MPT_UID,		"uidNPC")

	C(MC_QUEST_INTERACT_QOBJECTIVE_CANCEL_REQ,		MCDT_MACHINE2MACHINE, MCF_C2S, "Quest Interact End Req")

	C(MC_QUEST_INTERACT_QOBJECTIVE_END,		MCDT_MACHINE2MACHINE, MCF_S2C, "Quest Interact End")
											P(MPT_UID,		"uidLooter")										

	C(MC_CHALLENGER_QUEST_LIST,				MCDT_MACHINE2MACHINE, MCF_S2C, "Challenger Quest List")
											P(MPT_BLOB,		"TD_CHALLENGER_QUEST")
	
	C(MC_CHALLENGER_QUEST_COMPLETE,			MCDT_MACHINE2MACHINE, MCF_S2C, "Challenger Quest Complete")

	C(MC_CHALLENGER_QUEST_LIST_REFRESH,		MCDT_MACHINE2MACHINE, MCF_S2C, "Challenger Quest List")
											P(MPT_BLOB,		"TD_CHALLENGER_QUEST")

	// 다이얼로그(DIALOG) -------------------------------------------------------------------------------------------
	C(MC_DIALOG_START,						MCDT_MACHINE2MACHINE, MCF_S2C, "Start Dialog")
											P(MPT_INT,		"nSayTextID")
											P(MPT_BLOB,		"Select")

	C(MC_DIALOG_SELECT_REQ,					MCDT_MACHINE2MACHINE, MCF_C2S, "Request Select")
											P(MPT_INT,		"nSelectID")

	C(MC_DIALOG_SELECT,						MCDT_MACHINE2MACHINE, MCF_S2C, "Response Select")
											P(MPT_INT,		"nSayTextID")
											P(MPT_BLOB,		"Select")


	// 트레이닝(TRAININIG) -------------------------------------------------------------------------------------------
	C(MC_TRAINING_TRAIN_REQ,				MCDT_MACHINE2MACHINE, MCF_C2S, "Request Train")
											P(MPT_INT,		"eSkillSetTpye")
											P(MPT_INT,		"nTalentID")
	C(MC_TRAINING_TRAIN,					MCDT_MACHINE2MACHINE, MCF_S2C, "Train")
											P(MPT_INT,		"eSkillSetTpye")
											P(MPT_INT,		"nTalentID")
											P(MPT_BOOL,		"bConsumeTP")
	C(MC_TRAINING_UNTRAIN_ALL,				MCDT_MACHINE2MACHINE, MCF_S2C, "Untrain All")
											P(MPT_INT,		"eSkillSetTpye")
											P(MPT_INT,		"nTotalTP")
											P(MPT_BOOL,		"bForce")
	C(MC_TRAINING_TRAIN_LIMIT_INFO_REQ,		MCDT_MACHINE2MACHINE, MCF_C2S, "Train Limit Info Req")
											P(MPT_INT,		"eSkillSetTpye")
	C(MC_TRAINING_TRAIN_LIMIT_INFO,			MCDT_MACHINE2MACHINE, MCF_S2C, "Train Limit Info")
											P(MPT_INT,		"eSkillSetTpye")
											P(MPT_BOOL,		"bTrainLimit")
											P(MPT_INT,		"nTalentStyle")
											P(MPT_INT64,	"nNoLimitCharacterCreateDate")
											P(MPT_INT64,	"nLimitReleaseDate")

	// (SKILLSET) -------------------------------------------------------------------------------------------
	C(MC_SKILLSET_SWITCH_REQ,				MCDT_MACHINE2MACHINE, MCF_C2S, "SkillSet Swtich Req")
											P(MPT_INT,		"eSkillSetType")
	C(MC_SKILLSET_SWITCH,					MCDT_MACHINE2MACHINE, MCF_S2C, "SkillSet Swtich")
											P(MPT_INT,		"eSkillSetType")
											P(MPT_INT,		"TP")
	C(MC_SKILLSET_SWITCH_UI,				MCDT_MACHINE2MACHINE, MCF_S2C, "Show Create UI")
	C(MC_SKILLSET_SWITCH_PREPARE_REQ,		MCDT_MACHINE2MACHINE, MCF_C2S, "SkillSet Switch Prepare Req")
	C(MC_SKILLSET_SWITCH_PREPARE,			MCDT_MACHINE2MACHINE, MCF_S2C, "SkillSet Switch Prepare")


	// 버프(BUFF) -------------------------------------------------------------------------------------------
	C(MC_BUFF_GAIN,							MCDT_MACHINE2MACHINE, MCF_S2C, "Gain Buff")
											P(MPT_USHORT,	"nTargetUIID")
											P(MPT_INT,		"nBuffID")
											P(MPT_FLOAT,	"fDurationTime")

	C(MC_BUFF_GAIN_WITH_STACK,				MCDT_MACHINE2MACHINE, MCF_S2C, "Gain Buff (with stack)")
											P(MPT_USHORT,	"nTargetUIID")
											P(MPT_INT,		"nBuffID")
											P(MPT_FLOAT,	"fDurationTime")
											P(MPT_INT,		"nStack")
				
	C(MC_BUFF_LOST_REQ,						MCDT_MACHINE2MACHINE, MCF_C2S, "Lost Buff Req")
											P(MPT_INT,		"nBuffID")

	C(MC_BUFF_LOST,							MCDT_MACHINE2MACHINE, MCF_S2C, "Lost Buff")
											P(MPT_USHORT,	"nTargetUIID")
											P(MPT_INT,		"nBuffID")

	C(MC_BUFF_HEAL,							MCDT_MACHINE2MACHINE, MCF_S2C, "Gain Buff HealEffect")
											P(MPT_USHORT,	"nUserUIID")
											P(MPT_USHORT,	"nTargetUIID")
											P(MPT_INT,		"nBuffID")
											P(MPT_INT,		"nAmount")
											P(MPT_BOOL,		"bCritical")

	C(MC_BUFF_RESTORE_EN,					MCDT_MACHINE2MACHINE, MCF_S2C, "Gain Buff RestoreEffect - EN")
											P(MPT_USHORT,	"nUserUIID")
											P(MPT_USHORT,	"nTargetUIID")
											P(MPT_INT,		"nBuffID")
											P(MPT_INT,		"nAmount")

	C(MC_BUFF_RESTORE_STA,					MCDT_MACHINE2MACHINE, MCF_S2C, "Gain Buff RestoreEffect - STA")
											P(MPT_USHORT,	"nUserUIID")
											P(MPT_USHORT,	"nTargetUIID")
											P(MPT_INT,		"nBuffID")
											P(MPT_INT,		"nAmount")

	C(MC_BUFF_INSTANT_EFFECT_GAIN,			MCDT_MACHINE2MACHINE, MCF_S2C, "Gain Buff Instant Effect")
											P(MPT_USHORT,	"nTargetUIID")
											P(MPT_INT,		"nBuffID")
											P(MPT_BOOL,		"bCritical")

	C(MC_TALENT_INSTANT_EFFECT_GAIN,		MCDT_MACHINE2MACHINE, MCF_S2C, "Gain Talent Instant Effect")
											P(MPT_USHORT,	"nTargetUIID")
											P(MPT_INT,		"nTalentID")

	C(MC_BUFF_STACK_INCREASE,				MCDT_MACHINE2MACHINE, MCF_S2C, "Buff Stack Increase")
											P(MPT_USHORT,	"nTargetUIID")
											P(MPT_INT,		"nBuffID")
											P(MPT_FLOAT,	"fDurationTime")

	C(MC_BUFF_STACK_DECREASE,				MCDT_MACHINE2MACHINE, MCF_S2C, "Buff Stack Decrease")
											P(MPT_USHORT,	"nTargetUIID")
											P(MPT_INT,		"nBuffID")

	C(MC_TALENT_USE_DISPOSABLE_TALENT,		MCDT_MACHINE2MACHINE, MCF_S2C, "Use Disposable Talent")
											P(MPT_INT,		"nTalentID")

	C(MC_ACTION_TALENT_ADJUST_COOLTIME,		MCDT_MACHINE2MACHINE, MCF_S2C, "Adjust Talent Cooltime")
											P(MPT_INT,		"nTalentID")
											P(MPT_INT,		"nAdjustTime")

	// 트레이드(TRADE) -------------------------------------------------------------------------------------------
	C(MC_TRADE_START_REQ,					MCDT_MACHINE2MACHINE, MCF_C2S, "Request Trade Start")										
											P(MPT_UID,		"uidTarPlayer")

	C(MC_TRADE_START_REQ_RELAY,				MCDT_MACHINE2MACHINE, MCF_S2C, "Request Trade Start Route")
											P(MPT_UID,		"uidReqPlayer")

	C(MC_TRADE_START_ACCEPT_REQ,			MCDT_MACHINE2MACHINE, MCF_C2S, "Request Trade Start Accept")
											P(MPT_BOOL,		"bAccept")
											
	C(MC_TRADE_START,						MCDT_MACHINE2MACHINE, MCF_S2C, "Trade Start")
											P(MPT_UID,		"uidAcceptPlayer")
											P(MPT_INT,		"nAcceptPlayerLevel")
											
	C(MC_TRADE_START_REJECT,				MCDT_MACHINE2MACHINE, MCF_S2C, "Trade Reject")
											
	C(MC_TRADE_PUTUP_ITEM_REQ,				MCDT_MACHINE2MACHINE, MCF_C2S, "Request Put Up Item")
											P(MPT_SINGLE_BLOB,	"ItemInfo")
	C(MC_TRADE_PUTUP_ITEM,					MCDT_MACHINE2MACHINE, MCF_S2C, "Put Up Item")
											P(MPT_UID,			"uidReqPlayer")
											P(MPT_SINGLE_BLOB,	"ItemInfo")
	C(MC_TRADE_PUTDOWN_ITEM_REQ,			MCDT_MACHINE2MACHINE, MCF_C2S, "Request Put Down Item")
											P(MPT_SINGLE_BLOB,	"ItemInfo")
	C(MC_TRADE_PUTDOWN_ITEM,				MCDT_MACHINE2MACHINE, MCF_S2C, "Put Down Item")
											P(MPT_UID,			"uidReqPlayer")
											P(MPT_SINGLE_BLOB,	"ItemInfo")

	C(MC_TRADE_PUTUP_MONEY_REQ,				MCDT_MACHINE2MACHINE, MCF_C2S, "Request Put Up Money")
											P(MPT_INT,		"Money")
	C(MC_TRADE_PUTUP_MONEY,					MCDT_MACHINE2MACHINE, MCF_S2C, "Put Up Money")
											P(MPT_UID,		"uidReqPlayer")
											P(MPT_INT,		"Money")

	C(MC_TRADE_CANCEL_REQ,					MCDT_MACHINE2MACHINE, MCF_C2S, "Request Trade Cancel")
	C(MC_TRADE_CANCEL,						MCDT_MACHINE2MACHINE, MCF_S2C, "Trade Cancel")

	C(MC_TRADE_CONFIRM_REQ,					MCDT_MACHINE2MACHINE, MCF_C2S, "Request Trade Confirm")
											
	C(MC_TRADE_CONFIRM,						MCDT_MACHINE2MACHINE, MCF_S2C, "Trade Confirm")
											P(MPT_UID,		"uidReqPlayer")

	C(MC_TRADE_COMPLETE,					MCDT_MACHINE2MACHINE, MCF_S2C, "Trade Complete")											

	// 파티 ---------------
	C(MC_PARTY_INVITE_REQ,					MCDT_MACHINE2MACHINE, MCF_C2S, "Party.RequestInvite")										
											P(MPT_UID,		"uidTargetPlayer")
	C(MC_PARTY_INVITE_BY_NAME_REQ,			MCDT_MACHINE2MACHINE, MCF_C2S, "Party.RequestInviteByName")
											P(MPT_WSTR,		"TargetPlayerName")
	C(MC_PARTY_INVITE,						MCDT_MACHINE2MACHINE, MCF_S2C, "Party.Invite")										
											P(MPT_INT,		"RespondMsg")
	C(MC_PARTY_LEAVE_REQ,					MCDT_MACHINE2MACHINE, MCF_C2S, "Party.Leave")										
	C(MC_PARTY_KICK_REQ,					MCDT_MACHINE2MACHINE, MCF_C2S, "Party.Kick")										
											P(MPT_UID,		"uidPlayer")
	C(MC_PARTY_INVITE_QUESTION,				MCDT_MACHINE2MACHINE, MCF_S2C, "Party.Invite Question")										
											P(MPT_UID,		"uidInvitor")
											P(MPT_WSTR,		"strInvitorName")
											P(MPT_BOOL,		"bCombat")
	C(MC_PARTY_INVITE_QUESTION_RESPOND,		MCDT_MACHINE2MACHINE, MCF_C2S, "Party.Invite Question Respond")										
											P(MPT_INT,		"RespondMsg")
	C(MC_PARTY_REFRESH_INFO_ALL,			MCDT_MACHINE2MACHINE, MCF_S2C, "Party.PartyInfoAll")										
											P(MPT_BLOB,		"PartyInfo")
											P(MPT_BLOB,		"MemberInfos")
											P(MPT_BLOB,		"MemberBuffList1")
											P(MPT_BLOB,		"MemberBuffList2")
											P(MPT_BLOB,		"MemberBuffList3")
											P(MPT_BLOB,		"MemberBuffList4")
											P(MPT_BLOB,		"MemberBuffList5")

	C(MC_PARTY_REFRESH_INFO,				MCDT_MACHINE2MACHINE, MCF_S2C, "Party.PartyInfo")										
											P(MPT_BLOB,		"MemberInfo")
											P(MPT_BLOB,		"MemberBuffList")
	C(MC_PARTY_NOTIFY_JOIN,					MCDT_MACHINE2MACHINE, MCF_S2C, "Party.Join")										
											P(MPT_UID,		"uidPlayer")
											P(MPT_WSTR,		"strName")
	C(MC_PARTY_NOTIFY_LEAVE,				MCDT_MACHINE2MACHINE, MCF_S2C, "Party.Leave")										
											P(MPT_UID,		"uidPlayer")
	C(MC_PARTY_JOIN_REQ,					MCDT_MACHINE2MACHINE, MCF_C2S, "Party.RequestJoin")										
											P(MPT_UID,		"uidParty")
	C(MC_PARTY_JOIN_RES,					MCDT_MACHINE2MACHINE, MCF_S2C, "Party.ResponseJoin")
											P(MPT_INT,		"RespondMsg")
	C(MC_PARTY_JOIN_QUESTION,				MCDT_MACHINE2MACHINE, MCF_S2C, "Party.AskJoin")
											P(MPT_UID,		"uidPlayer")
											P(MPT_WSTR,		"strPlayerName")
											P(MPT_INT,		"nRequestPlayerLevel")
											P(MPT_INT,		"nRequestPlayerTitleStyle")
											P(MPT_INT,		"nRequestPlayerFieldID")
	C(MC_PARTY_JOIN_QUESTION_RESPOND,		MCDT_MACHINE2MACHINE, MCF_C2S, "Party.AnswerJoin")
											P(MPT_UID,		"uidJoinRequestPlayer")
											P(MPT_INT,		"RespondMsg")

	C(MC_PARTY_ACCEPT_CANCEL,				MCDT_MACHINE2MACHINE, MCF_S2C, "Party.AcceptCancel")
											P(MPT_INT,		"nReason")

	C(MC_PARTY_CHANGE_PUBLIC_PARTY_SETTING_REQ, MCDT_MACHINE2MACHINE, MCF_C2S, "Party.ChangePublicPartySetting Req.")
											P(MPT_BOOL,		"bPublic")
											P(MPT_WSTR,		"strPartyName")

	C(MC_PARTY_CHANGE_PUBLIC_PARTY_SETTING,		MCDT_MACHINE2MACHINE, MCF_S2C, "Party.ChangePublicPartySetting")
											P(MPT_BOOL,		"bPublic")
											P(MPT_WSTR,		"strPartyName")

	C(MC_PARTY_CHANGE_LEADER_REQ,			MCDT_MACHINE2MACHINE, MCF_C2S, "Party.ChangeLeader Req.")
											P(MPT_UID,		"uidMember")

	C(MC_PARTY_CHANGE_LEADER,				MCDT_MACHINE2MACHINE, MCF_S2C, "Party.ChangeLeader")
											P(MPT_UID,		"uidNewLeader")

	C(MC_PARTY_CHANGE_LOOTING_RULE_REQ,		MCDT_MACHINE2MACHINE, MCF_C2S, "Party.ChangeLootingRule Req.")
											P(MPT_CHAR,		"LootingRuleCommon")
											P(MPT_CHAR,		"LootingRuleRare")
											P(MPT_CHAR,		"LootingRuleRareFor")	

	C(MC_PARTY_CHANGE_LOOTING_RULE,			MCDT_MACHINE2MACHINE, MCF_S2C, "Party.ChangeLootingRule")
											P(MPT_CHAR,		"LootingRuleCommon")
											P(MPT_CHAR,		"LootingRuleRare")
											P(MPT_CHAR,		"LootingRuleRareFor")

	C(MC_PARTY_CHANGE_QUESTID_REQ,			MCDT_MACHINE2MACHINE, MCF_C2S, "Party.ChangeQuestID Req.")
											P(MPT_INT,		"QuestID")

	C(MC_PARTY_CHANGE_QUESTID,				MCDT_MACHINE2MACHINE, MCF_S2C, "Party.ChangeQuestID")
											P(MPT_INT,		"QuestID")

	C(MC_PARTY_CHANGE_LEADER_NOTIFY,		MCDT_MACHINE2MACHINE, MCF_S2C, "Party.ChangeLeaderNotify")
											P(MPT_UID,		"uidOldLeader")
											P(MPT_UID,		"uidNewLeader")

	C(MC_PARTY_REFRESH_MEMBER_POSITION,		MCDT_MACHINE2MACHINE, MCF_S2C, "Party.RefreshMemberPosition")
											P(MPT_UID,		"uidMember")
											P(MPT_VEC,		"vPosition")

	C(MC_PARTY_SHOW_INFO_REQ,				MCDT_MACHINE2MACHINE, MCF_C2S, "Party.ShowInfo Req")
											P(MPT_UID,		"uidParty")

	C(MC_PARTY_SHOW_INFO,					MCDT_MACHINE2MACHINE, MCF_S2C, "Party.ShowInfo")
											P(MPT_BLOB,		"PartyInfo")
											P(MPT_BLOB,		"MemberInfos")

	C(MC_PARTY_CREATE_SINGLE_PARTY_REQ,		MCDT_MACHINE2MACHINE, MCF_C2S, "Party.CreateSingleParty Req")
											P(MPT_BOOL,		"bPublic")
											P(MPT_WSTR,		"strPartyName")

	C(MC_PARTY_CREATE_SINGLE_PARTY_RES,		MCDT_MACHINE2MACHINE, MCF_S2C, "Party.CreateSingleParty Res")
											P(MPT_INT,		"RespondMsg")

	C(MC_PARTY_MATCHING_SHOW_PUBLIC_PARTY_LIST_REQ, MCDT_MACHINE2MACHINE, MCF_C2S, "Party.MatchingShowPublicPartyList Req")
											P(MPT_CHAR,		"nPage")
											P(MPT_CHAR,		"nSearchLevelMin")
											P(MPT_CHAR,		"nSearchLevelMax")
											P(MPT_WSTR,		"strSearchString")

	C(MC_PARTY_MATCHING_SHOW_PUBLIC_PARTY_LIST, MCDT_MACHINE2MACHINE | MCDT_NON_ENCRYPTED, MCF_S2C, "Party.MatchingShowPublicPartyList")
											P(MPT_INT,		"result")
											P(MPT_BLOB,		"vector<TD_PARTY_MATCHING_PUBLIC_PARTY_LIST_ITEM>")
											P(MPT_INT,		"nPartyCount")
											
// NPC TRADE ---------
	C(MC_NPCSHOP_SHOW,						MCDT_MACHINE2MACHINE, MCF_S2C, "See Item")
											P(MPT_BLOB,		"vecBuyableUsableItem")
											P(MPT_BLOB,		"vecBuyableUnusableItem")
											P(MPT_BLOB,		"vecUnBuyableItem")
											P(MPT_FLOAT,	"fBuyMod")
											P(MPT_FLOAT,	"fSellMod")
											P(MPT_BOOL,		"bRepairer")
											P(MPT_FLOAT,	"fRepairPriceMod")

	C(MC_NPCSHOP_BUY_REQ,					MCDT_MACHINE2MACHINE, MCF_C2S, "Request Buy")
											P(MPT_INT,		"nItemID")
											P(MPT_USHORT,	"nItemAmount")
	C(MC_NPCSHOP_BUY,						MCDT_MACHINE2MACHINE, MCF_S2C, "Buy")

	C(MC_NPCSHOP_SELL_REQ,					MCDT_MACHINE2MACHINE, MCF_C2S, "Request Sell")
											P(MPT_INT,		"nInvenSlotID")
											P(MPT_USHORT,	"nItemAmount")
	C(MC_NPCSHOP_SELL,						MCDT_MACHINE2MACHINE, MCF_S2C, "Sell")

	C(MC_NPCSHOP_SELL_VERYCOMMON_REQ,		MCDT_MACHINE2MACHINE, MCF_C2S, "Request Sell Very Commmon")
	C(MC_NPCSHOP_SELL_VERYCOMMON,			MCDT_MACHINE2MACHINE, MCF_S2C, "Sell Very Commmon")	

	C(MC_NPCSHOP_REPAIR_ONE_REQ,			MCDT_MACHINE2MACHINE, MCF_C2S, "Request Repair One")
											P(MPT_CHAR,		"slotType")
											P(MPT_INT,		"nSlotID")
	C(MC_NPCSHOP_REPAIR_ALL_REQ,			MCDT_MACHINE2MACHINE, MCF_C2S, "Request Repair All")
	C(MC_NPCSHOP_REPAIR,					MCDT_MACHINE2MACHINE, MCF_S2C, "Repair")
											P(MPT_BLOB,		"vecItemSlotInfo")
	

// LOOT -------------
	C(MC_LOOT_BEGIN_REQ,					MCDT_MACHINE2MACHINE, MCF_C2S, "Request Start")
											P(MPT_UID,		"uidNPC")

	C(MC_LOOT_BEGIN,						MCDT_MACHINE2MACHINE, MCF_S2C, "Response Start")
											P(MPT_UID,		"uidLooter")
											P(MPT_UID,		"uidNPC")
											P(MPT_FLOAT,	"fLootTime")

	C(MC_LOOT_SEE_ITEM_REQ,					MCDT_MACHINE2MACHINE, MCF_C2S, "Request SeeItem")
											P(MPT_UID,		"uidNPC")

	C(MC_LOOT_SEE_ITEM,						MCDT_MACHINE2MACHINE, MCF_S2C, "Response SeeItem")
											P(MPT_BLOB,		"vecTDDropItem")

	C(MC_LOOT_LOOT_ITEM_REQ,				MCDT_MACHINE2MACHINE, MCF_C2S, "Request LootItem")
											P(MPT_UID,		"uidNPC")
											P(MPT_BLOB,		"vecItemUID")

	C(MC_LOOT_ROLLRESULT,					MCDT_MACHINE2MACHINE, MCF_S2C, "RollResult")
											P(MPT_BLOB,		"vecTDRollItem")
											P(MPT_BLOB,		"vecTDRollResult")

	C(MC_LOOT_MASTERLOOT_UI,				MCDT_MACHINE2MACHINE, MCF_S2C, "MasterLoot UI")
											P(MPT_INT,		"nGameServerID")
											P(MPT_UID,		"uidField")
											P(MPT_UID,		"uidNPC")
											P(MPT_BLOB,		"vecTDMasterLootUI")

	C(MC_LOOT_MASTERLOOT_NOTIFY,			MCDT_MACHINE2MACHINE, MCF_S2C, "MasterLoot Notify")
											P(MPT_BLOB,		"vecTDMasterLootUI")

	C(MC_LOOT_MASTERLOOT_ITEM_REQ,			MCDT_MACHINE2MACHINE, MCF_C2S, "Request MasterLoot Item")
											P(MPT_UID,		"uidNPC")
											P(MPT_UID,		"uidDropItem")
											P(MPT_UCHAR,	"nSelectedIndex")
											P(MPT_BOOL,		"bRandom")

	C(MC_LOOT_MASTERLOOT_UI_REMOVE,			MCDT_MACHINE2MACHINE, MCF_S2C, "MasterLoot UI Remove")
											P(MPT_UID,		"uidField")
											P(MPT_UID,		"uidNPC")
	
	C(MC_LOOT_REFRESH_ITEM,					MCDT_MACHINE2MACHINE, MCF_S2C, "Refresh Item")
											P(MPT_BLOB,		"vecTDDropItem")

	C(MC_LOOT_OTHER_GAIN_ITEM,				MCDT_MACHINE2MACHINE, MCF_S2C, "Other Gain Item")
											P(MPT_UID,		"uidPlayer")
											P(MPT_BLOB,		"vecTDOtherGainItem")

	C(MC_LOOT_GETTABLE_ITEM_ADD,			MCDT_MACHINE2MACHINE, MCF_S2C, "GettableItem Add")
											P(MPT_BLOB,		"vecTDGettableItemAdd")

	C(MC_LOOT_END_REQ,						MCDT_MACHINE2MACHINE, MCF_C2S, "Request End Looting")
	C(MC_LOOT_END,							MCDT_MACHINE2MACHINE, MCF_S2C, "Response End Looting")
											P(MPT_UID,		"uidGetter")

	C(MC_LOOT_INSERT_LOOTABLE_NPC,			MCDT_MACHINE2MACHINE, MCF_S2C, "Insert Lootable NPC")
											P(MPT_UID,		"uidNPC")
	C(MC_LOOT_REMOVE_LOOTABLE_NPC,			MCDT_MACHINE2MACHINE, MCF_S2C, "Remove Lootable NPC")
											P(MPT_UID,		"uidNPC")

// INTERACTION -------------

	C(MC_INTERACTION_INTERACTION_REQ,		MCDT_MACHINE2MACHINE, MCF_C2S, "Request Interaction")
											P(MPT_UID,		"uidTarget")
											P(MPT_VEC,		"pos")
											P(MPT_VEC,		"dir")

// NPC INTERACTION -------------
	C(MC_NPCINTERACTION_SELECTABLE_IELEMENT, MCDT_MACHINE2MACHINE, MCF_S2C, "Selectable IElement")
											P(MPT_UID,		"uidNPC")
											P(MPT_WSTR,		"strOpeningText")
											P(MPT_BLOB,		"InteractionElement")

	C(MC_NPCINTERACTION_SELECT_IELEMENT_REQ, MCDT_MACHINE2MACHINE, MCF_C2S, "Select IElement")
											P(MPT_UID,		"uidTarget")
											P(MPT_INT,		"nIElementID")

	C(MC_NPCINTERACTION_REFRESH_SELECTABLE_IELEMENT_REQ, MCDT_MACHINE2MACHINE, MCF_C2S, "Select IElement")

	C(MC_NPCINTERACTION_REFRESH_SELECTABLE_IELEMENT, MCDT_MACHINE2MACHINE, MCF_S2C, "Refresh Selectable IElement")
											P(MPT_UID,		"uidNPC")
											P(MPT_WSTR,		"strOpeningText")
											P(MPT_BLOB,		"InteractionElement")

	C(MC_NPCINTERACTION_END_REQ,			MCDT_MACHINE2MACHINE, MCF_C2S, "NPCInteraction End Req")
	C(MC_NPCINTERACTION_END,				MCDT_MACHINE2MACHINE, MCF_S2C, "NPCInteraction End")

	C(MC_NPCINTERACTION_ICON,				MCDT_MACHINE2MACHINE, MCF_S2C, "NPCInteraction ICon")
											P(MPT_BLOB,		"NPCIcon")
											P(MPT_BLOB,		"FarNPCIcon")

	C(MC_NPCINTERACTION_LOOT_START,			MCDT_MACHINE2MACHINE, MCF_S2C, "NPCInteraction Loot Start")
											P(MPT_UID,		"uidLooter")
											P(MPT_UID,		"uidNPC")	

// 미니맵 ----------------
	C(MC_MINIMAP_MARK_POS_REQ,				MCDT_MACHINE2MACHINE, MCF_C2S, "MiniMap Mark Position")
											P(MPT_VEC,		"Position")

	C(MC_MINIMAP_MARK_POS,					MCDT_MACHINE2MACHINE, MCF_S2C, "Response MiniMap Mark Position")
											P(MPT_VEC,		"Position")
	
// 피로도 ----------------
	C(MC_FATIGUE_UPDATE,					MCDT_MACHINE2MACHINE, MCF_S2C, "Update Fatigue")
											P(MPT_INT,		"nFatigueType")

// 크래프트 ----------------
	C(MC_CRAFT_SHOW,						MCDT_MACHINE2MACHINE, MCF_S2C, "Show Craft UI")
											P(MPT_BLOB,		"vecRecipeID")
											P(MPT_FLOAT,	"fMakeMod")

	C(MC_CRAFT_MAKE_REQ,					MCDT_MACHINE2MACHINE, MCF_C2S, "Make Req")
											P(MPT_INT,		"nRecipeID")
											P(MPT_INT,		"nCraftAmount")
	C(MC_CRAFT_MAKE,						MCDT_MACHINE2MACHINE, MCF_S2C, "Make")
											P(MPT_INT,		"nRecipeID")
	C(MC_CRAFT_RECIPE_INSERT_REQ,			MCDT_MACHINE2MACHINE, MCF_C2S, "Recipe Insert Req")
											P(MPT_INT,		"nRecipeID")
	C(MC_CRAFT_RECIPE_INSERT,				MCDT_MACHINE2MACHINE, MCF_S2C, "Recipe Insert")
											P(MPT_INT,		"nRecipeID")

	C(MC_CRAFT_RECIPE_DELETE_REQ,			MCDT_MACHINE2MACHINE, MCF_C2S, "Recipe Delete Req")
											P(MPT_INT,		"nRecipeID")
	C(MC_CRAFT_RECIPE_DELETE,				MCDT_MACHINE2MACHINE, MCF_S2C, "Recipe Delete")
											P(MPT_INT,		"nRecipeID")

// 여관 ----------------
	C(MC_INN_BEGIN_SLEEP,					MCDT_MACHINE2MACHINE, MCF_S2C, "Begin Sleep")
	C(MC_INN_END_SLEEP,						MCDT_MACHINE2MACHINE, MCF_S2C, "End Sleep")

// 길드 ----------------
	C(MC_GUILD_SHOW_CREATE_UI,				MCDT_MACHINE2MACHINE, MCF_S2C, "Show Create UI")
	C(MC_GUILD_SHOW_DESTROY_UI,				MCDT_MACHINE2MACHINE, MCF_S2C, "Show Destroy UI")

	C(MC_GUILD_CREATE_REQ,					MCDT_MACHINE2MACHINE, MCF_C2S, "Create Req")
											P(MPT_WSTR,		"GuildName")
	C(MC_GUILD_CREATE,						MCDT_MACHINE2MACHINE, MCF_S2C, "Create")
											P(MPT_USHORT,	"nPlayerUIID")
											P(MPT_WSTR,		"GuildName")

	C(MC_GUILD_INVITE_REQ,					MCDT_MACHINE2MACHINE, MCF_C2S, "Invite Req")
											P(MPT_UID,		"uidPlayer")
	C(MC_GUILD_INVITE_NOTIFY,				MCDT_MACHINE2MACHINE, MCF_S2C, "Invite Notify")
											P(MPT_UID,		"uidPlayer")
											P(MPT_WSTR,		"GuildName")
	C(MC_GUILD_INVITE_RES,					MCDT_MACHINE2MACHINE, MCF_C2S, "Invite Res")
											P(MPT_BOOL,		"bAccept")

	C(MC_GUILD_INVITE_REJECT,				MCDT_MACHINE2MACHINE, MCF_S2C, "Invite Reject")
											P(MPT_UID,		"uidPlayer")

	C(MC_GUILD_JOIN_ME,						MCDT_MACHINE2MACHINE, MCF_S2C, "Join Me")
											P(MPT_BLOB,		"Guild")
											P(MPT_BLOB,		"GuildMember")
	C(MC_GUILD_JOIN_OTHER,					MCDT_MACHINE2MACHINE, MCF_S2C, "Join Other")
											P(MPT_WSTR,		"Name")
											P(MPT_INT,		"nLevel")
											P(MPT_INT,		"nFieldID")
											P(MPT_INT,		"nChannelID")
	C(MC_GUILD_JOIN_SECTOR,					MCDT_MACHINE2MACHINE, MCF_S2C, "Join Sector")
											P(MPT_USHORT,	"nPlayerUIID")
											P(MPT_WSTR,		"strGuildName")

	C(MC_GUILD_LEAVE_REQ,					MCDT_MACHINE2MACHINE, MCF_C2S, "Leave Req")
	C(MC_GUILD_LEAVE,						MCDT_MACHINE2MACHINE, MCF_S2C, "Leave")
											P(MPT_WSTR,		"strName")
	C(MC_GUILD_LEAVE_SECTOR,				MCDT_MACHINE2MACHINE, MCF_S2C, "Leave Sector")
											P(MPT_USHORT,	"nPlayerUIID")

	C(MC_GUILD_DESTROY_REQ,					MCDT_MACHINE2MACHINE, MCF_C2S, "Destroy Req")
	C(MC_GUILD_DESTROY,						MCDT_MACHINE2MACHINE, MCF_S2C, "Destroy")

	C(MC_GUILD_ONLINE,						MCDT_MACHINE2MACHINE, MCF_S2C, "Online")
											P(MPT_WSTR,		"strName")
											P(MPT_INT,		"nFieldID")
											P(MPT_INT,		"nChannelID")
	C(MC_GUILD_OFFLINE,						MCDT_MACHINE2MACHINE, MCF_S2C, "Offline")
											P(MPT_WSTR,		"strName")
	C(MC_GUILD_MOVEFIELD,					MCDT_MACHINE2MACHINE, MCF_S2C, "MoveField")
											P(MPT_WSTR,		"strName")
											P(MPT_INT,		"nFieldID")
											P(MPT_INT,		"nChannelID")											

	C(MC_GUILD_CHANGE_MASTER_REQ,			MCDT_MACHINE2MACHINE, MCF_C2S, "Change Master Req")
											P(MPT_WSTR,		"strNewMasterName")
	C(MC_GUILD_CHANGE_MASTER,				MCDT_MACHINE2MACHINE, MCF_S2C, "Change Master")
											P(MPT_WSTR,		"strOldMasterName")
											P(MPT_WSTR,		"strNewMasterName")

	C(MC_GUILD_INFO,						MCDT_MACHINE2MACHINE, MCF_S2C, "Guild info")
											P(MPT_BLOB,	"Guild")
											P(MPT_BLOB,	"vecGuildMember")
	
	C(MC_GUILD_KICK_REQ,					MCDT_MACHINE2MACHINE, MCF_C2S, "Kick Req")
											P(MPT_WSTR,		"strName")

	C(MC_GUILD_KICK,						MCDT_MACHINE2MACHINE, MCF_S2C, "Kick")
											P(MPT_WSTR,		"strName")

	C(MC_GUILD_SHOW_STORAGE_UI,				MCDT_MACHINE2MACHINE, MCF_S2C, "Show Storage UI")
											P(MPT_INT,	"nStorageMoney")
											P(MPT_BLOB,	"vecItem")											

	C(MC_GUILD_DEPOSIT_STORAGEMONEY_REQ,	MCDT_MACHINE2MACHINE, MCF_C2S, "Deposit StorageMoney Req")
											P(MPT_INT,		"nDepositMoney")

	C(MC_GUILD_DEPOSIT_STORAGEMONEY,		MCDT_MACHINE2MACHINE, MCF_S2C, "Deposit StorageMoney")
											P(MPT_WSTR,		"strName")
											P(MPT_INT,		"nDepositMoney")
											P(MPT_INT,		"nStorageMoney")

	C(MC_GUILD_WITHDRAW_STORAGEMONEY_REQ,	MCDT_MACHINE2MACHINE, MCF_C2S, "Deposit StorageMoney Req")
											P(MPT_INT,		"nWithdrawMoney")

	C(MC_GUILD_WITHDRAW_STORAGEMONEY,		MCDT_MACHINE2MACHINE, MCF_S2C, "Deposit StorageMoney")
											P(MPT_WSTR,		"strName")
											P(MPT_INT,		"nWithdrawMoney")
											P(MPT_INT,		"nStorageMoney")

	C(MC_GUILD_MOVE_STORAGEITEM,			MCDT_MACHINE2MACHINE, MCF_S2C, "Move StorageItem Sync")
											P(MPT_WSTR,		"strName")											
											P(MPT_BLOB,		"vecItem")
	
	

// 결투 ---------------
	C(MC_DUEL_REQUEST,						MCDT_MACHINE2MACHINE, MCF_C2S, "Duel.Request")										
											P(MPT_UID,		"uidTargetPlayer")
	C(MC_DUEL_QUESTION,						MCDT_MACHINE2MACHINE, MCF_S2C, "Duel.Question")										
											P(MPT_UID,		"uidInvitor")
											P(MPT_WSTR,		"strInvitorName")
											P(MPT_BOOL,		"bCombat")
	C(MC_DUEL_QUESTION_RESPOND,				MCDT_MACHINE2MACHINE, MCF_C2S, "Duel.QuestionRespond")										
											P(MPT_BOOL,		"Answer")
	C(MC_DUEL_CANCEL,						MCDT_MACHINE2MACHINE, MCF_S2C, "Duel.Cancel")										
											P(MPT_UID,		"uidFighter1")	
											P(MPT_UID,		"uidFighter2")	

	C(MC_DUEL_READY,						MCDT_MACHINE2MACHINE, MCF_S2C, "Duel.Ready")										
											P(MPT_UID,		"uidFighter1")	
											P(MPT_UID,		"uidFighter2")	
	C(MC_DUEL_FIGHT,						MCDT_MACHINE2MACHINE, MCF_S2C, "Duel.Fight")
											P(MPT_USHORT,	"SessionId")
											P(MPT_UID,		"uidFighter1")	
											P(MPT_UID,		"uidFighter2")	
	C(MC_DUEL_FINISHED,						MCDT_MACHINE2MACHINE, MCF_S2C, "Duel.Finished")										
											P(MPT_UID,		"uidWinner")	
											P(MPT_UID,		"uidLoser")	
											P(MPT_BOOL,		"bShowMessage")
	C(MC_DUEL_PARTY_READY,					MCDT_MACHINE2MACHINE, MCF_S2C, "Duel.Ready")										
											P(MPT_BLOB,		"uidFighter1")	
											P(MPT_BLOB,		"uidFighter2")	
	C(MC_DUEL_PARTY_FIGHT,					MCDT_MACHINE2MACHINE, MCF_S2C, "Duel.Fight")										
											P(MPT_USHORT,	"SessionId")
											P(MPT_BLOB,		"uidFighters")	
	C(MC_DUEL_PARTY_FINISHED,				MCDT_MACHINE2MACHINE, MCF_S2C, "Duel.Finished")										
											P(MPT_INT,		"nWinnerTeam")	
											P(MPT_UID,		"uidWinTeamLeader")
											P(MPT_UID,		"uidLoseTeamLeader")
											P(MPT_INT,		"nWinPlayerCount")
											P(MPT_INT,		"nLosePlayerCount")											
											P(MPT_BOOL,		"bShowMessage")
	C(MC_DUEL_PARTY_CANCEL,					MCDT_MACHINE2MACHINE, MCF_S2C, "Duel.Cancel")										
											P(MPT_BLOB,		"uidFighters")	
	C(MC_DUEL_DEFEATED,						MCDT_MACHINE2MACHINE, MCF_S2C, "Duel.Defeated")										
											P(MPT_UID,		"uidDefeater")	
	C(MC_DUEL_PARTY_REQUEST,				MCDT_MACHINE2MACHINE, MCF_C2S, "Duel.Request")										
											P(MPT_UID,		"uidTargetPlayer")
	C(MC_DUEL_READY_COUNT,					MCDT_MACHINE2MACHINE, MCF_S2C, "Duel.ReadyCount")
											P(MPT_INT,		"nReadySecond")
											

// 투기장 ------------------------------	------------------------------------------------------------------
	C(MC_BATTLEARENA_REGISTER_REQ,			MCDT_MACHINE2MACHINE, MCF_C2S, "BattleArena.RegisterRequest")										
	C(MC_BATTLEARENA_REGISTER,				MCDT_MACHINE2MACHINE, MCF_S2C, "BattleArena.Register")	
											P(MPT_INT,		"nGameType")
	C(MC_BATTLEARENA_DEREGISTER_REQ,		MCDT_MACHINE2MACHINE, MCF_C2S, "BattleArena.DeregisterRequest")										
	C(MC_BATTLEARENA_DEREGISTER,			MCDT_MACHINE2MACHINE, MCF_S2C, "BattleArena.Deregister")	
	C(MC_BATTLEARENA_MATCHED,				MCDT_MACHINE2MACHINE, MCF_S2C, "BattleArena.Matched")	
	C(MC_BATTLEARENA_COUNTING,				MCDT_MACHINE2MACHINE, MCF_S2C, "BattleArena.Counting")	
											P(MPT_FLOAT,	"fWaitSecond")
	C(MC_BATTLEARENA_GAMESTART,				MCDT_MACHINE2MACHINE, MCF_S2C, "BattleArena.GameStart")	
											P(MPT_BLOB,		"EntryInfo")
	C(MC_BATTLEARENA_READY_DEATHMATCH,		MCDT_MACHINE2MACHINE, MCF_S2C, "BattleArena.ReadyDeathmatch")	
											P(MPT_INT,		"nMaxScore")
	C(MC_BATTLEARENA_REJOIN_DEATHMATCH, 	MCDT_MACHINE2MACHINE, MCF_S2C, "BattleArena.GameStart")	
											P(MPT_FLOAT,	"fGameElaspedTime")
											P(MPT_INT,		"nMaxScore")
											P(MPT_UID,		"Team")
											P(MPT_UID,		"Team")
											P(MPT_INT,		"nScore")
											P(MPT_INT,		"nScore")
										
	C(MC_BATTLEARENA_SCOREBOARD_DEATHMATCH,	MCDT_MACHINE2MACHINE, MCF_S2C, "BattleArena.ScoreboardForDeathmatch")	
											P(MPT_FLOAT,	"fWaitSecond")
											P(MPT_UID,		"uidWinnerEntry")
											P(MPT_INT,		"nWinnerScore")
											P(MPT_INT,		"nLoserScore")
											P(MPT_FLOAT,	"fGameElaspedTime")
	C(MC_BATTLEARENA_DEATHMATCH_UPDATE,		MCDT_MACHINE2MACHINE, MCF_S2C, "BattleArena.Update")	
											P(MPT_BLOB,		"ScoreInfo")
	C(MC_BATTLEARENA_FINISHED,				MCDT_MACHINE2MACHINE, MCF_S2C, "BattleArena.Finished")	
	C(MC_BATTLEARENA_DIE,					MCDT_MACHINE2MACHINE, MCF_S2C, "BattleArena.Die")	
											P(MPT_UID,		"uidPlayer")
											P(MPT_FLOAT,	"fRespawnTime")
	C(MC_BATTLEARENA_REBIRTH_REQ,			MCDT_MACHINE2MACHINE, MCF_C2S, "BattleArena.RebirthRequest")	

	C(MC_BATTLEARENA_KILL_INFO,				MCDT_MACHINE2MACHINE, MCF_S2C, "BattleArena.KillInfo")
											P(MPT_UID,		"uidKiller")
											P(MPT_UID,		"uidVictim")
											P(MPT_INT,		"nKillingTalentID")
											

// 탤런트포커스 ---------------										
	C(MC_FOCUS_GAIN,						MCDT_MACHINE2MACHINE, MCF_S2C, "Focus.Gain")
											P(MPT_UID,		"uidActor")
											P(MPT_INT,		"nFocusType")
											P(MPT_FLOAT,	"fDurationTime")
	C(MC_FOCUS_LOST,						MCDT_MACHINE2MACHINE, MCF_S2C, "Focus.Lost")										
											P(MPT_UID,		"uidActor")
											P(MPT_INT,		"nFocusType")
	C(MC_FOCUS_UPDATE_BUBBLE,				MCDT_MACHINE2MACHINE, MCF_S2C, "Focus.UpdateBubble")
											P(MPT_INT,		"nFocusType")
											P(MPT_UCHAR,	"nBubblePercentage")



// 팔레트 ---------------										
	C(MC_PALETTE_SELECT_REQ,				MCDT_MACHINE2MACHINE, MCF_C2S, "Select Req")
											P(MPT_INT,				"eSkillSetTpye")
											P(MPT_UCHAR,			"nNum")
	C(MC_PALETTE_SELECT,					MCDT_MACHINE2MACHINE, MCF_S2C, "Select")
											P(MPT_INT,				"eSkillSetTpye")
											P(MPT_UCHAR,			"nNum")
	C(MC_PALETTE_SET_PRIMARY_REQ,			MCDT_MACHINE2MACHINE, MCF_C2S, "Set Primary Req")
											P(MPT_INT,				"eSkillSetTpye")
											P(MPT_UCHAR,			"nNum")
	C(MC_PALETTE_SET_PRIMARY,				MCDT_MACHINE2MACHINE, MCF_S2C, "Set Primary")
											P(MPT_INT,				"eSkillSetTpye")
											P(MPT_UCHAR,			"nNum")
	C(MC_PALETTE_SET_SECONDARY_REQ,			MCDT_MACHINE2MACHINE, MCF_C2S, "Set Secondary Req")
											P(MPT_INT,				"eSkillSetTpye")
											P(MPT_UCHAR,			"nNum")
	C(MC_PALETTE_SET_SECONDARY,				MCDT_MACHINE2MACHINE, MCF_S2C, "Set Secondary")
											P(MPT_INT,				"eSkillSetTpye")
											P(MPT_UCHAR,			"nNum")

	
	C(MC_PALETTE_PUTUP_REQ,					MCDT_MACHINE2MACHINE, MCF_C2S, "Putup Req")
											P(MPT_INT,				"eSkillSetTpye")
											P(MPT_UCHAR,			"nNum")
											P(MPT_UCHAR,			"nSlot")
											P(MPT_UCHAR,			"nType")
											P(MPT_INT,				"nItemIDorTalentID")
	C(MC_PALETTE_PUTUP,						MCDT_MACHINE2MACHINE, MCF_S2C, "Putup ")
											P(MPT_INT,				"eSkillSetTpye")
											P(MPT_UCHAR,			"nNum")
											P(MPT_UCHAR,			"nSlot")
											P(MPT_UCHAR,			"nType")
											P(MPT_INT,				"nItemIDorTalentID")
	C(MC_PALETTE_PUTDOWN_REQ,				MCDT_MACHINE2MACHINE, MCF_C2S, "Putdown Req")
											P(MPT_INT,				"eSkillSetTpye")
											P(MPT_UCHAR,			"nNum")
											P(MPT_UCHAR,			"nSlot")										
	C(MC_PALETTE_PUTDOWN,					MCDT_MACHINE2MACHINE, MCF_S2C, "Putdown")
											P(MPT_INT,			"eSkillSetTpye")
											P(MPT_BLOB,			"nNum")
											P(MPT_BLOB,			"nSlot")
	C(MC_PALETTE_CHANGE_REQ,				MCDT_MACHINE2MACHINE, MCF_C2S, "Change Req")
											P(MPT_INT,				"eSkillSetTpye")
											P(MPT_UCHAR,			"nNum1")
											P(MPT_UCHAR,			"nSlot1")
											P(MPT_UCHAR,			"nNum2")
											P(MPT_UCHAR,			"nSlot2")
	C(MC_PALETTE_CHANGE,					MCDT_MACHINE2MACHINE, MCF_S2C, "Change")
											P(MPT_INT,				"eSkillSetTpye")
											P(MPT_UCHAR,			"nNum1")
											P(MPT_UCHAR,			"nSlot1")
											P(MPT_UCHAR,			"nNum2")
											P(MPT_UCHAR,			"nSlot2")

	C(MC_PALETTE_PUTDOWN_ALLTALENT,			MCDT_MACHINE2MACHINE, MCF_S2C, "PutDown AllTalent")
											P(MPT_INT,				"eSkillSetTpye")

// 소울 ---------------										
	C(MC_SOUL_DRAIN_ME,						MCDT_MACHINE2MACHINE, MCF_S2C, "Drain Me")
											P(MPT_UID,			"uidNPC")
											P(MPT_CHAR,			"nCharmItemEquipSlotID")
											P(MPT_UCHAR,		"nSoulQuantity")

	C(MC_SOUL_DRAIN_OTHER,					MCDT_MACHINE2MACHINE, MCF_S2C, "Drain Other")
											P(MPT_UID,			"uidNPC")
											P(MPT_UID,			"uidAbsorber")	// 흡수자
											P(MPT_UCHAR,		"nSoulType")
											P(MPT_UCHAR,		"nSoulQuantity")

// 팩션 ---------------
	C(MC_FACTION_INCREASE,					MCDT_MACHINE2MACHINE, MCF_S2C, "Faction Relation UP")
											P(MPT_UCHAR,		"nFactionID")
											P(MPT_USHORT,		"nFactionQuantity")

	C(MC_FACTION_DECREASE,					MCDT_MACHINE2MACHINE, MCF_S2C, "Faction Relation Down")
											P(MPT_UCHAR,		"nFactionID")
											P(MPT_USHORT,		"nFactionQuantity")
	

// 컷씬 ---------------
	C(MC_CUTSCENE_BEGIN_REQ,				MCDT_MACHINE2MACHINE, MCF_C2S, "Cutscene Begin Req")
											P(MPT_INT,		"nCutsceneID")
	C(MC_CUTSCENE_BEGIN_ME,					MCDT_MACHINE2MACHINE, MCF_S2C, "Cutscene Begin Me")
											P(MPT_INT,		"nCutsceneID")
	C(MC_CUTSCENE_BEGIN_OTHER,				MCDT_MACHINE2MACHINE, MCF_S2C, "Cutscene Begin Other")
											P(MPT_UID,		"uidPlayer")

	C(MC_CUTSCENE_END_REQ,					MCDT_MACHINE2MACHINE, MCF_C2S, "Cutscene End Req")
											P(MPT_INT,		"nCutsceneID")
	C(MC_CUTSCENE_END_ME,					MCDT_MACHINE2MACHINE, MCF_S2C, "Cutscene End Me")
											P(MPT_INT,		"nCutsceneID")

	C(MC_CUTSCENE_END_OTHER,				MCDT_MACHINE2MACHINE, MCF_S2C, "Cutscene End Other")
											P(MPT_UID,		"uidPlayer")
// 환경 --------------------
	C(MC_ENV_PLAY_BGM,						MCDT_MACHINE2MACHINE, MCF_S2C, "Enviroment Play BGM")
											P(MPT_WSTR,		"strBGM Name")

// 우편 ----------------
	C(MC_MAIL_MAILBOX_IS_OPENED,			MCDT_MACHINE2MACHINE, MCF_S2C, "Mailbox is opened - interaction response")
											P(MPT_BLOB, "TD_MAIL_MAILBOX_MAIL_INFO Checksum")

	C(MC_MAIL_MAILBOX_IS_CLOSED,			MCDT_MACHINE2MACHINE, MCF_S2C, "Mailbox is closed")

	C(MC_MAIL_POST_REQ,						MCDT_MACHINE2MACHINE, MCF_C2S, "Post Mail Req")
											P(MPT_SINGLE_BLOB,	"TD_MAIL_POST_REQ_INFO")
											P(MPT_WSTR,			"strMailText")

	C(MC_MAIL_POST_RES,						MCDT_MACHINE2MACHINE, MCF_S2C, "Post Mail Res")
											P(MPT_INT,			"nResult")

	C(MC_MAIL_READ_REQ,						MCDT_MACHINE2MACHINE, MCF_C2S, "Read Mail Req")
											P(MPT_UID,	"uidMail")
	
	C(MC_MAIL_READ_RES,						MCDT_MACHINE2MACHINE, MCF_S2C, "Read Mail Res")
											P(MPT_SINGLE_BLOB,	"TD_MAIL_CONTENT_INFO")
											P(MPT_WSTR,			"MailText")
											P(MPT_BLOB,			"Appended Item List(TD_ITEM)")

	C(MC_MAIL_DELETE_REQ,					MCDT_MACHINE2MACHINE, MCF_C2S, "Delete Mail Req")
											P(MPT_UID,	"uidMail")

	C(MC_MAIL_DELETE_RES,					MCDT_MACHINE2MACHINE, MCF_S2C, "Delete Mail Res")
											P(MPT_UID,	"uidMail")

	C(MC_MAIL_TAKE_APPENDED_ITEM_REQ,		MCDT_MACHINE2MACHINE, MCF_C2S, "Take Appended Item Req")
											P(MPT_UID,		"uidMail")
											P(MPT_UCHAR,	"MailSlotID")

	C(MC_MAIL_TAKE_APPENDED_ITEM_RES,		MCDT_MACHINE2MACHINE, MCF_S2C, "Take Appended Item Res")
											P(MPT_UID,		"uidMail")
											P(MPT_UCHAR,	"MailSlotID")
											P(MPT_INT,		"NewThumbnailItemID")

	C(MC_MAIL_TAKE_APPENDED_MONEY_REQ,		MCDT_MACHINE2MACHINE, MCF_C2S, "Take Appended Money Req")
											P(MPT_UID,	"uidMail")

	C(MC_MAIL_TAKE_APPENDED_MONEY_RES,		MCDT_MACHINE2MACHINE, MCF_S2C, "Take Appended Money Res")
											P(MPT_UID,	"uidMail")

	C(MC_MAIL_NOTIFY_MAIL,					MCDT_MACHINE2MACHINE, MCF_S2C, "Notify Mail")
											P(MPT_BOOL,	"IsNew")
											P(MPT_BLOB, "TD_MAIL_MAILBOX_MAIL_INFO")

	C(MC_MAIL_APPEND_PUTUP_ITEM_REQ,		MCDT_MACHINE2MACHINE, MCF_C2S, "Req Put Up Item to Mail")
											P(MPT_SINGLE_BLOB,	"TD_MAIL_RESERVE_ITEM")

	C(MC_MAIL_APPEND_PUTUP_ITEM_RES,		MCDT_MACHINE2MACHINE, MCF_S2C, "Res Put Up Item to Mail")
											P(MPT_SINGLE_BLOB,	"TD_MAIL_RESERVE_ITEM")

	C(MC_MAIL_APPEND_PUTDOWN_ITEM_REQ,		MCDT_MACHINE2MACHINE, MCF_C2S, "Req Put Down Item from Mail")
											P(MPT_SINGLE_BLOB,	"TD_MAIL_RESERVE_ITEM")

	C(MC_MAIL_APPEND_PUTDOWN_ITEM_RES,		MCDT_MACHINE2MACHINE, MCF_S2C, "Res Put Down Item from Mail")											
											P(MPT_SINGLE_BLOB,	"TD_MAIL_RESERVE_ITEM")

	C(MC_MAIL_APPEND_RESET_ITEM_REQ,		MCDT_MACHINE2MACHINE, MCF_C2S, "Reset Appended Mail Item")

// 강화 ----------------
	C(MC_ENCHANT_CHECK_REQ,					MCDT_MACHINE2MACHINE, MCF_C2S, "Enchant Request")
											P(MPT_INT,		"nTargetSlotType")
											P(MPT_INT,		"nTargetSlotID")
											P(MPT_INT,		"nEnchantStoneSlotID")
											P(MPT_INT,		"nEnchantAgentSlotID")

	C(MC_ENCHANT_CHECK_RESULT,				MCDT_MACHINE2MACHINE, MCF_S2C, "Enchant Request")
											P(MPT_INT,		"nErrorCode")

	C(MC_ENCHANT_REQ,						MCDT_MACHINE2MACHINE, MCF_C2S, "Enchant Request")
											P(MPT_INT,		"nTargetSlotType")
											P(MPT_INT,		"nTargetSlotID")
											P(MPT_INT,		"nEnchantStoneSlotID")
											P(MPT_INT,		"nEnchantAgentSlotID")
	C(MC_ENCHANT_SUCCESS,					MCDT_MACHINE2MACHINE, MCF_S2C, "Enchant Success")
											P(MPT_BOOL,		"EquipSlot")
											P(MPT_INT,		"nItemSlot")
											P(MPT_CHAR,		"nEnchantSlot")
											P(MPT_INT,		"nEnchantItemID")
	C(MC_ENCHANT_BROKEN,					MCDT_MACHINE2MACHINE, MCF_S2C, "Enchant Broken")
											P(MPT_BOOL,		"EquipSlot")
											P(MPT_INT,		"nItemSlot")
											P(MPT_CHAR,		"nEnchantSlot")
	C(MC_ENCHANT_PREPARE,					MCDT_MACHINE2MACHINE, MCF_C2S, "Enchant Prepare")
	C(MC_ENCHANT_CANCEL,					MCDT_MACHINE2MACHINE, MCF_C2S, "Enchant Cancel")

// 자동파티 ---------------
	C(MC_AUTOPARTY_ENQUEUE_REQ,				MCDT_MACHINE2MACHINE, MCF_C2S, "AutoParty Enqueue Request")
											P(MPT_INT,		"nQuestID")
	C(MC_AUTOPARTY_INVITE,					MCDT_MACHINE2MACHINE, MCF_S2C, "AutoParty Invite")
											P(MPT_INT,		"nQuestID")
	C(MC_AUTOPARTY_INVITE_RES,				MCDT_MACHINE2MACHINE, MCF_C2S, "AutoParty Invite Respond")
											P(MPT_BOOL,		"bYes")
	C(MC_AUTOPARTY_INVITE_FAIL,				MCDT_MACHINE2MACHINE, MCF_S2C, "AutoParty Invite Failed")
											P(MPT_INT,		"nQuestID")
	C(MC_AUTOPARTY_DEQUEUE_REQ,				MCDT_MACHINE2MACHINE, MCF_C2S, "AutoParty Dequeue Request")
											P(MPT_INT,		"nQuestID")
	C(MC_AUTOPARTY_CHECK_SENSOR_REQ,		MCDT_MACHINE2MACHINE, MCF_C2S, "AutoParty Check Sensor Request")
											P(MPT_INT,		"nQuestID")
	C(MC_AUTOPARTY_CHECK_SENSOR,			MCDT_MACHINE2MACHINE, MCF_C2S, "AutoParty Check Sensor")
											P(MPT_INT,		"nQuestID")
											P(MPT_INT,		"nAutoPartyState")
	C(MC_AUTOPARTY_CHANGE_STATE_REQ,		MCDT_MACHINE2MACHINE, MCF_C2S, "AutoParty Change State Request")
											P(MPT_INT,		"nQuestID")
											P(MPT_INT,		"nAutoPartyState")
	C(MC_AUTOPARTY_CHANGE_STATE,			MCDT_MACHINE2MACHINE, MCF_S2C, "AutoParty Change State")
											P(MPT_INT,		"nQuestID")
											P(MPT_INT,		"nAutoPartyState")

// 퀘스트PVP ---------------
	C(MC_QUESTPVP_TEAMCOUNT,				MCDT_MACHINE2MACHINE, MCF_S2C, "TeamCount")
											P(MPT_INT,		"nTeamOneCount")											
											P(MPT_INT,		"nTeamTwoCount")

	C(MC_QUESTPVP_ENTER_REQ,				MCDT_MACHINE2MACHINE, MCF_C2S, "Enter Request")
	C(MC_QUESTPVP_ENTER,					MCDT_MACHINE2MACHINE, MCF_S2C, "Enter")
											P(MPT_USHORT,	"nPlayerUIID")
											P(MPT_INT,		"nTeam")											

	C(MC_QUESTPVP_BEGIN_EVENT,				MCDT_MACHINE2MACHINE, MCF_S2C, "Begin Event")
											P(MPT_INT,		"nQPEventID")

	C(MC_QUESTPVP_END_EVENT,				MCDT_MACHINE2MACHINE, MCF_S2C, "Begin Event")
											P(MPT_INT,		"nQPEventID")
											P(MPT_INT,		"nWinnerTeam")

	C(MC_QUESTPVP_MARK_NPCPOS,				MCDT_MACHINE2MACHINE, MCF_S2C, "Mark NPCPos")
											P(MPT_INT,		"nNPCID")
											P(MPT_VEC,		"vecPos")
	C(MC_QUESTPVP_UNMARK_NPCPOS,			MCDT_MACHINE2MACHINE, MCF_S2C, "Unmark NPCPos")
											P(MPT_INT,		"nNPCID")

// 보관함 -----------------
	C(MC_STORAGE_SHOW_UI,					MCDT_MACHINE2MACHINE, MCF_S2C, "Show UI")
											P(MPT_INT,		"nStorageMoney")
											P(MPT_BLOB,		"vecItem")
											P(MPT_BOOL,		"bSerialize")
	C(MC_STORAGE_DEPOSIT_MONEY_REQ,			MCDT_MACHINE2MACHINE, MCF_C2S, "Deposit Money Req")
											P(MPT_INT,		"nMoney")
	C(MC_STORAGE_DEPOSIT_MONEY,				MCDT_MACHINE2MACHINE, MCF_S2C, "Deposit Money")
											P(MPT_INT,		"nMoney")
	C(MC_STORAGE_WITHDRAW_MONEY_REQ,		MCDT_MACHINE2MACHINE, MCF_C2S, "Withdraw Money Req")
											P(MPT_INT,		"nMoney")
	C(MC_STORAGE_WITHDRAW_MONEY,			MCDT_MACHINE2MACHINE, MCF_S2C, "Withdraw Money")
											P(MPT_INT,		"nMoney")

// 보관함 ---------------
	C(MC_SC_TRADEMARKET_EXCEPTION,			MCDT_MACHINE2MACHINE, MCF_S2C, "Trademarket exception")
											P(MPT_UCHAR,	"error code")
	C(MC_CS_TRADEMARKET_PUT_ON,				MCDT_MACHINE2MACHINE, MCF_C2S, "Trademarket put on")
	C(MC_SC_TRADEMARKET_PUTTED_ON,			MCDT_MACHINE2MACHINE, MCF_S2C, "Trademarket items have put on")
											P(MPT_INT,		"slot from")
											P(MPT_INT,		"item count")
											P(MPT_INT64,	"market item id")
	C(MC_CS_TRADEMARKET_BUY,				MCDT_MACHINE2MACHINE, MCF_C2S, "Trademarket buy")
											P(MPT_INT64,	"market item id")
											P(MPT_INT,		"item count")
	C(MC_SC_TRADEMARKET_BOUGHT,				MCDT_MACHINE2MACHINE, MCF_S2C, "Trademarket bought")
											P(MPT_INT64,	"market item id")
											P(MPT_INT,		"slot to")
											P(MPT_INT,		"item count")
	C(MC_CS_TRADEMARKET_SEARCH,				MCDT_MACHINE2MACHINE, MCF_C2S, "Trademarket search")
	C(MC_CS_TRADEMARKET_SEARCH_NEXT,		MCDT_MACHINE2MACHINE, MCF_C2S, "Trademarket search next")
	C(MC_CS_TRADEMARKET_REFRESH,			MCDT_MACHINE2MACHINE, MCF_C2S, "Trademarket refresh")
	C(MC_SC_TRADEMARKET_ITEM_LIST,			MCDT_MACHINE2MACHINE, MCF_S2C, "Trademarket item list")
											P(MPT_BLOB,	"items")
	C(MC_CS_TRADEMARKET_GET_MYLIST,			MCDT_MACHINE2MACHINE, MCF_C2S, "get my trade item list")
	C(MC_SC_TRADEMARKET_MYLIST,				MCDT_MACHINE2MACHINE, MCF_S2C, "my trade item list")
											P(MPT_BLOB,	"items")


// Guide Book ---------------
	C(MC_SC_GUIDEBOOK_GAIN,					MCDT_MACHINE2MACHINE, MCF_S2C, "GuideBook Gain")
											P(MPT_INT, "nGuideBookID")


	// 보안(GameGuard) ------------------------------------------------------------------------------------------
	C(MC_GG_AUTH_REQ,						MCDT_MACHINE2MACHINE, MCF_S2C, "GameGuard Auth Req")
											P(MPT_SINGLE_BLOB,		"Auth Info")
	C(MC_GG_AUTH_RESPOND,					MCDT_MACHINE2MACHINE, MCF_C2S, "GameGuard Auth Respond")

	// COMMON --------------
	C(MC_TIME_SYNCH,						MCDT_MACHINE2MACHINE, MCF_S2C, "Time Synch")
											P(MPT_UINT,		"nGlobalTime")

	END_CMD_DESC();
}


bool CCommandVersionChecker::IsValid(int nCommandVersion)
{
	if (nCommandVersion == SH_COMMAND_VERSION)
	{
		return true;
	}

	return false;
}
