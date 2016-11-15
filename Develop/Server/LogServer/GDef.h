#ifndef _GDEF_H
#define _GDEF_H

#include <vector>
#include <set>
#include <map>
#include <string>
#include "MTypes.h"
#include "MMath.h"

#include "CSDef.h"

using namespace std;
using namespace minet;

#define DEFAULT_SERVER_NAME					"테스트"	///< 기본 서버 이름
#define DEFAULT_MAX_PLAYER			1000				///< 기본 서버 동시접속자

#define FILENAME_SERVER_CONFIG		"./server.ini"
#define FILENAME_SERVER_CONFIG_REL	"./server_rel.ini"
#define FILENAME_PROFILE			"profile.txt"
#define FILENAME_LOG				"mlog.txt"

#define DEFAULT_GAME_TICK			20			///< 게임틱
#define MAX_TICK_DELTA				(1.0f)		///< 아무리 랙이 걸려도 delta값은 1초를 초과하지 않는다.
#define APP_SERVERVIEW_UPDATE_TICK	(1.0f)		///< 서버창에 보여주는 서버 정보 틱(일단 1초)

#define MAX_EXECUTE_QUERY			1			///< 한틱에 실행하는 최대 쿼리

#define LOG_POINT_TOP_PRIORITY		100			///< 최고 순위 로그 점수
#define LOG_POINT_SECOND_PRIORITY	10			///< 두번째 순위 로그 점수
#define LOG_POINT_THIRD_PRIORITY	1			///< 세번째 순위 로그 점수


#endif