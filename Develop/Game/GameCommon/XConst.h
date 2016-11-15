#pragma once

#include "MTypes.h"
#include "CSChrInfo.h"
#include "XDef.h"
#include "MWLua.h"
#include "MXml.h"

struct XPlayerModelInfo
{
	tstring			MODEL_NAME;
	tstring			MODEL_NAME_SIMPLE;
	tstring			DEFAULT_MESHNAME_HAT;
	tstring			DEFAULT_MESHNAME_CHEST;
	tstring			DEFAULT_MESHNAME_HANDS;
	tstring			DEFAULT_MESHNAME_LEGS;
	tstring			DEFAULT_MESHNAME_FEET;
	vector<tstring>	HAIR_MODEL;
	vector<tstring>	FACE_MODEL;
};

class XPlayerModelConst
{
private:
	void ParseDefaultModelPath(MXmlElement* pElement);
	void ParseDefaultMesh(MXmlElement* pElement);
	void ParseHair(MXmlElement* pElement);
	void ParseFace(MXmlElement* pElement);
	SEX TransSex(const TCHAR* szValue);
public:
	XPlayerModelInfo	Info[SEX_MAX];
public:
	XPlayerModelConst() {}
	bool Init(const TCHAR* szFileName);
};

struct XDefaultEquipments
{
	int		count;
	int		items[ FEATURE_ITEMSLOT_MAX];
};


/// 상수값
class XCONST
{
public:
	static XPlayerModelConst	PlayerModel;

	static XDefaultEquipments	DEFAULT_EQUIPMENTS[ 10];
	static XDefaultEquipments	DEFAULT_STYLES[ 6];
	static string		DEFAULT_TATTOO[ 30];

	// 설정 상수 --------------------------
	static int			CAMERA_FOV;							// Field of view ( 4:3 , Y 기준 )
	static float		CAMERA_NEAR_Z;
	static float		CAMERA_FAR_Z;

	static float		GRASS_LOD_NEAR;
	static float		GRASS_LOD_FAR;
	static TCHAR		PATH_TEXTURE[1024];

	static float		DOF_FOCUS_DIST;
	static float		DOF_FOCUS_RANGE;
	static float		DOF_COC_SIZE;
	static float		CHAR_DOF_FOCUS_DIST;
	static float		CHAR_DOF_FOCUS_RANGE;
	static float		CHAR_DOF_COC_SIZE;

	// 이동값
	static float		MOVE_SPEED_NONE;
	static float		MOVE_SPEED_1HS;
	static float		MOVE_SPEED_1HB;
	static float		MOVE_SPEED_1HP;
	static float		MOVE_SPEED_2HD;
	static float		MOVE_SPEED_STF;
	static float		MOVE_SPEED_ARC;
	static float		MOVE_SPEED_2HB;
	static float		MOVE_SPEED_DWD;
	static float		MOVE_SPEED_DWP;

	static float		MOVE_BACK_SPEED_NONE;
	static float		MOVE_BACK_SPEED_1HS;
	static float		MOVE_BACK_SPEED_1HB;
	static float		MOVE_BACK_SPEED_1HP;
	static float		MOVE_BACK_SPEED_2HD;
	static float		MOVE_BACK_SPEED_STF;
	static float		MOVE_BACK_SPEED_ARC;
	static float		MOVE_BACK_SPEED_2HB;
	static float		MOVE_BACK_SPEED_DWD;
	static float		MOVE_BACK_SPEED_DWP;

	static float		JUMP_SPEED;
	static float		MOTION_FACTOR_KNOCKBACK_MOTION_EVENT_DIST;		// 거리가 얼마나 남았을 때 애니메이션 변환하나
	static float		MOTION_FACTOR_THROWUP_MOTION_EVENT_DIST;		// 거리가 얼마나 남았을 때 애니메이션 변환하나

	static float		HIT_FREEZE_TIME;		// 공격 판정 때 프레임 멈춤 시간(1 = 1초)
	static float		HIT_FREEZE_DELAY;		// 공격 판정 때 프레임 멈춤 시간 딜레이(1 = 1초)
	// UI 관련
	static int			LOADING_IMAGE_COUNT;

	static TCHAR			CHARACTER_CAPTION_FONT_NAME[256];
	static TCHAR			CHARACTER_CAPTION_FONT_FILE[256];
	static int			CHARACTER_CAPTION_FONT_HEIGHT;

	static TCHAR			SENSOR_EFFECT_NAME[256];
	static TCHAR			INTERACT_QUEST_OBJECTIVE_ANI_NAME[256];
	static float		INTERACT_QUEST_OBJECTIVE_ANI_TIME;

	// RealSpace3 설정 관련
	static bool			USING_COMPILED_SHADER;
	static bool			RESOURCE_LOADING_LOG;

