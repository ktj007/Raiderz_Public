#ifndef _XCOMMAND_TABLE_H
#define _XCOMMAND_TABLE_H

#include "MCommandTable.h"
#include "CCommandTable.h"
#include "CCommandTable_Login.h"

// 커맨드 선언 ////////////////////////////////////////////////////////////////
// 커맨드 번호 30001 ~ 40000 까지 사용

enum XCommandTable
{

	MC_LOCAL_RELOAD_UI				= 30001,		///< UI 리로드
	MC_LOCAL_CHANGE_GAMEFRAME,						///< 게임 프레임 변환


	// 테스트 관련
	MC_LOCAL_TEST_CHANGE_ENV,						///< 환경 변경
	MC_LOCAL_TEST_SELF_MOTION_FACTOR,				///< 내 캐릭터 모션 팩터 테스트

};



void XAddCommandTable();

#endif