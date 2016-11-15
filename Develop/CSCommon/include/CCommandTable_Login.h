#ifndef _CCOMMAND_TABLE_LOGIN_H
#define _CCOMMAND_TABLE_LOGIN_H

#include "CSCommonLib.h"

enum CLoginCommandTable
{
	// 커맨드 선언 //////////////////////////////////////////////////////////////////////////
	// 커맨드 번호 10001 ~ 12000 까지 사용

// 네트웍 통신(COMM) ---------------------------------------------------------------------------------
	MC_COMM_REQUEST_LOGIN						= 10001,		///< (C->S) 로그인 요청
	MC_COMM_RESPONSE_LOGIN						= 10002,		///< (S->C) 로그인 응답

	MC_COMM_REQUEST_LOGIN_GAME_SERVER			= 10011,		///< (C->S) GameServer 로그인 요청
	MC_COMM_RESPONSE_LOGIN_GAME_SERVER			= 10012,		///< (S->C) GameServer 로그인 응답

	MC_COMM_ENTER_WORLD_MSG						= 10013,		///< (S->C) 월드 입장 처리중 메시지
	MC_COMM_ENTER_WORLD_LOCAL_MSG				= 10014,		///< (LOCAL) 월드 입장 처리중 메시지 (로컬)

	MC_COMM_MOVE_GAME_SERVER					= 10021,		///< (S->C) 게임서버 이동 지시 (From GS To GS)
	MC_COMM_REQUEST_MOVE_GAME_SERVER			= 10022,		///< (C->S) 게임서버이동 예약 확인 요청
	MC_COMM_RESPONSE_MOVE_GAME_SERVER			= 10023,		///< (S->C) 게임서버이동 예약 확인 응답

	MC_COMM_REQUEST_MOVE_LOGIN_SERVER			= 10032,		///< (C->S) 로그인서버이동 예약 확인 요청
	MC_COMM_RESPONSE_MOVE_LOGIN_SERVER			= 10033,		///< (S->C) 로그인서버이동 예약 확인 응답

	MC_COMM_REQUEST_LOGIN_ON_PMANG				= 10101,		///< (C->S) 피망을 통한 로그인 요청 (MC_COMM_REQUEST_LOGIN 와 동일한 일을 한다.)
	MC_COMM_RESPONSE_LOGIN_ON_PMANG				= 10102,		///< (S->C) 피망을 통한 로그인 응답 (MC_COMM_RESPONSE_LOGIN 와 동일한 일을 한다.)

	MC_COMM_REQUEST_LOGIN_GAME_SERVER_ON_PMANG	= 10103,		///< (C->S) 피망접속 시, GameServer 로그인 요청 
	MC_COMM_REQUEST_MOVE_GAME_SERVER_ON_PMANG	= 10104,		///< (C->S) 피망접속 시, 게임서버이동 예약 확인 요청
	MC_COMM_REQUEST_MOVE_LOGIN_SERVER_ON_PMANG	= 10105,		///< (C->S) 피망접속 시, 로그인서버이동 예약 확인 요청

};



// 서버,클라이언트 모두 공유하는 커맨드 등록
CSCOMMON_API void AddLoginCommandTable();


#endif
