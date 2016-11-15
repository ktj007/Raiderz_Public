#pragma once

#include "CSCommonLib.h"

/// MasterServer - GameServer 간의 커맨드 테이블
/// - 커맨드 번호 : 14001 ~ 15000
enum CMaster2GameCommandTable
{
	// 네트웍 통신
	MMC_COMM_REQUEST_LOGIN_G2M						= 14001,	///< (G->M)	게임 서버가 마스터 서버에 로그인 요청
	MMC_COMM_RESPONSE_LOGIN_M2G						= 14002,	///< (M->G) 게임 서버가 마스터 서버에 로그인 응답
	MMC_COMM_REQUEST_LOGIN_GAME_SERVER				= 14003,	///< (M->G) 플레이어 GameServer 로그인예약 요청
	MMC_COMM_RESPONSE_LOGIN_GAME_SERVER 			= 14004,	///< (G->M) 플레이어 GameServer 로그인예약 응답
	MMC_COMM_REQUEST_RESERVE_MOVE_GAME_SERVER		= 14005,	///< (G->M) 게임서버 이동 예약 요청
	MMC_COMM_RESPONSE_RESERVE_MOVE_GAME_SERVER		= 14006,	///< (M->G) 게임서버 예약 응답
	MMC_COMM_REQUEST_READY_ENTER_MOVE_PLAYER		= 14007,	///< (M->G) 이동 플레이어 입장 준비 요청
	MMC_COMM_RESPONSE_READY_ENTER_MOVE_PLAYER		= 14008,	///< (G->M) 이동 플레이어 입장 준비 응답
	MMC_COMM_REQUEST_APP_SERVER_LIST				= 14009,	///< (G->M) 앱서버 목록 요청
	MMC_COMM_RESPONSE_APP_SERVER_LIST				= 14010,	///< (M->G)	앱서버 목록 응답
	MMC_COMM_CANCEL_MOVE_GAME_SERVER				= 14011,	///< (G->M) 게임서버 이동 취소
	MMC_COMM_CANCEL_RESERVED_MOVE_PLAYER			= 14012,	///< (M->G) 게임서버 이동 예약 취소
		// GS -> LS 이동
	MMC_COMM_RESERVE_MOVE_LOGINSERVER_REQ			= 14020,	///< (G->M) 로그인서버로 이동 예약 요청
	MMC_COMM_RESERVE_MOVE_LOGINSERVER_RES			= 14021,	///< (M->G) 로그인서버로 이동 예약 응답
	MMC_COMM_CANCEL_MOVE_LOGINSERVER				= 14022,	///< (G->M) 로그인서버 이동 취소

	// Player 관련
	MMC_PLAYER_DELETE_PLAYER						= 14101,	///< (G->M) 플레이어 제거 (추가는 LoginServer 에서)
	MMC_PLAYER_DUPLICATED_PLAYER_LOGIN				= 14102,	///< (M->G)	플레이어 중복 로그인

	MMC_PLAYER_STATE_IN_WORLD_REQ					= 14103,	///< (G->M) 플레이어 상태 변경 요청 - (월드 입장)
	MMC_PLAYER_STATE_IN_WORLD_RES					= 14104,	///< (M->G)	플레이어 상태 변경 응답 - (월드 입장)
	MMC_PLAYER_IN_FIELD_NOTIFY						= 14105,	///< (G->M) 플레이어가 필드에 들어옴 통지

	// Field 관련
	MMC_FIELD_ADD_SHARED_FIELD						= 14201,	///< (M->G) 공유 필드 추가됨
	MMC_FIELD_DEL_SHARED_FIELD						= 14202,	///< (M->G) 공유 필드 제거됨
	MMC_FIELD_SHARED_FIELD_INFO_ALL					= 14203,	///< (M->G) 전체 공유 필드 정보
	MMC_FIELD_REQUEST_SHARED_FIELD_INFO_ALL			= 14204,	///< (G->M) 전체 공유 필드 정보 요청

