#pragma once

#include <vector>
#include <set>
#include <map>
#include <string>
#include "MTypes.h"
#include "MMath.h"
#include "MiNet.h"
#include "CSDef.h"
#include "SDef.h"

using namespace std;
using namespace minet;

#define APP_CLASS_NAME					L"AppServer"
#define APP_NAME						L"AppServer"
#define APPLICATION_WIDTH				500
#define APPLICATION_HEIGHT				700

#define DEFAULT_SERVER_NAME				L"AppServer"


#define PATH_DATA						L"./data/"


#define FILENAME_CONFIG					L"./server.ini"
#define FILENAME_CONFIG_REL				L"./server_rel.ini"
#define FILENAME_PROFILE				L"profile.txt"
#define FILENAME_LOG					L"mlog.txt"

#define DEFAULT_UPDATE_TICK						20		///< 업데이트 틱
#define APP_SERVERVIEW_UPDATE_TICK				(1.0f)	// 서버창에 보여주는 서버 정보 틱(일단 1초)

#define MAX_EXECUTE_QUERY			1			///< 한틱에 실행하는 최대 쿼리

#define LOG_POINT_TOP_PRIORITY		100			///< 최고 순위 로그 점수
#define LOG_POINT_SECOND_PRIORITY	10			///< 두번째 순위 로그 점수
#define LOG_POINT_THIRD_PRIORITY	1			///< 세번째 순위 로그 점수


// 게임서버 로그인 요청 정보
struct SERVER_LOGIN_DESC
{
	wstring strServerName;
	int nServerType;
	int nWorldID;
	int nServerID;
	int nCommandVersion;

	SERVER_LOGIN_DESC()
	: nServerType(SERVER_GAME)
	, nWorldID(INVALID_ID)
	, nServerID(INVALID_ID)
	, nCommandVersion(0)
	{}
};
