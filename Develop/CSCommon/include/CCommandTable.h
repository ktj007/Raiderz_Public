#ifndef _CCOMMAND_TABLE_H
#define _CCOMMAND_TABLE_H

#include "CSCommonLib.h"

// 레이더즈 커맨드 버전
#define SH_COMMAND_VERSION				129
// Rev. 128 - Added SoulHunterZ Item Attunement Commands
// Rev. 129 - Added 4 more Soul Attunement Commands _PREPARE, _CANCEL, _CHECK_REQ, _CHECK_RESULT

// ----------------------------------------------------------------------
// 커맨드 테이블 ID 할당
//
// Client - GameServer : 1001 ~ 10000 (CCommandTable)
// LoginServer - MasterServer : 10001 ~ 12000 (CCommandTable_Login)
// GameServer - LogServer : 12001 ~ 14000 (CCommandTable_Log)
// MasterServer - All Server : 14001 ~ 16000 (CCommandTable_Masater)
// GameServer - AppServer : 16001 ~ 18000 (CCommandTable_App)
// ServerCommon : 18001 ~ 19000 (SCommandTable)
// Client - WorldLocator : 20000 ~ 21000 (CCommandTable_WorldLocator)
// ----------------------------------------------------------------------


enum CCommandTable
{
// 커맨드 선언 //////////////////////////////////////////////////////////////////////////
// 커맨드 번호 1001 ~ 10000 까지 사용

// 네트웍 통신(COMM) ---------------------------------------------------------------------------------
	MC_COMM_REQUEST_ACCOUNT_CHAR_LIST			= 1011,		///< (C->S) 캐릭터 리스트 요청
	MC_COMM_RESPONSE_ACCOUNT_CHAR_LIST			= 1012,		///< (S->C) 캐릭터 리스트 응답
	MC_COMM_MOVE_TO_GAME_SERVER					= 1013,		///< (S->C) 게임서버로 이동해라 (From LS To GS)
	MC_COMM_DUPLICATED_PLAYER_LOGIN				= 1014,		///< (S->C) 다른 곳에서 중복으로 로그인
	MC_COMM_START_MOVE_TO_LOGIN_SERVER_REQ		= 1015,		///< (C->S) 캐릭터 선택 화면으로 이동 시작 요청
	MC_COMM_START_MOVE_TO_LOGIN_SERVER_RES		= 1016,		///< (S->C) 캐릭터 선택 화면으로 이동 시작 응답

// 캐릭터 관리 --------------------------------------------------------------------------------------
	MC_CHAR_REQUEST_INSERT_CHAR		= 1020,		///< (C->S) 캐릭터 생성 요청
	MC_CHAR_RESPONSE_INSERT_CHAR,				///< (S->C) 캐릭터 생성 응답
	MC_CHAR_REQUEST_DELETE_CHAR,				///< (C->S) 캐릭터 삭제 요청
	MC_CHAR_RESPONSE_DELETE_CHAR,				///< (S->C) 캐릭터 삭제 응답

	MC_CHAR_REQUEST_SEL_MYCHAR,					///< (C->S) 내 캐릭터 선택
	MC_CHAR_RESPONSE_SEL_MYCHAR,				///< (S->C) 캐릭터 선택 응답

	MC_CHAR_GAMESTART,							///< (C->S) 게임 시작하자

// 캐릭터(CHAR) --------------------------------------------------------------------------------------
	MC_CHAR_MYINFO					= 1040,		///< (S->C) 내 정보 전달
	MC_CHAR_MYTIMEINFO,							///< (S->C) 내 시간 정보 전달 (버프 시간, 탤런트 쿨타임, 부활 대기시간)
	MC_CHAR_REBIRTH_REQ,						///< (C->S) 부활 요청 [최적화]
	MC_CHAR_REBIRTH_MYPLAYER,					///< (S->C) 부활 내게 알림 [최적화]
	MC_CHAR_REBIRTH_NETPLAYER,					///< (S->C) 부활 다른이들에게 알림 [최적화]
	MC_CHAR_REWARD_EXP,							///< (S->C) 경험치 획득
	MC_CHAR_LEVEL_UP,							///< (S->C) 레벨업
	MC_CHAR_UPDATE_SIMPLE_STATUS,				///< (S->C) 상태(HP, EN)가 변화했다. [최적화]
	MC_CHAR_SAVE_SOULBINDING,					///< (S->C) 소울 바인딩 저장
	MC_CHAR_SAVE_CHECKPOINT,					///< (S->C) 체크포인트 저장
	MC_CHAR_UPDATE_MONEY,						///< (S->C) 실버 획득
	MC_CHAR_ENEMY_INFO,							///< (S->C) 적들의 HP 정보
	MC_CHAR_ENEMY_INFO_CLEAR,					///< (S->C) 적들이 없어짐
	MC_CHAR_UPDATE_TP,							///< (S->C) TP 변경
	MC_CHAR_TIP,								///< (S->C) 개인용 Narration
	MC_CHAR_ADD_ENEMYNPC,						///< (S->C) 어그로를 가진 NPC가 추가됨
	MC_CHAR_DEL_ENEMYNPC,						///< (S->C) 어그로를 가진 NPC가 삭제됨
	MC_CHAR_TARGET_INFO_REQ,					///< (C->S) 레일을 쏴서 선택된 대상의 인포 요청
	MC_CHAR_TARGET_INFO,						///< (S->C) 레일을 쏴서 선택된 대상의 인포
	MC_CHAR_DIE_REQ,							///< (C->S) 자살 요청

// 정보 ///////////////
	MC_FIELD_IN_PLAYER				= 1115,		///< (S->C) 플레이어 들어옴 [최적화]
	MC_FIELD_OUT_PLAYER				= 1116,		///< (S->C) 플레이어 나감 [최적화]
	MC_FIELD_IN_NPC					= 1117,		///< (S->C) NPC 들어옴
	MC_FIELD_OUT_NPC				= 1118,		///< (S->C) NPC 나감 [최적화]
	MC_FIELD_IN_BUFFENTITY			= 1119,		///< (S->C) 마법진 생김 [최적화]
	MC_FIELD_OUT_BUFFENTITY			= 1120,		///< (S->C) 마법진 없어짐 [최적화]
	MC_FIELD_TRAP_TRIGGERED			= 1121,		///< (S->C) 트랩 밟아서 없어짐 [최적화]
	MC_FIELD_SECTOR_ENTITY_INFO		= 1123,		///< (S->C) 주위 엔티티 정보
	MC_FIELD_SET_TIME				= 1124,		///< (S->C) 게임 시간 설정 [최적화]
	MC_FIELD_SET_WEATHER			= 1125,		///< (S->C) 게임 날씨 설정 [최적화]
	MC_FIELD_SET_TIME_WEATHER		= 1126,		///< (S->C) 시간, 날씨 설정 [최적화]
	MC_FIELD_WHO_IS_REQ				= 1127,		///< (C->S) 해당 UID가 누구냐 (UID)
	MC_FIELD_WHO_IS_UIID_REQ		= 1128,		///< (C->S) 해당 UID가 누구냐 (UIID)
	MC_FIELD_PLAYER_EXTRA_INFO_REQ	= 1129,		///< (C->S) 플레이어 추가 정보 주세요
	MC_FIELD_IN_BPART				= 1130,		///< (S->C) BPART 들어옴
	MC_FIELD_OUT_BPART				= 1131,		///< (S->C) BPART 나감

// 커뮤니티 ///////////
	MC_MSG_REQ						= 1200,		///< (C->S) 대화 발신
	MC_MSG_RES,									///< (S->C) 대화 수신
	
	MC_MSG_CREATE_CHANNEL_REQ,					///< (C->S) 채팅 채널 생성 요청	
	MC_MSG_ENTER_CHANNEL_REQ,					///< (C->S) 채팅 채널 입장 요청
	MC_MSG_ENTER_CHANNEL,						///< (S->C) 채팅 채널 입장
	MC_MSG_LEAVE_CHANNEL_REQ,					///< (C->S) 채팅 채널 퇴장 요청
	MC_MSG_LEAVE_CHANNEL,						///< (S->C) 채팅 채널 퇴장
	MC_MSG_KICK_PLAYER_REQ,						///< (C->S) 채팅 채널 플레이어 kick 요청
	MC_MSG_KICK_PLAYER,							///< (S->C) 채팅 채널 플레이어 kick
	MC_MSG_BAN_PLAYER_REQ,						///< (C->S) 채팅 채널 플레이어 ban 요청
	MC_MSG_BAN_PLAYER,							///< (S->C) 채팅 채널 플레이어 ban	
	MC_MSG_UNBAN_PLAYER_REQ,					///< (C->S) 채팅 채널 플레이어 unban 요청	
	MC_MSG_UNBAN_PLAYER,						///< (S->C) 채팅 채널 플레이어 unban
	MC_MSG_CHANGE_HOST_REQ,						///< (C->S) 채팅 채널 호스트 변경 요청
	MC_MSG_CHANGE_HOST,							///< (S->C) 채팅 채널 호스트 변경

