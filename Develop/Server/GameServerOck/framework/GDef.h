#pragma once

#include <vector>
#include <set>
#include <map>
#include <string>
#include "MTypes.h"
#include "MMath.h"

#include "CSDef.h"
#include "SDef.h"
#include "MCommand.h"

using namespace std;
using namespace minet;

#define GAMESERVER_CLASS_NAME		L"GameServer"

#define DEFAULT_SERVER_NAME			L"GameServer"	///< 기본 서버 이름
#define DEFAULT_MAX_PLAYER			1000			///< 기본 서버 동시접속자
#define DEFAULT_SERVER_TYPE			0

#define MAX_NUM_PLAYER				10		// 최대 플레이어
#define MAX_NUM_NPC					10		// 최대 NPC
#define MAX_NUM_MONSTER				10000	// 최대 몬스터
#define MAX_NUM_PROJECTILE			10000	// 최대 발사체
#define MAX_NUM_OBJ					(MAX_NUM_PLAYER + MAX_NUM_NPC + MAX_NUM_MONSTER)

#define MAX_NUM_FIELD				10	// 최대 필드
#define MAX_NUM_CHANNEL				10 // 최대 채널

#define FILENAME_SERVER_CONFIG		L"./server.ini"
#define FILENAME_SERVER_CONFIG_REL	L"./server_rel.ini"

#define PATH_ZONE					L"./zone/"
#define PATH_FIELD					L"./field/"

#define PATH_SCRIPT					L"./script/"

#define PATH_SCRIPT_FIELD			PATH_SCRIPT L"field/"
#define PATH_SCRIPT_QUEST			PATH_SCRIPT L"quest/"

#define PATH_SCRIPT_NPC				PATH_SCRIPT L"npc/"
#define PATH_SCRIPT_NPC_LUA			PATH_SCRIPT_NPC L"lua/"
#define PATH_SCRIPT_NPC_COLT		PATH_SCRIPT_NPC L"colt/"


#define PATH_SYSTEM					L"./system/"
#define PATH_STRING					PATH_SYSTEM L"Strings/"

#define FILENAME_NPCINFO			PATH_SYSTEM	L"npc.xml"
#define FILENAME_BPART_REWARD		PATH_SYSTEM	L"bpart_reward.xml"
#define FILENAME_TALENTINFO			PATH_SYSTEM	L"talent.xml"
#define FILENAME_BUFFINFO			PATH_SYSTEM	L"buff.xml"
#define FILENAME_TALENTINFO_EXT		PATH_SYSTEM	L"talent_ext.xml"
#define FILENAME_TALENT_HIT_INFO	PATH_SYSTEM	L"talent_hit_info.xml"
#define FILENAME_TALENT_POS_INFO	PATH_SYSTEM	L"talent_pos_info.xml"
#define FILENAME_ITEMINFO			PATH_SYSTEM	L"xitem.xml"
#define FILENAME_QUEST				PATH_SYSTEM	L"quest.xml"
#define FILENAME_LEVEL_TABLE		PATH_SYSTEM	L"level_table.xml"
#define FILENAME_MESH_INFO			PATH_SYSTEM	L"mesh_info.xml"
#define FILENAME_LOOT_INFO			PATH_SYSTEM	L"loot.xml"
#define FILENAME_SOULBINDING		PATH_SYSTEM	L"soulbinding.xml"
#define FILENAME_CONDITIONS			PATH_SYSTEM	L"conditions.xml"
#define FILENAME_CONDITIONS_ITEM	PATH_SYSTEM	L"conditions_item.xml"
#define FILENAME_DIALOG				PATH_SYSTEM	L"dialog.xml"
#define FILENAME_NPCSHOP			PATH_SYSTEM	L"npcshop.xml"
#define FILENAME_TRAINING			PATH_SYSTEM	L"training.xml"
#define FILENAME_RECIPE				PATH_SYSTEM	L"recipe.xml"
#define FILENAME_CRAFT				PATH_SYSTEM	L"craft.xml"
#define FILENAME_INNROOM			PATH_SYSTEM	L"innroom.xml"
#define FILENAME_FACTION			PATH_SYSTEM	L"faction.xml"
#define FILENAME_FACTION_RELATION	PATH_SYSTEM	L"faction_relation.xml"
#define FILENAME_CUTSCENES			PATH_SYSTEM	L"cutscenes.xml"
#define FILENAME_EMBLEM				PATH_SYSTEM	L"emblem.xml"
#define FILENAME_CHALLENGER_QUEST	PATH_SYSTEM	L"challenger.xml"
#define FILENAME_QPEVENT			PATH_SYSTEM	L"qpevent.xml"
#define FILENAME_PRESET				PATH_SYSTEM	L"preset.xml"

