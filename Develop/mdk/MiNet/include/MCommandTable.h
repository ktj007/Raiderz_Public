#ifndef MSHAREDCOMMANDTABLE_H
#define MSHAREDCOMMANDTABLE_H

#include "MiNetLib.h"

namespace minet {

class MCommandManager;


enum MCommandTable
{
// 기본 커맨드 리스트
	MC_LOCAL_VERSION				= 1,		///< (A) 커맨드 버전 확인

	MC_LOCAL_NET_CONNECT			= 101,		///< (C) 커뮤니케이터와 연결
	MC_LOCAL_NET_CONNECT_WITH_NIC	= 102,		///< (C) 커뮤니케이터와 연결 (지정된 네트웍카드 사용)
	MC_LOCAL_NET_DISCONNECT			= 103,		///< (C) 커뮤니케이터와 연결 해제
	MC_LOCAL_NET_ONCONNECT			= 104,		///< (C) 커뮤니케이터와 연결되었을 때
	MC_LOCAL_NET_ONDISCONNECT		= 105,		///< (A) 커뮤니케이터와 연결이 끊겼을 때
	MC_LOCAL_NET_ONDISCONNECT_HARD	= 106,		///< (A) 커뮤니케이터와 연결이 끊겼을 때
	MC_LOCAL_NET_CHECKPING			= 107,		///< (A) 커뮤니케이터 반응속도 검사
	MC_LOCAL_NET_ONERROR			= 108,		///< (A) 커뮤니케이터와 연결 에러
	MC_LOCAL_NET_ONACCEPT			= 109,		///< (S) 커뮤니케이터가 접속했을 때
	MC_LOCAL_NET_CLEAR				= 110,		///< (S) 커뮤니케이터와 연결 해제 후 관련자원 처리

	MC_NET_REPLY_CONNECT			= 111,		///< (S->C) 연결에 대한 응답. 이때 클라이언트는 할당된 UID를 받아온다.
	MC_NET_REALLOC_CLIENT_UID		= 112,		///< (S->C) 클라이언트의 UID 재할당

	MC_NET_PING						= 121,		///< 커뮤니케이터 반응속도검사 요청
	MC_NET_PONG						= 122,		///< 커뮤니케이터 반응속도검사 응답
	MC_NET_KEEP_ALIVE				= 123,		///< (C->S) 접속 유지
};


/// MINET에서 사용하는 커맨드 등록
MINET_API void MAddSharedCommandTable();

#define SET_COMMAND(_a)				MCommand* pTempCmd = _a;	\
									int nParamIndex = 0


#define GET_PARAM(_a, _b)			if(pTempCmd->GetParameter(_a, nParamIndex++, MPT_##_b##, sizeof(_a)) == false)	\
											return CR_ERROR;														\

#define GET_PARAM_STR(_a)			if(pTempCmd->GetParameter(_a, nParamIndex++, MPT_STR) == false)	\
											return CR_ERROR;										\

#define GET_PARAM_WSTR(_a)			if(pTempCmd->GetParameter(_a, nParamIndex++, MPT_WSTR) == false) \
											return CR_ERROR;										 \


#define GET_PARAM_BLOB()			pTempCmd->GetParameter(nParamIndex)->GetPointer();						\
									if(pTempCmd->GetParameter(nParamIndex++)->GetType()!=MPT_BLOB) return
} // namespace minet

#endif