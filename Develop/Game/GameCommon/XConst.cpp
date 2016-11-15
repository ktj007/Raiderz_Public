#include "stdafx.h"
#include "XConst.h"
#include "RTreeDef.h"
#include "FileInfo.h"
#include "XDef.h"
#include "MFileSystem.h"
#include "MLocale.h"

XPlayerModelConst	XCONST::PlayerModel;
XDefaultEquipments	XCONST::DEFAULT_EQUIPMENTS[ 10];
XDefaultEquipments	XCONST::DEFAULT_STYLES[ 6];
string	XCONST::DEFAULT_TATTOO[ 30];

// 상수 기본값 정의
int		XCONST::CAMERA_FOV				= 90;
float	XCONST::CAMERA_NEAR_Z			= 10.0f;
float	XCONST::CAMERA_FAR_Z			= 10000.0f;
float	XCONST::GRASS_LOD_NEAR			= 2000.0f;
float	XCONST::GRASS_LOD_FAR			= 3000.0f;
TCHAR	XCONST::PATH_TEXTURE[1024]		= DEFAULT_PATH_TEXTURES;

float	XCONST::DOF_FOCUS_DIST			= 100.0f;
float	XCONST::DOF_FOCUS_RANGE			= 2500.0f;
float	XCONST::DOF_COC_SIZE			= 5.0;
float	XCONST::CHAR_DOF_FOCUS_DIST		= 100.0f;
float	XCONST::CHAR_DOF_FOCUS_RANGE	= 2500.0f;
float	XCONST::CHAR_DOF_COC_SIZE		= 5.0;

float	XCONST::MOVE_SPEED_NONE			= 550.0f;
float	XCONST::MOVE_SPEED_1HS			= 360.0f;
float	XCONST::MOVE_SPEED_1HB			= 360.0f;
float	XCONST::MOVE_SPEED_1HP			= 360.0f;
float	XCONST::MOVE_SPEED_2HD			= 360.0f;
float	XCONST::MOVE_SPEED_STF			= 360.0f;
float	XCONST::MOVE_SPEED_ARC			= 360.0f;
float	XCONST::MOVE_SPEED_2HB			= 360.0f;
float	XCONST::MOVE_SPEED_DWD			= 360.0f;
float	XCONST::MOVE_SPEED_DWP			= 360.0f;

float	XCONST::MOVE_BACK_SPEED_NONE	= 550.0f;
float	XCONST::MOVE_BACK_SPEED_1HS		= 360.0f;
float	XCONST::MOVE_BACK_SPEED_1HB		= 360.0f;
float	XCONST::MOVE_BACK_SPEED_1HP		= 360.0f;
float	XCONST::MOVE_BACK_SPEED_2HD		= 360.0f;
float	XCONST::MOVE_BACK_SPEED_STF		= 360.0f;
float	XCONST::MOVE_BACK_SPEED_ARC		= 360.0f;
float	XCONST::MOVE_BACK_SPEED_2HB		= 360.0f;
float	XCONST::MOVE_BACK_SPEED_DWD		= 360.0f;
float	XCONST::MOVE_BACK_SPEED_DWP		= 360.0f;

float	XCONST::JUMP_SPEED				= 680.0f;
float	XCONST::MOTION_FACTOR_KNOCKBACK_MOTION_EVENT_DIST = 400.0f;
float	XCONST::MOTION_FACTOR_THROWUP_MOTION_EVENT_DIST = 800.0f;
float	XCONST::HIT_FREEZE_TIME			= 0.12f;
float	XCONST::HIT_FREEZE_DELAY		= 0.0f;

float	XCONST::MIN_CAMERA_DIST			= 50.0f;
float	XCONST::MAX_CAMERA_DIST			= 500.0f;
float	XCONST::CAMERA_TARGET_DIST		= 240.0f;
float	XCONST::CAMERA_TARGET_HEIGHT	= 140.0f;
float	XCONST::CAMERA_TARGET_LEFT		= 0.0f;
float	XCONST::MOUSE_WHEEL_SENSIBILITY = 0.8f;

bool	XCONST::SEX_MALE						= false;
int		XCONST::LOADING_IMAGE_COUNT				= 1;

TCHAR	XCONST::STANDALONE_EQUIP_ITEMS[256] = _T("");
bool	XCONST::SHOW_UI					= true;
bool	XCONST::CAMERA_FIX				= true;
bool	XCONST::DEBUG_EXECUTE_UNITTEST = false;
bool	XCONST::DEBUG_LOG = false;
bool	XCONST::USE_NET_DEBUGGER = false;
bool	XCONST::USE_PHYSX_REMOTE_DEBUGGER = false;
bool	XCONST::AUTO_REPORT_CRASHDUMP = false;
bool	XCONST::FULL_DUMP = false;

vec3	XCONST::HIT_EFFECT_MODEL_ADD_COLOR = vec3::ZERO;
bool	XCONST::USE_HIT_EFFECT_MODEL_ADD_COLOR = false;
float	XCONST::HIT_EFFECT_MODEL_ADD_COLOR_TIME = 0.0f;
vec3	XCONST::HIT_EFFECT_BPARTS_ADD_COLOR = vec3::ZERO;

TCHAR	XCONST::HIT_FAKE_BEATEN_DEFAULT_ANI_NAME[256] = _T("MF_BT2");
float	XCONST::HIT_FAKE_BEATEN_ANIMATION_BLEND_WEIGHT = 0.5f;
float	XCONST::HIT_HUGE_FAKE_BEATEN_ANIMATION_BLEND_WEIGHT = 0.2f;
float	XCONST::HIT_FAKE_BEATEN_ANIMATION_BLEND_SPEED = 1.0f;
float	XCONST::HIT_HUGE_FAKE_BEATEN_ANIMATION_BLEND_SPEED = 1.0f;
float	XCONST::HIT_FAKE_BEATEN_ANIMATION_BLEND_CIRITICAL_WEIGHT = 1.3f;
float	XCONST::HIT_FAKE_BEATEN_ANIMATION_BLEND_CIRITICAL_SPEED = 1.0f;

float	XCONST::HIT_FAKE_BEATEN_WEAPON_TYPE_BLEND_WEIGHT[9] = {1.0f, };
float	XCONST::HIT_FAKE_BEATEN_WEAPON_TYPE_BLEND_SPEED[9] = {1.0f, };

float	XCONST::MOVE_MOTION_SPEED_RATIO = 1.0f;

uint32	XCONST::SCREEN_DEBUG_FONT_COLOR		= 0xFFFFFFFF;		// 흰색
int		XCONST::SCREEN_DEBUG_TOP			= 10;

TCHAR	XCONST::DEFAULT_UI_FONT[256] = _T("굴림");
int		XCONST::DEFAULT_UI_FONT_HEIGHT = 12;

TCHAR	XCONST::CHARACTER_CAPTION_FONT_NAME[256] = _T("굴림");
TCHAR	XCONST::CHARACTER_CAPTION_FONT_FILE[256] = _T("");
int		XCONST::CHARACTER_CAPTION_FONT_HEIGHT = 12;

TCHAR	XCONST::SENSOR_EFFECT_NAME[256] = _T("");
TCHAR	XCONST::INTERACT_QUEST_OBJECTIVE_ANI_NAME[256] = _T("");
float	XCONST::INTERACT_QUEST_OBJECTIVE_ANI_TIME = 2.0f;

