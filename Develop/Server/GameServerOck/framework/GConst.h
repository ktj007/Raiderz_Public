#pragma once

#include "GFrameworkLib.h"
#include "CSMotionFactorInfo.h"
#include "CSDef.h"
#include "CSItemData.h"

class MWLua;

/// 상수값
class GFRAMEWORK_API GConst
{
public:
	static bool		AI_OPTIMIZATION;						///< AI 최적화 - 유닛테스트 때에는 false해야 제대로 된 결과가 나온다.
	static bool		PVP_MODE;								///< 플레이어간 전투 가능 여부
	static bool		RUN_RESOURCE_VALIDATOR;					///< 외부 데이터 파일 유효성 체크 여부
	static bool		RUN_FOR_TEST;							///< 서버를 테스트용으로 구동할지 여부, 다른 디버그용 CONST 값이 켜져있어도 이 값이 false이면 디버그, 테스트 코드가 작동하지 않습니다.

	// 리젠
	static int		REGEN_TICK;								///< 리젠 주기(초 단위)
	static int		HP_REGEN_AMOUNT;						///< 초당 STA 리젠량
	static float	HP_REGEN_RATE_IN_COMBAT;				///< 전투중 STA 리젠 보정치
	static int		EN_REGEN_AMOUNT;						///< 초당 STA 리젠량
	static float	EN_REGEN_RATE_IN_COMBAT;				///< 전투중 STA 리젠 보정치
	static int		STA_REGEN_AMOUNT;						///< 초당 STA 리젠량
	static float	STA_REGEN_RATE_IN_COMBAT;				///< 전투중 STA 리젠 보정치	
	static float	SITTING_REGEN_RATE;						///< 앉아있을 때 리젠 보정치


	static int		STRESS_ADD_INTERRUPT_DURING_ATTACKING;	///< 공격중에 인터럽트 됐을때의 추가되는 스트레스 수치
	static int		STRESS_ADD_AVOID_MELEE_ATTACK;			///< NPC의 접근 공격을 적이 회피
	static int		STRESS_ADD_AVOID_RANGE_ATTACK;			///< NPC의 원거리 공격을 적이 회피
	static int		STRESS_ADD_LIMITED_CHASE_TIME;			///< 추격중에 얻는 스트레스 수치
	static float	STRESS_CHASE_INTERVAL;					///< 스트레스 수치를 받게 되는 추격 유지 시간

	static float	STATE_FACTOR_FOR_DAMAGE_BY_BACK;		///< 상태에 따른 데미지 보정 뒤에서 맞았을 때
	static float	STATE_FACTOR_FOR_DAMAGE[MF_SIZE];		///< 상태에 따른 데미지 보정 MF_STATE
	
	static float	STATE_FACTOR_FOR_MF_VALUE_BY_BACK;		///< 상태에 따른 모션팩터 보정 뒤에서 맞았을 때
	static float	STATE_FACTOR_FOR_MF_VALUE[MF_SIZE];		///< 상태에 따른 모션팩터 보정 MF_STATE

	static wstring	REPORT_NPC_FILENAME;					///< report npc filename

	static float	VICTORY_DEFAULT_DELAY;					///< NPC가 승리했을때 콜트에 명시되지 않았을때 기본적으로 대기하는 시간

	static float	INVINCIBLE_TIME_REVIVE;					///< PC/NPC가 부활했을때 무적 시간
	static float	INVINCIBLE_TIME_ENTER_FIELD;			///< PC가 필드 입장 했을때의 무적 시간
	static int		INVINCIBLE_BUFF_ID;						///< 무적 버프의 ID

	static int		INVISIBLETONPC_BUFF_ID;					///< NPC에게 투명인 버프의 ID

	// 부활 관련 --------
	static float	REBIRTH_VALID_DISTANCE_XY;				///< 클라가 서버로 부활 요청을 했을때 유효한 거리 차이 XY
	static float	REBIRTH_VALID_DISTANCE_Z;				///< 클라가 서버로 부활 요청을 했을때 유효한 거리 차이 Z

	static int		REBIRTH_DEBUFF_TARGET_LEVEL;			///< 부활 디버프 적용 대상레벨
	static int		REBIRTH_SOULBINDING_DEBUFF;				///< 영혼석 부활 디버프
	static int		REBIRTH_CHECKPOINT_DEBUFF;				///< 안전지점 부활 디버프
	static int		REBIRTH_QUESTPVP_DEBUFF;				///< 퀘스트PVP 부활 디버프
	static int		REBIRTH_QUESTPVP_BUFF;					///< 퀘스트PVP 부활 버프