	MMC_FIELD_ADD_DYNAMIC_FIELD_GROUP				= 14208,	///< (M->G) 동적 필드 그룹 추가됨
	MMC_FIELD_REQUEST_DEL_DYNAMIC_FIELD_GROUP		= 14209,	///< (G->M) 동적 필드 그룹 제거 요청
	MMC_FIELD_DEL_DYNAMIC_FIELD_GROUP				= 14210,	///< (M->G) 동적 필드 그룹 제거됨
	MMC_FIELD_REQUEST_DYNAMIC_FIELD_GROUP_INFO_ALL	= 14211,	///< (M->G) 전체 동적 필드 그룹 정보 요청
	MMC_FIELD_DYNAMIC_FIELD_GROUP_INFO_ALL			= 14212,	///< (M->G) 전체 동적 필드 그룹 정보

	MMC_FIELD_UPDATE_PROXY_FIELD_INFO				= 14220,	///< (M->G) 게임서버의 Proxy 필드 정보 갱신

	// Gate 관련
	MMC_GATE_ASK_SHARED_FIELD						= 14301,	///< (G->M) 공유 필드 이동 문의
	MMC_GATE_ASK_DYNAMIC_FIELD						= 14302,	///< (G->M) 동적 필드 이동 문의
	MMC_GATE_RES									= 14303,	///< (M->G) 필드 이동 응답
	MMC_GATE_MOVE_SERVER							= 14304,	///< (M->G) 서버이동
	MMC_GATE_PREPARE_CHANGE_CHANNEL_REQ				= 14305,	///< (G->M) 채널 변경 예약 요청
	MMC_GATE_PREPARE_CHANGE_CHANNEL_RES				= 14306,	///< (M->G) 채널 변경 예약 응답
	MMC_GATE_CANCEL_CHANGE_CHANNEL					= 14307,	///< (G->M) 채널 변경 취소
	MMC_GATE_ASK_CHANGE_CHANNEL						= 14308,	///< (G->M) 채널 변경 문의

	// Party 관련	
	MMC_PARTY_ADD									= 14401,	///< (M->G) 파티 추가
	MMC_PARTY_REMOVE								= 14402,	///< (M->G) 파티 제거
	MMC_PARTY_ADD_MEMBER							= 14403,	///< (M->G) 멤버 추가
	MMC_PARTY_REMOVE_MEMBER							= 14404,	///< (M->G) 멤버 제거	

	MMC_PARTY_INVITE_REQ							= 14410,	///< (G->M) 파티 가입 요청
	MMC_PARTY_INVITE_RES							= 14411,	///< (M->G) 파티 가입 응답	
	MMC_PARTY_ACCEPT_REQ							= 14412,	///< (M->G) 가입 수락 요청
	MMC_PARTY_ACCEPT_RES							= 14413,	///< (G->M) 가입 수락 응답
	MMC_PARTY_ACCEPT_CANCEL							= 14414,	///< (M->G) 가입 수락 취소

	MMC_PARTY_UPDATE								= 14415,	///< (G->M) 파티 정보 변경
	MMC_PARTY_SYNC									= 14416,	///< (M->G) 파티 정보 갱신
	MMC_PARTY_UPDATE_MEMBER							= 14417,	///< (G->M) 파티원 정보 변경
	MMC_PARTY_MEMBER_SYNC							= 14418,	///< (M->G) 파티원 정보 갱신

	MMC_PARTY_ADD_QUEST								= 14419,	///< (G->M) 퀘스트 추가
	MMC_PARTY_QUEST_SYNC							= 14420,	///< (M->G) 퀘스트 갱신
	MMC_PARTY_ADD_FIELD								= 14421,	///< (G->M) 필드 추가
	MMC_PARTY_FIELD_SYNC							= 14422,	///< (M->G) 필드 갱신
	MMC_PARTY_UPDATE_QUEST_RUNNER					= 14423,	///< (G->M) 퀘스트 러너 정보 변경
	MMC_PARTY_QUEST_RUNNER_SYNC						= 14424,	///< (M->G) 퀘스트 러너 정보 갱신

	MMC_PARTY_LEAVE_REQ								= 14428,	///< (G->M) 탈퇴 요청
	MMC_PARTY_LEAVE_RES								= 14429,	///< (M->G) 탈퇴 응답

