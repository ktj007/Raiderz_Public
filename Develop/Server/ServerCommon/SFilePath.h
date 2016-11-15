#pragma once

#include "ServerCommonLib.h"
#include "MTstring.h"

/// 파일 패쓰를 처리하는 클래스
class SCOMMON_API SFilePath
{
private:
public:
	SFilePath() {}
	static wstring MakeFieldList(const wchar_t* szSystemPath, bool bCheckDebugFieldList=true);
	static wstring MakeZoneList(const wchar_t* szSystemPath);
};