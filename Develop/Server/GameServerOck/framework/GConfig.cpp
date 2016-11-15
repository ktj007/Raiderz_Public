#include "stdafx.h"
#include "GConfig.h"
#include "GDef.h"
#include "CSNet.h"
#include "MFileSystem.h"
#include "STypes.h"
#include "MLocale.h"

std::wstring GConfig::m_strGameDB_Server = L"";
std::wstring GConfig::m_strGameDB_DatabaseName = L"";
std::wstring GConfig::m_strGameDB_UserName = L"";
std::wstring GConfig::m_strGameDB_Password = L"";

std::wstring GConfig::m_strLogDB_Server = L"";
std::wstring GConfig::m_strLogDB_DatabaseName = L"";
std::wstring GConfig::m_strLogDB_UserName = L"";
std::wstring GConfig::m_strLogDB_Password = L"";

int GConfig::m_nMaxUser = DEFAULT_MAX_PLAYER;
std::wstring GConfig::m_strServerName = DEFAULT_SERVER_NAME;
uint8 GConfig::m_nServerType = DEFAULT_SERVER_TYPE;
int GConfig::m_SSN = 0;
int GConfig::m_nMyWorldID = 0;
int GConfig::m_nMyServerID = 0;
wstring GConfig::m_strOdbcDriver = L"SQL Server";	
wstring GConfig::m_strPublicIP = L"";					// 디폴트값 널문자열 - 내부IP 사용
int GConfig::m_nSocketPoolSize = 2000;
int GConfig::m_nSendPendingLimitCount = 10000;			// 1만개. (1천개 하고 싶으나, 테스트용으로 NPC 디버그 스트링 요청 등으로 5천정도까지 쌓일 수도 있음.)

bool GConfig::m_bWindowVisible = true;
int	GConfig::m_nMyNetworkCardID = 0;
int GConfig::m_nPort = DEFAULT_GAME_SERVER_PORT;
bool GConfig::m_bStandAlone = false;

bool GConfig::m_bEnableHeartBeat = true;
int GConfig::m_nHeartBeatTickTime = 30000;
int GConfig::m_nHeartBeatTimeout = 600000;

wchar_t GConfig::m_szTestSuite[4][128] = {L"", L"", L"", L""};
DWORD GConfig::m_dwPacketDelayTime = 0;
bool GConfig::m_bDBTaskCount = false;
bool GConfig::m_bNoDB = false;
bool GConfig::m_bLogPoolCount = false;

bool GConfig::m_bExportUpdateDB = false;
bool GConfig::m_bExportWriteFile = false;
wstring GConfig::m_strExportLocale = L"ko_KR";

bool GConfig::m_bAutoTestActive = false;
wstring GConfig::m_strAutoTestType = L"";
GAITestType GConfig::m_nAITestType = AITEST_NONE;
GCommandRecordingType GConfig::m_nCommandRecordingType = COMMANDREC_NONE;
int GConfig::m_nRepeat=0;
DWORD GConfig::m_dwTime=0;	
int GConfig::m_nSpeed=0;
float GConfig::m_fLimitFrame=0.0f;

wstring GConfig::m_strAutoTestFieldlistFileName;
int GConfig::m_nNPCCount=0;
int GConfig::m_nFieldID=0;
int GConfig::m_nQPVPEventID=0;
bool GConfig::m_bFullMemoryCrashDump = false;
bool GConfig::m_bDumpEnable = true;
wstring GConfig::m_strDumpDestServerIP = L"localhost";
int GConfig::m_nDumpDestServerPort = m_nDumpDestServerPort;

int GConfig::m_nPrivateNetworkCardID = 0;
wstring GConfig::m_strMasterServerIP = L"localhost";
int	GConfig::m_nMasterServerPort = DEFAULT_MASTER_SERVER_PORT;

bool GConfig::m_bLimitRunTime = false;
DWORD GConfig::m_dwRunTime = 0;