bool	XCONST::CONST_MAKE_COMMAND_RESULT = false;
bool	XCONST::DEBUG_LOG_COMMAND = false;
bool	XCONST::AUTO_SAVE_REPLAY = false;
bool	XCONST::COMMAND_PROFILER_AUTO_SAVE = false;
bool	XCONST::DEBUG_SHOW_INTERPOLATION_TARGET_INFO_NETPLAYER = false;

uint32	XCONST::CAPTION_EFFECT_COLOR_MY_DAMAGE = DEFAULT_CAPTION_EFFECT_COLOR_MY_DAMAGE;
uint32	XCONST::CAPTION_EFFECT_COLOR_MY_CRITICAL = DEFAULT_CAPTION_EFFECT_COLOR_MY_CRITICAL;
uint32	XCONST::CAPTION_EFFECT_COLOR_MY_MISS = DEFAULT_CAPTION_EFFECT_COLOR_MY_MISS;
uint32	XCONST::CAPTION_EFFECT_COLOR_ENEMY_DAMAGE = DEFAULT_CAPTION_EFFECT_COLOR_ENEMY_DAMAGE;
uint32	XCONST::CAPTION_EFFECT_COLOR_ENEMY_CRITICAL = DEFAULT_CAPTION_EFFECT_COLOR_ENEMY_CRITICAL;
uint32	XCONST::CAPTION_EFFECT_COLOR_ENEMY_MISS = DEFAULT_CAPTION_EFFECT_COLOR_ENEMY_MISS;
uint32	XCONST::CAPTION_EFFECT_COLOR_MY_BUFF = DEFAULT_CAPTION_EFFECT_COLOR_MY_BUFF;
uint32	XCONST::CAPTION_EFFECT_COLOR_MY_DEBUFF = DEFAULT_CAPTION_EFFECT_COLOR_MY_DEBUFF;
uint32	XCONST::CAPTION_EFFECT_COLOR_MY_GUARD_DEFENSE = DEFAULT_CAPTION_EFFECT_COLOR_MY_GUARD_DEFENSE;
uint32	XCONST::CAPTION_EFFECT_COLOR_MY_HEAL = DEFAULT_CAPTION_EFFECT_COLOR_MY_HEAL;
uint32	XCONST::CAPTION_EFFECT_COLOR_MY_HP_REGEN = DEFAULT_CAPTION_EFFECT_COLOR_MY_HP_REGEN;
uint32	XCONST::CAPTION_EFFECT_COLOR_MY_EXP = DEFAULT_CAPTION_EFFECT_COLOR_MY_EXP;
uint32	XCONST::CAPTION_EFFECT_COLOR_MY_EN = DEFAULT_CAPTION_EFFECT_COLOR_MY_EN;

int		XCONST::CAPTION_DAMAGE_FONT_SIZE = 25;

int		XCONST::REFILL_POTION_HP_ITEMID = 32001;
int		XCONST::REFILL_POTION_MP_ITEMID = 32002;
int		XCONST::REFILL_POTION_SP_ITEMID = 32003;

bool	XCONST::USING_COMPILED_SHADER = true;
bool	XCONST::RESOURCE_LOADING_LOG = false;

TCHAR	XCONST::DIEBGMNAME[256] = _T("bgm_die_1");

float	XCONST::CONST_CAMERA_LIGHT_EFFECT_DISTANCE = 50.0f;
float 	XCONST::CONST_CAMERA_LIGHT_EFFECT_RADIUS = 50.0f;
uint32	XCONST::CONST_CAMERA_LIGHT_EFFECT_COLOR = 0xffffffff;
bool	XCONST::CONST_CAMERA_LIGHT_EFFECT_ENABLE = false;

TCHAR	XCONST::HIT_BASE_BLOOD_EFFECT_NAME[256] = _T("");
float	XCONST::HIT_CRITICAL_BLOOD_EFFECT_SIZE = 2.0f;

bool	XCONST::IGNORE_NON_TRANSLATION_MARK = false;

int		XCONST::INGAME_TIP_MAX_LEVEL = 4;
int		XCONST::INGAME_TIP_TICK = 120;

AGE_LIMIT XCONST::PLAYER_AGE_LIMIT = AL_18_UNDER;

// CrashDump
tstring XCONST::CONST_CRASH_DUMP_DEST_IP_LOCAL = _T("icecream");
tstring XCONST::CONST_CRASH_DUMP_DEST_IP= _T("icecream");
int XCONST::CONST_CRASH_DUMP_DEST_PORT = 21;

float	XCONST::DECALEFFECT_DEPTH_VALUE = 100.0f;		// 1미터

float	XCONST::COLLISION_CAPSULE_RADIUS_OFFSET = 0.0f;
float	XCONST::COLLISION_TERRAIN_FALLING_START_TIME = 0.1f;

vec4	XCONST::INTERACTION_TARGET_EDGE_COLOR = vec4( 0.77f, 0.76f, 0.45f, 1.0f);
float	XCONST::INTERACTION_TARGET_EDGE_TICKNESS = 1.2f;
float	XCONST::INTERACTION_TARGET_EDGE_BLUR_FACTOR = 1.9f;

vec4	XCONST::INTERACTION_BPART_TARGET_EDGE_COLOR = vec4( 0.77f, 1.0f, 0.0f, 0.0f);

vec4	XCONST::GROUND_TARGET_EDGE_COLOR = vec4(0.69f, 0.7f, 0.93f, 1.0f);

int		XCONST::BPART_DEFAULT_NPC_ID = 910000;
TCHAR	XCONST::BPART_GAIN_BUFF_EFFECT_NAME[256] = _T("darkbinder_boom");
TCHAR	XCONST::BPART_LOST_BUFF_EFFECT_NAME[256] = _T("darkbinder_boom");
TCHAR	XCONST::BPART_LOOTABLE_EFFECT_NAME[256] = _T("ef_loot");
int		XCONST::BPART_LOOTABLE_EFFECT_MIN_SIZE = 100;
int		XCONST::BPART_LOOTABLE_EFFECT_MAX_SIZE = 400;
float	XCONST::BPART_LOOTABLE_EFFECT_SCALE_RATIO = 1.0;

TCHAR	XCONST::SOUND_DODGE_EFFECT_NAME[256] = _T("");
TCHAR	XCONST::SOUND_RIPOSTE_EFFECT_NAME[256] = _T("");
TCHAR	XCONST::SOUND_RESIST_PERFECT_EFFECT_NAME[256] = _T("");
TCHAR	XCONST::SOUND_RESIST_PARTIAL_EFFECT_NAME[256] = _T("");
TCHAR	XCONST::SOUND_DRAIN_EFFECT_NAME[256] = _T("");
TCHAR	XCONST::SOUND_IMMUNE_EFFECT_NAME[256] = _T("");
TCHAR	XCONST::SOUND_MISS_EFFECT_NAME[256] = _T("");
TCHAR	XCONST::SOUND_AVOID_EFFECT_NAME[256] = _T("");


#define CONST_XML_TAG_CONST			"CONST"
#define CONST_XML_TAG_MAIET			"maiet"