	MC_MSG_ENTER_CHANNEL_PLAYER_INFO,			///< (S->C) 채팅 채널 입장플레이어 정보
	MC_MSG_LEAVE_CHANNEL_PLAYER_INFO,			///< (S->C) 채팅 채널 퇴장플레이어 정보
	MC_MSG_KICK_PLAYER_INFO,					///< (S->C) 채팅 채널 강퇴플레이어 정보

// 필드 이동(FIELD) ----------------------------------------------------------------------------------
	MC_FIELD_PREPARING_TO_LOAD		= 1331,		///< (S->C) 로딩 준비해라
	MC_FIELD_LOADING_COMPLETE		= 1332,		///< (C->S) 로딩 완료(아직 로딩 화면)
	MC_FIELD_START_GAME				= 1333,		///< (S->C) 게임 시작(로딩화면에서 게임화면으로 전환)
	MC_FIELD_CHANGE_FIELD			= 1334,		///< (S->C) 플레이어 필드 이동 (공유/인스턴스 공통)
	MC_FIELD_PREPARE_CHANGE_CHANNEL_REQ	= 1335,		///< (C->S) 플레이어 채널 변경 준비 요청
	MC_FIELD_PREPARE_CHANGE_CHANNEL_RES	= 1336,		///< (S->C) 플레이어 채널 변경 준비 응답
	MC_FIELD_CANCEL_CHANGE_CHANNEL	= 1337,		///< (C->S) 플레이어 채널 변경 준비 요청
	MC_FIELD_REQUEST_CHANGE_CHANNEL	= 1338,		///< (C->S) 플레이어 채널 변경 요청 (준비 패킷 후에 10초 뒤에 날려야 함)
	MC_FIELD_REQUEST_CHANNEL_LIST	= 1339,		///< (C->S) 플레이어 채널 목록 요청
	MC_FIELD_CHANNEL_LIST			= 1340,		///< (S->C) 플레이어 채널 목록
	MC_FIELD_CANCEL_CHANGE_FIELD	= 1341,		///< (S->C) 필드 이동 취소

	MC_DYNAMIC_FIELD_ENTER_REQ		= 1350,		///< (C->S) 플레이어 다이나믹 필드 참가 (공유/인스턴스 공통)
	MC_DYNAMIC_FIELD_LEAVE_REQ		= 1351,		///< (C->S) 플레이어 다이나믹 필드 나가기
	MC_DYNAMIC_FIELD_EXPIRING		= 1352,		///< (S->C) 동적필드 파기중 알림
	MC_DYNAMIC_FIELD_CANCEL_EXPIRE	= 1353,		///< (S->C) 동적필드 파기 cancel


// 액션(ACTION) --------------------------------------------------------------------------------------
	MC_ACTION_FAILED				= 1401,		///< (S->C) 클라이언트가 요청한 액션이 실패되었다.

	MC_ACTION_MOVE_REQ				= 1411,		///< (C->S) 필드에서 이동 요청 [최적화]
	MC_ACTION_MOVE					= 1412,		///< (S->C) 이동 [최적화]
	MC_ACTION_STOP_REQ				= 1413,		///< (C->S) 필드에서 정지 요청 [최적화]
	MC_ACTION_STOP					= 1414,		///< (S->C) 정지 [최적화]
	MC_ACTION_STOP_MOVE_REQ			= 1415,		///< (C->S) 플레이어 정지 요청 [최적화]
	MC_ACTION_STOP_MOVE				= 1416,		///< (S->C) 플레이어 정지 [최적화]
	MC_ACTION_ROTATE_REQ			= 1417,		///< (C->S) 방향 변경 요청 [최적화]
	MC_ACTION_ROTATE				= 1418,		///< (S->C) 방향 변경 [최적화]

	MC_ACTION_CHANGE_STANCE_REQ		= 1419,		///< (C->S) 스탠스 변경 [최적화]
	MC_ACTION_CHANGE_STANCE			= 1420,		///< (S->C) 스탠스 변경 [최적화]

	MC_ACTION_ATTACK_MELEE_REQ		= 1421,		///< (C->S) 플레이어 공격
	MC_ACTION_ATTACK_ARCHERY_REQ	= 1423,		///< (C->S) 활로 공격 요청
	MC_ACTION_ATTACK_ARCHERY		= 1424,		///< (S->C) 활로 공격
	MC_ACTION_ATTACK_ARCHERY_DETAIL_REQ	= 1425,		///< (C->S) 활로 공격 요청
	MC_ACTION_ATTACK_ARCHERY_DETAIL		= 1426,		///< (S->C) 활로 공격
	MC_ACTION_ATTACK_GUN_REQ		= 1427,		///< (C->S) 총으로 공격 요청
	MC_ACTION_ATTACK_GUN			= 1428,		///< (S->C) 총으로 공격

	MC_ACTION_ATTACK_CANCEL_REQ		= 1429,		///< (C->S) 공격 취소 [최적화]
	MC_ACTION_ATTACK_CANCEL			= 1430,		///< (S->C) 공격 취소 [최적화]

	MC_ACTION_PREPARE_ATTACK_REQ	= 1431,		///< (C->S) 플레이어 공격 준비 (활 시위 당기기등) [최적화]
	MC_ACTION_PREPARE_ATTACK		= 1432,		///< (S->C) 플레이어 공격 준비 [최적화]

	MC_ACTION_GUARD_REQ				= 1433,		///< (C->S) 가드 시작 요청 [최적화]
	MC_ACTION_GUARD					= 1434,		///< (S->C) 가드 시작 [최적화]
	MC_ACTION_GUARD_FAILED			= 1435,		///< (S->C) 가드 실패
	MC_ACTION_GUARD_RELEASED_REQ	= 1436,		///< (C->S) 가드 해제 요청 [최적화]
	MC_ACTION_GUARD_RELEASED		= 1437,		///< (S->C) 가드 해제 [최적화]


	MC_ACTION_GUARD_DEFENSE			= 1440,		///< (S->C) 방어 성공 [최적화]
	MC_ACTION_GUARD_DEFENSE_PARTIAL	= 1441,		///< (S->C) 방어 성공 [최적화]

	MC_ACTION_USE_TALENT_REQ		= 1442,		///< (C->S) 탤런트 사용 요청 [최적화]
	MC_ACTION_USE_TALENT			= 1443,		///< (S->C) 탤런트 사용(즉시 사용 탤런트일 경우 실제 사용, 아닐 경우 탤런트 준비 단계) [최적화]
	MC_ACTION_ACT_TALENT_REQ		= 1444,		///< (S->C) 탤런트 실제 사용 [최적화]
	MC_ACTION_ACT_TALENT_WITH_HITCAPSULE_REQ = 1445,	///< (S->C) 탤런트 실제 사용 (히트캡슐 정보 포함)
	MC_ACTION_ACT_TALENT_WITH_GROUND_REQ	 = 1446,	///< (S->C) 탤런트 실제 사용 (지상목표 정보 포함)
	MC_ACTION_ACT_TALENT			= 1447,		///< (S->C) 탤런트 실제 사용 요청 [최적화]
	MC_ACTION_ACT_TALENT_WITH_GROUND = 1448,		///< (S->C) 탤런트 실제 사용 요청 (땅을 지정) [최적화]
	MC_ACTION_CANCEL_TALENT_REQ		= 1449,		///< (C->S) 탤런트 취소 요청 [최적화]
	MC_ACTION_CANCEL_TALENT			= 1450,		///< (S->C) 탤런트 취소 [최적화]
	MC_ACTION_END_TALENT_COOLDOWN	= 1451,		///< (S->C) 쿨다운 끝. 이제 탤런트 사용 가능하다. 파라메타는 탤런트 ID [최적화]
	MC_ACTION_EXTRA_ACT_TALENT		= 1452,		///< (S->C) 탤런트 Extra Phase로 이동 [최적화]
	MC_ACTION_REQUEST_FAIL_TALENT	= 1453,		///< (S->C) 탤런트 사용 요청 실패 [최적화]

	MC_ACTION_ACT_TALENT_PROJECTILE	= 1454,		///< (S->C) 마법 사용2 (ACT_SPELL과 같은 용도이지만 파라메타로 TargetUIDList대신 목표의 위치를 사용한다.)
	MC_ACTION_ACT_SPELL_MAGICAREA	= 1455,		///< (S->C) 마법 사용3 (ACT_SPELL과 같은 용도이지만 파라메타로 TargetUIDList대신 목표의 위치를 사용한다.) [최적화]
	MC_ACTION_RECASTING_SPELL		= 1456,		///< (S->C) 마법 캐스팅 시작 (EN이 부족하여 캐스팅이 멈춘것을 푼다.) [최적화]
	
