#include "stdafx.h"
#include "TConst.h"
#include "RTreeDef.h"
#include "FileInfo.h"
#include "MFileSystem.h"
#include "MLocale.h"

TPlayerModelConst	TCONST::PlayerModel;
TDefaultEquipments	TCONST::DEFAULT_EQUIPMENTS[ 5];

// 상수 기본값 정의
int		TCONST::CAMERA_FOV				= 90;
float	TCONST::CAMERA_NEAR_Z			= 10.0f;
float	TCONST::CAMERA_FAR_Z			= 10000.0f;
float	TCONST::GRASS_LOD_NEAR			= 2000.0f;
float	TCONST::GRASS_LOD_FAR			= 3000.0f;
TCHAR	TCONST::PATH_TEXTURE[1024]		= DEFAULT_PATH_TEXTURES_TOOL;

float	TCONST::DOF_FOCUS_DIST			= 100.0f;
float	TCONST::DOF_FOCUS_RANGE			= 2500.0f;
float	TCONST::DOF_COC_SIZE			= 5.0;
float	TCONST::CHAR_DOF_FOCUS_DIST		= 100.0f;
float	TCONST::CHAR_DOF_FOCUS_RANGE	= 2500.0f;
float	TCONST::CHAR_DOF_COC_SIZE		= 5.0;

float	TCONST::MOVE_SPEED_NONE			= 550.0f;
float	TCONST::MOVE_SPEED_1HS			= 360.0f;
float	TCONST::MOVE_SPEED_1HB			= 360.0f;
float	TCONST::MOVE_SPEED_1HP			= 360.0f;
float	TCONST::MOVE_SPEED_2HD			= 360.0f;
float	TCONST::MOVE_SPEED_STF			= 360.0f;
float	TCONST::MOVE_SPEED_ARC			= 360.0f;
float	TCONST::MOVE_SPEED_2HB			= 360.0f;
float	TCONST::MOVE_SPEED_DWD			= 360.0f;
float	TCONST::MOVE_SPEED_DWP			= 360.0f;

float	TCONST::MOVE_BACK_SPEED_NONE	= 550.0f;
float	TCONST::MOVE_BACK_SPEED_1HS		= 360.0f;
float	TCONST::MOVE_BACK_SPEED_1HB		= 360.0f;
float	TCONST::MOVE_BACK_SPEED_1HP		= 360.0f;
float	TCONST::MOVE_BACK_SPEED_2HD		= 360.0f;
float	TCONST::MOVE_BACK_SPEED_STF		= 360.0f;
float	TCONST::MOVE_BACK_SPEED_ARC		= 360.0f;
float	TCONST::MOVE_BACK_SPEED_2HB		= 360.0f;
float	TCONST::MOVE_BACK_SPEED_DWD		= 360.0f;
float	TCONST::MOVE_BACK_SPEED_DWP		= 360.0f;

float	TCONST::JUMP_SPEED				= 680.0f;
float	TCONST::MOTION_FACTOR_KNOCKBACK_MOTION_EVENT_DIST = 400.0f;
float	TCONST::MOTION_FACTOR_THROWUP_MOTION_EVENT_DIST = 800.0f;
float	TCONST::HIT_FREEZE_TIME			= 0.12f;float	TCONST::HIT_FREEZE_DELAY		= 0.0f;

float	TCONST::MIN_CAMERA_DIST			= 50.0f;
float	TCONST::MAX_CAMERA_DIST			= 500.0f;
float	TCONST::CAMERA_TARGET_DIST		= 240.0f;
float	TCONST::CAMERA_TARGET_HEIGHT	= 140.0f;
float	TCONST::CAMERA_TARGET_LEFT		= 0.0f;
float	TCONST::MOUSE_WHEEL_SENSIBILITY = 0.8f;

bool	TCONST::SEX_MALE						= false;
int		TCONST::LOADING_IMAGE_COUNT				= 1;

TCHAR	TCONST::STANDALONE_EQUIP_ITEMS[256] = _T("");
bool	TCONST::SHOW_UI					= true;
bool	TCONST::CAMERA_FIX				= true;
bool	TCONST::DEBUG_EXECUTE_UNITTEST = false;
bool	TCONST::DEBUG_LOG = false;
bool	TCONST::USE_NET_DEBUGGER = false;
bool	TCONST::USE_PHYSX_REMOTE_DEBUGGER = false;
bool	TCONST::AUTO_REPORT_CRASHDUMP = false;
bool	TCONST::FULL_DUMP = false;

vec3	TCONST::HIT_EFFECT_MODEL_ADD_COLOR = vec3::ZERO;
bool	TCONST::USE_HIT_EFFECT_MODEL_ADD_COLOR = false;
float	TCONST::HIT_EFFECT_MODEL_ADD_COLOR_TIME = 0.0f;
vec3	TCONST::HIT_EFFECT_BPARTS_ADD_COLOR = vec3::ZERO;

