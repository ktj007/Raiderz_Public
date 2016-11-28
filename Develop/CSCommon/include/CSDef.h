#pragma once

#include <tchar.h>


#define GAME_NAME_STR						_T("Raiderz")
#define GAME_NAME_STR_LOWER					_T("raiderz")

// 게임 로직 -----------------------------------------------------------------
#define GAME_TARGET_NPC_CHECK_DISTANCE		300.0f		// NPC 탐색 거리
#define ENTITY_KILL_CONTRIBUTE_DISTANCE		20000.0f	// 엔터티를 죽이는데 같이 기여했다고 인정되는 파티원의 거리
#define GAME_INTERACTION_DISTANCE_Z			400.0f		// 인터랙션을 인정하는 Z축 거리
#define GAME_INTERACTION_DISTANCE_XY_MULTIPLICATION	1.3f		// 인터랙션을 인정하는 XY축 거리를 구할때 사용하는 곱셈값
#define GAME_INTERACTION_DISTANCE_XY_ADDITION		200.0f		// 인터랙션을 인정하는 XY축 거리를 구할때 사용하는 덧셈값
#define GAME_INTERACTION_SERVER_TOLER_DISTANCE_XY	500.0f		// 서버에서는 인터랙션 가능거리에서 이거리까지 더해서 인터랙션 거리를 허용한다.
#define GAME_INTERACTION_BPART_LOOTING_DISTANCE		1000.0f		// BPart 루팅 인터렉션 거리(10미터)


// NPC 상점에서 우호도에 따른 Buy/Sell Modulate
#define NPC_SHOP_BUY_MOD_FACTION_GOOD				0.98f
#define NPC_SHOP_BUY_MOD_FACTION_EXCELLENT			0.95f
#define NPC_SHOP_SELL_MOD_FACTION_GOOD				1.02f
#define NPC_SHOP_SELL_MOD_FACTION_EXCELLENT			1.05f
#define NPC_SHOP_REPAIR_MOD_FACTION_GOOD			0.98f
#define NPC_SHOP_REPAIR_MOD_FACTION_EXCELLENT		0.95f

// 크래프트에서 우호도에 따른 Make Modulate
#define CRAFT_MAKE_MOD_FACTION_GOOD					0.98f
#define CRAFT_MAKE_MOD_FACTION_EXCELLENT			0.95f

// 상수
//
#define MAX_ACCOUNT_CHAR_COUNT		5					// 계정당 최대 4개의 캐릭터를 가질 수 있다.

#define ACCOUNT_NAME_LEN	32							// 계정 이름 최대 길이
#define PLAYER_NAME_LEN		32							// 플레이어 이름 최대 길이
#define PLAYER_SUR_NAME_LEN	32							// 플레이어 성 최대 길이

#define PLAYER_DEFAULT_TP	0							// 캐릭터 생성할 때 기본적으로 제공하는 TP
#define PLAYER_DEFAULT_SOULBINDINGID	109				// 캐릭터 생성시 기본 소울바인딩 ID
#define PLAYER_DEFAULT_INNROOMID		1				// 캐릭터 생성시 기본 여관방 ID
#define PLAYER_DEFAULT_PRIPALETTE		5				// 캐릭터 생성시 주무기에 바인딩된 팔레트 ID
#define PLAYER_DEFAULT_SECPALETTE		5				// 캐릭터 생성시 보조무기에 바인딩된 팔레트 ID

#define GUILD_NAME_LEN		32							// 길드 이름 최대 길이


#define SERVER_NAME_LEN		64							// 서버 이름 최대 길이

#define WORLD_NAME_LEN		32							// 월드 이름 최대 길이
#define IP_STRING_LEN		64							// IP 길이 "###.###.###.###"