	MC_ACTION_TALENT_HIT				= 1460,		///< (S->C) 탤런트 맞음(데미지만 닳을 때 명령어 사용) [최적화]
	MC_ACTION_TALENT_HIT_NO_MF			= 1461,		///< (S->C) 탤런트 맞음(보내줘야 될 값이 여러개일 경우) [최적화]
	MC_ACTION_TALENT_HIT_KNOCKBACK_MF	= 1462,		///< (S->C) 탤런트 맞음(넉백, 쓰로우 업 모션팩터) [최적화]
	MC_ACTION_TALENT_HEAL				= 1463,		///< (S->C) 탤런트로 회복

	MC_ACTION_BUFF_HIT				= 1464,		///< (S->C) 버프 맞음(데미지만 닳을 때 명령어 사용) [최적화]
	MC_ACTION_BUFF_HIT_NO_MF		= 1465,		///< (S->C) 버프 맞음(보내줘야 될 값이 여러개일 경우) [최적화]
	MC_ACTION_TALENT_HIT_MISS		= 1466,		///< (S->C) 탤런트 회피함(회피했을 때) [최적화]
	MC_ACTION_TALENT_DRAIN			= 1467,		///< (S->C) 탤런트로 적의 스탯을 흡수함 [최적화]

	MC_ACTION_BREAK_PART			= 1470,		///< (S->C) NPC의 브레이커블 파츠 파괴됨
	MC_ACTION_BPART_RECOVERY		= 1471,		///< (S->C) NPC의 브레이커블 복구
	MC_ACTION_BREAK_PART_EX			= 1472,		///< (S->C) NPC의 브레이커블 파츠 파괴됨 (사용가능한 파츠 포함)

	MC_ACTION_JUMP_REQ				= 1480,		///< (C->S) 점프 요청 [최적화]
	MC_ACTION_JUMP					= 1481,		///< (S->C) 플레이어 점프 [최적화]
	MC_ACTION_FALLING_REQ			= 1482,		///< (C->S) 나 떨어진다 [최적화]
	MC_ACTION_FALLING				= 1483,		///< (S->C) 플레이어 떨어짐 [최적화]
	MC_ACTION_END_FALLING			= 1484,		///< (C->S) 나 떨어져서 땅에 닿음 [최적화]
	MC_ACTION_FALLING_DAMAGE		= 1485,		///< (S->C) 낙하 데미지 [최적화]

	MC_ACTION_DIE					= 1490,		///< (S->C) 엔티티 죽음	[최적화]
	MC_ACTION_DESPAWN				= 1492,		///< (S->C) 엔티티 소멸 [최적화]

	MC_ACTION_SWIM_REQ				= 1493,		///< (C->S) 수영(or 수영 해제) 요청 [최적화]

	MC_ACTION_CANCEL_NEXT_ATTACK	= 1500,		///< (S->C) 다음 공격은 캔슬하라 [최적화]

	MC_ACTION_STANDUP_REQ			= 1501,		///< (C->S) 넉다운에서 기상 요청 [최적화]
	MC_ACTION_STANDUP				= 1502,		///< (S->C) 넉다운에서 기상 [최적화]

	MC_ACTION_POS_SYNC_REQ			= 1503,		///< (C->S) MoveReq 이외의 위치 동기가 필요하면 이 커맨드를 사용한다. [최적화]
	MC_ACTION_MOVING_MF_POS_SYNC_REQ  = 1504,	///< (C->S) 모션 팩터가 끝났을 때의 위치 동기 맞추기 요청 [최적화]

	MC_ACTION_SWITCHING_WEAPON_BEGIN_REQ	= 1510,		///< (C->S) 주무기/보조무기 변경 시작 [최적화]
	MC_ACTION_SWITCHING_WEAPON_BEGIN		= 1511,		///< (S->C) 주무기/보조무기 변경 시작 [최적화]
	MC_ACTION_SWITCHING_WEAPON_REQ	= 1512,		///< (C->S) 주무기/보조무기 변경 [최적화]
	MC_ACTION_SWITCHING_WEAPON		= 1513,		///< (S->C) 주무기/보조무기 변경 [최적화]

	MC_ACTION_SIT_REQ				= 1514,		///< (C->S) 앉기 요청 [최적화]
	MC_ACTION_SIT					= 1515,		///< (S->C) 앉기 [최적화]

	MC_ACTION_SIT_RISE_REQ			= 1516,		///< (C->S) 서기 요청 [최적화]
	MC_ACTION_SIT_RISE				= 1517,		///< (S->C) 서기 [최적화]

	MC_ACTION_HIDE					= 1520,		///< (S->C) 투명화 - 숨기
	MC_ACTION_AFK_BEGIN				= 1521,		///< (S->C) 자리비움 시작
	MC_ACTION_AFK_END				= 1522,		///< (S->C) 자리비움 끝
	MC_ACTION_AFK_REQ				= 1523,		///< (C->S) 자리비움 요청
	MC_ACTION_END_COMBAT			= 1524,		///< (S->C) 전투 종료

	// 트리거 커맨드(TRIGGER) -----------------------------------------------------------------------------
	MC_TRIGGER_TOUCH_SENSOR_ENTER			= 1531,		///< (C->S) 터치 센서 지역으로 들어옴
	MC_TRIGGER_TOUCH_SENSOR_LEAVE			= 1532,		///< (C->S) 터치 센서 지역으로 나감
	MC_TRIGGER_UPDATE_USABLE_SENSOR			= 1533,		///< (S->C) 사용가능한 센서 정보 변경
	MC_TRIGGER_SENSOR_DESTINATION_SEL_REQ	= 1534,		///< (S->C) 센서의 복수 목적지 선택 요청
	MC_TRIGGER_SENSOR_DESTINATION_SEL		= 1535,		///< (C->S) 센서의 복수 목적지 선택
	MC_TRIGGER_SENSOR_INTERACTION_REQ		= 1536,		///< (C->S) 센서에서 인터랙션 시도
	MC_TRIGGER_SENSOR_ERROR_MSG				= 1537,		///< (S->C) 센서 목적지 에러 통보
	MC_TRIGGER_SENSOR_FOLLOW_PARTY_REQ		= 1538,		///< (S->C) 센서를 통한 파티원 이동 시, 쫒아갈지 여부확인
	MC_TRIGGER_SENSOR_FOLLOW_PARTY_RES		= 1539,		///< (C->S) 센서를 통한 파티원 이동 시, 쫒아갈지 여부결정

// 모든 개체 명령어
	MC_ENTITY_WARP					= 1541,		///< (S->C) Entity 순간 이동

// NPC 전용 커맨드(NPC) --------------------------------------------------------------------------------
	MC_NPC_MOVE						= 1601,		///< (S->C) NPC 이동(걷기) [최적화]
	MC_NPC_FACE_TO					= 1602,		///< (S->C) NPC 바로보는 방향 변경 [최적화]
	MC_NPC_RUN						= 1603,		///< (S->C) NPC 이동(달리기) [최적화]
	MC_NPC_MOVE_STOP				= 1604,		///< (S->C) NPC 이동 정지 [최적화]
	MC_NPC_CHANGE_CAPSULE			= 1609,		///< (S->C) NPC의 충돌 캡슐을 변경
	MC_NPC_SAY						= 1610,		///< (S->C) NPC 말하기
	MC_NPC_BALLOON					= 1611,		///< (S->C) NPC 말풍선
	MC_NPC_SHOUT					= 1612,		///< (S->C) NPC 외치기
	MC_NPC_NARRATION				= 1613,		///< (S->C) 나레이션
	MC_NPC_SAY_AND_NARRATION		= 1614,		///< (S->C) NPC 말풍선과 나레이션을 동시에 말하기
	MC_NPC_CHANGE_ATTACKABLE		= 1615,		///< (S->C) NPC에게 공격 가능 여부 변경
	MC_NPC_ENABLE_INTERACT			= 1616,		///< (S->C) 인터랙션가능 여부 변경
	MC_NPC_CHANGE_COMBAT_TARGET		= 1617,		///< (S->C) 싸울 목표 액터가 변경됨
	MC_NPC_SPEWUP					= 1618,		///< (S->C) NPC가 먹은후 뱉기
	MC_NPC_SPEWUP_INTERRUPT			= 1619,		///< (S->C) NPC가 먹은후 공격을 받아 뱉기


// 아이템(ITEM) ----------------------------------------------------------------------------------------
	MC_FIELD_REQUEST_FEATURE		= 1701,		///< (C->S) 외모 정보 요청
	MC_FIELD_FEATURE				= 1702,		///< (S->C) 외모 정보 (장비정보만)

	MC_ITEM_EQUIP_REQ				= 1703,		///< (C->S) 아이템 착용 요청
	MC_ITEM_EQUIP					= 1704,		///< (S->C) 아이템 착용
	MC_ITEM_EQUIP_SWAPTOINVEN		= 1705,		///< (S->C) 인벤에 있는 아이템을 착용하면서 벗겨진 아이템을 인벤위치로 swap

