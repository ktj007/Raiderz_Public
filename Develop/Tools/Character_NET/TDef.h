#pragma once

#include <string>

//위치가 좀 달라서 따로 선언해준다.
//#define FILENAME_MOTION				"../../Data/System/Motion.xml"
//#define FILENAME_MOTION_NPC			"../../Data/system/motion_npc.xml"
//#define FILENAME_MOTION_PC			"../../Data/system/motion_pc.xml"
//#define FILENAME_TALENT				"../../Data/system/talent.xml"
#define FILENAME_TALENT_HIT_INFO		"../../Data/system/talent_hit_info.xml"
#define FILENAME_TALENT_EXT_TOOL		"../../data/system/talent_ext.xml"
#define FILENAME_BUFFINFO_TOOL			"../../data/system/buff.xml"
#define FILENAME_BUFFEFFECTINFO_TOOL	"../../data/system/buff_Effect_info.xml"
#define FILENAME_ENCHANT_BUFFEFFECTINFO_TOOL	"../../data/system/enchant_buff_Effect_info.xml"
#define FILENAME_TOOL_MESH_INFO			"../../Data/system/mesh_info.xml"
#define FILENAME_TALENT_EFFECTINFO_TOOL	"../../Data/system/talent_Effect_info.xml"
#define FILENAME_EFFECT_INFO_TOOL		"../../Data/system/effect.xml"

#define SERVER_FILENAME_TALENT_EXT		"../../Dev/GameServer/system/talent_ext.xml"
#define SERVER_FILENAME_TALENT_HIT_INFO	"../../Dev/GameServer/system/talent_hit_info.xml"
#define SERVER_FILENAME_TOOL_MESH_INFO	"../../Dev/GameServer/system/mesh_info.xml"

#define SERVER_FILENAME_TALENTINFO		"../GameServer/system/talent.xml"
#define SERVER_FILENAME_TALENT_POS_INFO	"../GameServer/system/talent_pos_info.xml"

#define FILENAME_XITEM_TOOL				"../../Data/System/xitem.xml"

#define FILENAME_NPC_TOOL				"../../Data/system/npc.xml"
#define FILENAME_NPC_CHANGEMTRL_TOOL	"../../Data/system/npc_changemtrl.xml"
#define FILENAME_ITEM_CHANGEMTRL_TOOL	"../../Data/system/xitem_changemtrl.xml"
#define FILENAME_TALENT_TOOL			"../../Data/system/talent.xml"
#define FILENAME_TALENT_EFFECT_TOOL		"../../Data/system/talenteffect.xml"
#define FILENAME_SWORD_TRAIL_SAMPLING_TOOL	"../../Data/system/swordtrailsampling.xml"

#define PATH_MODEL_TOOL					"../../Data/Model/"
#define PATH_PLAYER_TOOL				"../../Data/Model/Player/"
#define PATH_MONSTER_TOOL				"../../Data/Model/Monster/"
#define PATH_NPC_TOOL					"../../Data/Model/NPC/"
#define PATH_MAPOBJ_TOOL				"../../Data/Model/MapObject/"
#define PATH_WEAPON_TOOL				"../../Data/Model/weapons/"
#define PATH_SFX_TOOL					"../../Data/SFX/"
#define PATH_SOUND_TOOL					"../../Data/Sound/"
#define PATH_SOUND_BGM_TOOL				"../../Data/Sound/BGM/"
#define PATH_SOUND_EFFECT_TOOL			"../../Data/Sound/Effect/"
#define PATH_SFX_TOOL_CAMERA			"../../data/sfx/camera/"

#define FILENAME_HUMAN_FEMALE_TOOL		"../../Data/Model/Player/hf/hf.elu"

#define FILENAME_SOUNDLIST_TOOL			"../../Data/Sound/sound.xml"

#define PATH_PLAYER_MALE_TOOL			"../../Data/model/Player/hm/"
#define PATH_PLAYER_FEMALE_TOOL			"../../Data/model/Player/hf/"

#define FILENAME_SCRIPT_CONST_TOOL		"../../data/Script/const.lua"
#define FILENAME_SCRIPT_CONST_REL_TOOL	"../../data/script/rel_const.lua"

#define DEFAULT_PATH_TEXTURES_TOOL ".;../../Data/Texture/Character;../../Data/Texture/Map;../../Data/Texture/Tile;../../Data/Texture/SFX;../../Data/Texture/SpeedTree;../../Data/Texture/Weapon;../../Data/Texture/Sky"

#define WEAPON_TYPE_MAX	WEAPON_MAX

#define HUMAN_FEMALE	"hf"
#define HUMAN_MALE		"hm"

#define ANISPEED			10.f

#define TARGET_HOOK_DUMMY_NAME		"dummy_hook_%02d_%02d"

#define SWIMMING_HEIGHT_VALUE_TEST 140.0f

#define WEAPON_EFFECT_DUMMY_MAX_COUNT_TOOL		6
#define WEAPON_EFFECT_DUMMY_NAME_TOOL			"dummy_ef_enchant_"

#define AXIS_X_COLOR				0xffff0000
#define AXIS_Y_COLOR				0xff00ff00
#define AXIS_Z_COLOR				0xff0000ff

#define COLLISION_BOX_START_INDEX	1000

//유니코드용
namespace std
{
#ifdef _UNICODE
	typedef wstring tstring;
#else
	typedef string tstring;
#endif
}

using namespace std;

#define IsKeyDown(key) ((GetAsyncKeyState(key) & 0x8000)!=0)
#define MStringToCharPointer(stringname) \
	(const char*) System::Runtime::InteropServices::Marshal \
	::StringToHGlobalAnsi(stringname).ToPointer()

// This macro frees a pointer that was allocated with MStringToCharPointer.
#define MFreeCharPointer(stringname) \
	System::Runtime::InteropServices::Marshal::FreeHGlobal(IntPtr((void*) \
	stringname))

string GetPureFileName(const string& strFileName);
string GetPathName(const string& szFileName);
string GetRemovePathName(const string& strPathName);
string GetFolderName(const string& szFileName);

void ReplaceSlashToBackSlash(string& strPath);
void ReplaceUnderBarToBlank(string& strPath);

string GetRemoveUnderBarlast(const string& strPathName);
string GetRemoveBlanklast(const string& strPathName);
string GetRemoveExtName(const string& strPathName);

string GetFormatString(string& strPathName, const char *pFormat, ...);
string GetExtName(const string& szFileName);

bool ExistFile(const char* str);
string GetRemoveUnderBarfirst(const string& strPathName);

bool IsEqualString( System::String^ Name, System::String^ FindName );

static bool descendingInt(int &a, int &b)
{
	return a > b;
}