void XCONST::Init()
{
	InitFromScript();
	InitFromExt();
	if (PlayerModel.Init(FILENAME_PLAYER_MODEL) == false)
	{
		mlog("Failed Open File : %s\n", MLocale::ConvTCHARToAnsi(FILENAME_PLAYER_MODEL).c_str());
	}
}

void XCONST::SetStringValue( MWLua* pLua, TCHAR* szoutValue, int nValueLen, const TCHAR* szScriptValueName )
{
	if (pLua->GetVar<char*>(MLocale::ConvTCHARToAnsi(szScriptValueName).c_str()))
	{
		_tcscpy_s(szoutValue, nValueLen, MLocale::ConvAnsiToTCHAR(pLua->GetVar<char*>(MLocale::ConvTCHARToAnsi(szScriptValueName).c_str())).c_str());
	}
	else
	{
		_tcscpy_s(szoutValue, nValueLen, _T(""));
	}

}

static uint32 ARGB(unsigned char a, unsigned char r,unsigned char g, unsigned char b)
{
	return ( ((((unsigned long int)a)&0xFF)<<24) | ((((unsigned long int)r)&0xFF)<<16) | ((((unsigned long int)g)&0xFF)<<8) | (((unsigned long int)b)&0xFF) );
}


void XCONST::InitFromScript()
{
	MWLua lua;
	lua.Open();

	if ( true)
	{
		MFile mf;
		if ( mf.Open(MLocale::ConvTCHARToAnsi(FILENAME_SCRIPT_CONST_REL).c_str()) == true)
		{
			unsigned int _size = mf.GetLength();
			char* pBuff = new char[ _size + 1];
			memset( pBuff, 0, _size + 1);
			mf.ReadAll( pBuff, _size);
			mf.Close();

			lua.RunString( pBuff, _size, WLUA_DEBUG_ARG);
			delete [] pBuff;
		}
	}


#ifndef _PUBLISH
	if ( true)
	{
		MFile mf;
		if ( mf.Open(MLocale::ConvTCHARToAnsi(FILENAME_SCRIPT_CONST).c_str()) == true)
		{
			unsigned int _size = mf.GetLength();
			char* pBuff = new char[ _size + 1];
			memset( pBuff, 0, _size + 1);
			mf.ReadAll( pBuff, _size);
			mf.Close();

			lua.RunString( pBuff, _size, WLUA_DEBUG_ARG);
			delete [] pBuff;
		}
	}
#endif


	XCONST::CAMERA_FOV					= lua.GetVar<int>("CONST_CAMERA_FOV");
	XCONST::CAMERA_NEAR_Z				= lua.GetVar<float>("CONST_CAMERA_NEAR_Z");
	XCONST::CAMERA_FAR_Z				= lua.GetVar<float>("CONST_CAMERA_FAR_Z");
	XCONST::GRASS_LOD_FAR				= lua.GetVar<float>("CONST_GRASS_LOD_FAR");
	if (lua.GetVar<char*>("CONST_PATH_TEXTURE") != NULL)
		_tcscpy_s(XCONST::PATH_TEXTURE, MLocale::ConvAnsiToTCHAR(lua.GetVar<char*>("CONST_PATH_TEXTURE")).c_str());

	XCONST::DOF_FOCUS_DIST				= lua.GetVar<float>("CONST_DOF_FOCUS_DIST");
	XCONST::DOF_FOCUS_RANGE				= lua.GetVar<float>("CONST_DOF_FOCUS_RANGE");
	XCONST::DOF_COC_SIZE				= lua.GetVar<float>("CONST_DOF_COC_SIZE");
	XCONST::CHAR_DOF_FOCUS_DIST			= lua.GetVar<float>("CONST_CHAR_DOF_FOCUS_DIST");
	XCONST::CHAR_DOF_FOCUS_RANGE		= lua.GetVar<float>("CONST_CHAR_DOF_FOCUS_RANGE");
	XCONST::CHAR_DOF_COC_SIZE			= lua.GetVar<float>("CONST_CHAR_DOF_COC_SIZE");

	XCONST::MOVE_SPEED_NONE				= lua.GetVar<float>("CONST_MOVE_SPEED_NONE");
	XCONST::MOVE_SPEED_1HS				= lua.GetVar<float>("CONST_MOVE_SPEED_1HS");
	XCONST::MOVE_SPEED_1HB				= lua.GetVar<float>("CONST_MOVE_SPEED_1HB");
	XCONST::MOVE_SPEED_1HP				= lua.GetVar<float>("CONST_MOVE_SPEED_1HP");
	XCONST::MOVE_SPEED_2HD				= lua.GetVar<float>("CONST_MOVE_SPEED_2HD");
	XCONST::MOVE_SPEED_STF				= lua.GetVar<float>("CONST_MOVE_SPEED_STF");
	XCONST::MOVE_SPEED_ARC				= lua.GetVar<float>("CONST_MOVE_SPEED_ARC");
	XCONST::MOVE_SPEED_2HB				= lua.GetVar<float>("CONST_MOVE_SPEED_2HB");
	XCONST::MOVE_SPEED_DWD				= lua.GetVar<float>("CONST_MOVE_SPEED_DWD");
	XCONST::MOVE_SPEED_DWP				= lua.GetVar<float>("CONST_MOVE_SPEED_DWP");

	XCONST::MOVE_BACK_SPEED_NONE		= lua.GetVar<float>("CONST_MOVE_BACK_SPEED_NONE");
	XCONST::MOVE_BACK_SPEED_1HS			= lua.GetVar<float>("CONST_MOVE_BACK_SPEED_1HS");
	XCONST::MOVE_BACK_SPEED_1HB			= lua.GetVar<float>("CONST_MOVE_BACK_SPEED_1HB");
	XCONST::MOVE_BACK_SPEED_1HP			= lua.GetVar<float>("CONST_MOVE_BACK_SPEED_1HP");
	XCONST::MOVE_BACK_SPEED_2HD			= lua.GetVar<float>("CONST_MOVE_BACK_SPEED_2HD");
	XCONST::MOVE_BACK_SPEED_STF			= lua.GetVar<float>("CONST_MOVE_BACK_SPEED_STF");
	XCONST::MOVE_BACK_SPEED_ARC			= lua.GetVar<float>("CONST_MOVE_BACK_SPEED_ARC");
	XCONST::MOVE_BACK_SPEED_2HB			= lua.GetVar<float>("CONST_MOVE_BACK_SPEED_2HB");
	XCONST::MOVE_BACK_SPEED_DWD			= lua.GetVar<float>("CONST_MOVE_BACK_SPEED_DWD");
	XCONST::MOVE_BACK_SPEED_DWP			= lua.GetVar<float>("CONST_MOVE_BACK_SPEED_DWP");

	XCONST::JUMP_SPEED					= lua.GetVar<float>("CONST_JUMP_SPEED");

	XCONST::MOTION_FACTOR_KNOCKBACK_MOTION_EVENT_DIST				= lua.GetVar<float>("CONST_MOTION_FACTOR_KNOCKBACK_MOTION_EVENT_DIST");
	XCONST::MOTION_FACTOR_THROWUP_MOTION_EVENT_DIST					= lua.GetVar<float>("CONST_MOTION_FACTOR_THROWUP_MOTION_EVENT_DIST");

	XCONST::MIN_CAMERA_DIST				= lua.GetVar<float>("CONST_CAMERA_MIN_DIST");
	XCONST::MAX_CAMERA_DIST				= lua.GetVar<float>("CONST_CAMERA_MAX_DIST");
	XCONST::CAMERA_TARGET_DIST			= lua.GetVar<float>("CONST_CAMERA_TARGET_DIST");
	XCONST::CAMERA_TARGET_HEIGHT		= lua.GetVar<float>("CONST_CAMERA_TARGET_HEIGHT");
	XCONST::CAMERA_TARGET_LEFT			= lua.GetVar<float>("CONST_CAMERA_TARGET_LEFT");
	XCONST::MOUSE_WHEEL_SENSIBILITY		= lua.GetVar<float>("CONST_MOUSE_WHEEL_SENSIBILITY");

	XCONST::HIT_FREEZE_TIME				= lua.GetVar<float>("HIT_FREEZE_TIME");
	XCONST::HIT_FREEZE_DELAY			= lua.GetVar<float>("HIT_FREEZE_DELAY");

	SetStringValue(&lua, XCONST::STANDALONE_EQUIP_ITEMS, 64, _T("CONST_STANDALONE_EQUIP_ITEMS"));
	XCONST::SEX_MALE = lua.GetVar<bool>("CONST_SEX_MALE");


	SetColor(&lua, &XCONST::SCREEN_DEBUG_FONT_COLOR, _T("CONST_SCREEN_DEBUG_FONT_COLOR"));

#ifndef _PUBLISH
	XCONST::SHOW_UI						= lua.GetVar<bool>("CONST_SHOW_UI");
	XCONST::CAMERA_FIX					= lua.GetVar<bool>("CONST_CAMERA_FIX");
	XCONST::DEBUG_EXECUTE_UNITTEST		= lua.GetVar<bool>("CONST_DEBUG_EXECUTE_UNITTEST");
	XCONST::DEBUG_LOG					= lua.GetVar<bool>("CONST_DEBUG_LOG");
	XCONST::USE_NET_DEBUGGER			= lua.GetVar<bool>("CONST_USE_NET_DEBUGGER");
	XCONST::USE_PHYSX_REMOTE_DEBUGGER	= lua.GetVar<bool>("CONST_USE_PHYSX_REMOTE_DEBUGGER");
	XCONST::CONST_MAKE_COMMAND_RESULT	= lua.GetVar<bool>("CONST_MAKE_COMMAND_RESULT");
	XCONST::DEBUG_LOG_COMMAND			= lua.GetVar<bool>("CONST_LOG_COMMAND");
	XCONST::AUTO_SAVE_REPLAY			= lua.GetVar<bool>("CONST_AUTO_SAVE_REPLAY");
	XCONST::COMMAND_PROFILER_AUTO_SAVE	= lua.GetVar<bool>("CONST_COMMAND_PROFILER_AUTO_SAVE");
	XCONST::MOVE_MOTION_SPEED_RATIO		= lua.GetVar<float>("MOVE_MOTION_SPEED_RATIO");
	XCONST::DEBUG_SHOW_INTERPOLATION_TARGET_INFO_NETPLAYER = lua.GetVar<bool>("CONST_DEBUG_SHOW_INTERPOLATION_TARGET_INFO_NETPLAYER");
#endif

	XCONST::USE_HIT_EFFECT_MODEL_ADD_COLOR	= lua.GetVar<bool>("CONST_USE_HIT_EFFECT_MODEL_ADD_COLOR");
	XCONST::HIT_EFFECT_MODEL_ADD_COLOR_TIME	= lua.GetVar<float>("CONST_HIT_EFFECT_MODEL_ADD_COLOR_TIME");
	XCONST::HIT_EFFECT_MODEL_ADD_COLOR.x	= lua.GetVar<float>("CONST_HIT_EFFECT_MODEL_ADD_COLOR_R");
	XCONST::HIT_EFFECT_MODEL_ADD_COLOR.y	= lua.GetVar<float>("CONST_HIT_EFFECT_MODEL_ADD_COLOR_G");
	XCONST::HIT_EFFECT_MODEL_ADD_COLOR.z	= lua.GetVar<float>("CONST_HIT_EFFECT_MODEL_ADD_COLOR_B");

	XCONST::HIT_EFFECT_BPARTS_ADD_COLOR.x	= lua.GetVar<float>("CONST_HIT_EFFECT_BPARTS_ADD_COLOR_R");
	XCONST::HIT_EFFECT_BPARTS_ADD_COLOR.y	= lua.GetVar<float>("CONST_HIT_EFFECT_BPARTS_ADD_COLOR_G");
	XCONST::HIT_EFFECT_BPARTS_ADD_COLOR.z	= lua.GetVar<float>("CONST_HIT_EFFECT_BPARTS_ADD_COLOR_B");

	SetStringValue(&lua, XCONST::HIT_FAKE_BEATEN_DEFAULT_ANI_NAME, 256, _T("CONST_HIT_FAKE_BEATEN_DEFAULT_ANI_NAME"));
	XCONST::HIT_FAKE_BEATEN_ANIMATION_BLEND_WEIGHT	= lua.GetVar<float>("CONST_HIT_FAKE_BEATEN_ANIMATION_BLEND_WEIGHT");
	XCONST::HIT_HUGE_FAKE_BEATEN_ANIMATION_BLEND_WEIGHT	= lua.GetVar<float>("CONST_HIT_HUGE_FAKE_BEATEN_ANIMATION_BLEND_WEIGHT");
	XCONST::HIT_FAKE_BEATEN_ANIMATION_BLEND_SPEED = lua.GetVar<float>("CONST_HIT_FAKE_BEATEN_ANIMATION_BLEND_SPEED");
	XCONST::HIT_HUGE_FAKE_BEATEN_ANIMATION_BLEND_SPEED = lua.GetVar<float>("CONST_HIT_HUGE_FAKE_BEATEN_ANIMATION_BLEND_SPEED");

	XCONST::HIT_FAKE_BEATEN_ANIMATION_BLEND_CIRITICAL_WEIGHT = lua.GetVar<float>("CONST_HIT_FAKE_BEATEN_ANIMATION_BLEND_CRITICAL_WEIGHT");
	XCONST::HIT_FAKE_BEATEN_ANIMATION_BLEND_CIRITICAL_SPEED = lua.GetVar<float>("CONST_HIT_FAKE_BEATEN_ANIMATION_BLEND_CRITICAL_SPEED");
	

	SetFakeBeatenWeapontypeAnimationBlend(&lua);

	XCONST::INTERACTION_TARGET_EDGE_COLOR.r	= lua.GetVar<float>("CONST_INTERACTION_TARGET_EDGE_COLOR_R");
	XCONST::INTERACTION_TARGET_EDGE_COLOR.g	= lua.GetVar<float>("CONST_INTERACTION_TARGET_EDGE_COLOR_G");
	XCONST::INTERACTION_TARGET_EDGE_COLOR.b	= lua.GetVar<float>("CONST_INTERACTION_TARGET_EDGE_COLOR_B");
	XCONST::INTERACTION_TARGET_EDGE_COLOR.a	= lua.GetVar<float>("CONST_INTERACTION_TARGET_EDGE_COLOR_A");
	XCONST::INTERACTION_TARGET_EDGE_TICKNESS	= lua.GetVar<float>("CONST_INTERACTION_TARGET_EDGE_TICKNESS");
	XCONST::INTERACTION_TARGET_EDGE_BLUR_FACTOR	= lua.GetVar<float>("CONST_INTERACTION_TARGET_EDGE_BLUR_FACTOR");

	XCONST::INTERACTION_BPART_TARGET_EDGE_COLOR.r	= lua.GetVar<float>("CONST_INTERACTION_BPART_TARGET_EDGE_COLOR_R");
	XCONST::INTERACTION_BPART_TARGET_EDGE_COLOR.g	= lua.GetVar<float>("CONST_INTERACTION_BPART_TARGET_EDGE_COLOR_G");
	XCONST::INTERACTION_BPART_TARGET_EDGE_COLOR.b	= lua.GetVar<float>("CONST_INTERACTION_BPART_TARGET_EDGE_COLOR_B");
	XCONST::INTERACTION_BPART_TARGET_EDGE_COLOR.a	= lua.GetVar<float>("CONST_INTERACTION_BPART_TARGET_EDGE_COLOR_A");

	XCONST::GROUND_TARGET_EDGE_COLOR.r	= lua.GetVar<float>("CONST_GROUND_TARGET_EDGE_COLOR_R");
	XCONST::GROUND_TARGET_EDGE_COLOR.g	= lua.GetVar<float>("CONST_GROUND_TARGET_EDGE_COLOR_G");
	XCONST::GROUND_TARGET_EDGE_COLOR.b	= lua.GetVar<float>("CONST_GROUND_TARGET_EDGE_COLOR_B");
	XCONST::GROUND_TARGET_EDGE_COLOR.a	= lua.GetVar<float>("CONST_GROUND_TARGET_EDGE_COLOR_A");

	// RealSpace3
	XCONST::USING_COMPILED_SHADER		= lua.GetVar<bool>("CONST_USING_COMPILED_SHADER");
	XCONST::RESOURCE_LOADING_LOG		= lua.GetVar<bool>("CONST_RESOURCE_LOADING_LOG");

	// Font
	SetStringValue(&lua, XCONST::DEFAULT_UI_FONT, 256, _T("CONST_DEFAULT_FONT_NAME"));
	XCONST::DEFAULT_UI_FONT_HEIGHT = lua.GetVar<int>("CONST_DEFAULT_FONT_HEIGHT");

	SetStringValue(&lua, XCONST::CHARACTER_CAPTION_FONT_NAME, 256, _T("CONST_CHARACTER_CAPTION_FONT_NAME"));
	SetStringValue(&lua, XCONST::CHARACTER_CAPTION_FONT_FILE, 256, _T("CONST_CHARACTER_CAPTION_FONT_FILE"));
	XCONST::CHARACTER_CAPTION_FONT_HEIGHT = lua.GetVar<int>("CONST_CHARACTER_CAPTION_FONT_HEIGHT");

	SetStringValue(&lua, XCONST::SENSOR_EFFECT_NAME, 256, _T("CONST_SENSOR_EFFECT_NAME"));
	SetStringValue(&lua, XCONST::INTERACT_QUEST_OBJECTIVE_ANI_NAME, 256, _T("CONST_INTERACT_QUEST_OBJECTIVE_ANI_NAME"));
	XCONST::INTERACT_QUEST_OBJECTIVE_ANI_TIME	= lua.GetVar<float>("CONST_INTERACT_QUEST_OBJECTIVE_ANI_TIME");

	XCONST::FULL_DUMP						= lua.GetVar<bool>("CONST_FULL_DUMP");

	SetStringValue(&lua, XCONST::DIEBGMNAME		, 256	, _T("CONST_DEFAULT_BGM_DIE"));

	SetColor(&lua, &XCONST::CONST_CAMERA_LIGHT_EFFECT_COLOR,			_T("CONST_CAMERA_LIGHT_EFFECT_COLOR"));
	XCONST::CONST_CAMERA_LIGHT_EFFECT_DISTANCE	= lua.GetVar<float>("CONST_CAMERA_LIGHT_EFFECT_DISTANCE");
	XCONST::CONST_CAMERA_LIGHT_EFFECT_RADIUS	= lua.GetVar<float>("CONST_CAMERA_LIGHT_EFFECT_RADIUS");
	XCONST::CONST_CAMERA_LIGHT_EFFECT_ENABLE	= lua.GetVar<bool>("CONST_CAMERA_LIGHT_EFFECT_ENABLE");
	
	SetStringValue(&lua, XCONST::HIT_BASE_BLOOD_EFFECT_NAME, 256, _T("HIT_BASE_BLOOD_EFFECT_NAME"));
	XCONST::HIT_CRITICAL_BLOOD_EFFECT_SIZE		= lua.GetVar<float>("HIT_CRITICAL_BLOOD_EFFECT_SIZE");

	XCONST::IGNORE_NON_TRANSLATION_MARK			= lua.GetVar<bool>("CONST_IGNORE_NON_TRANSLATION_MARK");

	XCONST::INGAME_TIP_MAX_LEVEL				= lua.GetVar<int>("CONST_INGAME_TIP_MAX_LEVEL");
	XCONST::INGAME_TIP_TICK						= lua.GetVar<int>("CONST_INGAME_TIP_TICK");

	InitDefaultEquipments(&lua);
	InitDefaultStyles(&lua);
	InitDefaultTattoo(&lua);
	InitCaptionEffectColor(&lua);

	XCONST::CAPTION_DAMAGE_FONT_SIZE			= lua.GetVar<int>("CONST_CAPTION_DAMAGE_FONT_SIZE");

	InitAgeLimit(&lua);


	// CrashDump	
	if (lua.GetVar<char*>("CONST_CRASH_DUMP_DEST_IP_LOCAL") != NULL)
	{
		XCONST::CONST_CRASH_DUMP_DEST_IP_LOCAL = MLocale::ConvAnsiToTCHAR(lua.GetVar<char*>("CONST_CRASH_DUMP_DEST_IP_LOCAL"));
	}

	if (lua.GetVar<char*>("CONST_CRASH_DUMP_DEST_IP") != NULL)
	{
		XCONST::CONST_CRASH_DUMP_DEST_IP = MLocale::ConvAnsiToTCHAR(lua.GetVar<char*>("CONST_CRASH_DUMP_DEST_IP"));
	}

	XCONST::CONST_CRASH_DUMP_DEST_PORT			= lua.GetVar<int>("CONST_CRASH_DUMP_DEST_PORT");

	XCONST::DECALEFFECT_DEPTH_VALUE				= lua.GetVar<float>("DECALEFFECT_DEPTH_VALUE");

	XCONST::COLLISION_CAPSULE_RADIUS_OFFSET		= lua.GetVar<float>("CONST_COLLISION_CAPSULE_RADIUS_OFFSET");
	XCONST::COLLISION_TERRAIN_FALLING_START_TIME = lua.GetVar<float>("CONST_COLLISION_TERRAIN_FALLING_START_TIME");

	// BPart
	XCONST::BPART_DEFAULT_NPC_ID = lua.GetVar<int>("CONST_BPART_DEFAULT_NPC_ID");
	SetStringValue(&lua, XCONST::BPART_GAIN_BUFF_EFFECT_NAME, 256, _T("CONST_BPART_GAIN_BUFF_EFFECT_NAME"));
	SetStringValue(&lua, XCONST::BPART_LOST_BUFF_EFFECT_NAME, 256, _T("CONST_BPART_LOST_BUFF_EFFECT_NAME"));
	SetStringValue(&lua, XCONST::BPART_LOOTABLE_EFFECT_NAME, 256, _T("CONST_BPART_LOOTABLE_EFFECT_NAME"));
	XCONST::BPART_LOOTABLE_EFFECT_MIN_SIZE = lua.GetVar<int>("CONST_BPART_LOOTABLE_EFFECT_MIN_SIZE");
	XCONST::BPART_LOOTABLE_EFFECT_MAX_SIZE = lua.GetVar<int>("CONST_BPART_LOOTABLE_EFFECT_MAX_SIZE");
	XCONST::BPART_LOOTABLE_EFFECT_SCALE_RATIO = lua.GetVar<float>("CONST_BPART_LOOTABLE_EFFECT_SCALE_RATIO");

	// sound effect
	SetStringValue(&lua, XCONST::SOUND_DODGE_EFFECT_NAME,			256, _T("CONST_SOUND_DODGE_EFFECT_NAME"));
	SetStringValue(&lua, XCONST::SOUND_RIPOSTE_EFFECT_NAME,			256, _T("CONST_SOUND_RIPOSTE_EFFECT_NAME"));
	SetStringValue(&lua, XCONST::SOUND_RESIST_PERFECT_EFFECT_NAME,	256, _T("CONST_SOUND_RESIST_PERFECT_EFFECT_NAME"));
	SetStringValue(&lua, XCONST::SOUND_RESIST_PARTIAL_EFFECT_NAME,	256, _T("CONST_SOUND_RESIST_PARTIAL_EFFECT_NAME"));
	SetStringValue(&lua, XCONST::SOUND_DRAIN_EFFECT_NAME,			256, _T("CONST_SOUND_DRAIN_EFFECT_NAME"));
	SetStringValue(&lua, XCONST::SOUND_IMMUNE_EFFECT_NAME,			256, _T("CONST_SOUND_IMMUNE_EFFECT_NAME"));
	SetStringValue(&lua, XCONST::SOUND_MISS_EFFECT_NAME,			256, _T("CONST_SOUND_MISS_EFFECT_NAME"));
	SetStringValue(&lua, XCONST::SOUND_AVOID_EFFECT_NAME,			256, _T("CONST_SOUND_AVOID_EFFECT_NAME"));


	lua.Close();
}