	static float	REBIRTH_SOULBINDING_RECOVER;			///< 영혼석 부활 회복률
	static float	REBIRTH_CHECKPOINT_RECOVER;				///< 안전지점 부활 회복률
	static float	REBIRTH_QUESTPVP_RECOVER;				///< 퀘스트PVP 부활 회복률
	static float	REBIRTH_BATTLEARENA_RECOVER;			///< 배틀아레나 부활 회복률

	static bool		INCLUDE_DEV_FIELD_LIST;					///< 개발중인 필드도 로딩할 지 여부
	static int		DYNAMICFIELD_EXPIRED_TIME;
	static int		TIME_SYNCH_INTERVAL;
	static int		MAX_BUFF_QTY;							// 최대로 가질 수 있는 버프 갯수
	static int		MAX_DEBUFF_QTY;							// 최대로 가질 수 있는 디버프 갯수

	static float	TARGET_INFO_OLD_SEND_TIME;				// 타겟 인포를 보낸 시간이 오래되었다고 판단하는 시간

	static float	COMBAT_CHASE_MAX_DURATION_TIME;			// 전투중 탤런트를 쓰기위한 추적의 최대시간

	static float	ROAM_DEFAULT_IDLE_TIME;					// 로밍중에 기본으로 기다리는 대기시간

	static float	INTERACT_QUEST_OBJECTIVE_ANI_TIME;		// 인터랙션으로 퀘스트 목적 상태값을 변경 시킬때 사용하는 애니메이션 시간
	static float	LOOT_ROLL_TIME;							// 주사위 굴림을 할 수 있는 시간
	
	// 전투 관련 -----
	static int		NO_WEAPON_DAMAGE_LEFT;					// 왼손 무기를 장착하지 않았을 경우의 기본 데미지
	static int		NO_WEAPON_DAMAGE_RIGHT;					// 오른손 무기를 장착하지 않았을 경우의 기본 데미지
	static float	NPC_ASSIST_INTERVAL;					// NPC가 주변 아군 NPC를 돕는 주기
	static float	NPC_FIRSTSTRIKE_DELAY;					// NPC 스폰후 선공 딜레이
	static float	PRE_HIT_CHECK_TIME;						// 실제 판정 시간보다 먼저 판정하는 시간(기본값 = 0.2초)
	static float	NPC_FINDPATH_RADIUS_LEVEL1;				// 길찾기 레벨1 적용범위
	static float	NPC_FINDPATH_RADIUS_LEVEL2;				// 길찾기 레벨2 적용범위
	static float	NPC_FINDPATH_RADIUS_LEVEL3;				// 길찾기 레벨3 적용범위
	static float	NPC_FINDPATH_TICK_LEVEL1;				// 길찾기 레벨1 갱신 틱시간
	static float	NPC_FINDPATH_TICK_LEVEL2;				// 길찾기 레벨2 갱신 틱시간
	static float	NPC_FINDPATH_TICK_LEVEL3;				// 길찾기 레벨3 갱신 틱시간
	static float	NPC_FINDPATH_TICK_OTHER;				// 길찾기 범위밖 갱신 틱시간
	static float	NPC_FINDPATH_MOVESPEED_FACTOR;			// 길찾기 갱신 틱시간 속도 보정치 (틱시간 * 이동속도 / 팩터)
	static float	NPC_DEFAULT_COMBAT_RADIUS;				// NPC 기본 전투 범위 (범위를 넘어서면 전투 종료, 단위: cm)
	
	// PVP 관련 --------------
	static float	PVP_DURATIONTIME;						// 플레이어간 피격시 PVP상태 지속시간

	// 마법진 관련 --------------
	static float	MAGICAREA_EFFECTION_PERIOD;				// 마법진 효과 주기

	// 투명화 관련
	static float	INVISIBILITY_DETECT_TICK;				// 투명화 탐지 처리 틱 주기 (second)
	static int		INVISIBILITY_DETECT_DISTANCE;			// 투명화 탐지거리 (cm)

