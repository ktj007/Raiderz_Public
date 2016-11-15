#pragma once

#include "ZDef.h"

/// 서버 상태
enum ZServerStat
{
	SERVER_STAT_INACTIVE = 0,
	SERVER_STAT_ACTIVE,			///< 동작중
	SERVER_STAT_INCHECK,		///< 접속불가. 서버 점검 중
	SERVER_STAT_ERROR,			///< 접속불가. 기타
};

/// 서버 정보
struct ZServerInfo
{
	// 정적 정보
	wstring			strServerName;		///< 서버 이름
	ZServerStat		nStat;				///< 서버 상태
	uint32			nMaxRecvBPS;
	uint32			nMaxSendBPS;
	LONGLONG		nTotalRecvBPS;
	LONGLONG		nTotalSendBPS;
	LONGLONG		nFrameIndex;

	int				nMaxRequiredLoginServerCount;	///< 접속 받아야할 로그인 서버 수
	int				nMaxRequiredGameServerCount;	///< 접속 받아야할 게임 서버 수
	int				nMaxRequiredAppServerCount;		///< 접속 받아야할 앱 서버 수

	int				nSharedFieldInfoCount;	///< 동적 필드 정보 개수

	// 동적 정보
	int				nFPS;						///< FPS
	int				nIsLoginServerConnected;	///< 현재 서버에 접속 중인 로그인 서버 여부 (0: Disconnected , 1: Connected)
	int				nCurrGameServerCount;		///< 현재 서버에 접속 중인 게임 서버 수
	int				nCurrAppServerCount;		///< 현재 서버에 접속 중인 앱 서버 수
	int				nCurrPlayerCount;			///< 현재 월드에 접속 중인 플레이어 수
	uint32			nRecvBPS;					///< 초당 받은 용량
	uint32			nSendBPS;					///< 초당 보낸 용량
	uint32			nRecvCPS;					///< 초당 받은 커맨드 수
	uint32			nSendCPS;					///< 초당 보낸 커맨드 수
	uint32			nLocalCPS;					///< 초당 로컬 커맨드 수

	int				nAllSharedFieldCount;		///< 전체 동적 필드(채널) 개수

	// 테스트 관련

	ZServerInfo()
	{
		strServerName = DEFAULT_SERVER_NAME;
		nStat = SERVER_STAT_INACTIVE;

		nMaxRequiredLoginServerCount = 0;
		nMaxRequiredGameServerCount = 0;
		nMaxRequiredAppServerCount = 0;

		nIsLoginServerConnected = 0;
		nCurrGameServerCount = 0;
		nCurrAppServerCount = 0;

		nCurrPlayerCount = 0;

		nSharedFieldInfoCount = 0;
		nAllSharedFieldCount = 0;
		
		nMaxRecvBPS = 0;
		nMaxSendBPS = 0;
		nTotalRecvBPS = 0;
		nTotalSendBPS = 0;
		nFrameIndex = 0;
		nFPS = 0;
		nRecvBPS = 0;
		nSendBPS = 0;
		nRecvCPS = 0;
		nSendCPS = 0;
		nLocalCPS = 0;
	}

};