TCHAR	TCONST::HIT_FAKE_BEATEN_DEFAULT_ANI_NAME[256] = _T("MF_BT2");
float	TCONST::HIT_FAKE_BEATEN_ANIMATION_BLEND_WEIGHT = 0.5f;
float	TCONST::HIT_HUGE_FAKE_BEATEN_ANIMATION_BLEND_WEIGHT = 0.2f;
float	TCONST::HIT_FAKE_BEATEN_ANIMATION_BLEND_SPEED = 1.0f;
float	TCONST::HIT_HUGE_FAKE_BEATEN_ANIMATION_BLEND_SPEED = 1.0f;
float	TCONST::HIT_FAKE_BEATEN_ANIMATION_BLEND_CIRITICAL_WEIGHT = 1.3f;
float	TCONST::HIT_FAKE_BEATEN_ANIMATION_BLEND_CIRITICAL_SPEED = 1.0f;

float	TCONST::HIT_FAKE_BEATEN_WEAPON_TYPE_BLEND_WEIGHT[9] = {1.0f, };
float	TCONST::HIT_FAKE_BEATEN_WEAPON_TYPE_BLEND_SPEED[9] = {1.0f, };

float	TCONST::MOVE_MOTION_SPEED_RATIO = 1.0f;

uint32	TCONST::SCREEN_DEBUG_FONT_COLOR		= 0xFFFFFFFF;		// 흰색
int		TCONST::SCREEN_DEBUG_TOP			= 10;

TCHAR	TCONST::DEFAULT_UI_FONT[256] = _T("굴림");
int		TCONST::DEFAULT_UI_FONT_HEIGHT = 12;

TCHAR	TCONST::CHARACTER_CAPTION_FONT_NAME[256] = _T("굴림");
TCHAR	TCONST::CHARACTER_CAPTION_FONT_FILE[256] = _T("");
int		TCONST::CHARACTER_CAPTION_FONT_HEIGHT = 12;

TCHAR	TCONST::SENSOR_EFFECT_NAME[256] = _T("");
TCHAR	TCONST::INTERACT_QUEST_OBJECTIVE_ANI_NAME[256] = _T("");
float	TCONST::INTERACT_QUEST_OBJECTIVE_ANI_TIME = 2.0f;

bool	TCONST::CONST_MAKE_COMMAND_RESULT = false;
bool	TCONST::DEBUG_LOG_COMMAND = false;
bool	TCONST::AUTO_SAVE_REPLAY = false;
bool	TCONST::COMMAND_PROFILER_AUTO_SAVE = false;

uint32	TCONST::CAPTION_EFFECT_COLOR_MY_DAMAGE = 0;
uint32	TCONST::CAPTION_EFFECT_COLOR_MY_CRITICAL = 0;
uint32	TCONST::CAPTION_EFFECT_COLOR_ENEMY_DAMAGE = 0;
uint32	TCONST::CAPTION_EFFECT_COLOR_ENEMY_CRITICAL = 0;
uint32	TCONST::CAPTION_EFFECT_COLOR_MY_BUFF = 0;
uint32	TCONST::CAPTION_EFFECT_COLOR_MY_DEBUFF = 0;
uint32	TCONST::CAPTION_EFFECT_COLOR_MY_GUARD_DEFENSE = 0;
uint32	TCONST::CAPTION_EFFECT_COLOR_MY_HEAL = 0;
uint32	TCONST::CAPTION_EFFECT_COLOR_MY_HP_REGEN = 0;
uint32	TCONST::CAPTION_EFFECT_COLOR_MY_EXP = 0;
uint32	TCONST::CAPTION_EFFECT_COLOR_MY_EN = 0;

int		TCONST::CAPTION_DAMAGE_FONT_SIZE = 25;

int		TCONST::REFILL_POTION_HP_ITEMID = 32001;
int		TCONST::REFILL_POTION_MP_ITEMID = 32002;
int		TCONST::REFILL_POTION_SP_ITEMID = 32003;

bool	TCONST::USING_COMPILED_SHADER = true;
bool	TCONST::RESOURCE_LOADING_LOG = false;

TCHAR	TCONST::DIEBGMNAME[256] = _T("bgm_die_1");

float	TCONST::CONST_CAMERA_LIGHT_EFFECT_DISTANCE = 50.0f;
float 	TCONST::CONST_CAMERA_LIGHT_EFFECT_RADIUS = 50.0f;
uint32	TCONST::CONST_CAMERA_LIGHT_EFFECT_COLOR = 0xffffffff;
bool	TCONST::CONST_CAMERA_LIGHT_EFFECT_ENABLE = false;

