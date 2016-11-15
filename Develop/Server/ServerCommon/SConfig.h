#pragma once

#include "ServerCommonLib.h"
#include "MTstring.h"

#define CONFIG_TOKEN_APP_COMMAND_PROFILER	L"COMMAND_PROFILER"

// 각 서버의 Config클래스의 부모 클래스
class SCOMMON_API SConfig
{
protected:
	static void Init_INI_Common(const wchar_t* szFileName);
	static void InitNetworkCard(const wchar_t* szFileName);
	static bool GetPrivateProfileBool(const wchar_t* szAppName, const wchar_t* szKeyName, bool bDefault, const wchar_t* szFileName);	

public:
	static bool				IsExpoMode();
	static string			GetServerModeANSIString();

	// server info
	static int				m_nServerMode;			///< 서버 모드. 0 - normal, 1 - Expo

	// log
	static std::wstring		m_strLogPath;			///< 로그를 남길 경로
	static int				m_nLogLevel;			///< 로그 경로

	// command profiler
	static wstring	m_strCommandProfiler_LogFolder;			// 커맨드 프로파일러 로그가 남길 폴더
	static wstring	m_strCommandProfiler_LogName;			// 커맨드 프로파일러 로그 파일 이름
	static bool		m_bCommandProfiler_AutoStart;			// 커맨드 프로파일러를 자동으로 시작할 것인지 여부
	static bool		m_bCommandProfiler_AutoSave;			// 커맨드 프로파일러 로그 자동으로 남길지 여부
	static int		m_nCommandProfiler_AutoSaveTickMinute;	// 커맨드 프로파일러 로그 자동으로 남길 때 틱 분
	
	// debug
	static bool		m_isCrashDumpAutoAgree;			// 크래시 덤프 리포트 자동보내기
	static bool		m_iocpAppService;
};