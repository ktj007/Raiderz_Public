#pragma once

#include "LDef.h"

/// 서버 상태
enum LServerStat
{
	SERVER_STAT_INACTIVE = 0,
	SERVER_STAT_ACTIVE,			///< 동작중
	SERVER_STAT_INCHECK,		///< 접속불가. 서버 점검 중
	SERVER_STAT_ERROR,			///< 접속불가. 기타
};

/// 서버 정보
struct LServerInfo
{
	// 정적 정보
	wstring			strServerName;		///< 서버 이름
	LServerStat		nStat;				///< 서버 상태
	uint32			nMaxRecvBPS;
	uint32			nMaxSendBPS;
	LONGLONG		nTotalRecvBPS;
	LONGLONG		nTotalSendBPS;
	LONGLONG		nFrameIndex;

	// 동적 정보
	int				nFPS;					///< FPS
	int				nCurrPlayerCount;		///< 플레이어 수
	int				nLinkCount;				///< 
	uint32			nRecvBPS;				///< 초당 받은 용량
	uint32			nSendBPS;				///< 초당 보낸 용량
	uint32			nRecvCPS;				///< 초당 받은 커맨드 수
	uint32			nSendCPS;				///< 초당 보낸 커맨드 수
	uint32			nLocalCPS;				///< 초당 로컬 커맨드 수

	// 테스트 관련

	LServerInfo()
	{
		strServerName = DEFAULT_SERVER_NAME;
		nStat = SERVER_STAT_INACTIVE;
		nCurrPlayerCount = 0;
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
		nLinkCount = 0;
	}

};