#define FILENAME_PROFILE			L"profile.txt"
#define FILENAME_SCRIPT_ENTRY		PATH_SCRIPT L"entry.lua"				// 맨처음에 entry.lua 스크립트를 읽음
#define FILENAME_ABUSE				PATH_STRING L"abuse.txt"
#define FILENAME_GAMEWORDS			PATH_STRING L"gamewords.txt"
#define FILENAME_NAME_ILLEGALWORDS	PATH_STRING L"name_illegalwords.txt"
#define FILENAME_NAME_ALLOWEDCHARS	PATH_STRING L"name_allowedchars.txt"

#define DEFAULT_GAME_TICK						20			///< 게임틱
#define MAX_TICK_DELTA							(1.0f)		///< 아무리 랙이 걸려도 delta값은 1초를 초과하지 않는다.
#define APP_SERVERVIEW_UPDATE_TICK				(1.0f)		///< 서버창에 보여주는 서버 정보 틱(일단 1초)


#define MAX_ID_STRING_LENGTH 16

// 트리거 스크립트
#define TRIGGER_INVALID_SENSOR_ID			0


// 전투
#define PICKING_ADDED_VALUE_Z		100.0f		// 충돌, 길찾기 때 원래 좌표보다 200 위에서 좌표를 Picking한다.


// vector<MUID> 풀 설정
#define SHARED_MEM_MUID_POOL_SIZE		10
#define SHARED_MEM_MUID_VECOTR_SIZE		2000


enum GEntityInOut
{
	PC_IN		= 0x01,
	PC_OUT		= 0x02,
	NPC_IN		= 0x11,
	NPC_OUT		= 0x12
};

struct GDamageInfo
{
	uint32				nType;
	uint32				nAmount;

	GDamageInfo(): nType(DA_NONE), nAmount(0) 
	{}
};

struct GDamageRangedInfo
{
	int						nMinDamage;
	int						nMaxDamage;
	GDamageRangedInfo() : nMinDamage(0), nMaxDamage(0) {}
	GDamageRangedInfo(int nMaxDmg, int nMinDmg) : nMinDamage(nMinDmg), nMaxDamage(nMaxDmg) {}
};

struct GHealRangedInfo
{
	int						nMin;
	int						nMax;
	GHealRangedInfo() : nMin(0), nMax(0) {}
	GHealRangedInfo(int nMax_, int nMin_) : nMax(nMax_), nMin(nMin_) {}
};

struct SpawnDetailInfo
{
	MUID uidParent;	// 자신을 스폰시킨 주인 NPC UID

	struct Lootable
	{
		bool bEnable;				// 루팅 가능한 상태로 스폰할지 여부
		MUID uidCorpseForPlayer;	// 시체가 된 플레이어 uid
		MUID uidRewarder;			// 수혜자
		bool bPartyReward;			// 수혜자의 파티원도 수혜자가 될지 여부
		bool bRouteLootable;

		Lootable()
		{
			bEnable = false;
			bRouteLootable = false;
			bPartyReward = false;
		}
	} lootable;

	struct BPartEntity
	{
		bool bEnable; // 루팅 가능한 상태로 스폰할지 여부
		int nNPCID;
		int nPartID;
		int nFlag;
		int nLootID;

		BPartEntity()
		{
			bEnable = false;
		}
	} bpart;
};

const int	INVALID_NPC_ID = 0;

#define NONE_DYNAMIC_FIELD_KEY	0

enum MovingWay
{
	MW_STRAIGHT = 0,
	MW_LEFT,
	MW_RIGHT,
	MW_RANDOM
};

enum GTaskMoveStrategy
{
	MS_STRAIGHT = 0,	///< 직선 이동
	MS_DEFINITE			///< 확정적 이동
};



// 유틸리티 매크로 ----------------------------------------------------------------------------

template <typename T, size_t N>
char (&ArraySizeHelper(T (&array)[N]))[N];
#define ArraySize(array) (sizeof(ArraySizeHelper(array)))

#define NOTIFY_OBSERVERS(obs_type, set, event)					\
	for each (obs_type* pOvr in set)				\
		pOvr->event;									\


// ---------------------------------------------------------------------------------------------

enum CHEAT_TYPE
{
	CT_INVALID	= -1,
	CT_POSITION	= 0,
};

enum POSITION_CHEAT_TYPE
{
	// 이동관련 메소드타입
	POSCHEAT_OK = -1,
	POSCHEAT_POS_SPEEDHACK = 0,
	POSCHEAT_POS_WARP,
};

enum CHEAT_TREAT_TYPE
{
	CHEATTREAT_NONE,
	CHEATTREAT_TRY,
	CHEATTREAT_WARNING,
	CHEATTREAT_DISCONNECT, 
};

class GPercentDice : public MTestMemPool<GPercentDice>
{
public:
	virtual bool Dice(float fPercent)
	{
		if (fPercent <= 0.0f) return false;
		if (fPercent >= 100.0f) return true;

		if (RandomNumber(0.0f, 100.0f) < fPercent) return true;
		return false;
	};
};