	MC_ITEM_UNEQUIP_REQ				= 1710,		///< (C->S) 아이템 장착 해제 요청
	MC_ITEM_UNEQUIP					= 1711,		///< (S->C) 아이템 장착 해제

	MC_ITEM_CHANGE_LOOK_EQUIP_ITEM	= 1712,		///< (S->C) 아이템 변경 정보 알림
	MC_ITEM_CHANGE_LOOK_UNEQUIP_ITEM= 1713,		///< (S->C) 아이템 변경 정보 알림

	MC_ITEM_USE_REQ					= 1714,		///< (C->S) 아이템 사용 요청
	MC_ITEM_USE_FAIL				= 1715,		///< (S->C) 아이템 사용 실패
	MC_ITEM_TALENT					= 1716,		///< (S->C) 아이템 탤런트 사용 허락
	
	MC_ITEM_ADD						= 1717,		///< (S->C) 아이템 획득
	MC_ITEM_DROP_REQ				= 1718,		///< (C->S) 아이템 버림 요청
	MC_ITEM_REMOVE					= 1719,		///< (S->C)	아이템 버림	

	MC_ITEM_UPDATE_DURABILITY		= 1722,		///< (S->C)	아이템 내구도 변경

	MC_ITEM_INTERACT_NPC_REQ		= 1723,		///< (C->S) 아이템의 인터랙션 요청.

	MC_ITEM_MOVE_REQ				= 1726,		///< (C->S) 아이템 이동(이동, 합치기, 쪼개기) 요청
	MC_ITEM_MOVE					= 1727,		///< (S->C) 아이템 이동

	MC_ITEM_SORT_INVEN_SLOT_REQ		= 1730,		///< (C->S) 아이템 인벤토리 위치 정렬 요청
	MC_ITEM_SORT_INVEN_SLOT			= 1731,		///< (S->C) 아이템 인벤토리 위치 정렬

	MC_ITEM_DYE_PREPARE_REQ			= 1740,		///< (C->S)	아이템 염색 준비 요청
	MC_ITEM_DYE_PREPARECANCEL_REQ	= 1741,		///< (S->C)	아이템 염색 준비 취소 요청
	MC_ITEM_DYE_REQ					= 1742,		///< (C->S)	아이템 염색 요청
	MC_ITEM_DYE						= 1743,		///< (S->C)	아이템 염색	


// 퀘스트(QUEST) ---------------------------------------------------------------------------------------
	MC_QUEST_GIVE_REQ				= 1801,		///< (C->S) 퀘스트 제공 요청
	MC_QUEST_GIVE					= 1802,		///< (S->C) 퀘스트 제공 응답
	MC_QUEST_UPDATE_OBJECTIVE		= 1803,		///< (S->C) 오브젝티브 달성 상태 변경 알림
	MC_QUEST_COMPLETE				= 1804,		///< (S->C) 오브젝티브가 없어서 퀘스트를 받자마자 완료
	MC_QUEST_INCOMPLETE				= 1805,		///< (S->C) 오브젝티브가 없어서 퀘스트를 받자마자 완료
	MC_QUEST_FAIL					= 1806,		///< (S->C) 퀘스트 실패
	MC_QUEST_REWARD_REQ				= 1807,		///< (C->S) 퀘스트 보상 요청
	MC_QUEST_REWARD					= 1808,		///< (S->C) 퀘스트 보상 응답
	MC_QUEST_CANCEL_REQ				= 1809,		///< (C->S) 퀘스트 취소 요청
	MC_QUEST_CANCEL					= 1810,		///< (S->C) 퀘스트 취소 응답
	MC_QUEST_SHOW_ACCPET_UI			= 1812,		///< (S->C) 퀘스트 수락 UI 보여주기
	MC_QUEST_SHOW_REWARD_UI			= 1813,		///< (S->C) 퀘스트 보상 UI 보여주기
	
	MC_QUEST_SHARE_REQ				= 1814,		///< (C->S) 퀘스트 공유 요청
	MC_QUEST_SHARE_REQ_RELAY		= 1815,		///< (S->C) 퀘스트 공유 요청 전달
	MC_QUEST_SHARE					= 1816,		///< (S->C) 퀘스트 공유 결과
	MC_QUEST_SHARE_GIVE_REQ			= 1818,		///< (C->S) 퀘스트 공유를 통한 퀘스트 제공  요청

	MC_QUEST_INTERACT_QOBJECTIVE_BEGIN	= 1819,		///< (S->C) 인터랙션으로 퀘스트 목적 갱신 시작
	MC_QUEST_INTERACT_QOBJECTIVE_CANCEL_REQ	= 1820,	///< (C->S) 인터랙션으로 퀘스트 목적 갱신 취소 요청
	MC_QUEST_INTERACT_QOBJECTIVE_END	= 1821,		///< (S->C) 인터랙션으로 퀘스트 목적 갱신 종료

	MC_CHALLENGER_QUEST_LIST		= 1822,		///< (S->C) 인터랙션으로 퀘스트 목적 갱신 종료
	MC_CHALLENGER_QUEST_COMPLETE	= 1823,		///< (S->C) 도전자퀘스트 완료
	MC_CHALLENGER_QUEST_LIST_REFRESH = 1824,	///< (S->C) 도전자퀘스트 완료


// 다이얼로그(DIALOG) -----------------------------------------------------------------------------------
	MC_DIALOG_START					= 1901,		///< (S->C) 다이얼로그 시작
	MC_DIALOG_SELECT_REQ			= 1902,		///< (C->S) 다이얼로그 분기 선택 요청
	MC_DIALOG_SELECT				= 1903,		///< (S->C) 다이얼로그 진행

// 트레이닝(TRAINING)----------------------------------------------------------------------------------------
	MC_TRAINING_TRAIN_REQ	= 2001,		///< (C->S)	탤런트 학습 요청
	MC_TRAINING_TRAIN		= 2002,		///< (S->C)	탤런트 학습 응답	
	MC_TRAINING_UNTRAIN_ALL	= 2003,		///< (S->C)	모든 탤런트 초기화
	
// 버프(BUFF) -------------------------------------------------------------------------------------------
	MC_BUFF_GAIN					= 2101,		///< (S->C)	버프 효과 얻음 [최적화]
	MC_BUFF_LOST_REQ				= 2102,		///< (C->S)	버프 효과 잃음 요청 [최적화]
	MC_BUFF_LOST					= 2103,		///< (S->C)	버프 효과 잃음 [최적화]
	MC_BUFF_HEAL					= 2104,		///< (S->C)	버프 회복효과

	MC_BUFF_INSTANT_EFFECT_GAIN		= 2105,		///< (S->C) 버프의 인스턴트 효과를 얻음 [최적화]
	MC_TALENT_INSTANT_EFFECT_GAIN	= 2106,		///< (S->C)	탤런트의 인스턴트 효과를 얻음 [최적화]
	MC_BUFF_STACK_INCREASE			= 2107,		///< (S->C) 버프 스택 추가
	MC_BUFF_STACK_DECREASE			= 2108,		///< (S->C) 버프 스택 제거

// 트레이드(TRADE) --------------------------------------------------------------------------------------
	MC_TRADE_START_REQ				= 2201,		///< (C->S) 거래 시작 요청
	MC_TRADE_START_REQ_RELAY		= 2202,		///< (S->C) 거래 시작 요청 전달
	MC_TRADE_START_ACCEPT_REQ		= 2203,		///< (C->S) 거래 시작 응답 요청	
	MC_TRADE_START					= 2204,		///< (S->C) 거래 시작 요청 응답
	MC_TRADE_START_REJECT			= 2205,		///< (S->C) 거래 시작 요청 거절

	MC_TRADE_PUTUP_ITEM_REQ			= 2211,		///< (C->S) 거래창에 아이템 올리기요청
	MC_TRADE_PUTUP_ITEM				= 2212,		///< (S->C) 거래창에 아이템 올리기응답
	MC_TRADE_PUTDOWN_ITEM_REQ		= 2213,		///< (C->S) 거래창에서 아이템 내리기 요청
	MC_TRADE_PUTDOWN_ITEM			= 2214,		///< (S->C) 거래창에서 아이템 내리기 응답

	MC_TRADE_PUTUP_MONEY_REQ		= 2221,		///< (C->S) 거래창에 실버 올리기 요청
	MC_TRADE_PUTUP_MONEY			= 2222,		///< (S->C) 거래창에 실버 올리기 응답 시작