	// 포커스 관련 --------------
	static float	FOCUS_ADVENT_DURATION_TIME;				// 강신 포커스가 얼마나 지속되는가
	static int		FOCUS_BERSERK_HIT_COUNT;				// 몇번을 때려야 버서크에 걸리는가
	static float	FOCUS_BERSERK_DURATION_TIME;			// 버서크 포커스가 얼마나 지속되는가
	static float	FOCUS_BERSERK_RELAYHIT_TIME;			// 버서크 포커스의 버블이 유지되는 시간
	static float	FOCUS_COUNTER_DURATION_TIME;			// 반격 포커스가 얼마나 지속되는가
	static float	FOCUS_ENLIGHTEN_DURATION_TIME;			// 각성 포커스가 얼마나 지속되는가
	static int		FOCUS_ENLIGHTEN_HIT_COUNT;				// 몇번을 떄려야 각성에 걸리는가
	static float	FOCUS_ENLIGHTEN_RELAYHIT_TIME;			// 각성 포커스의 버블이 유지되는 시간
	
	// 낙하 데미지 관련 ---------
	static int		FALLING_DEBUFF_LV1_ID;					// 낙하시 '다리부상 LV1' 디버프 ID
	static int		FALLING_DEBUFF_LV2_ID;					// 낙하시 '다리부상 LV2' 디버프 ID

	// 투기장 관련 --------
	static int		ARENA_FIELDGROUP_ID;					// 투기장 필드그룹 아이디
	static int		ARENA_FIELD_ID;							// 투기장 필드 아이디
	static float	ARENA_GAME_COUNT;						// 투기장 게임시작 대기시간
	static float	ARENA_SCOREBOARD_COUNT;					// 투기장 점수판 대기시간
	static int		ARENA_DEATHMATCH_MAX_SCORE;				// 투기장 데스매치 최대점수
	static float	ARENA_DEATHMATCH_REBIRTH_TIME;			// 투기장 데스매치 부활시간

	// 시간,날씨 정보 ------------
	static int		CONST_ENV_TIME_DAWN_START_HOUR;
	static int		CONST_ENV_TIME_DAYTIME_START_HOUR;
	static int		CONST_ENV_TIME_SUNSET_START_HOUR;
	static int		CONST_ENV_TIME_NIGHT_START_HOUR;

	static int		CONST_LOCALTIME_PER_GAMETIME_ONE_HOUR;	// 실시간 14분이 게임 1시간
	static int		CONST_ENV_TIME_UPDATE_TICK_SEC;			// 시간체크 틱값(초) - 60초
	static int		CONST_ENV_WEATHER_TICK_SEC;				// 날씨 변화 틱 84분 (84 * 60)

	// NPC 시체소멸 관련 ------------
	static float	NPC_DECAY_TIME_NOITEM;					// NPC가 아이템 없을때 시체가 소멸되는 시간
	static float	NPC_DECAY_TIME_GRADE[UCHAR_MAX];		// NPC가 아이템이 있을때 등급별 시체가 소멸되는 시간

	//  분노 관련 -----------------
	static int		HIT_RAGE_AMOUNT;						// NPC가 피격을 입을 때 얻을 분노량
	static int		CRIHIT_RAGE_AMOUNT;						// NPC가 치명타를 입을 때 얻을 추가 분노량 (기본 피격분노량에 추가)

	// 헤이트테이블 관련 -----------------
	static float	HATETABLE_ENTRY_TIMEOUT;				// 헤이트 테이블에 들어간 항목이 자동으로 제거되는 시간 (단위: 초)
	static float	HATETABLE_ENTRY_PARTY_SCAN_DISTANCE;	// 헤이트 테이블에 추가된 플레이어의 파티원 탐색 거리 (cm)

	// 퀘스트 관련 ------------------
	static float	QUEST_SHARE_DISTANCE;					// 퀘스트 공유 가능 거리
	static float	QUEST_ESCORT_DISTANCE;					// 퀘스트 호위 인정 거리
	static float	QUEST_OBJECTIVE_UPDATE_SHARE_DISTANCE;	// 퀘스트 목적 업데이트 공유 가능 거리

	// AutoTest 관련 ------------
	static bool		AUTO_TEST_ACTIVE;
	static wstring	AUTO_TEST_TYPE;
	static float	AUTO_TEST_LIMIT_FRAME;
	static int		AUTO_TEST_REPEAT;
	static DWORD	AUTO_TEST_TIME;
	static int		AUTO_TEST_SPEED;	
	static int		AUTO_TEST_NPC_COUNT;
	static int		AUTO_TEST_FIELD_ID;
	static int		AUTO_TEST_QPVPEVENT_ID;
	