	MMC_PARTY_REMOVE_QUEST							= 14430,	///< (G->M) 퀘스트 제거
	MMC_PARTY_REMOVE_FIELD							= 14431,	///< (G->M) 필드 제거
	MMC_PARTY_ERASE_QUEST_RUNNER					= 14432,	///< (G->M) 퀘스트 러너 제거 요청
	MMC_PARTY_ERASE_QUEST_RUNNER_SYNC				= 14433,	///< (M->G) 퀘스트 러너 제거 갱신

	MMC_PARTY_KICK_REQ								= 14434,	///< (G->M) 강퇴 요청
	MMC_PARTY_KICK_RES								= 14435,	///< (M->G) 강퇴 갱신
	
	MMC_PARTY_DO_OFFLINE							= 14438,	///< (G->M) 오프라인 상태 전달
	MMC_PARTY_DO_ONLINE								= 14439,	///< (G->M) 온라인 상태 전달
	MMC_PARTY_ADD_OFFLINE_MEMBER					= 14440,	///< (M->G) 오프라인 파티원 추가
	MMC_PARTY_REMOVE_OFFLINE_MEMBER					= 14441,	///< (M->G) 오프라인 파티원 제거

	MMC_PARTY_JOIN_INVITE_REQ						= 14442,	///< (G->M) 가입 요청
	MMC_PARTY_JOIN_INVITE_RES						= 14443,	///< (M->G) 가입 응답
	MMC_PARTY_JOIN_ACCEPT_REQ						= 14444,	///< (M->G) 수락 요청
	MMC_PARTY_JOIN_ACCEPT_RES						= 14445,	///< (G->M) 수락 응답
	MMC_PARTY_JOIN_ACCEPT_CANCEL					= 14446,	///< (M->G) 수락 취소

	MMC_PARTY_MOVE_SERVER							= 14447,	///< (G->M) 서버이동 상태 통보
	MMC_PARTY_MOVE_SERVER_SYNC						= 14448,	///< (M->G) 서버이동 완료 상태 전달

	MMC_PARTY_CREATE_SINGLE_REQ						= 14449,	///< (G->M) 1인파티 생성 요청(Debug)

	MMC_PARTY_INFO_ALL_REQ							= 14450,	///< (G->M) 전체 파티정보 요청
	MMC_PARTY_INFO_ALL_RES							= 14451,	///< (M->G) 전체 파티정보 전달
	MMC_PARTY_REMOVE_FIELD_SELF						= 14452,	///< (G->M) 필드 제거

	MMC_PARTY_CHANGE_NAME_REQ						= 14456,	///< (G->M) 파티 이름 변경 요청	
	MMC_PARTY_CHANGE_NAME							= 14457,	///< (M->G) 파티 이름 변경
	
	MMC_PARTY_CHANGE_LEADER_REQ						= 14458,	///< (G->M) 파티장 변경 요청
	MMC_PARTY_CHANGE_LEADER							= 14459,	///< (M->G) 파티장 변경

	MMC_PARTY_CHANGE_LOOTING_RULE_REQ				= 14460,	///< (G->M) 파티 루팅룰 변경 요청
	MMC_PARTY_CHANGE_LOOTING_RULE					= 14461,	///< (M->G) 파티 루팅룰 변경
	MMC_PARTY_CHANGE_QUESTID_REQ					= 14462,	///< (G->M) 파티 수행퀘스트 변경 요청
	MMC_PARTY_CHANGE_QUESTID						= 14463,	///< (M->G) 파티 수행퀘스트 변경
	
	MMC_PARTY_FIXED_LOG_ON							= 14464,	///< (G->M) Expo 대비 준비된 파티에 로그온
	MMC_PARTY_CREATE_AUTO_PARTY_REQ					= 14465,	///< (G->M) 발기인 모아 파티 만들기 요청
	MMC_PARTY_JOIN_ATUTO_PARTY_REQ					= 14466,	///< (G->M) 자동파티 가입

	MMC_PARTY_FAIL									= 14490,	///< (M->G) 처리 실패


	// Loot
	MMC_LOOT_MASTERLOOT_NOTIFY_REQ					= 14501,	///< (G->M) 마스터룻알림 전달 요청	
	MMC_LOOT_MASTERLOOT_NOTIFY						= 14502,	///< (M->G) 마스터룻알림 전달