	MC_TRADE_CANCEL_REQ				= 2231,		///< (C->S) 거래 취소 요청
	MC_TRADE_CANCEL					= 2232,		///< (S->C) 거래 취소 요청 응답
	MC_TRADE_CONFIRM_REQ			= 2233,		///< (C->S) 거래 확인 요청
	MC_TRADE_CONFIRM				= 2234,		///< (S->C) 거래 확인 요청 응답
	MC_TRADE_COMPLETE				= 2235,		///< (S->C) 거래 완료

// 파티(PARTY) ------------------------------------------------------------------------------------------
	MC_PARTY_INVITE_REQ					= 2253,		///< (C->S) 파티 초대
	MC_PARTY_INVITE						= 2254,		///< (S->C) 파티 초대 응답
	MC_PARTY_LEAVE_REQ					= 2255,		///< (C->S) 파티 떠남
	MC_PARTY_KICK_REQ					= 2257,		///< (C->S) 파티 추방
	MC_PARTY_INVITE_QUESTION			= 2261,		///< (S->C) 파티 초대 물음
	MC_PARTY_INVITE_QUESTION_RESPOND	= 2262,		///< (C->S) 파티 초대 물음 응답
	MC_PARTY_NOTIFY_JOIN				= 2263,		///< (S->C) 참가 알림
	MC_PARTY_NOTIFY_LEAVE				= 2264,		///< (S->C) 떠남 알림
	MC_PARTY_REFRESH_INFO_ALL			= 2265,		///< (S->C) 파티의 모든 정보
	MC_PARTY_REFRESH_INFO				= 2266,		///< (S->C) 파티 Player 정보
	MC_PARTY_INVITE_FOR_ME_REQ			= 2267,		///< (C->S) 파티장에게 가입 요청
	MC_PARTY_INVITE_FOR_ME_QUESTION		= 2268,		///< (S->C) 파티장에게 가입 요청 물음
	MC_PARTY_INVITE_FOR_ME_QUESTION_RESPOND	= 2269,	///< (C->S) 가입 요청 물음 응답

	MC_PARTY_ACCEPT_CANCEL				= 2270,		///< (S->C)	가입 수락 응답 취소

	MC_PARTY_CHANGE_NAME_REQ			= 2273,		///< (C->S) 파티 이름 변경 요청
	MC_PARTY_CHANGE_NAME				= 2274,		///< (S->C) 파티 이름 변경
	MC_PARTY_CHANGE_LEADER_REQ			= 2275,		///< (C->S) 파티장 변경 요청
	MC_PARTY_CHANGE_LEADER				= 2276,		///< (S->C) 파티장 변경
	MC_PARTY_CHANGE_LOOTING_RULE_REQ	= 2277,		///< (C->S) 파티 루팅룰 변경 요청	[최적화]
	MC_PARTY_CHANGE_LOOTING_RULE		= 2278,		///< (S->C) 파티 루팅룰 변경		[최적화]
	MC_PARTY_CHANGE_QUESTID_REQ			= 2279,		///< (C->S) 파티 수행퀘스트 변경 요청
	MC_PARTY_CHANGE_QUESTID				= 2280,		///< (S->C) 파티 수행퀘스트 변경
	MC_PARTY_CHANGE_LEADER_NOTIFY		= 2281,		///< (S->C) 파티장 변경 알림


// 엔피씨 상점(NPC SHOP)
	MC_NPCSHOP_SHOW						= 2351,		///< (S->C) 구매할수 있는 아이템 목록 응답
	MC_NPCSHOP_BUY_REQ					= 2352,		///< (C->S)	구매 요청
	MC_NPCSHOP_BUY						= 2353,		///< (S->C)	구매 응답
	MC_NPCSHOP_SELL_REQ					= 2354,		///< (C->S)	판매 요청
	MC_NPCSHOP_SELL						= 2355,		///< (S->C)	판매 응답
	MC_NPCSHOP_SELL_VERYCOMMON_REQ		= 2356,		///< (C->S)	매우 흔함 아이템 판매 요청
	MC_NPCSHOP_SELL_VERYCOMMON			= 2357,		///< (S->C)	매우 흔함 아이템 판매 응답
	MC_NPCSHOP_REPAIR_ONE_REQ			= 2358,		///< (C->S)	수리 한개 요청
	MC_NPCSHOP_REPAIR_ALL_REQ			= 2359,		///< (C->S)	수리 모두 요청
	MC_NPCSHOP_REPAIR					= 2360,		///< (S->C)	수리 응답	

// 루팅(LOOTING) ----------------------------------------------------------------------------------------	
	MC_LOOT_BEGIN_REQ				= 2401,		///< (C->S) 루팅 시작 요청
	MC_LOOT_BEGIN					= 2402,		///< (S->C) 루팅 시작 응답

	MC_LOOT_SEE_ITEM_REQ			= 2403,		///< (C->S) 아이템 목록 요청
	MC_LOOT_SEE_ITEM				= 2404,		///< (S->C) 아이템 목록 응답

	MC_LOOT_LOOT_ITEM_REQ			= 2405,		///< (C->S) Gettable : 획득처리, Rollable : 주사위굴림, MasterLootable : UI 및 NOTIFY 처리
	MC_LOOT_ROLLRESULT				= 2406,		///< (S->C) 주사위 굴림
	MC_LOOT_MASTERLOOT_UI			= 2407,		///< (S->C) 마스터룻 UI
	MC_LOOT_MASTERLOOT_NOTIFY		= 2408,		///< (S->C) 마스터룻 공지	
	
	MC_LOOT_MASTERLOOT_ITEM_REQ		= 2410,		///< (C->S) 아이템 마스터룻 요청
	MC_LOOT_MASTERLOOT_UI_REMOVE	= 2414,		///< (S->C) 마스터룻UI 제거(시체가 사라졌을때 사용)

	MC_LOOT_REFRESH_ITEM			= 2415,		///< (S->C) 루팅 가능한 아이템 목록 갱신
	MC_LOOT_OTHER_GAIN_ITEM			= 2416,		///< (S->C) 다른 보상자가 아이템 획득

	MC_LOOT_GETTABLE_ITEM_ADD		= 2417,		///< (S->C) 획득 가능한 아이템이 추가됨(다른 필드에 드랍

	MC_LOOT_END_REQ					= 2418,		///< (C->S) 루팅 완료 요청
	MC_LOOT_END						= 2419,		///< (S->C) 루팅 완료 응답

	MC_LOOT_INSERT_LOOTABLE_NPC		= 2420,		///< (S->C) 루팅 가능하게 된 NPC 목록
	MC_LOOT_REMOVE_LOOTABLE_NPC		= 2421,		///< (S->C) 루팅 불가능하게 된 NPC 목록
	

// 인터랙션(INTERACTION) ----------------------------------------------------------------------------------------
	MC_INTERACTION_INTERACTION_REQ	= 2501,		///< (C->S) 인터랙션 요청

// 인터랙션(NPC INTERACTION) ----------------------------------------------------------------------------------------
	MC_NPCINTERACTION_INTERACTION	= 2502,		///< (S->C) 인터랙션 응답
	MC_NPCINTERACTION_IELEMENT_REQ	= 2503,		///< (C->S) 인터랙션 엘리먼트 요청
	MC_NPCINTERACTION_END_REQ		= 2504,		///< (C->S) 인터랙션 종료 요청
	MC_NPCINTERACTION_END			= 2505,		///< (S->C) 인터랙션 종료 응답

	MC_NPCINTERACTION_ICON			= 2507,		///< (S->C) NPC 아이콘 응답
	MC_NPCINTERACTION_LOOT_START	= 2508,		///< (S->C) 인터랙션으로 루팅 시작	
	

// 미니맵, 월드맵 ------------------------------------------------------------------------------------------
	MC_MINIMAP_MARK_POS_REQ			= 2601,		///< (C->S) 미니맵 클릭좌표 알림
	MC_MINIMAP_MARK_POS				= 2602,		///< (S->C) 미니맵 클릭좌표 파티원들에게 알림


// 피로도 -------------------------------------------------------------------------------------------------
	MC_FATIGUE_UPDATE				= 2701,		///< (S->C) 피로도 변경

// 크래프트 -------------------------------------------------------------------------------------------------
	MC_CRAFT_SHOW				= 2801,		///< (S->C) 크래프트 UI 보여주기
	MC_CRAFT_MAKE_REQ			= 2802,		///< (C->S) 크래프팅 시작 요청
	MC_CRAFT_MAKE				= 2803,		///< (S->C) 크래프팅 시작 응답
	MC_CRAFT_RECIPE_INSERT_REQ	= 2804,		///< (C->S) 레시피 추가 요청
	MC_CRAFT_RECIPE_INSERT		= 2805,		///< (S->C) 레시피 추가
	MC_CRAFT_RECIPE_DELETE_REQ	= 2806,		///< (C->S) 레시피 삭제 요청
	MC_CRAFT_RECIPE_DELETE		= 2807,		///< (S->C) 레시피 삭제

// 여관 ------------------------------------------------------------------------------------------------
	MC_INN_BEGIN_SLEEP				= 2903,		///< (S->C) 여관방 잠자기 시작
	MC_INN_END_SLEEP				= 2904,		///< (C->S) 여관방 잠자기 종료 요청

// 길드 ------------------------------------------------------------------------------------------------
	MC_GUILD_SHOW_CREATE_UI			= 3001,		///< (S->C) 길드 생성 UI
	MC_GUILD_SHOW_DESTROY_UI		= 3002,		///< (S->C) 길드 생성 UI