// PMS
bool GConfig::m_bPMSEnable = false;

// GameGuard
bool GConfig::m_bGGEnable = false;
bool GConfig::m_bGGShowDebugLog = false;
int GConfig::m_nGGInitNumActive = 50;
int GConfig::m_nGGTimeLimit = 30;
int GConfig::m_nGGCondition = 50;
int GConfig::m_nGGAuthInterval = 5 * 60; // 5분 권장

// pmang db encrypt.
bool GConfig::m_bPmangDBEncrypt = false;

// command profiler
tstring	GConfig::m_strCommandProfiler_LogName_ForMasterServer = L"profile_mastersrv_command";
tstring	GConfig::m_strCommandProfiler_LogName_ForAppServer = L"profile_appsrv_command";

#define CONFIG_TOKEN_APP_DB					L"DB"
#define CONFIG_TAKEN_APP_LOGDB				L"LOGDB"
#define CONFIG_TOKEN_APP_CONFIG				L"CONFIG"
#define CONFIG_TOKEN_APP_DEBUG				L"DEBUG"
#define CONFIG_TOKEN_APP_NET				L"NET"
#define CONFIG_TOKEN_APP_SERVER_INFO		L"SERVER_INFO"
#define CONFIG_TOKEN_ADD_EXPORT				L"EXPORT"
#define CONFIG_TOKEN_APP_GAMEGUARD			L"GAMEGUARD"
#define CONFIG_TOKEN_APP_DEVELOP			L"DEVELOP"

