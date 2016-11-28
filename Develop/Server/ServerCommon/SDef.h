#pragma once

#include "CSDef.h"

// 컴파일타임 어썰트, 웹에서 줒어옴 - praptor
template<bool> struct CompileTimeAssert;
template<> struct CompileTimeAssert<true> {};
#define COMPILE_TIME_ASSERT(exp) (CompileTimeAssert< (exp) >())


// 파일 이름 -----------------
#define PURE_FILENAME_ZONELIST			L"zonelist.xml"
#define PURE_FILENAME_FIELDLIST			L"fieldlist.xml"
#define PURE_FILENAME_FIELDLIST_DEBUG	L"fieldlist_debug.xml"


#define FILEEXT_ZONE_COLLISION		L".server.cl2"	// 충돌 맵 파일 확장자
#define FILEEXT_ZONE				L".zone.xml"
#define FILEEXT_FIELD				L".field.xml"
#define FILEEXT_COLT				L".colt.xml"


#define SERVER_UID_SEED_POWER		10000

// 유틸리티 매크로 ----------------------------------------------------------------------------
#ifndef _PUBLISH
	#define		DCHECK(x)			{ if (!(x)) { mlog3("\""#x"\" is invalid. (%s() ==> %s:%d)\n", __FUNCTION__, __FILE__, __LINE__); }}
	#define		RVALID(x)			{ if (!(x)) { mlog3("\""#x"\" is invalid. (%s() ==> %s:%d)\n", __FUNCTION__, __FILE__, __LINE__); return; }}
	#define		RVALID_RET(x, r)	{ if (!(x)) { mlog3("\""#x"\" is invalid. (%s() ==> %s:%d)\n", __FUNCTION__, __FILE__, __LINE__); return (r); }}
	// #define		VALID(x)			{ assert(x); RVALID(x) }
	// #define		VALID_RET(x, r)		{ assert(x); RVALID_RET(x,r) }
	#define		VALID(x)			{ _ASSERT(x); RVALID(x) }
	#define		VALID_RET(x, r)		{ _ASSERT(x); RVALID_RET(x,r) }
#else
	#define		DCHECK(x)			
	#define		RVALID(x)			{ if (!(x)) return; }
	#define		RVALID_RET(x, r)	{ if (!(x)) return(r); }
	#define		VALID(x)			{ RVALID(x) }
	#define		VALID_RET(x, r)		{ RVALID_RET(x,r) }
#endif
// ---------------------------------------------------------------------------------------------


/// 동적 필드 타입
enum DYNAMIC_FIELD_TYPE
{
	DFT_NONE = 0,

	DFT_TRIAL,
	DFT_INNROOM,
	DFT_BATTLEARENA,

	DFT_MAX
};

/// 필드 이동 응답 타입
enum RESPONSE_GATE_TYPE
{
	RGT_FAIL = 0,			///< 실패 - 에러처리
	RGT_REMAIN,				///< 유지 - GM 명령 등으로 없는 필드 이동 시도, 동적필드 개수제한으로 추가생성 실패
	RGT_EXECUTE,			///< 이동실행 - 마스터서버가 필드를 생성해 뒀다
	RGT_RETRY,				///< 재시도 - 잠시 뒤에 다시 시도
};

/// 최초 접속시 입장할 필드 ID - GConfig::TEST_ENABLE_TUTORIAL
// moved to GConst to allow change of field IDs
/*
#define FIRST_ENTER_FIELD_ID	109			///< 부러진 돛대 필드
#define FIRST_TUTORIAL_FIELD_ID 1090000		///< 부러진 돛대 튜토리얼 필드
#define FIRST_ENTER_MARKER_ID	1			///< 1번 마커
*/


/// 피망
#define PMANG_USER_SERIAL_LEN	10	///< 피망 UserSerial 길이, UserSerial은 long값이나 문자열로 다룬다.
#define PMANG_USER_ID_LEN		12	///< 피망 UserID 길이
#define PMANG_PCBID_LEN			12  ///< 피망 PC BID 길이
#define PMANG_SERVICE_TYPE_LEN	12  ///< 피망 ServiceType 길이 (구매상품번호)


#define TIMEOUT_TICK_MOVE_SERVER	180000		///< 서버 이동시 Timeout 체크 시간. (3분)