TCHAR	TCONST::HIT_BASE_BLOOD_EFFECT_NAME[256] = _T("");
float	TCONST::HIT_CRITICAL_BLOOD_EFFECT_SIZE = 2.0f;

AGE_LIMIT TCONST::PLAYER_AGE_LIMIT = AL_18_UNDER;

// CrashDump
tstring TCONST::CONST_CRASH_DUMP_DEST_IP_LOCAL = _T("icecream");
tstring TCONST::CONST_CRASH_DUMP_DEST_IP= _T("icecream");
int TCONST::CONST_CRASH_DUMP_DEST_PORT = 21;

float	TCONST::DECALEFFECT_DEPTH_VALUE = 100.0f;		// 1미터

float	TCONST::COLLISION_CAPSULE_RADIUS_OFFSET = 0.0f;

vec4	TCONST::INTERACTION_TARGET_EDGE_COLOR = vec4( 0.77f, 0.76f, 0.45f, 1.0f);
float	TCONST::INTERACTION_TARGET_EDGE_TICKNESS = 1.2f;
float	TCONST::INTERACTION_TARGET_EDGE_BLUR_FACTOR = 1.9f;

vec4	TCONST::GROUND_TARGET_EDGE_COLOR = vec4(0.69f, 0.7f, 0.93f, 1.0f);


#define CONST_XML_TAG_CONST			"CONST"
#define CONST_XML_TAG_MAIET			"maiet"

void TCONST::Init()
{
	InitFromScript();
	//InitFromExt();
	//if (PlayerModel.Init(FILENAME_PLAYER_MODEL) == false)
	//{
	//	mlog("Failed Open File : %s\n", MLocale::ConvTCHARToAnsi(FILENAME_PLAYER_MODEL).c_str());
	//}
}

void TCONST::SetStringValue( MWLua* pLua, TCHAR* szoutValue, int nValueLen, const TCHAR* szScriptValueName )
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


