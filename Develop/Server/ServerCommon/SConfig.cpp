#include "stdafx.h"
#include "SConfig.h"
#include "SNetworkCardMgr.h"
#include "CSDef.h"


// server info
int SConfig::m_nServerMode = SERVER_MODE_NORMAL;

// log
std::wstring	SConfig::m_strLogPath;
int				SConfig::m_nLogLevel = MLOG_LEVEL_INFORMATION;

// command profiler
wstring	SConfig::m_strCommandProfiler_LogFolder = L"./";
wstring	SConfig::m_strCommandProfiler_LogName = L"profile_command";
bool	SConfig::m_bCommandProfiler_AutoStart = false;
bool	SConfig::m_bCommandProfiler_AutoSave = false;
int		SConfig::m_nCommandProfiler_AutoSaveTickMinute = 60;

// debug
bool	SConfig::m_isCrashDumpAutoAgree = true;
bool	SConfig::m_iocpAppService = false;


#define CONFIG_TOKEN_APP_SERVER_INFO		L"SERVER_INFO"
#define CONFIG_TOKEN_APP_LOG				L"LOG"
#define CONFIG_TOKEN_APP_NETWORK_CARD		L"NETWORK_CARD"
#define CONFIG_TOKEN_APP_DEBUG				L"DEBUG"


void SConfig::Init_INI_Common(const wchar_t* szFileName)
{
	wchar_t szValue[256];

	// server info
	m_nServerMode		= GetPrivateProfileInt(CONFIG_TOKEN_APP_SERVER_INFO,	L"SERVER_MODE",	SERVER_MODE_NORMAL,	szFileName);

	// log
	GetPrivateProfileString(CONFIG_TOKEN_APP_LOG, L"LOG_PATH",	L"",		szValue, 256, szFileName);	m_strLogPath = szValue;
	m_nLogLevel = GetPrivateProfileInt(CONFIG_TOKEN_APP_LOG,	L"LOG_LEVEL",	MLOG_LEVEL_INFORMATION,	szFileName);


	// command profiler
	GetPrivateProfileString(CONFIG_TOKEN_APP_COMMAND_PROFILER, L"LOG_FOLDER",	L"./",				szValue, 256, szFileName);	m_strCommandProfiler_LogFolder = szValue;
	GetPrivateProfileString(CONFIG_TOKEN_APP_COMMAND_PROFILER, L"LOG_FILE",		L"profile_command",	szValue, 256, szFileName);	m_strCommandProfiler_LogName = szValue;
	m_bCommandProfiler_AutoStart = GetPrivateProfileBool(CONFIG_TOKEN_APP_COMMAND_PROFILER,			L"AUTO_START",				false,	szFileName);
	m_bCommandProfiler_AutoSave = GetPrivateProfileBool(CONFIG_TOKEN_APP_COMMAND_PROFILER,			L"AUTO_SAVE",				false,	szFileName);
	m_nCommandProfiler_AutoSaveTickMinute = GetPrivateProfileInt(CONFIG_TOKEN_APP_COMMAND_PROFILER,	L"AUTO_SAVE_TICK_MINUTE",	0,		szFileName);

	if (!m_strCommandProfiler_LogFolder.empty())
	{
		wchar_t c = m_strCommandProfiler_LogFolder[m_strCommandProfiler_LogFolder.size()-1];
		if (c != L'/' || c != L'\\')
		{
			m_strCommandProfiler_LogFolder = m_strCommandProfiler_LogFolder + L"/";
		}
	}

	// debug
	m_isCrashDumpAutoAgree = GetPrivateProfileBool(CONFIG_TOKEN_APP_DEBUG,	L"CRASH_DUMP_AUTO_AGREE",	true,	szFileName);
	m_iocpAppService = GetPrivateProfileBool(CONFIG_TOKEN_APP_DEBUG,	L"IOCP_APP_SERVICE",	false,	szFileName);
}

void SConfig::InitNetworkCard(const wchar_t* szFileName)
{
	GetNetworkCardMgr()->Init();

	int nNetworkCardCount = GetPrivateProfileInt(CONFIG_TOKEN_APP_NETWORK_CARD,	L"COUNT",	1,	szFileName);

	wchar_t szValue[512] = {0,};

	wchar_t szTempNum[4]		= {0,};
	wchar_t szTempCardIP[32]	= L"CARD_IP_";
	wstring strCardIP_Token;

	for (int i = 1; i <= nNetworkCardCount; ++i)
	{
		strCardIP_Token = szTempCardIP;
		_itow_s(i, szTempNum, 4, 10);
		
		strCardIP_Token.append(szTempNum, wcslen(szTempNum));

		GetPrivateProfileString(CONFIG_TOKEN_APP_NETWORK_CARD, strCardIP_Token.c_str(),	L"",	szValue, 256, szFileName);

		GetNetworkCardMgr()->RegCard(szValue, i);
	}
}


bool SConfig::GetPrivateProfileBool(const wchar_t* szAppName, const wchar_t* szKeyName, 
									bool bDefault, const wchar_t* szFileName)
{
	int nDefault = bDefault ? 1 : 0;

	int n;
	n = GetPrivateProfileInt(szAppName, szKeyName, nDefault, szFileName);
	if (n == 0) return false;
	return true;
}

bool SConfig::IsExpoMode()
{
	return (m_nServerMode == SERVER_MODE_EXPO);
}

string SConfig::GetServerModeANSIString()
{
	if (m_nServerMode == SERVER_MODE_NORMAL)	return "NORMAL";
	else if (m_nServerMode == SERVER_MODE_EXPO) return "EXPO";

	return "NONE";
}