void GConfig::Init_INI()
{
	const wchar_t* szFileName = FILENAME_SERVER_CONFIG_REL;
	if (MIsExistFile(FILENAME_SERVER_CONFIG)) szFileName = FILENAME_SERVER_CONFIG;

	SConfig::Init_INI_Common(szFileName);
	SConfig::InitNetworkCard(szFileName);

	wchar_t szValue[256];

	// server info
	GetPrivateProfileString(CONFIG_TOKEN_APP_SERVER_INFO, L"NAME",	DEFAULT_SERVER_NAME,	szValue, 256, szFileName);	m_strServerName = szValue;
	m_nServerType		= SERVER_GAME;
	m_nMyWorldID		= GetPrivateProfileInt(CONFIG_TOKEN_APP_SERVER_INFO,	L"WORLD_ID",			0,	szFileName);
	m_SSN				= GetPrivateProfileInt(CONFIG_TOKEN_APP_SERVER_INFO,	L"PMANG_SSN",		0,	szFileName);
	m_nMyServerID		= GetPrivateProfileInt(CONFIG_TOKEN_APP_SERVER_INFO,	L"SERVER_ID",		0,	szFileName);
	m_bPMSEnable		= GetPrivateProfileBool(CONFIG_TOKEN_APP_SERVER_INFO,	L"PMS_ENABLE",		0,	szFileName);
	m_bPmangDBEncrypt	= GetPrivateProfileBool(CONFIG_TOKEN_APP_SERVER_INFO,	L"PMANG_DBENCRYPT",	0,	szFileName);	
	GetPrivateProfileString(CONFIG_TOKEN_APP_SERVER_INFO, L"ODBC", L"SQL Server", szValue, 256, szFileName); m_strOdbcDriver = szValue;
	m_nSocketPoolSize	= GetPrivateProfileInt(CONFIG_TOKEN_APP_SERVER_INFO,	L"SOCKET_POOL_SIZE",	2000,	szFileName);
	m_nSendPendingLimitCount = GetPrivateProfileInt(CONFIG_TOKEN_APP_SERVER_INFO, L"SEND_PENDING_LIMIT_COUNT", 10000, szFileName);

	m_bGGEnable			= GetPrivateProfileBool(CONFIG_TOKEN_APP_GAMEGUARD,	L"ENABLE",			0,	szFileName);
	m_bGGShowDebugLog	= GetPrivateProfileBool(CONFIG_TOKEN_APP_GAMEGUARD,	L"DEBUG_LOG",		0,	szFileName);
	m_nGGInitNumActive	= GetPrivateProfileInt(CONFIG_TOKEN_APP_GAMEGUARD,	L"INIT_NUM_ACTIVE",	0,	szFileName);
	m_nGGTimeLimit		= GetPrivateProfileInt(CONFIG_TOKEN_APP_GAMEGUARD,	L"TIME_LIMIT",		0,	szFileName);
	m_nGGCondition		= GetPrivateProfileInt(CONFIG_TOKEN_APP_GAMEGUARD,	L"CONDITION",		0,	szFileName);
	m_nGGAuthInterval	= GetPrivateProfileInt(CONFIG_TOKEN_APP_GAMEGUARD,	L"AUTH_INTERVAL",	0,	szFileName);
		
	// config
	m_bWindowVisible = GetPrivateProfileBool(CONFIG_TOKEN_APP_CONFIG,	L"WINDOW_VISIBLE",		true,						szFileName);	
	m_nMyNetworkCardID	= GetPrivateProfileInt(CONFIG_TOKEN_APP_CONFIG, L"NETWORK_CARD",	0,		szFileName);
	m_nPort = GetPrivateProfileInt(CONFIG_TOKEN_APP_CONFIG,				L"PORT",					DEFAULT_GAME_SERVER_PORT,	szFileName);
	m_bStandAlone = GetPrivateProfileBool(CONFIG_TOKEN_APP_CONFIG,		L"STAND_ALONE",			false,						szFileName);
	m_bEnableHeartBeat = GetPrivateProfileBool(CONFIG_TOKEN_APP_CONFIG,	L"ENABLE_HEART_BEAT",		true,						szFileName);
	m_nHeartBeatTickTime	= GetPrivateProfileInt(CONFIG_TOKEN_APP_CONFIG, L"HEART_BEAT_TICK",		30000,		szFileName);
	m_nHeartBeatTimeout		= GetPrivateProfileInt(CONFIG_TOKEN_APP_CONFIG, L"HEART_BEAT_TIMEOUT",	600000,		szFileName);
	

	GetPrivateProfileString(CONFIG_TOKEN_APP_CONFIG, L"PUBLIC_IP", L"", szValue, 256, szFileName);	m_strPublicIP = szValue;

	if (m_strPublicIP.empty())
	{
		// 외부 접속 IP 설정 없으면, 현재 내부 IP로 설정
		m_strPublicIP = MLocale::ConvAnsiToUCS2(MiNetHelper::GetHostIPString().c_str());
	}

	// net
	m_nPrivateNetworkCardID	= GetPrivateProfileInt(CONFIG_TOKEN_APP_NET, L"NETWORK_CARD",	0, szFileName);
	GetPrivateProfileString(CONFIG_TOKEN_APP_NET, L"MASTER_SERVER_IP",	L"localhost",			szValue, 256, szFileName);	m_strMasterServerIP = szValue;
	m_nMasterServerPort = GetPrivateProfileInt(CONFIG_TOKEN_APP_NET,	L"MASTER_SERVER_PORT",	DEFAULT_MASTER_SERVER_PORT, szFileName);

	// db
	GetPrivateProfileString(CONFIG_TOKEN_APP_DB, L"SERVER",		L"SH_DB",		szValue, 256, szFileName);	m_strGameDB_Server = szValue;
	GetPrivateProfileString(CONFIG_TOKEN_APP_DB, L"DATABASE",	L"SoulHuntDB",	szValue, 256, szFileName);	m_strGameDB_DatabaseName = szValue;
	GetPrivateProfileString(CONFIG_TOKEN_APP_DB, L"USERNAME",	L"dev",			szValue, 256, szFileName);	m_strGameDB_UserName = szValue;
	GetPrivateProfileString(CONFIG_TOKEN_APP_DB, L"PASSWORD",	L"dev",			szValue, 256, szFileName);	m_strGameDB_Password = szValue;

	InitLogDB(szFileName);

	// debug
	if (!GetPrivateProfileString(CONFIG_TOKEN_APP_DEBUG, L"TEST_SUITE",		L"", m_szTestSuite[0], 128, szFileName))
	{
		GetPrivateProfileString(CONFIG_TOKEN_APP_DEBUG, L"TEST_SUITE1",		L"", m_szTestSuite[0], 128, szFileName);
	}
	GetPrivateProfileString(CONFIG_TOKEN_APP_DEBUG, L"TEST_SUITE2",		L"", m_szTestSuite[1], 128, szFileName);
	GetPrivateProfileString(CONFIG_TOKEN_APP_DEBUG, L"TEST_SUITE3",		L"", m_szTestSuite[2], 128, szFileName);
	GetPrivateProfileString(CONFIG_TOKEN_APP_DEBUG, L"TEST_SUITE4",		L"", m_szTestSuite[3], 128, szFileName);


	m_dwPacketDelayTime = GetPrivateProfileInt(CONFIG_TOKEN_APP_DEBUG, L"PACKET_DELAY", 0, szFileName);
	m_bDumpEnable = GetPrivateProfileBool(CONFIG_TOKEN_APP_DEBUG,	L"DUMP_ENABLE",	true,						szFileName);
	m_bFullMemoryCrashDump = GetPrivateProfileBool(CONFIG_TOKEN_APP_DEBUG,	L"FULL_DUMP",	false,						szFileName);
	m_bDBTaskCount = GetPrivateProfileBool(CONFIG_TOKEN_APP_DEBUG,	L"DBTASK_COUNT",	false,						szFileName);
	m_bNoDB = GetPrivateProfileBool(CONFIG_TOKEN_APP_DEBUG,	L"NO_DB",	false,						szFileName);
	m_bLogPoolCount = GetPrivateProfileBool(CONFIG_TOKEN_APP_DEBUG,	L"LOG_MEMPOOL_COUNT",false, szFileName);

	m_bExportUpdateDB = GetPrivateProfileBool(CONFIG_TOKEN_ADD_EXPORT, L"UPDATE_DB", 0, szFileName);
	m_bExportWriteFile = GetPrivateProfileBool(CONFIG_TOKEN_ADD_EXPORT, L"WRITE_FILE", 0, szFileName);
	GetPrivateProfileString(CONFIG_TOKEN_ADD_EXPORT, L"EXPORT_LOCALE", L"ko_KR", szValue, 256, szFileName); m_strExportLocale = szValue;

	
	GetPrivateProfileString(CONFIG_TOKEN_APP_DEBUG,	L"DUMP_DEST_SERVERIP",	L"localhost", szValue, 128, szFileName); m_strDumpDestServerIP = szValue;
	m_nDumpDestServerPort = GetPrivateProfileInt(CONFIG_TOKEN_APP_DEBUG,	L"DUMP_DEST_SERVERPORT",	21,			szFileName);

	// command profiler
	GetPrivateProfileString(CONFIG_TOKEN_APP_COMMAND_PROFILER,	L"LOG_FILE_MASTER_SERVER",	L"profile_mastersrv_command",szValue, 256, szFileName);	m_strCommandProfiler_LogName_ForMasterServer = szValue;
	GetPrivateProfileString(CONFIG_TOKEN_APP_COMMAND_PROFILER,	L"LOG_FILE_APP_SERVER",		L"profile_appsrv_command",	szValue, 256, szFileName);	m_strCommandProfiler_LogName_ForAppServer = szValue;

	// StandAlone의 경우 디폴트값을 바꿔 다시 읽습니다.
	if (m_bStandAlone)
	{
		m_isCrashDumpAutoAgree = GetPrivateProfileBool(CONFIG_TOKEN_APP_DEBUG,	L"CRASH_DUMP_AUTO_AGREE",	false,	szFileName);
	}
}


