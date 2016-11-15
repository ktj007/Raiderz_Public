#ifndef _GSERVER_INFO_H
#define _GSERVER_INFO_H

#include "GDef.h"

/// 서버 상태
enum GServerStat
{
	SERVER_STAT_INACTIVE = 0,
	SERVER_STAT_ACTIVE,		///< 동작중
	SERVER_STAT_FULL,		///< 접속불가. 사용자 초과
	SERVER_STAT_INCHECK,	///< 접속불가. 서버 점검 중
	SERVER_STAT_ERROR,		///< 접속불가. 기타
};

/// 서버 정보
struct GServerInfo
{
	// 정적 정보
	wstring			strServerName;		///< 서버 이름
	GServerStat		nStat;				///< 서버 상태
	int				nMaxPlayer;			///< 서버에 동시에 접속한 최대수
	int				nMaxPlayerCapacity;	///< 서버에 동시 접속할 수 있는 수
	uint32			nMaxRecvBPS;
	uint32			nMaxSendBPS;
	LONGLONG		nFrameIndex;

	// 동적 정보
	int				nFPS;					///< FPS
	int				nNowPlayer;				///< 현재 서버에 접속해 있는 사용자 수
	int				nLinkCount;

	int64			nTotalRecvBytes;		///< 총 받은 용량
	int64			nTotalSendBytes;		///< 총 보낸 용량

	int				nDisconnCountAll;
	int				nDisconnCountByDup;
	int				nDisconnCountByCheat;


	uint32			nRecvBPS;				///< 초당 받은 용량
	uint32			nSendBPS;				///< 초당 보낸 용량
	uint32			nRecvCPS;				///< 초당 받은 커맨드 수
	uint32			nSendCPS;				///< 초당 보낸 커맨드 수
	uint32			nLocalCPS;				///< 초당 로컬 커맨드 수
	uint32			nAsyncDBTask;			///< 남은 디비 작업수.
	uint32			nAsyncDBTotalTask;		///< 지금까지 한 총 디비 작업수
	uint32			nMaxAsyncDBTask;		///< 최대 디비 작업수.
	uint32			nAsyncTaskCount;		///< 처리할 AsyncTask 갯수

	// 테스트 관련
	bool			bDamageTest;			///< 데미지 테스트중인지

	GServerInfo()
	{
		strServerName = DEFAULT_SERVER_NAME;
		nStat = SERVER_STAT_INACTIVE;
		nMaxPlayerCapacity = DEFAULT_MAX_PLAYER;
		nMaxPlayer = 0;
		nNowPlayer = 0;
		nMaxRecvBPS = 0;
		nMaxSendBPS = 0;
		nFrameIndex = 0;
		nFPS = 0;
		nTotalRecvBytes = 0;
		nTotalSendBytes = 0;

		ResetDisconnCount();

		nRecvBPS = 0;
		nSendBPS = 0;
		nRecvCPS = 0;
		nSendCPS = 0;
		nLocalCPS = 0;
		bDamageTest = false;
		nAsyncDBTask = 0;
		nMaxAsyncDBTask = 0;
		nAsyncDBTotalTask = 0;
		nLinkCount = 0;
		nAsyncTaskCount = 0;
	}

	void ResetDisconnCount()
	{
		nDisconnCountAll = 0;
		nDisconnCountByDup = 0;
		nDisconnCountByCheat = 0;
	}

};


#endif