	static int		AITEST_TYPE;

	static bool		SERVER_SLEEP;

	// DynamicField Test 관련 ------------
	static bool		DFTEST_ENABLE;
	static int		DFTEST_FIELDGROUP_ID;
	static int		DFTEST_FIELDGROUP_QTY;
	static float	DFTEST_SENSOR_ENTER_INTERVAL;

	// 최적화 관련
	static int		SECTOR_CROWDED_LEVEL_CAPACITY;		// 복잡레벨이 되기 위한 필요 플레이어 수
	static int		SECTOR_PACKET_IGNORE_EACH_COUNT;	// 복잡레벨당  무시될 패킷 횟수
	static int		SECTOR_PACKET_LIMIT_QUEUE_SIZE;		// 섹터당 관리될 플레이어별 카운터 최대 갯수
	static int		COMMAND_CROWDED_QUEUE_SIZE;			// 커멘드 복잡상태로 취급할 커멘드 큐 크기
	static int		COMMAND_OVERFULL_QUEUE_SIZE;		// 커멘드 초과상태로 취급할 커멘드 큐 크기
	
	// Chatting 관련 ---------------
	static bool		DEFAULT_GLOBAL_SAY;						// 전체채팅을 기본채팅으로 설정

	// 채집
	static float	GATHER_TIME[MAX_GATHER_TYPE][GATHER_TIER_COUNT + 1];
	static int		GATHER_RESPAWN_TIME[MAX_GATHER_TYPE];

	// 도전자 퀘스트
	static int		CQ_EXPIRED_HOUR;						// 도전자 퀘스트 만료시간 (0 ~ 23)

	// 소울 바인딩
	static int		SOULBINDING_DEFAULT_FIELDID;			// 소울바인딩한 ID가 유효하지 않을때 영혼석 부활시 사용하는 FieldID
	static int		SOULBINDING_DEFAULT_MARKERID;			// 소울바인딩한 ID가 유효하지 않을때 영혼석 부활시 사용하는 MarkerID

	// 퀘스트 PVP
	static float	QUESTPVP_REWARDOWNER_RELEASETIME;		// 퀘스트 PVP지역에서 공격을 일정시간 당하지 않으면 보상권한이 소멸되는대, 그 시간을 기술(단위: 초)
	static float	QUESTPVP_REWARDOWNER_DISTANCE;			// 퀘스트 PVP지역에서 공격을 했을때 보상권한이 같이 인정되는 멤버의 거리
	static float	QUESTPVP_INFO_SENDTIME;					// 퀘스트 PVP지역관련 정보전송 주기시간
	static float	QUESTPVP_MARKPOS_REFRESH_TIME;			// 퀘스트 PVP지역에서 마킹한 NPC위치 갱신 주기

	// 길드
	static int		GUILD_CREATE_COST;						// 길드 생성 비용


	// 치팅 관련
	static float	CHEAT_WARPHACK_LIMIT_DISTANCE;	// 워프핵 제한거리
	static float	CHEAT_SPEEDHACK_DISCONNECT_LIMIT_DISTANCE_RATE;	// 스피드핵 접속종료 제한거리 비율
	static int		CHEAT_SPEEDHACK_DISCONNECT_EXECUTE_COUNT;	// 스피드핵 접속종료 경고횟수
	static float	CHEAT_SPEEDHACK_DISCONNECT_EXECUTE_DURATION;	// 스피드핵 접속종료 경고 유지시간 (단위: 초, 이 시간동안 경고받지 않으면 초기화)
	static float	CHEAT_SPEEDHACK_WARNING_LIMIT_DISTANCE_RATE;	// 스피드핵 경고 제한거리 비율
	static int		CHEAT_SPEEDHACK_WARNING_EXECUTE_COUNT;	// 스피드핵 경고로그 경고횟수
	static float	CHEAT_SPEEDHACK_WARNING_EXECUTE_DURATION;	// 스피드핵 경고로그 유지시간 (단위: 초, 이 시간동안 경고받지 않으면 초기화)
	static bool		CHEAT_CHECK_DEVELOPER_TOO;	