#define ITEM_NAME_LEN		32							// 아이템 이름 길이
#define SECTOR_SIZE			3000						// 필드 섹터 한변의 길이
#define HUGE_SECTOR_SCALE	4							// 필드 거대 섹터의 일반 섹터 비율
#define MAX_RACE_NUM		1							// 종족수
#define MAX_LEVEL			60							// 최대 레벨은 60
#define MAX_TRADE_ITEM_SPACE		5					// 한번에 거래 가능한 아이템의 개수
#define MAX_TALENT_ADDITION_SIZE	3					// 한 탤런트를 배웠을때 추가적으로 얻는 탤런트의 개수

#define BASE_INVENTORY_SIZE			80					// 기본 인벤토리 크기.
#define MAX_INVENTORY_SIZE			160					// 확장했을때 최대로 커질 수 있는 인벤토리 크기.

#define BASE_STORAGE_SIZE			40					// 기본 보관함 크기.
#define MAX_STORAGE_SIZE			80					// 확장했을때 최대로 커질 수 있는 보관함 크기.

#define BASE_GUILDSTORAGE_SIZE			80				// 기본 길드 보관함 크기.
#define MAX_GUILDSTORAGE_SIZE			160				// 확장했을때 최대로 커질 수 있는 길드 보관함 크기.

#define MAX_SET_ITEM_COUNT			5


#define MAX_CREATION_PLAYER_FACE_MALE				14			// 캐릭터 생성시 고를 수 있는 얼굴 수
#define MAX_CREATION_PLAYER_HAIR_MALE				22			// 캐릭터 생성시 고를 수 있는 머리셋 수
#define MAX_CREATION_PLAYER_HAIR_COLOR_MALE			30
#define MAX_CREATION_PLAYER_SKIN_COLOR_MALE			32
#define MAX_CREATION_PLAYER_EYE_COLOR_MALE			16
#define MAX_CREATION_PLAYER_VOICE_MALE				3			// 캐릭터 생성시 고를 수 있는 목소리 수
#define MAX_CREATION_PLAYER_MAKEUP_MALE				3			// 캐릭터 생성시 고를 수 있는 화장 수
#define MAX_CREATION_PLAYER_TATTO_MALE				50			// 캐릭터 생성시 고를 수 있는 문신 수
#define MAX_CREATION_PLAYER_TATTO_COLOR_MALE		16

#define MAX_CREATION_PLAYER_FACE_FEMALE				14			// 캐릭터 생성시 고를 수 있는 얼굴 수
#define MAX_CREATION_PLAYER_HAIR_FEMALE				22			// 캐릭터 생성시 고를 수 있는 머리셋 수
#define MAX_CREATION_PLAYER_HAIR_COLOR_FEMALE		30
#define MAX_CREATION_PLAYER_SKIN_COLOR_FEMALE		32
#define MAX_CREATION_PLAYER_EYE_COLOR_FEMALE		16
#define MAX_CREATION_PLAYER_VOICE_FEMALE			3			// 캐릭터 생성시 고를 수 있는 목소리 수
#define MAX_CREATION_PLAYER_MAKEUP_FEMALE			3			// 캐릭터 생성시 고를 수 있는 화장 수
#define MAX_CREATION_PLAYER_TATTO_FEMALE			50			// 캐릭터 생성시 고를 수 있는 문신 수
#define MAX_CREATION_PLAYER_TATTO_COLOR_FEMALE		16

#define MAX_CREATION_PLAYER_EQUIPMENT_COLOR			27
#define MIN_CREATION_PLAYER_TATTOO_SCALE			50
#define MAX_CREATION_PLAYER_TATTOO_SCALE			200

#define CONSUME_TP_FOR_LEARN_TALENT			1			// 탤런트 배우기 위해 필요한 TP

#define MAX_MONEY 400000000								// 최대 돈 소유액

#define MAX_DB_TIMESTAMP_STR				32			// 퀘스트 받은시간, 만료시간 스트링 변환 최대 길이. MDB_TIMESTAMP_STR_LEN값과 같아야 한다.


#define DEFAULT_ENTITY_RADIUS		40.0f				// 오브젝트 기본 크기(플레이어 기준)

#define GRAVITY						(-1400.0f)			// 중력
#define UP_VECTOR					vec3(0,0,1)

#define MAX_STRINGID_LEN	1024	//현재 생짜데이타를 보내서 크기를 늘임			

