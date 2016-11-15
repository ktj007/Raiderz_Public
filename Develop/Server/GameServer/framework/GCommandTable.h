#pragma once

#include "GFrameworkLib.h"
#include "MCommandTable.h"
#include "CCommandTable.h"
#include "CCommandTable_Login.h"
#include "CCommandTable_Master.h"
#include "CCommandTable_App.h"
#include "SMinetInitScope.h"

// 커맨드 선언 ////////////////////////////////////////////////////////////////
// 커맨드 번호 20001 ~ 30000 까지 사용

// 로컬 ///////////////////////////////////////////////////////////////////////

// 프로세스가 통신
enum GCommandTable
{
//	MC_LOCAL_GAME_START						= 20001,	///< 플레이어가 게임 시작한당
//	MC_LOCAL_PLAYER_LOGOUT					= 20003,	///< 플레이어 로그아웃

};

GFRAMEWORK_API void GAddCommandTable();


class GMinetInitScope : public SMinetInitScope
{
public:
	GMinetInitScope() : SMinetInitScope()
	{
		GAddCommandTable();
	}
};