	// AFK
	static float	AFK_TIMEOUT;

	// 자동파티
	static float	AUTOPARTY_MATCH_PROCESS_INTERVAL; // 자동파티 매치작업 시간간격

	// 디버그용도 -----------
	static bool		NPC_AI_RUNNING;
	static bool		AUTO_PROFILE;
	static bool		TEST_SPAWN_TEST_START_POS;

	static bool		TEST_AI_DEBUG_MESSAGE;									///< 길찾기 실패 등의 AI 메세지 클라이언트에 보내주기
	static bool		TEST_SHOW_CLIENT_NPC_MOVE_PATH;								///< 클라이언트에 NPC 이동 경로를 보여줄지 여부
	static int		TEST_SHOW_CLIENT_NPC_PICKING;
	static bool		TEST_SHOW_NAVIGATION_MESH;

	static bool		TEST_ENABLE_TUTORIAL;					///< 튜토리얼 켜고끄기
					
	static bool		TEST_RANDOM_MOVE;

	static bool		TEST_MAIL_POST_TO_ONESELF;

	static int		PATH_NEAR_HORIZONE_RANGE;
	static int		PATH_NEAR_VERTICAL_RANGE;

	static int		GAME_TICK;
	static bool		LOG_TALENT;
	static bool		LOG_FSM;
	static bool		LOG_JOB;
	static bool		LOG_SENSOR;
	static bool		LOG_PACKET;
	static bool		LOG_PATHFIND;
	static bool		LOG_SCRIPT_TIMER;
	static bool		LOG_AUTOPARTY;
	static bool		LOG_NPCSESSION;

	static bool		ENABLE_COLT_TIMEOUT_CHECKER;						///< 콜트 타임아웃 체커의 사용여부
	static bool		SHOW_LUA_ERROR_LOG;
	static bool		DEBUG_CREATE_CONVERT_LUAFILE;
	static bool		TEST_MAKE_SOLO_PARTY;
	static bool		TEST_ENABLE_OFFLINE_PARTY_FOR_STAND_ALONE_MODE;		///< 스텐드얼론 모드에서, 모든 파티원 오프라인상태 허용	
	
	static bool		TEST_SHOW_CLIENT_FORMULA_DAMAGE;
	static bool		TEST_SHOW_CLIENT_FORMULA_GUARD;
	static bool		TEST_CMD_HANDLER_COMPARER;

	static bool		GODMODE_WITH_INVINCIBLE;
	static int		LOG_CRT;

	static int		MAX_NPC_REPORT_ALL_RECORD_QTY;			///< NPC리포트 기능에 캐슁해둘 총 레코드 갯수
	static int		MAX_NPC_REPORT_SPECIFIC_RECORD_QTY;		///< NPC리포트 기능에 캐슁해둘 특정 타입의 레코드 갯수

	static bool		ENABLE_CHEATER_DISCONNECT;
	static bool		DUEL_PARTY_TEST;
	static int		DUEL_LIMIT_OBSERVE_COUNT;

	static int		BPART_BUFFSTACK_ID;
	
	// AI Monitor -----------
	static bool		AIMON_ENABLE;
	static bool		AIMON_AUTO_TARGET;
	static int		AIMON_SET_FIELDID;
	static int		AIMON_SET_SPAWNID;
	static float	AIMON_UPDATE_INTERVAL;

	// 디버그용도 -----------
	static bool		DEBUG_CONSOLE_SHOW_GAMETICK;
	static int		DEBUG_CONSOLE_GAMETICK_TICK;
	static float	DEBUG_GAME_TICK_MOD_SPEED;
	static bool		DEBUG_DISABLE_DEFAULT_FIELD_SPAWN; // 필드 xml에 지정된 스폰을 무시
	

	static float	SPEAK_MAINTAIN_TIME;
	
	static class GFRAMEWORK_API PlayerStandupTimeTable
	{
	public:
		void Init();
		float GetStandupTime(WEAPON_TYPE nWeaponType);
	private:
		float m_fPlayerStandupTime[WEAPON_MAX];
	} PLAYER_STANDUPTIME;

	static void Init(MWLua* pLua);
	static void InitDebugConst(MWLua* pLua);


	static int GetFirstEnterFieldID();
};

inline bool GFRAMEWORK_API IsRunForTest()
{
	return GConst::RUN_FOR_TEST;
}