namespace INITIAL_ITEM_SET
{
	enum _
	{
		WEAPON_ID	= 1,	// 보급형 검
		BODY_ID		= 1011,	// 크롬레드 중갑 베스트
		LEG_ID		= 1012,	// 크롬레드 중갑 폴드
		HANDS_ID	= 1013,	// 크롬레드 중갑 건틀렛
		FEET_ID		= 1014	// 크롬레드 중갑 그리브
	};
}

enum GAITestType
{
	AITEST_NONE = 0,
	AITEST_STRESS_COMBAT_ONE_FIELD = 1,
	AITEST_STRESS_COMBAT_ALL_FIELD = 2,
};

enum GCommandRecordingType
{
	COMMANDREC_NONE = 0,
	COMMANDREC_RECORD = 1,
	COMMANDREC_REPLAY = 2,
};

enum FleeType
{
	Flee_FromEnemy = 0,	// 적으로부터 도망
	Flee_ToHome = 1,	// 전투 시작지역으로 도망
};

#define JOB_DURATION_INFINITE_TIME	0.0f

// birdkr의 mlog Helper 함수..-_-
inline void __cdecl mlog_birdkr(const char* pFormat, ...)
{
	char temp[1024];

	va_list args;
	va_start(args,pFormat);
	vsprintf(temp,pFormat,args);
	va_end(args);

	mlogf("birdkr", temp);
}
#define blog mlog_birdkr

inline void __cdecl mlog_mizelan(const char* pFormat, ...)
{
	char temp[1024];

	va_list args;
	va_start(args,pFormat);
	vsprintf(temp,pFormat,args);
	va_end(args);

	mlogf("pyo", temp);
}

#define mmlog mlog_mizelan

inline void __cdecl mlog_luna1x(const char* pFormat, ...)
{
	char temp[1024];

	va_list args;
	va_start(args,pFormat);
	vsprintf(temp,pFormat,args);
	va_end(args);

	mlogf("luna1x", temp);
}


// 남아있는 버프시간 
struct REMAIN_BUFF_TIME
{
	REMAIN_BUFF_TIME()
	: nID(INVALID_ID)
	, fRemainDurationSeconds(0.0f)
	, fRemainNextPeriodSeconds(0.0f)
	, fRemainBuffOffsetSeconds(0.0f)
	, nStackedCount(1)
	{}

	int		nID;						// 버프ID
	float	fRemainDurationSeconds;		// 남은 유효시간
	float	fRemainNextPeriodSeconds;	// 다음 주기까지 남은 시간
	float	fRemainBuffOffsetSeconds;	// 남은 BuffOffset
	int		nStackedCount;				// 중첩개수

	// TODO: 서버간 이동 시, 버프 이동을 고려할때. [2010/8/3 praptor]
	// TODO: fRemainNextPeriodSeconds 외에 fRemainPeriodTick이 필요할까? - praptor, 2010.03.09	
};


// 필드 진입 실패 이유
enum FIELD_ENTER_FAIL_TYPE
{
	ENTERFAIL_OK = 0,
	ENTERFAIL_SYSTEM_ERROR,	// 시스템에 문제가 있음
	ENTERFAIL_PROCEED_COMBAT, // 전투가 진행중
	ENTERFAIL_PROCEED_INTERMISSION, // 진입 중지중 (점수판 보이기, 씬 대기중등)
};


// 유닛 테스트
#define TEST_FRIEND_NPCSETTER friend class GNPCSetterForTest;

typedef unsigned long	SPAWN_ID;

// 유틸리티 매크로 ----------------------------------------------------------------------------
#define		ROUTE_SYTEM_MSG(msg)			{ pPlayer->RouteSystemMsg(msg); }

#ifndef _PUBLISH
#define		RVRSM(x,msg)		{ if (!(x)) { mlog3("\""#x"\" is invalid. (%s() ==> %s:%d)\n", __FUNCTION__, __FILE__, __LINE__); ROUTE_SYTEM_MSG(msg) return;		}}
#define		RVRSM_FAIL(x,msg)	{ if (!(x)) { mlog3("\""#x"\" is invalid. (%s() ==> %s:%d)\n", __FUNCTION__, __FILE__, __LINE__); ROUTE_SYTEM_MSG(msg) return false;	}}
#define		VRSM(x,msg)			{ assert(x); RVRSM(x,msg) }
#define		VRSM_FAIL(x,msg)	{ assert(x); RVRSM_FAIL(x,msg) }
#else
#define		RVRSM(x,msg)		{ if (!(x)) { ROUTE_SYTEM_MSG(msg) return; } }
#define		RVRSM_FAIL(x,msg)	{ if (!(x)) { ROUTE_SYTEM_MSG(msg) return false;} }
#define		VRSM(x,msg)			{ RVRSM(x) }
#define		VRSM_FAIL(x,msg)	{ RVRSM_FAIL(x,msg) }
#endif
// ---------------------------------------------------------------------------------------------


typedef int FieldID;
typedef int QuestID;
typedef MUID PlayerUID;
typedef MUID PartyUID;