	MC_GUILD_CREATE_REQ				= 3003,		///< (C->S) 길드 생성 요청
	MC_GUILD_CREATE					= 3004,		///< (S->C) 길드 생성

	MC_GUILD_INVITE_REQ				= 3005,		///< (C->S) 길드 초대 요청
	MC_GUILD_INVITE_NOTIFY			= 3006,		///< (S->C) 길드 초대 요청 전달
	MC_GUILD_INVITE_RES				= 3007,		///< (C->S) 길드 초대 응답 요청
	MC_GUILD_INVITE_REJECT			= 3008,		///< (S->C) 길드 초대 거절

	MC_GUILD_JOIN_ME				= 3009,		///< (S->C) 내가 길드 가입
	MC_GUILD_JOIN_OTHER				= 3010,		///< (S->C) 다른 사람이 내가 가입한 길드에 가입
	MC_GUILD_JOIN_SECTOR			= 3011,		///< (S->C) 같은 섹터에 있는 사람이 내가 가입하지 않은 길드에 가입

	MC_GUILD_LEAVE_REQ				= 3012,		///< (C->S) 길드 탈퇴 요청
	MC_GUILD_LEAVE					= 3013,		///< (S->C) 길드 탈퇴
	MC_GUILD_LEAVE_SECTOR			= 3014,		///< (S->C) 같은 섹터에 있는 사람이 내가 가입하지 않은 길드에서 탈퇴

	MC_GUILD_DESTROY_REQ			= 3015,		///< (C->S) 길드 해체 요청
	MC_GUILD_DESTROY				= 3016,		///< (S->C) 길드 해체

	MC_GUILD_ONLINE					= 3017,		///< (S->C) 길드원 접속
	MC_GUILD_OFFLINE				= 3018,		///< (S->C) 길드원 종료
	MC_GUILD_MOVEFIELD				= 3019,		///< (S->C) 길드원 필드이동

	MC_GUILD_CHANGE_MASTER_REQ		= 3020,		///< (C->S) 길드 마스터 변경 요청
	MC_GUILD_CHANGE_MASTER			= 3021,		///< (S->C) 길드 마스터 변경

	MC_GUILD_INFO					= 3022,		///< (S->C) 디비와 동기화된 길드 정보 전달.

	MC_GUILD_KICK_REQ				= 3023,		///< (C->S) 길드 강퇴 요청
	MC_GUILD_KICK					= 3024,		///< (S->C) 길드 강퇴

	MC_GUILD_SHOW_STORAGE_UI			= 3025,		///< (C->S) 길드 보관함 UI
	MC_GUILD_DEPOSIT_STORAGEMONEY_REQ	= 3026,		///< (C->S) 길드 보관함 입금 요청
	MC_GUILD_DEPOSIT_STORAGEMONEY		= 3027,		///< (S->C) 길드 보관함 입금
	MC_GUILD_WITHDRAW_STORAGEMONEY_REQ	= 3028,		///< (C->S) 길드 보관함 출금 요청
	MC_GUILD_WITHDRAW_STORAGEMONEY		= 3029,		///< (S->C) 길드 보관함 출금
	MC_GUILD_MOVE_STORAGEITEM			= 3030,		///< (S->C) 길드 보관함 아이템 이동

// 결투 ------------------------------------------------------------------------------------------------
	MC_DUEL_REQUEST					= 3101,		///< (C->S) 결투 신청
	MC_DUEL_QUESTION				= 3102,		///< (S->C) 결투 응답요청
	MC_DUEL_QUESTION_RESPOND		= 3103,		///< (S->C) 결투 응답
	MC_DUEL_CANCEL					= 3104,		///< (S->C) 결투 취소
	MC_DUEL_READY					= 3105,		///< (S->C) 결투 준비
	MC_DUEL_FIGHT					= 3106,		///< (S->C) 결투 시작
	MC_DUEL_FINISHED				= 3107,		///< (S->C) 결투 종료
	MC_DUEL_PARTY_READY				= 3108,		///< (S->C) 파티 결투 준비
	MC_DUEL_PARTY_FIGHT				= 3109,		///< (S->C) 파티 결투 시작
	MC_DUEL_PARTY_CANCEL			= 3110,		///< (S->C) 파티 결투 취소
	MC_DUEL_PARTY_FINISHED			= 3111,		///< (S->C) 파티 결투 종료
	MC_DUEL_DEFEATED				= 3112,		///< (S->C) 결투 패배자 알림
	MC_DUEL_PARTY_REQUEST			= 3113,		///< (C->S) 파티 결투 신청
	MC_DUEL_READY_COUNT				= 3114,		///< (S->C) 결투 시작 카운트다운

// 투기장 ------------------------------------------------------------------------------------------------
	MC_BATTLEARENA_REGISTER_REQ		= 3151,		///< (C->S) 투기장 신청 요청
	MC_BATTLEARENA_REGISTER			= 3152,		///< (S->C) 투기장 신청 응답
	MC_BATTLEARENA_DEREGISTER_REQ	= 3153,		///< (C->S) 투기장 신청취소 요청
	MC_BATTLEARENA_DEREGISTER		= 3154,		///< (S->C) 투기장 신청취소 응답
	MC_BATTLEARENA_MATCHED			= 3155,		///< (S->C) 투기장 매칭 알림
	MC_BATTLEARENA_COUNTING			= 3156,		///< (S->C) 투기장 게임시작 카운팅 알림
	MC_BATTLEARENA_GAMESTART		= 3157,		///< (S->C) 투기장 게임시작 알림
	MC_BATTLEARENA_READY_DEATHMATCH		= 3158,	///< (S->C) 투기장 데스매치용 준비
	MC_BATTLEARENA_SCOREBOARD_DEATHMATCH = 3159,///< (S->C) 투기장 데스매치용 점수판 알림
	MC_BATTLEARENA_REJOIN_DEATHMATCH = 3160,	///< (S->C) 투기장 데스매치용 재접속
	MC_BATTLEARENA_DEATHMATCH_UPDATE = 3161,	///< (S->C) 투기장 데스매치용 업데이트
	MC_BATTLEARENA_FINISHED			= 3162,		///< (S->C) 투기장 게임종료
	MC_BATTLEARENA_DIE				= 3163,		///< (S->C) 투기장 죽음
	MC_BATTLEARENA_REBIRTH_REQ		= 3164,		///< (C->S) 투기장 부활 요청	
	MC_BATTLEARENA_KILL_INFO		= 3165,		///< (S->C) 투기장 킬 정보 알림

// 탤런트포커스
	MC_FOCUS_GAIN					= 3201,		///< (S->C) 탤런트포커스를 얻음
	MC_FOCUS_LOST					= 3202,		///< (S->C) 탤런트포커스를 잃음
	MC_FOCUS_UPDATE_BUBBLE			= 3203,		///< (S->C) 탤런트포커스 버블 갯수 갱신
	

// 팔레트
	MC_PALETTE_SELECT_REQ			= 3301,		///< (C->S) 팔레트 선택 요청
	MC_PALETTE_SELECT				= 3302,		///< (S->C) 팔레트 선택
	MC_PALETTE_SET_PRIMARY_REQ		= 3303,		///< (C->S) 주장비에 팔레트 세팅 요청
	MC_PALETTE_SET_PRIMARY			= 3304,		///< (S->C) 주장비에 팔레트 세팅
	MC_PALETTE_SET_SECONDARY_REQ	= 3305,		///< (C->S) 보조 장비에 팔레트 세팅 요청
	MC_PALETTE_SET_SECONDARY		= 3306,		///< (S->C) 보조 장비에 팔레트 세팅
	
	MC_PALETTE_PUTUP_REQ			= 3307,		///< (C->S) 팔레트에 올리기 요청
	MC_PALETTE_PUTUP				= 3308,		///< (S->C) 팔레트에 올리기
	MC_PALETTE_PUTDOWN_REQ			= 3309,		///< (C->S) 팔레트에 내리기 요청
	MC_PALETTE_PUTDOWN				= 3310,		///< (S->C) 팔레트에 내리기	
	MC_PALETTE_CHANGE_REQ			= 3311,		///< (C->S) 팔레트에 올려진것 교환 요청
	MC_PALETTE_CHANGE				= 3312,		///< (S->C) 팔레트에 올려진것 교환
	MC_PALETTE_PUTDOWN_ALLTALENT	= 3313,		///< (S->C) 팔레트에 올려진 탤런트 모두 내리기