void XCONST::InitDefaultEquipments(MWLua* pLua)
{
	for ( int i = 0;  i < 10;  i++)
	{
		// Clear
		for ( int j = 0;  j < FEATURE_ITEMSLOT_MAX;  j++)	XCONST::DEFAULT_EQUIPMENTS[ i].items[ j] = 0;
		XCONST::DEFAULT_EQUIPMENTS[ i].count = 0;


		char szName[ 256];
		sprintf_s( szName, "CONST_DEFAULT_EQUIPMENTS_SET%d", i);

		if ( pLua->IsExistGlobal( szName) == false)		continue;

		string strValues = pLua->GetVar<char*>(szName);
		if ( strValues.empty() == true)		continue;
		strValues += ",";


		size_t _find = 0;
		for ( XCONST::DEFAULT_EQUIPMENTS[ i].count = 0;  ;  XCONST::DEFAULT_EQUIPMENTS[ i].count++)
		{
			size_t _next = strValues.find_first_of(",", _find);
			if ( _next == tstring::npos)		break;

			string strValue = strValues.substr( _find, _next - _find);
			_find = _next + 1;

			if ( strValue.empty() == false)
			XCONST::DEFAULT_EQUIPMENTS[ i].items[ XCONST::DEFAULT_EQUIPMENTS[ i].count] = atoi( strValue.c_str());
		}
	}
}

