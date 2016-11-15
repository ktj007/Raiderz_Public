#pragma once

#define FILENAME_LOG				L"mlog.txt"
#define LOG_FILE_HEADER				L"mlog"

/// 로그 관리 클래스 
class SLogger
{
private:
	static void AddPathSeperator( wstring& strOut );
	static wstring MakeFileName( wstring strLogPath );
public:
	static void Init(MLogLevel nLogLevel, wstring strLogPath);
};