 // 소울
	MC_SOUL_DRAIN_ME				= 3401,		///< (S->C) 소울 흡수(본인)
	MC_SOUL_DRAIN_OTHER				= 3402,		///< (S->C) 소울 흡수(다른이)	

// 팩션
	MC_FACTION_INCREASE				= 3501,		///< (S->C) 팩션수치가 올라감
	MC_FACTION_DECREASE				= 3502,		///< (S->C) 팩션수치가 내려감

// 컷씬
	MC_CUTSCENE_BEGIN_REQ			= 3601,		///< (C->S) 컷신 시작 요청
	MC_CUTSCENE_BEGIN_ME			= 3602,		///< (S->C) 컷신 내가 시작
	MC_CUTSCENE_BEGIN_OTHER			= 3603,		///< (S->C) 컷신 다른이가 시작
	MC_CUTSCENE_END_REQ				= 3604,		///< (C->S) 컷신 종료 요청
	MC_CUTSCENE_END_ME				= 3605,		///< (S->C) 컷신 내가 종료
	MC_CUTSCENE_END_OTHER			= 3606,		///< (S->C) 컷신 다른이가 종료

// 환경
	MC_ENV_PLAY_BGM					= 3700,		///< (S->C) BGM 재생

// 우편
	MC_MAIL_MAILBOX_IS_OPENED			= 3800,	///< (S->C) 편지함 인터렉션 응답 (with 편지함 목록)
	MC_MAIL_MAILBOX_IS_CLOSED,					///< (S->C) 편지함 인터렉션 종료 알림
	MC_MAIL_POST_REQ,							///< (C->S) 편지 쓰기 요청
	MC_MAIL_POST_RES,							///< (S->C) 편지 쓰기 응답
	MC_MAIL_READ_REQ,							///< (C->S) 편지 읽기 요청
	MC_MAIL_READ_RES,							///< (S->C) 편지 읽기 응답
	MC_MAIL_DELETE_REQ,							///< (C->S) 편지 삭제 요청
	MC_MAIL_DELETE_RES,							///< (S->C) 편지 삭제 응답
	MC_MAIL_TAKE_APPENDED_ITEM_REQ,				///< (C->S) 특정 슬롯의 편지 첨부 물품 취득 요청
	MC_MAIL_TAKE_APPENDED_ITEM_RES,				///< (S->C) 특정 슬롯의 편지 첨부 물품 취득 응답
	MC_MAIL_TAKE_APPENDED_MONEY_REQ,			///< (C->S) 첨부된 돈 취득 요청
	MC_MAIL_TAKE_APPENDED_MONEY_RES,			///< (S->C) 첨부된 돈 취득 응답
	MC_MAIL_NOTIFY_MAIL,						///< (S->C) 편지 정보 갱신 알림 알림

	MC_MAIL_APPEND_PUTUP_ITEM_REQ,				///< (C->S) 편지 첨부 아이템 올리기 요청
	MC_MAIL_APPEND_PUTUP_ITEM_RES,				///< (S->C) 편지 첨부 아이템 올리기 응답
	MC_MAIL_APPEND_PUTDOWN_ITEM_REQ,			///< (C->S) 편지 첨부 아이템 내리기 요청
	MC_MAIL_APPEND_PUTDOWN_ITEM_RES,			///< (S->C) 편지 첨부 아이템 내리기 응답	

	MC_MAIL_APPEND_RESET_ITEM_REQ,				///< (C->S)	편지 첨부 아이템 모두 내리기 (or 편지쓰기 취소)

// 강화
	MC_ENCHANT_CHECK_REQ			= 3900,							///< (C->S) 강화 가능검사 요청
	MC_ENCHANT_CHECK_RESULT			= 3901,							///< (S->C) 강화 가능검사 응답
	MC_ENCHANT_REQ					= 3902,							///< (C->S) 강화 요청
	MC_ENCHANT_SUCCESS				= 3903,							///< (S->C) 강화 완료
	MC_ENCHANT_BROKEN				= 3904,							///< (S->C) 강화 실패
	MC_ENCHANT_PREPARE				= 3905,							///< (C->S) 강화 준비
	MC_ENCHANT_CANCEL				= 3906,							///< (C->S) 강화 취소

//SoulHunterZ - Item Soul Attunement
	MC_SH_SA_REQ					= 3950,							///< (C->S)
	MC_SH_SA_RESULT					= 3951,							///< (S->C)
	MC_SH_SA_PREPARE				= 3952,
	MC_SH_SA_CANCEL					= 3953,
	MC_SH_SA_CHECK_REQ				= 3954,
	MC_SH_SA_CHECK_RESULT			= 3955,
	MC_SH_ITEM_XP					= 3956,							///< (S->C)

// 자동파티
	MC_AUTOPARTY_ENQUEUE_REQ		= 4000,							///< (C->S) 자동파티 참여 요청
	MC_AUTOPARTY_INVITE				= 4001,							///< (S->C) 자동파티 결성 완료
	MC_AUTOPARTY_INVITE_RES			= 4002,							///< (C->S) 자동파티 결성 응답
	MC_AUTOPARTY_INVITE_FAIL		= 4003,							///< (S->C) 자동파티 결성 실패
	MC_AUTOPARTY_DEQUEUE_REQ		= 4004,							///< (C->S) 자동파티 불참 요청
	MC_AUTOPARTY_CHECK_SENSOR_REQ	= 4005,							///< (S->C) 자동파티 센서 검사 요청
	MC_AUTOPARTY_CHECK_SENSOR		= 4006,							///< (C->S) 자동파티 센서 검사
	MC_AUTOPARTY_CHANGE_STATE_REQ	= 4007,							///< (C->S) 자동파티 상태변경 요청
	MC_AUTOPARTY_CHANGE_STATE		= 4008,							///< (S->C) 자동파티 상태변경

// 퀘스트PVP
	MC_QUESTPVP_TEAMCOUNT			= 4100,							///< (S->C) 팀인원수
	MC_QUESTPVP_ENTER_REQ			= 4101,							///< (C->S) QPVP지역 입장 요청
	MC_QUESTPVP_ENTER				= 4102,							///< (S->C) QPVP지역 입장
	MC_QUESTPVP_BEGIN_EVENT			= 4103,							///< (S->C) QPVP Event 시작
	MC_QUESTPVP_END_EVENT			= 4104,							///< (S->C) QPVP Event 종료
	MC_QUESTPVP_MARK_NPCPOS			= 4105,							///< (S->C) 맵에 NPC의 현재위치를 표시
	MC_QUESTPVP_UNMARK_NPCPOS		= 4106,							///< (S->C) 맵에 표시한 NPC의 위치를 제거

// 보관함
	MC_STORAGE_SHOW_UI				= 4201,							///< (S->C) 보관함 UI(디비와 동기화된 보관함 정보 포함)
	MC_STORAGE_DEPOSIT_MONEY_REQ	= 4202,							///< (C->S) 입금 요청 
	MC_STORAGE_DEPOSIT_MONEY		= 4203,							///< (S->C) 입금
	MC_STORAGE_WITHDRAW_MONEY_REQ	= 4204,							///< (C->S) 출금 요청 
	MC_STORAGE_WITHDRAW_MONEY		= 4205,							///< (S->C) 출금

// 물품거래소
	MC_SC_TRADEMARKET_EXCEPTION		= 4300,							///< (S->C) 예외

	MC_CS_TRADEMARKET_PUT_ON		= 4301,							///< (C->S) 물건을 등록한다
	MC_SC_TRADEMARKET_PUTTED_ON		= 4302,							///< (S->C) 물건이 등록되었다

	MC_CS_TRADEMARKET_BUY			= 4303,							///< (C->S) 물건을 구매한다
	MC_SC_TRADEMARKET_BOUGHT		= 4304,							///< (S->C) 물건을 구매하였다

	MC_CS_TRADEMARKET_SEARCH		= 4305,							///< (C->S) 물건을 검색한다
	MC_CS_TRADEMARKET_SEARCH_NEXT	= 4306,							///< (C->S) 마지막 검색의 다음 페이지를 요청
	MC_CS_TRADEMARKET_REFRESH		= 4307,							///< (C->S) 검색 결과를 최신으로 갱신한다
	MC_SC_TRADEMARKET_ITEM_LIST		= 4308,							///< (S->C) 검색 결과를 받았다