void XCONST::InitDefaultStyles(MWLua* pLua)
{
	for ( int i = 0;  i < 6;  i++)
	{
		// Clear
		for ( int j = 0;  j < FEATURE_ITEMSLOT_MAX;  j++)	XCONST::DEFAULT_STYLES[ i].items[ j] = 0;
		XCONST::DEFAULT_STYLES[ i].count = 0;


		char szName[ 256];
		sprintf_s( szName, "CONST_DEFAULT_STYLES_SET%d", i);

		if ( pLua->IsExistGlobal( szName) == false)		continue;

		string strValues = pLua->GetVar<char*>(szName);
		if ( strValues.empty() == true)		continue;
		strValues += ",";


		size_t _find = 0;
		for ( XCONST::DEFAULT_STYLES[ i].count = 0;  ;  XCONST::DEFAULT_STYLES[ i].count++)
		{
			size_t _next = strValues.find_first_of(",", _find);
			if ( _next == tstring::npos)		break;

			string strValue = strValues.substr( _find, _next - _find);
			_find = _next + 1;

			if ( strValue.empty() == false)
				XCONST::DEFAULT_STYLES[ i].items[ XCONST::DEFAULT_STYLES[ i].count] = atoi( strValue.c_str());
		}
	}
}

void XCONST::InitDefaultTattoo( MWLua* pLua)
{
	for ( int i = 0;  i < 30;  i++)
	{
		char szName[ 256];
		sprintf_s( szName, "CONST_DEFAULT_TATTOO%d", i);
		if ( pLua->IsExistGlobal( szName) == false)		continue;

		string strValues = pLua->GetVar<char*>( szName);
		if ( strValues.empty() == true)					continue;
		XCONST::DEFAULT_TATTOO[ i] = strValues;
	}
}