void GConfig::Init_Arg(const GConfig_Arg& arg)
{
	if (false != arg.bAutoTestActive)
	{
		m_bAutoTestActive = arg.bAutoTestActive;
	}

	if (false == arg.strAutoTestType.empty())
	{
		m_strAutoTestType = arg.strAutoTestType;
	}

	if (0 != arg.nAITestType)
	{
		m_nAITestType = arg.nAITestType;
	}

	if (0 != arg.nCommandRecordingType)
	{
		m_nCommandRecordingType = arg.nCommandRecordingType;
	}
	
	if (0 != arg.nRepeat)
	{
		m_nRepeat = arg.nRepeat;
	}

	if (0 != arg.dwTime)
	{
		m_dwTime = arg.dwTime;
	}

	if (0 != arg.nSpeed)
	{
		m_nSpeed = arg.nSpeed;
	}

	if (0.0f != arg.fLimitFrame)
	{
		m_fLimitFrame = arg.fLimitFrame;
	}

	if (0 != arg.nNPCCount)
	{
		m_nNPCCount = arg.nNPCCount;
	}

	if (0 != arg.nFieldID)
	{
		m_nFieldID = arg.nFieldID;
	}

	m_bLimitRunTime = arg.bLimitRunTime;
	m_dwRunTime = arg.dwRunTime;

	if (arg.bAutoStartCommandProfile)
	{
		// 커맨드라인에서 설정하면, INI 파일 무시하고 덮어쓴다.
		m_bCommandProfiler_AutoStart = arg.bAutoStartCommandProfile;		
	}
}

