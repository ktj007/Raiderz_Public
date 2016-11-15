#pragma once

#include <string>
#include "ServerCommonLib.h"

class SCOMMON_API SCrashReporter
{
private:
	static std::wstring m_strProgName;
	static bool m_bReportLog;
	static wstring m_strDumpDestServerIP;
	static int m_nDumpDestServerPort;
public:
	static void Init(const wchar_t* szProgName, bool bReportLog=true, wstring strServerIP=L"icecream", int nServerPort=21);
	static void CrashDumpHandler(const char* szFileName);
	static void ExecuteBAReport(const wchar_t* szProgName,  const wchar_t* szFileName, bool bReportLog, wstring strServerIP=L"icecream", int nServerPort=21);
};