void XCONST::InitCaptionEffectColor(MWLua* pLua)
{
	SetColor(pLua, &CAPTION_EFFECT_COLOR_MY_DAMAGE,			_T("CONST_CAPTION_EFFECT_COLOR_MY_DAMAGE"));
	SetColor(pLua, &CAPTION_EFFECT_COLOR_MY_CRITICAL,		_T("CONST_CAPTION_EFFECT_COLOR_MY_CRITICAL"));
	SetColor(pLua, &CAPTION_EFFECT_COLOR_MY_MISS,			_T("CONST_CAPTION_EFFECT_COLOR_MY_MISS"));
	SetColor(pLua, &CAPTION_EFFECT_COLOR_ENEMY_DAMAGE,		_T("CONST_CAPTION_EFFECT_COLOR_ENEMY_DAMAGE"));
	SetColor(pLua, &CAPTION_EFFECT_COLOR_ENEMY_CRITICAL,	_T("CONST_CAPTION_EFFECT_COLOR_ENEMY_CRITICAL"));
	SetColor(pLua, &CAPTION_EFFECT_COLOR_ENEMY_MISS,		_T("CONST_CAPTION_EFFECT_COLOR_ENEMY_MISS"));
	SetColor(pLua, &CAPTION_EFFECT_COLOR_MY_BUFF,			_T("CONST_CAPTION_EFFECT_COLOR_MY_BUFF"));
	SetColor(pLua, &CAPTION_EFFECT_COLOR_MY_DEBUFF,			_T("CONST_CAPTION_EFFECT_COLOR_MY_DEBUFF"));
	SetColor(pLua, &CAPTION_EFFECT_COLOR_MY_GUARD_DEFENSE,	_T("CONST_CAPTION_EFFECT_COLOR_MY_GUARD_DEFENSE"));
	SetColor(pLua, &CAPTION_EFFECT_COLOR_MY_HEAL,			_T("CONST_CAPTION_EFFECT_COLOR_MY_HEAL"));
	SetColor(pLua, &CAPTION_EFFECT_COLOR_MY_HP_REGEN,		_T("CONST_CAPTION_EFFECT_COLOR_MY_HP_REGEN"));
	SetColor(pLua, &CAPTION_EFFECT_COLOR_MY_EXP,			_T("CONST_CAPTION_EFFECT_COLOR_MY_EXP"));
	SetColor(pLua, &CAPTION_EFFECT_COLOR_MY_EN,				_T("CONST_CAPTION_EFFECT_COLOR_MY_EN"));
}

