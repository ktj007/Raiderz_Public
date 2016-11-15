#pragma once

#include <string>
#include "MTstring.h"
#include "CSCommonLib.h"

/// 클라이언트, 서버 Application의 헬퍼 클래스
class CSCOMMON_API CAppHelper
{
public:
	static std::tstring MakeTitleCaption(const TCHAR* szProgName, const TCHAR* szVersion, const TCHAR* szBuildNumber, const TCHAR* szDate);
	static std::tstring MakeServerVersion(const TCHAR* szVersion, const TCHAR* szBuildNumber, const TCHAR* szDate);
};