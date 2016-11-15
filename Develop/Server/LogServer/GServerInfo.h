#ifndef _GSERVER_INFO_H
#define _GSERVER_INFO_H

#include "GDef.h"

/// 서버 상태
enum GServerStat
{
	SERVER_STAT_ACTIVE = 0,		///< 동작중
	SERVER_STAT_FULL,		///< 접속불가. 사용자 초과
	SERVER_STAT_INCHECK,	///< 접속불가. 서버 점검 중
	SERVER_STAT_ERROR,		///< 접속불가. 기타
};

/// 서버 정보
struct GServerInfo
{
	// 정적 정보
	string			strServerName;		///< 서버 이름
	GServerStat		nStat;				///< 서버 상태
	int				nMaxPlayer;			///< 서버에 동시에 접속한 최대수
	int				nMaxPlayerCapacity;	///< 서버에 동시 접속할 수 있는 수
	uint32			nMaxRecvBPS;
	uint32			nMaxSendBPS;
	LONGLONG		nTotalRecvBPS;
	LONGLONG		nTotalSendBPS;
	LONGLONG		nFrameIndex;

	// 동적 정보
	int				nFPS;					///< FPS
	int				nNowPlayer;				///< 현재 서버에 접속해 있는 사용자 수
	uint32			nRecvBPS;				///< 초당 받은 용량
	uint32			nSendBPS;				///< 초당 보낸 용량
	uint32			nRecvCPS;				///< 초당 받은 커맨드 수
	uint32			nSendCPS;				///< 초당 보낸 커맨드 수
	uint32			nLocalCPS;				///< 초당 로컬 커맨드 수

	// 테스트 관련
	bool			bDamageTest;			///< 데미지 테스트중인지

	GServerInfo()
	{
		strServerName = DEFAULT_SERVER_NAME;
		nStat = SERVER_STAT_ACTIVE;
		nMaxPlayerCapacity = DEFAULT_MAX_PLAYER;
		nMaxPlayer = 0;
		nNowPlayer = 0;
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
		bDamageTest = false;
	}
};


#endif