#define MAX_TARGET_NUMBER	15

#define INVALID_ID				-1
#define GUARD_TALENT_ID			INVALID_ID
#define INVALID_BUFF_ID			INVALID_ID
#define INVALID_TALENT_ID		INVALID_ID
#define INVALID_SPAWN_GROUP_ID	0

#define BUFF_DURATION_INFINITY	(-1.0f)

#define MAX_BREAKABLE_PARTS		4						// 한 NPC에 최대 브레이커블 파츠는 4개로 제한한다.

#define MAX_PARTY_MEMBER_COUNT						5			///<< 최대 파티원 수
#define PARTY_INVITEE_QUESTION_TIMEOUT				30.f		// sec
#define PARTY_PUBLIC_PARTY_LIST_COUNT_PER_ONE_PAGE	13

#define HUGE_NPC_RADIUS		(300.0f)


#define NO_LIMIT_TIME 0

#define DEFAULT_CHANNEL_COUNT	2					///< 정적 필드의 기본 채널 수
#define	CHANNEL_PLAYER_LIMIT	200					///< 한 채널에 입장 가능한 인원 수
#define PRIME_CHANNELID			1					///< 최초 채널 ID
#define INVALID_CHANNELID		-1					///< 잘못된 채널 ID값
#define MAX_CHANNEL_SIZE		200					///< 최대 채널 개수
typedef int CHANNELID;

#define GAME_AUTO_REBIRTH_CHECK_TICK	300.f //5분

#define DEFAULT_SOULBINDING_ID 1

#define MAX_OWN_BUFF_NUMBER						16	// 최대 버프에 걸릴수 있는 수

#define DUEL_FLAG_NPCID		20002

#define TARGET_INFO_REFRESH_TIME	3000.f // sec

#define MAX_ENTERABLE_PRIVATE_CHAT_CHANNEL_COUNT	3	// 최대 입장 가능한 채팅 채널 개수
#define MAX_INTERACTABLE_SENSOR_COUNT				8	// 동시에 인터렉션 가능한 센서 최대 개수

// 무효한 히트캡슐그룹 아이디
#define INVALID_HITCAPSULEGROUP_ID		(-1)		// 히트 판정을 하지 않는 ID
#define NONE_HITCAPSULEGROUP_ID			(-99)		// 설정이 되지 않은 ID


// 채널이동 준비시간
#define CHANNEL_CHANGE_PREPARE_TIME_MIN		(8)		// sec
#define CHANNEL_CHANGE_PREPARE_TIME_MAX		(10*60)	// sec
#define CHANNEL_CHANGE_PREPARE_TIME			(10)	// sec

#define MAX_CHATTING_CHANNEL_NAME_LEN		64

// 틱
const DWORD SECOND	= 1000;
const DWORD MINUTE	= 60 * SECOND;
const DWORD HOUR	= 60 * MINUTE;
const DWORD DAY		= 24 * HOUR;

enum MAIL_TYPE
{
	MT_PLAYER,		// 플레이어가 보낸 메일
	MT_NPC,			// NPC가 보낸 메일
	MT_GM,			// GM이 보낸 메일
	MT_QPVP,		// QUEST PVP 보상으로 보낸 메일
};

// 전투 지역
#define BATTLE_AREA_OUT_TIME			30			// 전투지역 종료 시간

// NPC는 1미터 위에서 피킹한다.
#define NPC_PICKING_MAP_HEIGHT 100.0f

// 전투 판정시 결과
enum COMBAT_TURN_RESULT
{
	CTR_NORMAL = 0,
	CTR_MISS,
	CTR_RIPOSTE,
	CTR_RESIST_PERFECT,
	CTR_RESIST_PARTIAL,
	CTR_CRITICAL,
	CTR_DRAIN,
	CTR_ZERO_TALENT_DAMAGE,		// 탤런트의 대미지가 0
	CTR_IMMUNE,					// 무효(면역)
	CTR_AVOID,					// 회피(면역)
	CTR_FALLING,
	CTR_DEAD,					// 이 판정으로 죽었음
	CTR_HIT_BPART1,				// 브레이커블 파츠를 피격했음
	CTR_HIT_BPART2,				// 브레이커블 파츠를 피격했음
	CTR_HIT_BPART3,				// 브레이커블 파츠를 피격했음
	CTR_HIT_BPART4,				// 브레이커블 파츠를 피격했음

