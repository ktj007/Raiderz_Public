#pragma once

#include "CSCommonLib.h"


// 커맨드 번호 20000 ~ 21000  까지 사용
enum CWorldLocatorCommandTable
{					
	MWC_COMM_WORLD_INFO_REQ = 20001,		///< (C->S)	월드 정보 요청
	MWC_COMM_WORLD_INFO_RES = 20002,		///< (S->C) 월드 정보 응답
};

CSCOMMON_API void AddWorldLocatorCommandTable();