void XCONST::SetColor(MWLua* pLua, uint32* poutValue, const TCHAR* szValueName)
{
	if (pLua->IsExistGlobal(MLocale::ConvTCHARToAnsi(szValueName).c_str()) == false) return;
	if (pLua->IsExistGlobalTable(MLocale::ConvTCHARToAnsi(szValueName).c_str()) == false) return;

	MWLua::table t = pLua->GetVar<MWLua::table>(MLocale::ConvTCHARToAnsi(szValueName).c_str());

	vector<int> colors;
	t.iteration();
	int nVar;
	while (t.next<int>(&nVar))  
	{ 
		colors.push_back(nVar);
	} 

	int na = 0, nr = 0, ng = 0, nb = 0;

	if (colors.size() >= 4)
	{
		na = colors[0];
		nr = colors[1];
		ng = colors[2];
		nb = colors[3];
	}
	else
	{
		return;
	}

	if (poutValue)
	{
		*poutValue		= ARGB(na, nr, ng, nb);
	}
}

void XCONST::InitFromExt()
{
	int loading_counter=0;
	TCHAR loadingfilename[_MAX_PATH] = _T("");

	do {
		_stprintf_s(loadingfilename, PATH_UI_LOADING _T("loading%02d.jpg"), loading_counter);
		loading_counter++;
	}
	while( IsExist(loadingfilename) && loading_counter<100);

	LOADING_IMAGE_COUNT = loading_counter-1;
}

void XCONST::InitAgeLimit(MWLua* pLua)
{
	bool bYes	= pLua->GetVar<bool>("PLAYER_18_OVER");

	if(bYes)
		XCONST::PLAYER_AGE_LIMIT = AL_18_OVER;
}

void XCONST::SetFakeBeatenWeapontypeAnimationBlend( MWLua* pLua )
{
	XCONST::HIT_FAKE_BEATEN_WEAPON_TYPE_BLEND_WEIGHT[0] = pLua->GetVar<float>("CONST_HIT_FAKE_BEATEN_1HS_BLEND_WEIGHT");
	XCONST::HIT_FAKE_BEATEN_WEAPON_TYPE_BLEND_WEIGHT[1] = pLua->GetVar<float>("CONST_HIT_FAKE_BEATEN_1HB_BLEND_WEIGHT");
	XCONST::HIT_FAKE_BEATEN_WEAPON_TYPE_BLEND_WEIGHT[2] = pLua->GetVar<float>("CONST_HIT_FAKE_BEATEN_1HP_BLEND_WEIGHT");
	XCONST::HIT_FAKE_BEATEN_WEAPON_TYPE_BLEND_WEIGHT[3] = pLua->GetVar<float>("CONST_HIT_FAKE_BEATEN_2HD_BLEND_WEIGHT");
	XCONST::HIT_FAKE_BEATEN_WEAPON_TYPE_BLEND_WEIGHT[4] = pLua->GetVar<float>("CONST_HIT_FAKE_BEATEN_STA_BLEND_WEIGHT");
	XCONST::HIT_FAKE_BEATEN_WEAPON_TYPE_BLEND_WEIGHT[5] = pLua->GetVar<float>("CONST_HIT_FAKE_BEATEN_ARC_BLEND_WEIGHT");
	XCONST::HIT_FAKE_BEATEN_WEAPON_TYPE_BLEND_WEIGHT[6] = pLua->GetVar<float>("CONST_HIT_FAKE_BEATEN_2HB_BLEND_WEIGHT");
	XCONST::HIT_FAKE_BEATEN_WEAPON_TYPE_BLEND_WEIGHT[7] = pLua->GetVar<float>("CONST_HIT_FAKE_BEATEN_DWD_BLEND_WEIGHT");
	XCONST::HIT_FAKE_BEATEN_WEAPON_TYPE_BLEND_WEIGHT[8] = pLua->GetVar<float>("CONST_HIT_FAKE_BEATEN_DWP_BLEND_WEIGHT");

	XCONST::HIT_FAKE_BEATEN_WEAPON_TYPE_BLEND_SPEED[0] = pLua->GetVar<float>("CONST_HIT_FAKE_BEATEN_1HS_BLEND_SPEED");
	XCONST::HIT_FAKE_BEATEN_WEAPON_TYPE_BLEND_SPEED[1] = pLua->GetVar<float>("CONST_HIT_FAKE_BEATEN_1HB_BLEND_SPEED");
	XCONST::HIT_FAKE_BEATEN_WEAPON_TYPE_BLEND_SPEED[2] = pLua->GetVar<float>("CONST_HIT_FAKE_BEATEN_1HP_BLEND_SPEED");
	XCONST::HIT_FAKE_BEATEN_WEAPON_TYPE_BLEND_SPEED[3] = pLua->GetVar<float>("CONST_HIT_FAKE_BEATEN_2HD_BLEND_SPEED");
	XCONST::HIT_FAKE_BEATEN_WEAPON_TYPE_BLEND_SPEED[4] = pLua->GetVar<float>("CONST_HIT_FAKE_BEATEN_STA_BLEND_SPEED");
	XCONST::HIT_FAKE_BEATEN_WEAPON_TYPE_BLEND_SPEED[5] = pLua->GetVar<float>("CONST_HIT_FAKE_BEATEN_ARC_BLEND_SPEED");
	XCONST::HIT_FAKE_BEATEN_WEAPON_TYPE_BLEND_SPEED[6] = pLua->GetVar<float>("CONST_HIT_FAKE_BEATEN_2HB_BLEND_SPEED");
	XCONST::HIT_FAKE_BEATEN_WEAPON_TYPE_BLEND_SPEED[7] = pLua->GetVar<float>("CONST_HIT_FAKE_BEATEN_DWD_BLEND_SPEED");
	XCONST::HIT_FAKE_BEATEN_WEAPON_TYPE_BLEND_SPEED[8] = pLua->GetVar<float>("CONST_HIT_FAKE_BEATEN_DWP_BLEND_SPEED");
}


