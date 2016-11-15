#pragma once

#include "CSCommonLib.h"

enum CApp2GameCommandTable
{
	// 커맨드 선언 //////////////////////////////////////////////////////////////////////////
	// AppServer - GameServer 간의 커맨드 테이블
	// 커맨드 번호 16001 ~ 18000 까지 사용

	// 로그인
	MPC_COMM_REQUEST_LOGIN						= 16001,		///< (G->A) 게임서버가 앱서버에 로그인요청
	MPC_COMM_RESPONSE_LOGIN						= 16002,		///< (A->G) 앱서버로부터 게임서버에 로그인 응답

	MPC_MSG_REQ									= 16010,		///< (G->A) 채팅 요청
	MPC_MSG_RES									= 16011,		///< (A->G) 채팅 응답

	MPC_MSG_CREATE_PRIVATE_CHANNEL_REQ			= 16020,		///< (G->A)	사설 채팅 채널 생성 요청

	//////////////////////////////////////////////////////////////////////////
	/// 동기화 정보
	MPC_MSG_ADDED_CHANNEL_INFO					= 16030,		///< (A->G)	채팅 채널 생성 정보
	MPC_MSG_DELETED_CHANNEL_INFO				= 16031,		///< (A->G)	채팅 채널 제거 정보
	MPC_MSG_ADDED_PLAYER_INFO					= 16032,		///< (A->G)	채팅 채널 플레이어 입장 정보
	MPC_MSG_DELETED_PLAYER_INFO					= 16033,		///< (A->G)	채팅 채널 플레이어 퇴장 정보
	MPC_MSG_PLAYER_GAMESERVER_INFO				= 16034,		///< (A->G)	채팅 채널 플레이어 게임서버 정보
	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////
	/// 채팅 채널 기능
	MPC_MSG_ENTER_CHANNEL_REQ					= 16050,		///< (G->A)	채팅 채널 참여 요청
	MPC_MSG_ENTER_CHANNEL						= 16051,		///< (A->G)	채팅 채널 참여 대상 플레이어에게 전달
	MPC_MSG_ENTER_CHANNEL_PLAYER_INFO			= 16052,		///< (A->G)	같은 채널의 다른 플레이어들에게 참여 통지

	MPC_MSG_LEAVE_CHANNEL_REQ					= 16053,		///< (G->A)	채팅 채널 퇴장 요청
	MPC_MSG_LEAVE_CHANNEL						= 16054,		///< (A->G)	채팅 채널 퇴장 대상 플레이어에게 전달
	MPC_MSG_LEAVE_CHANNEL_PLAYER_INFO			= 16055,		///< (A->G)	같은 채널의 다른 플레이어들에게 퇴장 통지

	MPC_MSG_KICK_REQ							= 16056,		///< (G->A)	kick 요청
	MPC_MSG_KICK								= 16057,		///< (A->G)	kick 대상 플레이어에게 전달
	MPC_MSG_KICK_PLAYER_INFO					= 16058,		///< (A->G)	같은 채널의 다른 플레이어들에게 kick 통지

	MPC_MSG_BAN_REQ								= 16059,		///< (G->A)	ban 요청
	MPC_MSG_BAN									= 16060,		///< (A->G)	ban 대상 플레이어에게 전달
	MPC_MSG_BAN_PLAYER_INFO						= 16061,		///< (A->G)	같은 채널의 다른 플레이어들에게 ban 통지

	MPC_MSG_UNBAN_REQ							= 16062,		///< (G->A)	unban 요청
	MPC_MSG_UNBAN_RES							= 16063,		///< (A->G)	unban 대상 플레이어에게 전달
	
	MPC_MSG_CHANGE_HOST_REQ						= 16064,		///< (G->A)	방장 변경 요청
	MPC_MSG_CHANGE_HOST_INFO					= 16065,		///< (A->G)	같은 채널의 모든 플레이어들에게 방장 변경 통지
	
	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////
	/// 편지 시스템
	MPC_MAIL_ROUTE_NEW_MAIL_TO_RECEIVER			= 16100,		///< (G->A) 편지 수취인에게 새 편지 전달
	MPC_MAIL_TRANS_NEW_MAIL,									///< (A->G) 편지 수취인에게 새 편지 전달 통보
	MPC_MAIL_RETRY_TRANS_NEW_MAIL,								///< (G->A) 편지 수취인에게 새 편지 재전달
	//////////////////////////////////////////////////////////////////////////


	//////////////////////////////////////////////////////////////////////////
	/// 물품거래소
	MPC_GA_TRADEMARKET_ADD						= 16200,		///< (G->A) 물건이 추가되었다
	MPC_GA_TRADEMARKET_DEL,										///< (G->A) 물건이 제거되었다
	MPC_GA_TRADEMARKET_SEARCH,									///< (G->A) 검색요청
	MPC_GA_TRADEMARKET_SEARCH_NEXT,								///< (G->A) 다음페이지 검색
	MPC_AG_TRADEMARKET_SEARCH_RESULT,							///< (A->G) 검색 결과
	//////////////////////////////////////////////////////////////////////////
};

CSCOMMON_API void AddApp2GameCommandTable();