void GConfig::Init_Lua(const GConfig_Lua& lua)
{
	if (false != lua.bAutoTestActive)
	{
		m_bAutoTestActive = lua.bAutoTestActive;
	}

	if (false == lua.strAutoTestType.empty())
	{
		m_strAutoTestType = lua.strAutoTestType;
	}

	if (0 != lua.nAITestType)
	{
		m_nAITestType = lua.nAITestType;
	}

	if (0 != lua.nRepeat)
	{
		m_nRepeat = lua.nRepeat;
	}

	if (0 != lua.dwTime)
	{
		m_dwTime = lua.dwTime;
	}

	if (0 != lua.nSpeed)
	{
		m_nSpeed = lua.nSpeed;
	}

	if (0.0f != lua.fLimitFrame)
	{
		m_fLimitFrame = lua.fLimitFrame;
	}

	if (0 != lua.nNPCCount)
	{
		m_nNPCCount = lua.nNPCCount;
	}

	if (0 != lua.nFieldID)
	{
		m_nFieldID = lua.nFieldID;
	}

	if (0 != lua.nQPVPEventID)
	{
		m_nQPVPEventID = lua.nQPVPEventID;
	}
}

void GConfig::InitLogDB(const wchar_t* szFileName)
{
	wchar_t szValue[256];

	GetPrivateProfileString(CONFIG_TAKEN_APP_LOGDB, L"SERVER",		L"SH_DB",		szValue, 256, szFileName);	m_strLogDB_Server		= szValue;
	GetPrivateProfileString(CONFIG_TAKEN_APP_LOGDB, L"DATABASE",	L"LogDB",		szValue, 256, szFileName);	m_strLogDB_DatabaseName = szValue;
	GetPrivateProfileString(CONFIG_TAKEN_APP_LOGDB, L"USERNAME",	L"dev",			szValue, 256, szFileName);	m_strLogDB_UserName		= szValue;
	GetPrivateProfileString(CONFIG_TAKEN_APP_LOGDB, L"PASSWORD",	L"dev",			szValue, 256, szFileName);	m_strLogDB_Password		= szValue;
}