	// 16비트에 넣어서 패킷을 보내므로 16비트를 넘으면 안됨. - birdkr
};

//피로도
enum FATIGUE_TYPE
{
	FT_NONE = 0,
	FT_TOO_BAD,
	FT_BAD,
	FT_NORMAL,
	FT_GOOD,
	FT_VERY_GOOD,	

	FT_MAX
};

enum AGE_LIMIT
{
	AL_18_UNDER = 0,
	AL_18_OVER
};


// 탤런트포커스 타입
enum TALENT_FOCUS_TYPE
{
	TFT_NONE = -1,
	TFT_ADVENT = 0,		// 강신
	TFT_BERSERK,		// 광포
	TFT_COUNTER,		// 역습
	TFT_ENLIGHTEN,		// 각성
	TFT_PRECISION,		// 저격
	TFT_SNEAK,			// 은닉
	TFT_SIZE,
};


// 탈렌트 포커스 ID 정의
const int TALENT_FOCUS_ID [] = { 12000,  22000,  60501,  0,  0,  0 };

// 버프 포커스 ID 정의
const int BUFF_FOCUS_ID [] = { 1550,  1551,  1552,  1553,  1554,  1561 };


// 효과 소스: 탤런트와 버프가 동일한 효과를 가질때 서로 구분하기 위해 사용
enum EFFECT_SOURCE_TYPE
{
	EST_NONE = 0,
	
	EST_TALENT,
	EST_BUFF,

	EST_MAX
};

// 데미지 타입
enum DAMAGE_ATTRIB
{
	// 무기 데미지
	DA_NONE			= 0,
	DA_SLASH,
	DA_BLUNT,
	DA_PIERCE,
	DA_EXPLOSION,
	DA_INERTIA,

	// 부가적 속성
	DA_HOLY,
	DA_UNHOLY,
	DA_FIRE,
	DA_COLD,
	DA_LIGHTNING,
	DA_POISON,

	DA_MAX
};


// 데미지 타입 문자열
// **주의: 기존의 문자열을 수정하지 마세요.
static const TCHAR* DAMAGE_ATTRIB_NAME[] = 
{
	_T("DA_NONE"),
	_T("DA_SLASH"),
	_T("DA_BLUNT"),
	_T("DA_PIERCE"),
	_T("DA_EXPLOSION"),
	_T("DA_INERTIA"),

	_T("DA_HOLY"),
	_T("DA_UNHOLY"),
	_T("DA_FIRE"),
	_T("DA_COLD"),
	_T("DA_LIGHTNING"),
	_T("DA_POISON"),
};	

#define TALENT_REQUIRED_MOVE_TRUNCATED	0.01f

// 방어 타입
enum GUARD_TYPE
{
	GT_INVALID = -1,
	GT_PARTIAL = 0,
	GT_PERFECT = 1,
	GT_ABSOLUTE = 2,
};

enum GUARD_CRASH_LEVEL
{
	GCL_PARTIAL = 0,
	GCL_PERFECT,
	GCL_ABSOLUTE,
	GCL_ALL,
};

static const char* GUARD_CRASH_LEVEL_STR[] = 
{
	"PARTIAL",
	"PERFECT",
	"ABSOLUTE",
	"ALL",
};	

/// 서버 타입
enum SServerType
{
	SERVER_LOGIN		= 0,		///< 로그인 서버
	SERVER_MASTER		= 1,		///< 마스터 서버
	SERVER_GAME			= 2,		///< 게임 서버
	SERVER_APPLICATION	= 3			///< 어플리케이션 서버
};