	MMC_LOOT_OTHER_GAIN_ITEM_REQ					= 14503,	///< (G->M) 다른사람의 아이템획득 전달 요청
	MMC_LOOT_OTHER_GAIN_ITEM						= 14504,	///< (M->G) 다른사람의 아이템획득 전달

	MMC_LOOT_GETTABLE_ITEM_ADD_REQ					= 14505,	///< (G->M) 획득가능한 아이템 추가 전달 요청
	MMC_LOOT_GETTABLE_ITEM_ADD						= 14506,	///< (M->G) 획득가능한 아이템 추가 전달

	MMC_LOOT_ROLLRESULT_REQ							= 14507,	///< (G->M) 주사위 굴림 전달 요청
	MMC_LOOT_ROLLRESULT								= 14508,	///< (M->G) 주사위 굴림 전달

	// Guild
	MMC_GUILD_MEMBER_ONLINEINFO_REQ					= 14551,	///< (G->M) 길드원 접속여부 정보 요청
	MMC_GUILD_MEMBER_ONLINEINFO						= 14552,	///< (M->G) 길드원 접속여부 정보

	MMC_GUILD_DESTROY								= 14553,	///< (G->M) 길드삭제
	MMC_GUILD_DESTROY_SYNC							= 14554,	///< (M->G) 길드삭제 갱신

	MMC_GUILD_JOIN									= 14555,	///< (G->M) 길드가입
	MMC_GUILD_JOIN_SYNC								= 14556,	///< (M->G) 길드가입 갱신

	MMC_GUILD_LEAVE									= 14557,	///< (G->M) 길드탈퇴
	MMC_GUILD_LEAVE_SYNC							= 14558,	///< (M->G) 길드탈퇴 갱신

	MMC_GUILD_KICK									= 14559,	///< (G->M) 길드강퇴
	MMC_GUILD_KICK_SYNC								= 14560,	///< (M->G) 길드강퇴 갱신

	MMC_GUILD_ONLINE								= 14561,	///< (G->M) 길드원접속
	MMC_GUILD_ONLINE_SYNC							= 14562,	///< (M->G) 길드원접속 갱신

	MMC_GUILD_OFFLINE								= 14563,	///< (G->M) 길드원비접속
	MMC_GUILD_OFFLINE_SYNC							= 14564,	///< (M->G) 길드원비접속 갱신

	MMC_GUILD_MOVEFIELD								= 14565,	///< (G->M) 길드원필드이동
	MMC_GUILD_MOVEFIELD_SYNC						= 14566,	///< (M->G) 길드원필드이동 갱신

	MMC_GUILD_CHANGE_MASTER							= 14567,	///< (G->M) 길마변경
	MMC_GUILD_CHANGE_MASTER_SYNC					= 14568,	///< (M->G) 길마변경 갱신

	MMC_GUILD_DEPOSIT_STORAGEMONEY					= 14569,	///< (G->M) 길드보관함 입금
	MMC_GUILD_DEPOSIT_STORAGEMONEY_SYNC				= 14570,	///< (M->G) 길드보관함 입금 갱신

	MMC_GUILD_WITHDRAW_STORAGEMONEY					= 14571,	///< (G->M) 길드보관함 출금
	MMC_GUILD_WITHDRAW_STORAGEMONEY_SYNC			= 14572,	///< (M->G) 길드보관함 출금 갱신

	MMC_GUILD_MOVE_STORAGEITEM						= 14573,	///< (G->M) 길드보관함 아이템 이동(이동, 합치기, 쪼개기)
	MMC_GUILD_MOVE_STORAGEITEM_SYNC					= 14574,	///< (M->G) 길드보관함 아이템 이동(이동, 합치기, 쪼개기) 갱신

	// Char
	MMC_CHAR_NPCDIE_REWARD_REQ						= 14601,	///< (G->M) NPC를 죽여서 얻은 보상 요청 (경험치, 퀘스트, 팩션, 엠블렘) 
	MMC_CHAR_NPCDIE_REWARD							= 14602,	///< (M->G) NPC를 죽여서 얻은 보상	

	// Msg
	MMC_MSG_ANNOUNCE								= 14701,	///< (M->G) PMS 공지 전달