#define PLAYERMODEL_XML_TAG_MAIET					"maiet"
#define PLAYERMODEL_XML_TAG_DEFAULT_MODEL_PATH		"DefaultModelPath"
#define PLAYERMODEL_XML_TAG_DEFAULT_MESH			"DefaultMesh"
#define PLAYERMODEL_XML_TAG_HAIR					"Hair"
#define PLAYERMODEL_XML_TAG_FACE					"Face"
#define PLAYERMODEL_XML_TAG_HAIR_MODEL				"HairModel"
#define PLAYERMODEL_XML_TAG_FACE_MODEL				"FaceModel"

#define PLAYERMODEL_XML_ATTR_SEX					"sex"
#define PLAYERMODEL_XML_ATTR_PARTS					"parts"
#define PLAYERMODEL_XML_ATTR_COUNT					"count"
#define PLAYERMODEL_XML_ATTR_ID						"id"

bool XPlayerModelConst::Init( const TCHAR* szFileName )
{
	MXml xml;
	if (!xml.LoadFile(MLocale::ConvTCHARToAnsi(szFileName).c_str())) 
	{
		return false;
	}

	MXmlHandle docHandle = xml.DocHandle();
	MXmlElement* pElement = docHandle.FirstChild(PLAYERMODEL_XML_TAG_MAIET).FirstChildElement().Element();

	for( pElement; pElement != NULL; pElement=pElement->NextSiblingElement() )
	{
		if (!_stricmp(pElement->Value(), PLAYERMODEL_XML_TAG_DEFAULT_MODEL_PATH))
		{	
			ParseDefaultModelPath(pElement);
		}
		else if (!_stricmp(pElement->Value(), PLAYERMODEL_XML_TAG_DEFAULT_MESH))
		{	
			ParseDefaultMesh(pElement);
		}
		else if (!_stricmp(pElement->Value(), PLAYERMODEL_XML_TAG_HAIR))
		{	
			ParseHair(pElement);
		}
		else if (!_stricmp(pElement->Value(), PLAYERMODEL_XML_TAG_FACE))
		{	
			ParseFace(pElement);
		}

	}

	return true;
}

void XPlayerModelConst::ParseDefaultModelPath( MXmlElement* pElement )
{
	tstring strSex;
	if (_Attribute(strSex, pElement, PLAYERMODEL_XML_ATTR_SEX) == false) return;

	SEX nSex = TransSex(strSex.c_str());
	if (nSex == SEX_NA) return;

	string strValue;
	_Contents(strValue, pElement);

	Info[nSex].MODEL_NAME = MLocale::ConvAnsiToTCHAR(strValue.c_str());

	string nn = strValue.substr(strValue.rfind("/") + 1, strValue.rfind(".") - strValue.rfind("/") - 1);

	Info[nSex].MODEL_NAME_SIMPLE = MLocale::ConvAnsiToTCHAR(nn.c_str());
}

void XPlayerModelConst::ParseDefaultMesh( MXmlElement* pElement )
{
	string strSex;
	if (_Attribute(strSex, pElement, PLAYERMODEL_XML_ATTR_SEX) == false) return;

	SEX nSex = TransSex(MLocale::ConvAnsiToTCHAR(strSex.c_str()).c_str());
	if (nSex == SEX_NA) return;

	string strParts;
	if (_Attribute(strParts, pElement, PLAYERMODEL_XML_ATTR_PARTS) == false) return;

	string strValue;
	_Contents(strValue, pElement);

	if (!_stricmp(strValue.c_str(), "hat"))
	{
		Info[nSex].DEFAULT_MESHNAME_HAT = MLocale::ConvAnsiToTCHAR(strValue.c_str());
	}
	else if (!_stricmp(strValue.c_str(), "chest"))
	{
		Info[nSex].DEFAULT_MESHNAME_CHEST = MLocale::ConvAnsiToTCHAR(strValue.c_str());
	}
	else if (!_stricmp(strValue.c_str(), "hands"))
	{
		Info[nSex].DEFAULT_MESHNAME_HANDS = MLocale::ConvAnsiToTCHAR(strValue.c_str());
	}
	else if (!_stricmp(strValue.c_str(), "legs"))
	{
		Info[nSex].DEFAULT_MESHNAME_LEGS = MLocale::ConvAnsiToTCHAR(strValue.c_str());
	}
	else if (!_stricmp(strValue.c_str(), "feet"))
	{
		Info[nSex].DEFAULT_MESHNAME_FEET = MLocale::ConvAnsiToTCHAR(strValue.c_str());
	}
}

void XPlayerModelConst::ParseHair( MXmlElement* pElement )
{
	tstring strSex;
	if (_Attribute(strSex, pElement, PLAYERMODEL_XML_ATTR_SEX) == false) return;

	SEX nSex = TransSex(strSex.c_str());
	if (nSex == SEX_NA) return;

	int nCount = 0;
	if (_Attribute(&nCount, pElement, PLAYERMODEL_XML_ATTR_COUNT) == false) return;
	if (nCount <= 0) return;

	Info[nSex].HAIR_MODEL.resize(nCount);

	MXmlElement* pChild = pElement->FirstChildElement();
	for( pChild; pChild != NULL; pChild=pChild->NextSiblingElement() )
	{
		if (!_stricmp(pChild->Value(), PLAYERMODEL_XML_TAG_HAIR_MODEL))
		{
			int nID = -1;
			if (_Attribute(&nID, pChild, PLAYERMODEL_XML_ATTR_ID) == false) continue;

			// id는 1부터 시작
			int nIndex = nID - 1;

			string strValue;
			_Contents(strValue, pChild);

			if (nIndex >= 0 && nIndex < nCount)
			{
				Info[nSex].HAIR_MODEL[nIndex]  = MLocale::ConvAnsiToTCHAR(strValue.c_str());
			}
		}
	}
}

void XPlayerModelConst::ParseFace( MXmlElement* pElement )
{
	tstring strSex;
	if (_Attribute(strSex, pElement, PLAYERMODEL_XML_ATTR_SEX) == false) return;

	SEX nSex = TransSex(strSex.c_str());
	if (nSex == SEX_NA) return;

	int nCount = 0;
	if (_Attribute(&nCount, pElement, PLAYERMODEL_XML_ATTR_COUNT) == false) return;
	if (nCount <= 0) return;

	Info[nSex].FACE_MODEL.resize(nCount);

	MXmlElement* pChild = pElement->FirstChildElement();
	for( pChild; pChild != NULL; pChild=pChild->NextSiblingElement() )
	{
		if (!_stricmp(pChild->Value(), PLAYERMODEL_XML_TAG_FACE_MODEL))
		{
			int nID = -1;
			if (_Attribute(&nID, pChild, PLAYERMODEL_XML_ATTR_ID) == false) continue;

			// id는 1부터 시작
			int nIndex = nID - 1;

			string strValue;
			_Contents(strValue, pChild);

			if (nIndex >= 0 && nIndex < nCount)
			{
				Info[nSex].FACE_MODEL[nIndex]  = MLocale::ConvAnsiToTCHAR(strValue.c_str());
			}
		}
	}
}

SEX XPlayerModelConst::TransSex( const TCHAR* szValue )
{
	if (!_tcsicmp(szValue, _T("male")))
	{
		return SEX_MALE;
	}
	else if (!_tcsicmp(szValue, _T("female")))
	{
		return SEX_FEMALE;
	}
	return SEX_NA;
}