/// Applicatoin 모드
enum SAppServerMode
{
	APP_SERVER_CHAT		= 0,		///< 채팅 서버
	APP_SERVER_AUCTION,				///< 거래 서버 - 사용안함
	APP_SERVER_LOG,					///< 로그 서버 - 사용안함
};

/// 서버 모드
///		LoginServer, GameServer, MasterServer 에서 사용.
enum CSServerMode
{
	SERVER_MODE_INVALID		= -1,
	SERVER_MODE_NORMAL		= 0,	///< 일반적인 서버
	SERVER_MODE_EXPO		= 1,	///< 행사 시연용 서버
	SERVER_MODE_MAX			= 2,
};


/// 플레이어나 NPC의 인덱스 ID, UIID는 1부터 시작한다.
typedef unsigned short		UIID;
static const UIID UIID_INVALID = 0;

// NPC 로그 레코드타입
enum NPCLogRecordType
{
	NPCLOG_COMMON,
	NPCLOG_USETALENT,
	NPCLOG_MOVE,
	NPCLOG_COLT,

	///////////////////////////////
	NPCLOG_ALL,
};

static const TCHAR* NPCLOG_RECORDTYPE[] =
{
	_T("Common"),
	_T("UseTalent"),
	_T("Move"),
	_T("Colt"),

	////////////
	_T("All"),
};


/// Field ////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

#define FIELD_ATTR_CELL_SIZE	300 // cm

enum CHANNEL_STATUS
{
	CHANNEL_NORMAL,	 // 원할
	CHANNEL_CROWDED, // 혼잡
	CHANNEL_FULL,	 // 포화
};

// Challenger Quest Unable Type
enum ChallengerQuestUnableType
{
	CQUT_OK, // 받을 수 있음
	CQUT_FAIL_QUEST_CONDITION, // 퀘스트 컨디션 조건 불총족
	CQUT_FAIL_QUEST_NOT_TODAY, // 오늘의 퀘스트가 아님
};

//////////////////////////////////////////////////////////////////////////
/// Chatting

enum CHATTING_CHANNEL_TYPE
{
	CCT_PRIVATE,
	CCT_PARTY,	
	CCT_AUCTION,
	CCT_FIELD,
};

#define CHATTING_LIMIT_SIZE		100		// 체팅 최대길이

//////////////////////////////////////////////////////////////////////////


// 메시지 타입
enum CSMsgType
{
	MT_SAY = 0,			// 대화
	MT_NARRATION,		// 나레이션
	MT_SHOUT,			// 외침
	MT_SHOUT_FAKE,		// 외침(Fake)
	MT_AUCTION,			// 옥션
	MT_ANNOUNCE,		// 안내
	MT_WHISPER,			// 귓말
	MT_PARTY,			// 파티
	MT_GUILD,			// 길드
	MT_CHANNEL,			// 채널
	
	MT_FIELD,			// 필드
	MT_GLOBAL,			// 전체

	MT_NOVICE,			// Beginners Chat
	MT_LFP,				// Looking for Party Chat

	MT_SP_SYSTEM,		// 스~빼셜한 시스템

	MT_ADVICE,			// 조언 메시지

	MT_SYSTEM,			// 시스템
	MT_SYSTEM_MSG,		// MT_SYSTEM but only for chat window
	MT_BATTLE,			// 전투 메세지	_by tripleJ 110504
	MT_NOTICE,			// 공지

	MT_WHISPER_BACK,	// 귓말 자신 메시지 반송

	MT_SOCIAL,			// 소셜
	MT_SYSTEM_SCREEN,	// MT_SYSTEM but only with pop-up
};



//////////////////////////////////////////////////////////////////////////

/// 채집
enum GATHER_TYPE
{
	GATHER_NONE = -1,	// 없음

	GATHER_MINE = 0,	// 채광
	GATHER_HERB,		// 식물
	GATHER_RELIC,		// 발굴
	GATHER_LUMBER,		// 목재 채취

	MAX_GATHER_TYPE
};

#define GATHER_TIER_COUNT		10		// 채집 티어 1~10