	// Admin
	MMC_ADMIN_CHANGE_SERVER_MODE_REQ				= 14900,	///< (G->M) 서버 모드 변경 요청
	MMC_ADMIN_CHANGE_SERVER_MODE					= 14901,	///< (M->G) 서버 모드 변경

	// GM
	MMC_GM_SUMMON_REQ								= 14951,	///< (G->M) 플레이어 소환 요청
	MMC_GM_SUMMON									= 14952,	///< (G->M) 플레이어 소환


	// Debug
	MMC_DEBUG_DUMP									= 14910,	///< (M->G) 서버 정보 덤프		
};


/// MasterServer - GameServer 커맨드 등록
CSCOMMON_API void AddMaster2GameCommandTable();


/// MasterServer - LoginServer 간의 커맨드 테이블
/// - 커맨드 번호 : 15001 ~ 15500
enum CMaster2LoginCommandTable
{
	// 네트웍 통신
	MLC_COMM_REQUEST_LOGIN					= 15001,	///< (L->M)	로그인 서버가 마스터 서버에 로그인 요청
	MLC_COMM_RESPONSE_LOGIN					= 15002,	///< (M->L) 로그인 서버가 마스터 서버에 로그인 응답
	MLC_COMM_REQUEST_LOGIN_GAME_SERVER		= 15003,	///< (L->M) 플레이어의 GameServer 로그인 요청
	MLC_COMM_RESPONSE_LOGIN_GAME_SERVER 	= 15004,	///< (M->L) 플레이어의 GameServer 로그인 응답
	MLC_COMM_READY_ENTER_LOGINSERVER_REQ	= 15005,	///< (M->L) 플레이어 이동 준비 요청 From GameServer
	MLC_COMM_READY_ENTER_LOGINSERVER_RES	= 15006,	///< (L->M) 플레이어 이동 준비 응답 From GameServer
	MLC_COMM_CANCEL_MOVE_LOGINSERVER		= 15007,	///< (M->L) 로그인서버 이동 취소 From GameServer


	// Player 관련
	MLC_PLAYER_ADD_PLAYER_REQ				= 15101,	///< (L->M) 플레이어 추가 요청
	MLC_PLAYER_ADD_PLAYER_RES				= 15102,	///< (M->L) 플레이어 추가 응답
	MLC_PLAYER_DELETE_PLAYER				= 15103,	///< (L->M) 플레이어 제거
	MLC_PLAYER_DUPLICATED_PLAYER_LOGIN		= 15104,	///< (M->L) 플레이어 중복 로그인

	MLC_PLAYER_STATE_SELECT_CHAR			= 15105,	///< (L->M)	플레이어 상태 변경 (캐릭터 선택)

	MLC_PLAYER_REQUEST_SET_MYCHAR			= 15110,	///< (L->M) 플레이어의 캐릭터 설정 요청
	MLC_PLAYER_RESPONSE_SET_MYCHAR			= 15111,	///< (M->L) 플레이어의 캐릭터 설정 응답

	// Pmang
	MLC_PLAYER_PMANG_ADD_PLAYER_REQ			= 15112,	///< (L->M) 피망 플레이어 추가 요청
	MLC_PLAYER_PMANG_ADD_PLAYER_RES			= 15113,	///< (M->L) 피망 플레이어 추가 응답

	// Admin
	MLC_ADMIN_CHANGE_SERVER_MODE			= 15401,	///< (M->L) 서버 모드 변경
};

/// MasterServer - LoginServer 커맨드 등록
CSCOMMON_API void AddMaster2LoginCommandTable();


/// MasterServer - AppServer 간의 커맨드 테이블
/// - 커맨드 번호 : 15501 ~ 16000
enum CMaster2AppCommandTable
{
	// 네트웍 통신
	MAC_COMM_REQUEST_LOGIN				= 15501,	///< (A->M)	어플리케이션 서버가 마스터 서버에 로그인 요청
	MAC_COMM_RESPONSE_LOGIN				= 15502,	///< (M->A) 어플리케이션 서버가 마스터 서버에 로그인 응답
};

/// MasterServer - AppServer 커맨드 등록
CSCOMMON_API void AddMaster2AppCommandTable();