	// caption effect 색깔
	static uint32		CAPTION_EFFECT_COLOR_MY_DAMAGE;
	static uint32		CAPTION_EFFECT_COLOR_MY_CRITICAL;
	static uint32		CAPTION_EFFECT_COLOR_MY_MISS;
	static uint32		CAPTION_EFFECT_COLOR_ENEMY_DAMAGE;
	static uint32		CAPTION_EFFECT_COLOR_ENEMY_CRITICAL;
	static uint32		CAPTION_EFFECT_COLOR_ENEMY_MISS;
	static uint32		CAPTION_EFFECT_COLOR_MY_BUFF;
	static uint32		CAPTION_EFFECT_COLOR_MY_DEBUFF;
	static uint32		CAPTION_EFFECT_COLOR_MY_GUARD_DEFENSE;
	static uint32		CAPTION_EFFECT_COLOR_MY_HEAL;
	static uint32		CAPTION_EFFECT_COLOR_MY_HP_REGEN;
	static uint32		CAPTION_EFFECT_COLOR_MY_EXP;
	static uint32		CAPTION_EFFECT_COLOR_MY_EN;

	// caption 사이즈
	static int			CAPTION_DAMAGE_FONT_SIZE;

	// 카메라 라이트
	static float		CONST_CAMERA_LIGHT_EFFECT_DISTANCE;
	static float 		CONST_CAMERA_LIGHT_EFFECT_RADIUS;
	static uint32		CONST_CAMERA_LIGHT_EFFECT_COLOR;
	static bool			CONST_CAMERA_LIGHT_EFFECT_ENABLE;

	// 피 설정
	static TCHAR			HIT_BASE_BLOOD_EFFECT_NAME[256];
	static float		HIT_CRITICAL_BLOOD_EFFECT_SIZE;

	// 시스템
	static bool			IGNORE_NON_TRANSLATION_MARK;	// 미번역 마크(#)를 무시해서 읽을지 여부

	// 도움말
	static int			INGAME_TIP_MAX_LEVEL;			// 게임중에 팁을 보여주는 최대 레벨
	static int			INGAME_TIP_TICK;				// 게임중에 팁 나오는 틱(초)

	// 디버그용 상수 ----------------------
	static float		MIN_CAMERA_DIST;				// 카메라와 내 캐릭터와의 최소 거리
	static float		MAX_CAMERA_DIST;				// 카메라와 내 캐릭터와의 최대 거리
	static float		CAMERA_TARGET_DIST;
	static float		CAMERA_TARGET_HEIGHT;
	static float		CAMERA_TARGET_LEFT;
	static float		MOUSE_WHEEL_SENSIBILITY;		// 휠 민감도
	static bool			SEX_MALE;						// 내 캐릭터 성별


	static uint32		SCREEN_DEBUG_FONT_COLOR;		// 디버그 글씨 색깔
	static int			SCREEN_DEBUG_TOP;				// 화면 디버거 글씨 TOP 값
	static TCHAR			STANDALONE_EQUIP_ITEMS[256];	// 장비한 아이템 목록 ID(빈칸으로 여러개 지정 가능)
	static bool			SHOW_UI;						// 처음 시작시 UI를 보여줄지 여부
	static bool			CAMERA_FIX;						// 원래는 카메라 조정이 고정이지만 디버그 용으로 줌을 풀지 여부
	static bool			DEBUG_EXECUTE_UNITTEST;	
	static bool			DEBUG_LOG;						// 디버그 로그 출력 여부

	static bool			USE_NET_DEBUGGER;				// XNetDebugger를 사용할지 여부
	static bool			USE_PHYSX_REMOTE_DEBUGGER;		// Physx remote debugger 사용 여부

	static bool			USE_HIT_EFFECT_MODEL_ADD_COLOR;		// 피격시 바뀌는 모델 색깔 사용 여부
	static float		HIT_EFFECT_MODEL_ADD_COLOR_TIME;	// 피격시 바뀌는 모델 색깔 사용 시간
	static vec3			HIT_EFFECT_MODEL_ADD_COLOR;			// 피격시 바뀌는 모델 색깔
	static vec3			HIT_EFFECT_BPARTS_ADD_COLOR;			// 피격시 바뀌는 파츠 색깔

	static TCHAR		HIT_FAKE_BEATEN_DEFAULT_ANI_NAME[256];			///< 피격 리액션 애니 이름
	static float		HIT_FAKE_BEATEN_ANIMATION_BLEND_WEIGHT;			///< 피격 리액션 블렌딩 Weight
	static float		HIT_HUGE_FAKE_BEATEN_ANIMATION_BLEND_WEIGHT;	///< Huge 몬스터 피격 리액션 블렌딩 Weight
	static float		HIT_FAKE_BEATEN_ANIMATION_BLEND_SPEED;			///< 피격 리액션 블렌딩 Speed
	static float		HIT_HUGE_FAKE_BEATEN_ANIMATION_BLEND_SPEED;		///< Huge 몬스터 피격 리액션 블렌딩 Speed
	static float		HIT_FAKE_BEATEN_ANIMATION_BLEND_CIRITICAL_WEIGHT;			///< 피격 리액션 블렌딩 크리티컬 Weight
	static float		HIT_FAKE_BEATEN_ANIMATION_BLEND_CIRITICAL_SPEED;			///< 피격 리액션 블렌딩 크리티컬 Speed