void TCONST::InitFromScript()
{
	MWLua lua;
	lua.Open();

	if ( true)
	{
		MFile mf;
		if ( mf.Open(MLocale::ConvTCHARToAnsi(FILENAME_SCRIPT_CONST_REL_TOOL).c_str()) == true)
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
		if ( mf.Open(MLocale::ConvTCHARToAnsi(FILENAME_SCRIPT_CONST_TOOL).c_str()) == true)
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


	TCONST::CAMERA_FOV					= lua.GetVar<int>("CONST_CAMERA_FOV");
	TCONST::CAMERA_NEAR_Z				= lua.GetVar<float>("CONST_CAMERA_NEAR_Z");
	TCONST::CAMERA_FAR_Z				= lua.GetVar<float>("CONST_CAMERA_FAR_Z");
	TCONST::GRASS_LOD_FAR				= lua.GetVar<float>("CONST_GRASS_LOD_FAR");
	if (lua.GetVar<char*>("CONST_PATH_TEXTURE") != NULL)
		_tcscpy_s(TCONST::PATH_TEXTURE, MLocale::ConvAnsiToTCHAR(lua.GetVar<char*>("CONST_PATH_TEXTURE")).c_str());

	TCONST::DOF_FOCUS_DIST				= lua.GetVar<float>("CONST_DOF_FOCUS_DIST");
	TCONST::DOF_FOCUS_RANGE				= lua.GetVar<float>("CONST_DOF_FOCUS_RANGE");
	TCONST::DOF_COC_SIZE				= lua.GetVar<float>("CONST_DOF_COC_SIZE");
	TCONST::CHAR_DOF_FOCUS_DIST			= lua.GetVar<float>("CONST_CHAR_DOF_FOCUS_DIST");
	TCONST::CHAR_DOF_FOCUS_RANGE		= lua.GetVar<float>("CONST_CHAR_DOF_FOCUS_RANGE");
	TCONST::CHAR_DOF_COC_SIZE			= lua.GetVar<float>("CONST_CHAR_DOF_COC_SIZE");

	TCONST::MOVE_SPEED_NONE				= lua.GetVar<float>("CONST_MOVE_SPEED_NONE");
	TCONST::MOVE_SPEED_1HS				= lua.GetVar<float>("CONST_MOVE_SPEED_1HS");
	TCONST::MOVE_SPEED_1HB				= lua.GetVar<float>("CONST_MOVE_SPEED_1HB");
	TCONST::MOVE_SPEED_1HP				= lua.GetVar<float>("CONST_MOVE_SPEED_1HP");
	TCONST::MOVE_SPEED_2HD				= lua.GetVar<float>("CONST_MOVE_SPEED_2HD");
	TCONST::MOVE_SPEED_STF				= lua.GetVar<float>("CONST_MOVE_SPEED_STF");
	TCONST::MOVE_SPEED_ARC				= lua.GetVar<float>("CONST_MOVE_SPEED_ARC");
	TCONST::MOVE_SPEED_2HB				= lua.GetVar<float>("CONST_MOVE_SPEED_2HB");
	TCONST::MOVE_SPEED_DWD				= lua.GetVar<float>("CONST_MOVE_SPEED_DWD");
	TCONST::MOVE_SPEED_DWP				= lua.GetVar<float>("CONST_MOVE_SPEED_DWP");

	TCONST::MOVE_BACK_SPEED_NONE		= lua.GetVar<float>("CONST_MOVE_BACK_SPEED_NONE");
	TCONST::MOVE_BACK_SPEED_1HS			= lua.GetVar<float>("CONST_MOVE_BACK_SPEED_1HS");
	TCONST::MOVE_BACK_SPEED_1HB			= lua.GetVar<float>("CONST_MOVE_BACK_SPEED_1HB");
	TCONST::MOVE_BACK_SPEED_1HP			= lua.GetVar<float>("CONST_MOVE_BACK_SPEED_1HP");
	TCONST::MOVE_BACK_SPEED_2HD			= lua.GetVar<float>("CONST_MOVE_BACK_SPEED_2HD");
	TCONST::MOVE_BACK_SPEED_STF			= lua.GetVar<float>("CONST_MOVE_BACK_SPEED_STF");
	TCONST::MOVE_BACK_SPEED_ARC			= lua.GetVar<float>("CONST_MOVE_BACK_SPEED_ARC");
	TCONST::MOVE_BACK_SPEED_2HB			= lua.GetVar<float>("CONST_MOVE_BACK_SPEED_2HB");
	TCONST::MOVE_BACK_SPEED_DWD			= lua.GetVar<float>("CONST_MOVE_BACK_SPEED_DWD");
	TCONST::MOVE_BACK_SPEED_DWP			= lua.GetVar<float>("CONST_MOVE_BACK_SPEED_DWP");

	TCONST::MOTION_FACTOR_KNOCKBACK_MOTION_EVENT_DIST				= lua.GetVar<float>("CONST_MOTION_FACTOR_KNOCKBACK_MOTION_EVENT_DIST");
	TCONST::MOTION_FACTOR_THROWUP_MOTION_EVENT_DIST					= lua.GetVar<float>("CONST_MOTION_FACTOR_THROWUP_MOTION_EVENT_DIST");

	TCONST::MIN_CAMERA_DIST				= lua.GetVar<float>("CONST_CAMERA_MIN_DIST");
	TCONST::MAX_CAMERA_DIST				= lua.GetVar<float>("CONST_CAMERA_MAX_DIST");
	TCONST::CAMERA_TARGET_DIST			= lua.GetVar<float>("CONST_CAMERA_TARGET_DIST");
	TCONST::CAMERA_TARGET_HEIGHT		= lua.GetVar<float>("CONST_CAMERA_TARGET_HEIGHT");
	TCONST::CAMERA_TARGET_LEFT			= lua.GetVar<float>("CONST_CAMERA_TARGET_LEFT");
	TCONST::MOUSE_WHEEL_SENSIBILITY		= lua.GetVar<float>("CONST_MOUSE_WHEEL_SENSIBILITY");

	TCONST::HIT_FREEZE_TIME				= lua.GetVar<float>("HIT_FREEZE_TIME");
	TCONST::HIT_FREEZE_DELAY			= lua.GetVar<float>("HIT_FREEZE_DELAY");

	SetStringValue(&lua, TCONST::STANDALONE_EQUIP_ITEMS, 64, _T("CONST_STANDALONE_EQUIP_ITEMS"));
	TCONST::SEX_MALE = lua.GetVar<bool>("CONST_SEX_MALE");


	SetColor(&lua, &TCONST::SCREEN_DEBUG_FONT_COLOR, _T("CONST_SCREEN_DEBUG_FONT_COLOR"));

#ifndef _PUBLISH
	TCONST::SHOW_UI						= lua.GetVar<bool>("CONST_SHOW_UI");
	TCONST::CAMERA_FIX					= lua.GetVar<bool>("CONST_CAMERA_FIX");
	TCONST::DEBUG_EXECUTE_UNITTEST		= lua.GetVar<bool>("CONST_DEBUG_EXECUTE_UNITTEST");
	TCONST::DEBUG_LOG					= lua.GetVar<bool>("CONST_DEBUG_LOG");
	TCONST::USE_NET_DEBUGGER			= lua.GetVar<bool>("CONST_USE_NET_DEBUGGER");
	TCONST::USE_PHYSX_REMOTE_DEBUGGER	= lua.GetVar<bool>("CONST_USE_PHYSX_REMOTE_DEBUGGER");
	TCONST::CONST_MAKE_COMMAND_RESULT	= lua.GetVar<bool>("CONST_MAKE_COMMAND_RESULT");
	TCONST::DEBUG_LOG_COMMAND			= lua.GetVar<bool>("CONST_LOG_COMMAND");
	TCONST::AUTO_SAVE_REPLAY			= lua.GetVar<bool>("CONST_AUTO_SAVE_REPLAY");
	TCONST::COMMAND_PROFILER_AUTO_SAVE	= lua.GetVar<bool>("CONST_COMMAND_PROFILER_AUTO_SAVE");
	TCONST::MOVE_MOTION_SPEED_RATIO		= lua.GetVar<float>("MOVE_MOTION_SPEED_RATIO");
#endif

	TCONST::USE_HIT_EFFECT_MODEL_ADD_COLOR	= lua.GetVar<bool>("CONST_USE_HIT_EFFECT_MODEL_ADD_COLOR");
	TCONST::HIT_EFFECT_MODEL_ADD_COLOR_TIME	= lua.GetVar<float>("CONST_HIT_EFFECT_MODEL_ADD_COLOR_TIME");
	TCONST::HIT_EFFECT_MODEL_ADD_COLOR.x	= lua.GetVar<float>("CONST_HIT_EFFECT_MODEL_ADD_COLOR_R");
	TCONST::HIT_EFFECT_MODEL_ADD_COLOR.y	= lua.GetVar<float>("CONST_HIT_EFFECT_MODEL_ADD_COLOR_G");
	TCONST::HIT_EFFECT_MODEL_ADD_COLOR.z	= lua.GetVar<float>("CONST_HIT_EFFECT_MODEL_ADD_COLOR_B");

	TCONST::HIT_EFFECT_BPARTS_ADD_COLOR.x	= lua.GetVar<float>("CONST_HIT_EFFECT_BPARTS_ADD_COLOR_R");
	TCONST::HIT_EFFECT_BPARTS_ADD_COLOR.y	= lua.GetVar<float>("CONST_HIT_EFFECT_BPARTS_ADD_COLOR_G");
	TCONST::HIT_EFFECT_BPARTS_ADD_COLOR.z	= lua.GetVar<float>("CONST_HIT_EFFECT_BPARTS_ADD_COLOR_B");

	SetStringValue(&lua, TCONST::HIT_FAKE_BEATEN_DEFAULT_ANI_NAME, 256, _T("CONST_HIT_FAKE_BEATEN_DEFAULT_ANI_NAME"));
	TCONST::HIT_FAKE_BEATEN_ANIMATION_BLEND_WEIGHT	= lua.GetVar<float>("CONST_HIT_FAKE_BEATEN_ANIMATION_BLEND_WEIGHT");
	TCONST::HIT_HUGE_FAKE_BEATEN_ANIMATION_BLEND_WEIGHT	= lua.GetVar<float>("CONST_HIT_HUGE_FAKE_BEATEN_ANIMATION_BLEND_WEIGHT");
	TCONST::HIT_FAKE_BEATEN_ANIMATION_BLEND_SPEED = lua.GetVar<float>("CONST_HIT_FAKE_BEATEN_ANIMATION_BLEND_SPEED");
	TCONST::HIT_HUGE_FAKE_BEATEN_ANIMATION_BLEND_SPEED = lua.GetVar<float>("CONST_HIT_HUGE_FAKE_BEATEN_ANIMATION_BLEND_SPEED");

	TCONST::HIT_FAKE_BEATEN_ANIMATION_BLEND_CIRITICAL_WEIGHT = lua.GetVar<float>("CONST_HIT_FAKE_BEATEN_ANIMATION_BLEND_CRITICAL_WEIGHT");
	TCONST::HIT_FAKE_BEATEN_ANIMATION_BLEND_CIRITICAL_SPEED = lua.GetVar<float>("CONST_HIT_FAKE_BEATEN_ANIMATION_BLEND_CRITICAL_SPEED");
	

	SetFakeBeatenWeapontypeAnimationBlend(&lua);

	TCONST::INTERACTION_TARGET_EDGE_COLOR.r	= lua.GetVar<float>("CONST_INTERACTION_TARGET_EDGE_COLOR_R");
	TCONST::INTERACTION_TARGET_EDGE_COLOR.g	= lua.GetVar<float>("CONST_INTERACTION_TARGET_EDGE_COLOR_G");
	TCONST::INTERACTION_TARGET_EDGE_COLOR.b	= lua.GetVar<float>("CONST_INTERACTION_TARGET_EDGE_COLOR_B");
	TCONST::INTERACTION_TARGET_EDGE_COLOR.a	= lua.GetVar<float>("CONST_INTERACTION_TARGET_EDGE_COLOR_A");
	TCONST::INTERACTION_TARGET_EDGE_TICKNESS	= lua.GetVar<float>("CONST_INTERACTION_TARGET_EDGE_TICKNESS");
	TCONST::INTERACTION_TARGET_EDGE_BLUR_FACTOR	= lua.GetVar<float>("CONST_INTERACTION_TARGET_EDGE_BLUR_FACTOR");

	TCONST::GROUND_TARGET_EDGE_COLOR.r	= lua.GetVar<float>("CONST_GROUND_TARGET_EDGE_COLOR_R");
	TCONST::GROUND_TARGET_EDGE_COLOR.g	= lua.GetVar<float>("CONST_GROUND_TARGET_EDGE_COLOR_G");
	TCONST::GROUND_TARGET_EDGE_COLOR.b	= lua.GetVar<float>("CONST_GROUND_TARGET_EDGE_COLOR_B");
	TCONST::GROUND_TARGET_EDGE_COLOR.a	= lua.GetVar<float>("CONST_GROUND_TARGET_EDGE_COLOR_A");

	// RealSpace3
	TCONST::USING_COMPILED_SHADER		= lua.GetVar<bool>("CONST_USING_COMPILED_SHADER");
	TCONST::RESOURCE_LOADING_LOG		= lua.GetVar<bool>("CONST_RESOURCE_LOADING_LOG");

	// Font
	SetStringValue(&lua, TCONST::DEFAULT_UI_FONT, 256, _T("CONST_DEFAULT_FONT_NAME"));
	TCONST::DEFAULT_UI_FONT_HEIGHT = lua.GetVar<int>("CONST_DEFAULT_FONT_HEIGHT");

	SetStringValue(&lua, TCONST::CHARACTER_CAPTION_FONT_NAME, 256, _T("CONST_CHARACTER_CAPTION_FONT_NAME"));
	SetStringValue(&lua, TCONST::CHARACTER_CAPTION_FONT_FILE, 256, _T("CONST_CHARACTER_CAPTION_FONT_FILE"));
	TCONST::CHARACTER_CAPTION_FONT_HEIGHT = lua.GetVar<int>("CONST_CHARACTER_CAPTION_FONT_HEIGHT");

	SetStringValue(&lua, TCONST::SENSOR_EFFECT_NAME, 256, _T("CONST_SENSOR_EFFECT_NAME"));
	SetStringValue(&lua, TCONST::INTERACT_QUEST_OBJECTIVE_ANI_NAME, 256, _T("CONST_INTERACT_QUEST_OBJECTIVE_ANI_NAME"));
	TCONST::INTERACT_QUEST_OBJECTIVE_ANI_TIME	= lua.GetVar<float>("CONST_INTERACT_QUEST_OBJECTIVE_ANI_TIME");

	TCONST::FULL_DUMP						= lua.GetVar<bool>("CONST_FULL_DUMP");

	SetStringValue(&lua, TCONST::DIEBGMNAME		, 256	, _T("CONST_DEFAULT_BGM_DIE"));

	SetColor(&lua, &TCONST::CONST_CAMERA_LIGHT_EFFECT_COLOR,			_T("CONST_CAMERA_LIGHT_EFFECT_COLOR"));
	TCONST::CONST_CAMERA_LIGHT_EFFECT_DISTANCE	= lua.GetVar<float>("CONST_CAMERA_LIGHT_EFFECT_DISTANCE");
	TCONST::CONST_CAMERA_LIGHT_EFFECT_RADIUS	= lua.GetVar<float>("CONST_CAMERA_LIGHT_EFFECT_RADIUS");
	TCONST::CONST_CAMERA_LIGHT_EFFECT_ENABLE	= lua.GetVar<bool>("CONST_CAMERA_LIGHT_EFFECT_ENABLE");
	
	SetStringValue(&lua, TCONST::HIT_BASE_BLOOD_EFFECT_NAME, 256, _T("HIT_BASE_BLOOD_EFFECT_NAME"));
	TCONST::HIT_CRITICAL_BLOOD_EFFECT_SIZE		= lua.GetVar<float>("HIT_CRITICAL_BLOOD_EFFECT_SIZE");

	InitDefaultEquipments(&lua);
	InitCaptionEffectColor(&lua);

	TCONST::CAPTION_DAMAGE_FONT_SIZE			= lua.GetVar<int>("CONST_CAPTION_DAMAGE_FONT_SIZE");

	InitAgeLimit(&lua);


	// CrashDump	
	if (lua.GetVar<char*>("CONST_CRASH_DUMP_DEST_IP_LOCAL") != NULL)
	{
		TCONST::CONST_CRASH_DUMP_DEST_IP_LOCAL = MLocale::ConvAnsiToTCHAR(lua.GetVar<char*>("CONST_CRASH_DUMP_DEST_IP_LOCAL"));
	}

	if (lua.GetVar<char*>("CONST_CRASH_DUMP_DEST_IP") != NULL)
	{
		TCONST::CONST_CRASH_DUMP_DEST_IP = MLocale::ConvAnsiToTCHAR(lua.GetVar<char*>("CONST_CRASH_DUMP_DEST_IP"));
	}

	TCONST::CONST_CRASH_DUMP_DEST_PORT			= lua.GetVar<int>("CONST_CRASH_DUMP_DEST_PORT");

	TCONST::DECALEFFECT_DEPTH_VALUE				= lua.GetVar<float>("DECALEFFECT_DEPTH_VALUE");

	TCONST::COLLISION_CAPSULE_RADIUS_OFFSET		= lua.GetVar<float>("COLLISION_CAPSULE_RADIUS_OFFSET");

	lua.Close();
}

void TCONST::InitDefaultEquipments(MWLua* pLua)
{
	for ( int i = 0;  i < 5;  i++)
	{
		// Clear
		for ( int j = 0;  j < FEATURE_ITEMSLOT_MAX;  j++)	TCONST::DEFAULT_EQUIPMENTS[ i].items[ j] = 0;
		TCONST::DEFAULT_EQUIPMENTS[ i].count = 0;


		char szName[ 256];
		sprintf_s( szName, "CONST_DEFAULT_EQUIPMENTS_SET%d", i);

		string strValues = pLua->GetVar<char*>(szName);
		if ( strValues.empty() == true)		continue;
		strValues += ",";


		size_t _find = 0;
		for ( TCONST::DEFAULT_EQUIPMENTS[ i].count = 0;  ;  TCONST::DEFAULT_EQUIPMENTS[ i].count++)
		{
			size_t _next = strValues.find_first_of(",", _find);
			if ( _next == tstring::npos)		break;

			string strValue = strValues.substr( _find, _next - _find);
			_find = _next + 1;

			if ( strValue.empty() == false)
			TCONST::DEFAULT_EQUIPMENTS[ i].items[ TCONST::DEFAULT_EQUIPMENTS[ i].count] = atoi( strValue.c_str());
		}
	}
}

void TCONST::InitCaptionEffectColor(MWLua* pLua)
{
	SetColor(pLua, &CAPTION_EFFECT_COLOR_MY_DAMAGE,			_T("CONST_CAPTION_EFFECT_COLOR_MY_DAMAGE"));
	SetColor(pLua, &CAPTION_EFFECT_COLOR_MY_CRITICAL,		_T("CONST_CAPTION_EFFECT_COLOR_MY_DAMAGE"));
	SetColor(pLua, &CAPTION_EFFECT_COLOR_ENEMY_DAMAGE,		_T("CONST_CAPTION_EFFECT_COLOR_ENEMY_DAMAGE"));
	SetColor(pLua, &CAPTION_EFFECT_COLOR_ENEMY_CRITICAL,	_T("CONST_CAPTION_EFFECT_COLOR_ENEMY_CRITICAL"));
	SetColor(pLua, &CAPTION_EFFECT_COLOR_MY_BUFF,			_T("CONST_CAPTION_EFFECT_COLOR_MY_BUFF"));
	SetColor(pLua, &CAPTION_EFFECT_COLOR_MY_DEBUFF,			_T("CONST_CAPTION_EFFECT_COLOR_MY_DEBUFF"));
	SetColor(pLua, &CAPTION_EFFECT_COLOR_MY_GUARD_DEFENSE,	_T("CONST_CAPTION_EFFECT_COLOR_MY_GUARD_DEFENSE"));
	SetColor(pLua, &CAPTION_EFFECT_COLOR_MY_HEAL,			_T("CONST_CAPTION_EFFECT_COLOR_MY_HEAL"));
	SetColor(pLua, &CAPTION_EFFECT_COLOR_MY_HP_REGEN,		_T("CONST_CAPTION_EFFECT_COLOR_MY_HP_REGEN"));
	SetColor(pLua, &CAPTION_EFFECT_COLOR_MY_EXP,			_T("CONST_CAPTION_EFFECT_COLOR_MY_EXP"));
	SetColor(pLua, &CAPTION_EFFECT_COLOR_MY_EN,				_T("CONST_CAPTION_EFFECT_COLOR_MY_EN"));
}

void TCONST::SetColor(MWLua* pLua, uint32* poutValue, const TCHAR* szValueName)
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

void TCONST::InitFromExt()
{
	//int loading_counter=0;
	//TCHAR loadingfilename[_MAX_PATH] = _T("");

	//do {
	//	_stprintf_s(loadingfilename, PATH_UI_LOADING _T("loading%02d.jpg"), loading_counter);
	//	loading_counter++;
	//}
	//while( IsExist(loadingfilename) && loading_counter<100);

	//LOADING_IMAGE_COUNT = loading_counter-1;
}

void TCONST::InitAgeLimit(MWLua* pLua)
{
	bool bYes	= pLua->GetVar<bool>("PLAYER_18_OVER");

	if(bYes)
		TCONST::PLAYER_AGE_LIMIT = AL_18_OVER;
}

void TCONST::SetFakeBeatenWeapontypeAnimationBlend( MWLua* pLua )
{
	TCONST::HIT_FAKE_BEATEN_WEAPON_TYPE_BLEND_WEIGHT[0] = pLua->GetVar<float>("CONST_HIT_FAKE_BEATEN_1HS_BLEND_WEIGHT");
	TCONST::HIT_FAKE_BEATEN_WEAPON_TYPE_BLEND_WEIGHT[1] = pLua->GetVar<float>("CONST_HIT_FAKE_BEATEN_1HB_BLEND_WEIGHT");
	TCONST::HIT_FAKE_BEATEN_WEAPON_TYPE_BLEND_WEIGHT[2] = pLua->GetVar<float>("CONST_HIT_FAKE_BEATEN_1HP_BLEND_WEIGHT");
	TCONST::HIT_FAKE_BEATEN_WEAPON_TYPE_BLEND_WEIGHT[3] = pLua->GetVar<float>("CONST_HIT_FAKE_BEATEN_2HD_BLEND_WEIGHT");
	TCONST::HIT_FAKE_BEATEN_WEAPON_TYPE_BLEND_WEIGHT[4] = pLua->GetVar<float>("CONST_HIT_FAKE_BEATEN_STA_BLEND_WEIGHT");
	TCONST::HIT_FAKE_BEATEN_WEAPON_TYPE_BLEND_WEIGHT[5] = pLua->GetVar<float>("CONST_HIT_FAKE_BEATEN_ARC_BLEND_WEIGHT");
	TCONST::HIT_FAKE_BEATEN_WEAPON_TYPE_BLEND_WEIGHT[6] = pLua->GetVar<float>("CONST_HIT_FAKE_BEATEN_2HB_BLEND_WEIGHT");
	TCONST::HIT_FAKE_BEATEN_WEAPON_TYPE_BLEND_WEIGHT[7] = pLua->GetVar<float>("CONST_HIT_FAKE_BEATEN_DWD_BLEND_WEIGHT");
	TCONST::HIT_FAKE_BEATEN_WEAPON_TYPE_BLEND_WEIGHT[8] = pLua->GetVar<float>("CONST_HIT_FAKE_BEATEN_DWP_BLEND_WEIGHT");

	TCONST::HIT_FAKE_BEATEN_WEAPON_TYPE_BLEND_SPEED[0] = pLua->GetVar<float>("CONST_HIT_FAKE_BEATEN_1HS_BLEND_SPEED");
	TCONST::HIT_FAKE_BEATEN_WEAPON_TYPE_BLEND_SPEED[1] = pLua->GetVar<float>("CONST_HIT_FAKE_BEATEN_1HB_BLEND_SPEED");
	TCONST::HIT_FAKE_BEATEN_WEAPON_TYPE_BLEND_SPEED[2] = pLua->GetVar<float>("CONST_HIT_FAKE_BEATEN_1HP_BLEND_SPEED");
	TCONST::HIT_FAKE_BEATEN_WEAPON_TYPE_BLEND_SPEED[3] = pLua->GetVar<float>("CONST_HIT_FAKE_BEATEN_2HD_BLEND_SPEED");
	TCONST::HIT_FAKE_BEATEN_WEAPON_TYPE_BLEND_SPEED[4] = pLua->GetVar<float>("CONST_HIT_FAKE_BEATEN_STA_BLEND_SPEED");
	TCONST::HIT_FAKE_BEATEN_WEAPON_TYPE_BLEND_SPEED[5] = pLua->GetVar<float>("CONST_HIT_FAKE_BEATEN_ARC_BLEND_SPEED");
	TCONST::HIT_FAKE_BEATEN_WEAPON_TYPE_BLEND_SPEED[6] = pLua->GetVar<float>("CONST_HIT_FAKE_BEATEN_2HB_BLEND_SPEED");
	TCONST::HIT_FAKE_BEATEN_WEAPON_TYPE_BLEND_SPEED[7] = pLua->GetVar<float>("CONST_HIT_FAKE_BEATEN_DWD_BLEND_SPEED");
	TCONST::HIT_FAKE_BEATEN_WEAPON_TYPE_BLEND_SPEED[8] = pLua->GetVar<float>("CONST_HIT_FAKE_BEATEN_DWP_BLEND_SPEED");
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

bool TPlayerModelConst::Init( const TCHAR* szFileName )
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

void TPlayerModelConst::ParseDefaultModelPath( MXmlElement* pElement )
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

void TPlayerModelConst::ParseDefaultMesh( MXmlElement* pElement )
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

void TPlayerModelConst::ParseHair( MXmlElement* pElement )
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

void TPlayerModelConst::ParseFace( MXmlElement* pElement )
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

SEX TPlayerModelConst::TransSex( const TCHAR* szValue )
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

