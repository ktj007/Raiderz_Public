#pragma once

#include <vector>
#include <set>
#include <map>
#include <string>
#include "MTypes.h"
#include "MMath.h"
#include "MiNet.h"
#include "CSDef.h"

using namespace std;
using namespace minet;

#define APP_CLASS_NAME					L"LoginServer"
#define APP_NAME						L"LoginServer"
#define APPLICATION_WIDTH				500
#define APPLICATION_HEIGHT				700

#define DEFAULT_SERVER_NAME				L"LoginServer"
#define DEFAULT_MAX_PLAYER				1000			///< 기본 서버 동시접속자

#define PATH_DATA						L"./data/"


#define FILENAME_CONFIG					L"./server.ini"
#define FILENAME_CONFIG_REL				L"./server_rel.ini"
#define FILENAME_PROFILE				L"profile.txt"
#define FILENAME_LOG					L"mlog.txt"

#define FILENAME_ABUSE					PATH_DATA L"abuse.txt"
#define FILENAME_GAMEWORDS				PATH_DATA L"gamewords.txt"
#define FILENAME_NAME_ILLEGALWORDS		PATH_DATA L"name_illegalwords.txt"
#define FILENAME_NAME_ALLOWEDCHARS		PATH_DATA L"name_allowedchars.txt"

#define DEFAULT_UPDATE_TICK			20			///< 업데이트 틱
#define MAX_TICK_DELTA				(1.0f)		///< 아무리 랙이 걸려도 delta값은 1초를 초과하지 않는다.
#define APP_SERVERVIEW_UPDATE_TICK	(1.0f)		// 서버창에 보여주는 서버 정보 틱(일단 1초)


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
