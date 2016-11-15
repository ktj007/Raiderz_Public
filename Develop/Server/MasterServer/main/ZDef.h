#pragma once

#include <vector>
#include <set>
#include <map>
#include "string"
#include "MTypes.h"
#include "MMath.h"
#include "MiNet.h"
#include "CSDef.h"

using namespace std;
using namespace minet;

#define APP_CLASS_NAME					L"MasterServer"
#define APP_NAME						L"MasterServer"
#define APPLICATION_WIDTH				770
#define APPLICATION_HEIGHT				710

#define DEFAULT_SERVER_NAME				L"MasterServer"
#define DEFAULT_MAX_PLAYER				1000			///< 기본 서버 동시접속자
#define DEFAULT_SERVER_TYPE				0

#define PATH_DATA						L"./data/"
#define PATH_SCRIPT						L"./script/"


#define FILENAME_CONFIG					L"./server.ini"
#define FILENAME_CONFIG_REL				L"./server_rel.ini"
#define FILENAME_PROFILE				L"profile.txt"
#define FILENAME_LOG					L"mlog.txt"
#define FILENAME_SCRIPT_ENTRY			PATH_SCRIPT L"entry.lua"	// 제일 처음 읽어오는 스크립트

#define FILENAME_SERVER_LIST			PATH_DATA L"serverlist.xml"
#define FILENAME_FIXED_PARTY_INFO		PATH_DATA L"fixed_party.xml"


#define DEFAULT_UPDATE_TICK				20		///< 업데이트 틱
#define MAX_TICK_DELTA					(1.0f)	///< 아무리 랙이 걸려도 delta값은 1초를 초과하지 않는다.
#define APP_SERVERVIEW_UPDATE_TICK		(1.0f)	// 서버창에 보여주는 서버 정보 틱(일단 1초)

#define WORLD_PLAYER_LIMIT				5000	///< 월드 최대 동접자 수



// 월드 선택
enum WorldState
{
	WORLD_STATE_CLOSE = 0,
	WORLD_STATE_OPEN,
	WORLD_STATE_CHECK
};

struct ZWorldContext
{
	int			nID;
	wstring		strName;
	wstring		strIP;
	int			nType;
	int			nMaxPlayerCount;
	int			nServable;
	WorldState	nState;
	int			nMaxElapsedTime;
};