	static float		HIT_FAKE_BEATEN_WEAPON_TYPE_BLEND_WEIGHT[9];
	static float		HIT_FAKE_BEATEN_WEAPON_TYPE_BLEND_SPEED[9];

	static float		MOVE_MOTION_SPEED_RATIO;						///< 이동모션 속도배율

	static float		DECALEFFECT_DEPTH_VALUE;		// 데칼 높이 값


	// 인터랙션 대상 외곽선
	static vec4			INTERACTION_TARGET_EDGE_COLOR;				// 외곽선 색깔
	static float		INTERACTION_TARGET_EDGE_TICKNESS;		// 외곽선 두께
	static float		INTERACTION_TARGET_EDGE_BLUR_FACTOR;	// 외곽선 번짐 정도

	static vec4			INTERACTION_BPART_TARGET_EDGE_COLOR;	// BPart NPC 외곽선 색깔

	// 지형 타겟 외곽선
	static vec4			GROUND_TARGET_EDGE_COLOR;				// 외곽선 색깔


	// 디버그용 상수 ----------------------
	static bool			CONST_MAKE_COMMAND_RESULT;
	
	static TCHAR			DEFAULT_UI_FONT[256];
	static int			DEFAULT_UI_FONT_HEIGHT;
	static bool			AUTO_REPORT_CRASHDUMP;
	static bool			FULL_DUMP;
	static bool			DEBUG_LOG_COMMAND;
	static bool			AUTO_SAVE_REPLAY;				// 게임시작하면 자동으로 리플레이 저장
	static bool			COMMAND_PROFILER_AUTO_SAVE;		// 게임 종료시 커맨드 프로파일러 로그를 자동으로 저장할지 여부
	static bool			DEBUG_SHOW_INTERPOLATION_TARGET_INFO_NETPLAYER;

	static int			REFILL_POTION_HP_ITEMID;
	static int			REFILL_POTION_MP_ITEMID;
	static int			REFILL_POTION_SP_ITEMID;

	static AGE_LIMIT	PLAYER_AGE_LIMIT;

	static TCHAR			DIEBGMNAME[256];

	// CrashDump
	static tstring		CONST_CRASH_DUMP_DEST_IP_LOCAL;
	static tstring		CONST_CRASH_DUMP_DEST_IP;
	static int			CONST_CRASH_DUMP_DEST_PORT;

	// 충돌처리
	static float		COLLISION_CAPSULE_RADIUS_OFFSET;
	static float		COLLISION_TERRAIN_FALLING_START_TIME;


	// BPart
	static int			BPART_DEFAULT_NPC_ID;
	static TCHAR		BPART_GAIN_BUFF_EFFECT_NAME[256];
	static TCHAR		BPART_LOST_BUFF_EFFECT_NAME[256];
	static TCHAR		BPART_LOOTABLE_EFFECT_NAME[256];
	static int			BPART_LOOTABLE_EFFECT_MIN_SIZE;		
	static int			BPART_LOOTABLE_EFFECT_MAX_SIZE;
	static float		BPART_LOOTABLE_EFFECT_SCALE_RATIO;		

	// 효과음
	static TCHAR		SOUND_DODGE_EFFECT_NAME[256];
	static TCHAR		SOUND_RIPOSTE_EFFECT_NAME[256];
	static TCHAR		SOUND_RESIST_PERFECT_EFFECT_NAME[256];
	static TCHAR		SOUND_RESIST_PARTIAL_EFFECT_NAME[256];
	static TCHAR		SOUND_DRAIN_EFFECT_NAME[256];
	static TCHAR		SOUND_IMMUNE_EFFECT_NAME[256];
	static TCHAR		SOUND_MISS_EFFECT_NAME[256];
	static TCHAR		SOUND_AVOID_EFFECT_NAME[256];


	static void Init();
private:
	static void InitFromScript();
	static void InitFromExt();
	static void SetStringValue(MWLua* pLua, TCHAR* szoutValue, int nValueLen, const TCHAR* szScriptValueName);
	static void InitDefaultEquipments(MWLua* pLua);
	static void InitDefaultStyles(MWLua* pLua);
	static void InitDefaultTattoo( MWLua* pLua);
	static void InitCaptionEffectColor(MWLua* pLua);
	static void SetColor(MWLua* pLua, uint32* poutValue, const TCHAR* szValueName);
	static void InitAgeLimit(MWLua* pLua);
	static void SetFakeBeatenWeapontypeAnimationBlend(MWLua* pLua);
};