	MC_CS_TRADEMARKET_GET_MYLIST	= 4309,							///< (C->S) 내 거래소 아이템 목록을 얻는다
	MC_SC_TRADEMARKET_MYLIST		= 4310,							///< (S->C) 내 거래소 아이템 목록

// 관리(GM) ------------------------------------------------------------------------------------------
	MC_GM_REQUEST_SPAWN				= 9001,		///< (C->S) NPC 스폰 요청
	MC_GM_REQUEST_DESPAWN			= 9002,		///< (C->S) NPC 디스폰 요청
	MC_GM_MOVE_REQ					= 9003,		///< (C->S) GM 이동 요청
	MC_GM_MOVE_TO_PLAYER_REQ		= 9004,		///< (C->S) 특정 PC에게로 이동 요청
	MC_GM_MOVE_TO_MYSPOT_REQ		= 9005,		///< (C->S) 내 원 위치로 이동 요청(현재는 처음 스폰된 곳)
	MC_GM_REBIRTH_REQ				= 9006,		///< (C->S) GM 부활 요청
	MC_GM_GOD_REQ					= 9007,		///< (C->S) GM 무적 요청
	MC_GM_CHANGE_WEATHER_REQ		= 9008,		///< (C->S) 날씨 변경 요청
	MC_GM_CHANGE_TIME_REQ			= 9009,		///< (C->S) 시간 변경 요청
	MC_GM_QUEST_GIVE_REQ			= 9010,		///< (C->S) 퀘스트 제공을 요청
	MC_GM_ITEM_GIVE_REQ				= 9011,		///< (C->S) 아이템 제공 요청
	MC_GM_GOD						= 9012,		///< (S->C) GM 무적 응답
	MC_GM_AI_RUNNING_REQ			= 9013,		///< (C->S) AI 동작안하게 해주세요.
	MC_GM_AI_USE_TALENT_REQ			= 9014,		///< (C->S) 해당 NPC에게 특정 탤런트 사용하라고 명령
	MC_GM_AI_SET_MONITOR_TARGET		= 9015,		///< (C->S) 테스트 용도로 모니터링할 타겟 NPC 설정
	MC_GM_GET_PLAYERUID_REQ			= 9016,		///< (C->S) 플레이어 ID로 UID 요청
	MC_GM_GET_PLAYERUID				= 9017,		///< (S->C) 플레이어 ID로 UID 요청 응답
	MC_GM_REPORT_TARGET_ENTITY		= 9020,		///< (C->S) NPC Report
	MC_GM_KILL_ENTITY_REQ			= 9021,		///< (C->S) 엔터티 죽이기 요청
	MC_GM_RANGE_KILL_NPC_REQ		= 9022,		///< (C->S) 범위로 NPC 죽이기 요청
	MC_GM_SET_ME_REQ				= 9026,		///< (C->S) 내 정보 변경 요청
	MC_GM_QUEST_RESET_REQ			= 9027,		///< (C->S) 수행한 퀘스트 초기화 요청
	MC_GM_QUEST_RESET				= 9028,		///< (S->C) 수행한 퀘스트 초기화	
	MC_GM_SET_ITEM_REQ				= 9029,		///< (C->S) 아이템 정보 변경 요청
	MC_GM_NPC_AI_RUNNING_REQ		= 9030,		///< (C->S) 특정 NPC AI 토글 요청		
	MC_GM_INSERT_BUFF_REQ			= 9035,		///< (C->S)	버프 추가 요청
	MC_GM_DELETE_BUFF_REQ			= 9036,		///< (C->S)	버프 제거 요청
	MC_GM_RESET_COOLTIME_REQ		= 9037,		///< (C->S)	버프 제거 요청
	MC_GM_SHOW_ENEMY_UID_REQ		= 9038,		///< (C->S)	어그로를 가진 NPC UID 요청
	MC_GM_SHOW_ENEMY_UID			= 9039,		///< (S->C)	어그로를 가진 NPC UID
	MC_GM_MOVE_TO_NPC_REQ			= 9040,		///< (C->S) 특정 NPC에게로 이동 요청
	MC_GM_BREAK_PART_REQ			= 9041,		///< (C->S) 특정 NPC의 특정 bpart 부시기
	MC_GM_RANGE_BREAK_PART_REQ		= 9042,		///< (C->S) 주변 NPC의 모든 bpart 부시기
	MC_GM_QUEST_COMPLETE_REQ		= 9043,		///< (C->S) 퀘스트 완료
	MC_GM_QUEST_VAR_REQ				= 9044,		///< (C->S) quest_var 요청
	MC_GM_QUEST_VAR					= 9045,		///< (C->S) quest_var
	MC_GM_QUEST_FAIL_REQ			= 9046,		///< (C->S) 퀘스트 실패 요청
	MC_GM_CLEAR_INVENTORY_REQ		= 9047,		///< (C->S) 인벤토리 아이템 모두 제거
	MC_GM_QUEST_REWARD_REQ			= 9048,		///< (C->S) 퀘스트 보상받기

	MC_GM_LOG_CRT_INSERT_REQ		= 9050,		///< (C->S) FailCRT 로그 남기기 추가
	MC_GM_LOG_CRT_DELETE_REQ		= 9051,		///< (C->S) FailCRT 로그 남기기 제거
	MC_GM_LOG_CRT_LIST				= 9052,		///< (S->C) FailCRT 로그 남기는 목록
	MC_GM_QUERY_MULTILOGIN_REQ		= 9053,		///< (C->S) 멀티로그인 질의 요청
	MC_GM_QUERY_MULTILOGIN			= 9054,		///< (S->C) 멀티로그인 질의 응답


	MC_GM_FACTION_INCREASE_REQ		= 9060,		///< (C->S) 팩션 증가
	MC_GM_FACTION_DECREASE_REQ		= 9061,		///< (C->S) 팩션 감소
	MC_GM_SERVER_DUMP_REQ			= 9062,		///< (C->S) 서버 덤프 요청
	MC_GM_SERVER_DUMP_RESP			= 9063,		///< (C->S) 서버 덤프 응답
	MC_GM_GHOST_REQ					= 9064,		///< (C->S) GM 고스트 요청
	MC_GM_GHOST						= 9065,		///< (S->C) GM 고스트

	MC_GM_SPAWN						= 9066,		///< (S->C) GM NPC 스폰 처리 결과
	MC_GM_QUEST_RESET_ALL_REQ		= 9067,		///< (C->S) 수행한 퀘스트 및 가지고 있는 퀘스트 초기화 요청

	MC_GM_ENABLE_ICHECK_REQ			= 9068,		///< (C->S) 인터랙션 체크 켜기
	MC_GM_DISABLE_ICHECK_REQ		= 9069,		///< (C->S) 인터랙션 체크 끄기



	MC_GM_SET_NPC_REQ				= 9071,		///< (C->S) NPC의 상태값 변경 요청

	MC_GM_DYE_REQ					= 9072,		///< (C->S) 염색 요청

	MC_GM_SUMMON_REQ				= 9073,		///< (S->C) 특정 PC를 소환

	MC_GM_FACTION_RESET_REQ			= 9074,		///< (C->S) 팩션 초기화
	MC_GM_REGEN_REQ					= 9075,		///< (C->S) 생명력/정신력/기력[자동회복] 토글 요청
	MC_GM_DESPAWNBYID_REQ			= 9076,		///< (C->S) NPCID로 디스폰

	MC_GM_CHANGE_SERVER_MODE_REQ	= 9077,		///< (C->S) 서버 모드 변경 요청
	MC_GM_CHANGE_SERVER_MODE		= 9078,		///< (S->C) 서버 모드 변경

	MC_GM_RESET_EXPO_CHARACTERS_REQ	= 9079,		///< (C->S) Expo 용 캐릭터 Reset

// 보안(GameGuard) ------------------------------------------------------------------------------------------
	MC_GG_AUTH_REQ					= 9080,		///< (S->C) 주기적 인증 요청
	MC_GG_AUTH_RESPOND				= 9081,		///< (C->S) 주기적 인증 응답
	MC_GG_AUTH_ENABLE				= 9082,		///< (C->S) 인증 켤지 끌지 여부

// 테스트(TEST) -----------------------------------------------------------------------------------------
	MC_DEBUG_STRING					= 9103,		///< (S->C) 디버그 스트링을 클라이언트로 보내준다.
	MC_REQUEST_DEBUG_STRING			= 9104,		///< (C->S) 디버그 스트링을 서버로 보내준다.
	MC_AI_DEBUG_MESSAGE				= 9105,		///< (S->C) AI 디버그 정보를 클라이언트로 보내준다.
	MC_DEBUG_COMBATCALC				= 9106,		///< (S->C) 전투 공식 디버그 정보를 클라이언트로 보내준다.

	MC_DEBUG_START_COMMAND_PROFILE	= 9107,		///< (C->S) 커맨드 프로파일링 시작
	MC_DEBUG_DUMP_COMMAND_PROFILE	= 9108,		///< (C->S) 커맨드 프로파일 덤프
	MC_DEBUG_END_COMMAND_PROFILE	= 9109,		///< (C->S) 커맨드 프로파일링 끝

	MC_DEBUG_NPC_NETLOG				= 9110,		///< (S->C) 서버의 NPC 로깅 정보를 알려주기
	
	MC_DEBUG_ECHO_REQ				= 9111,		///< (C->S) 에코 요청
	MC_DEBUG_ECHO					= 9112,		///< (S->C) 에코 응답

// 공통(COMMON) -----------------------------------------------------------------------------------------
	MC_TIME_SYNCH					= 9200,		///< (S->C) 서버 로컬 타임을 클라에게 동기화 시키기 [최적화]

	MC_GM_BAN						= 11000,	/// (C->S) Ban Request

	
};



// 서버,클라이언트 모두 공유하는 커맨드 등록
CSCOMMON_API void AddCommonCommandTable();


namespace CCommandVersionChecker
{
	CSCOMMON_API bool IsValid(int nCommandVersion);
}

#endif