enum USABLE_BPART_DIRECTION
{
	UBD_NONE = -1,
	UBD_FRONT,
	UBD_BACK,
	UBD_LEFT,
	UBD_RIGHT,
	UBD_MAX,
};

static const char* USABLE_BPART_DIRECTION_STR[] = {
	"front",
	"back",
	"left",
	"right"
};

// 유닛 테스트
#define TEST_FORWARD_N(suite, test) \
	namespace Suite##suite { class Test##test; }

#define TEST_FRIEND_N(suite, test) \
	friend Suite##suite::Test##test

#define TEST_FORWARD_FT(suite, fixture, test) \
	namespace Suite##suite { class fixture##test##Helper; }

#define TEST_FRIEND_FT(suite, fixture, test) \
	friend Suite##suite::fixture##test##Helper

#define TEST_FORWARD_F(suite, fixture) \
	namespace Suite##suite { struct fixture; }

#define TEST_FRIEND_F(suite, fixture) \
	friend Suite##suite::fixture

#define TEST_VIRTUAL virtual


//////////////////////////////////////////////////////////////////////////
// PMang과 관련
#define PMANG_LOGIN_USER_ID_LEN		12
#define PMANG_LOGIN_STAT_INDEX_LEN	8

//////////////////////////////////////////////////////////////////////////
/// 우편
#define MAX_MAILBOX_MAIL_COUNT				40
#define MAX_MAIL_TITLE_LEN					20
#define MAX_MAIL_TEXT_LEN					500
#define MAX_MAIL_APPENDED_ITEM_COUNT		5
#define MAX_MAIL_DELETE_SECOND				2592000		// 편지 유효기간 30일

#define MAIL_BASE_POSTAGE					50			//	50 CP
#define MAIL_EXTRA_POSTAGE_PER_SLOT			100			//	 1 SP


//////////////////////////////////////////////////////////////////////////
/// 강화
#define ENCHANT_MAX_COUNT				6  // 최대 강화 가능 횟수
#define ENCHANT_UNUSED_SLOT				0  // 사용되지 않은 슬롯
#define ENCHANT_BROKEN_SLOT				-1 // 실패해 부서진 슬롯

static int ENCHANT_AGENT_ITEMID[5] =
{
	39999000 //저급강화제
	, 39999001 //일반강화제
	, 39999002 //상급강화제
	, 39999003 //최상급강화제
	, 39999004 //특급강화제
};

//////////////////////////////////////////////////////////////////////////
/// 자동파티
enum AUTOPARTY_STATE
{
	AUTOPARTY_INVALID	=-1,	// 무효한 상태
	AUTOPARTY_OFF		=0,		// 사용 안함
	AUTOPARTY_PAUSE		=1,		// 중지
	AUTOPARTY_STANDBY	=2,		// 검색 대기
	AUTOPARTY_LOOKUP	=3,		// 검색중
};

//////////////////////////////////////////////////////////////////////////
/// Skill Set Book
enum SKILL_SET_TYPE
{
	SST_COMMON = 0,		// talent is available for all sets (e.g. emotions)
	SST_MAIN,			// 1st set for style-specified talents.
	SST_BACKUP,			// 2nd set for style-specified talents.
	SST_PVP,			// PvP skill set (was unimplemented)
	SST_MAX,
};



/// 월드 정보
struct CSWorldInfo
{
	int					nID;
	wstring				strName;
	wstring				strIP;
	int					nCurrentPlayerCount;
	int					nMaxPlayerCount;
	int					nType;
	int					nOrderNum;

	CSWorldInfo(): nID(0), nCurrentPlayerCount(0), nMaxPlayerCount(0), nType(0), nOrderNum(0)
	{
	}
};


//////////////////////////////////////////////////////////////////////////
// 유틸리티 메크로
#define		_QUOTE(x)			# x
#define		QUOTE(x)			_QUOTE(x)
#define		__FILE__LINE__		__FILE__ "(" QUOTE(__LINE__) ") : "
#define		TODO( x )			message( __FILE__LINE__" TODO : " #x "\n" )
#define		todo( x )			TODO( x )