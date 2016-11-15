#include "stdafx.h"
#include "LConfig.h"
#include "LDef.h"
#include "CSNet.h"
#include "MFileSystem.h"
#include "STypes.h"

LDBConfig LConfig::m_AccountDBConfig;
LDBConfig LConfig::m_GameDBConfig;
LDBConfig LConfig::m_LogDBConfig;

bool LConfig::m_bWindowVisible = true;
int LConfig::m_nMyNetworkCardID = 0;
int LConfig::m_nPort = DEFAULT_LOGIN_SERVER_PORT;
int LConfig::m_nUDPPort = DEFAULT_LOCATOR_SERVER_PORT;
bool LConfig::m_bStandAlone = false;			///< 마스터서버 없이 실행할지 여부
int LConfig::m_nMoveServerTimeout = 300000;		// 5 분
int LConfig::m_nCommandTimeout = 300000;		// 5 분
bool LConfig::m_isAllowInsertNewAccount = true;

std::wstring	LConfig::m_strSystemPath;
std::wstring	LConfig::m_strFieldPath;
std::wstring	LConfig::m_strZonePath;


// server info
std::wstring LConfig::m_strServerName = L"LoginServer";
uint8 LConfig::m_nServerType = 0;
int LConfig::m_nServerID = 0;
int LConfig::m_nWorldID = 0;
int LConfig::m_SSN = 0;
wstring LConfig::m_strOdbcDriver = L"SQL Server";
int LConfig::m_nSocketPoolSize = 2000;
int LConfig::m_nSendPendingLimitCount = 100;		// 1백개. 조금만 쌓여도 가차없이 잘라버립니다.

// master server
int LConfig::m_nMasterServer_NetworkCardID = 0;
wstring LConfig::m_strMasterServerIP(L"localhost");
int LConfig::m_nMasterServerPort = DEFAULT_MASTER_SERVER_PORT;

// Pmang
wstring LConfig::m_strPmangServerKey = L"shsxk!@rptxld";		// 네오위즈에서 하드코딩하라고 했습니다.
uint32 LConfig::m_nPmangToleranceSeconds = 3600;			// 1시간 (런처 업데이트 포함 안전 시간)

// PMS
bool LConfig::m_bPMSEnable = false;

// pmang db encrypt.
bool LConfig::m_bPmangDBEncrypt = false;

// debug
wstring LConfig::m_strDumpDestServerIP = L"localhost";
int LConfig::m_nDumpDestServerPort = m_nDumpDestServerPort;

#define CONFIG_TOKEN_APP_DB					L"DB"
#define CONFIG_TOKEN_APP_ACCOUNTDB			L"ACCOUNTDB"
#define CONFIG_TOKEN_APP_LOGDB				L"LOGDB"
#define CONFIG_TOKEN_APP_CONFIG				L"CONFIG"
#define CONFIG_TOKEN_APP_CONST				L"CONST"
#define CONFIG_TOKEN_APP_SERVER_INFO		L"SERVER_INFO"
#define CONFIG_TOKEN_APP_MASTER_SERVER		L"MASTER_SERVER"
#define CONFIG_TOKEN_APP_PATH				L"PATH"
#define CONFIG_TOKEN_APP_PMANG				L"PMANG"
#define CONFIG_TOKEN_APP_DEBUG				L"DEBUG"


void LConfig::Init_INI()
{
	const wchar_t* szFileName = FILENAME_CONFIG_REL;
	if (MIsExistFile(FILENAME_CONFIG)) szFileName = FILENAME_CONFIG;

	SConfig::Init_INI_Common(szFileName);
	SConfig::InitNetworkCard(szFileName);
	

	wchar_t szValue[512];

	// config
	m_nMyNetworkCardID	= GetPrivateProfileInt(CONFIG_TOKEN_APP_CONFIG, L"NETWORK_CARD",	0, szFileName);
	m_nPort				= GetPrivateProfileInt(CONFIG_TOKEN_APP_CONFIG, L"PORT",			DEFAULT_MASTER_SERVER_PORT, szFileName);
	m_nUDPPort			= GetPrivateProfileInt(CONFIG_TOKEN_APP_CONFIG, L"UDP_PORT",		DEFAULT_LOCATOR_SERVER_PORT, szFileName);
	m_bStandAlone		= GetPrivateProfileBool(CONFIG_TOKEN_APP_CONFIG,L"STAND_ALONE",		false,						szFileName);

	// db
	InitAccountDB(szFileName);
	InitGameDB(szFileName);
	InitLogDB(szFileName);

	// server info
	GetPrivateProfileString(CONFIG_TOKEN_APP_SERVER_INFO, L"NAME",		L"LoginServer",		szValue, 256, szFileName);	m_strServerName = szValue;
	m_nServerType		= SERVER_LOGIN;
	m_nServerID			= GetPrivateProfileInt(CONFIG_TOKEN_APP_SERVER_INFO,		L"SERVER_ID",			0,	szFileName);	
	m_nWorldID			= GetPrivateProfileInt(CONFIG_TOKEN_APP_SERVER_INFO,			L"WORLD_ID",			0,	szFileName);
	GetPrivateProfileString(CONFIG_TOKEN_APP_SERVER_INFO, L"ODBC", L"SQL Server", szValue, 256, szFileName); m_strOdbcDriver = szValue;
	m_nSocketPoolSize	= GetPrivateProfileInt(CONFIG_TOKEN_APP_SERVER_INFO,	L"SOCKET_POOL_SIZE",	2000,	szFileName);
	m_nSendPendingLimitCount = GetPrivateProfileInt(CONFIG_TOKEN_APP_SERVER_INFO, L"SEND_PENDING_LIMIT_COUNT",	100, szFileName);

	// master server
	m_nMasterServer_NetworkCardID	= GetPrivateProfileInt(CONFIG_TOKEN_APP_MASTER_SERVER, L"NETWORK_CARD",	0, szFileName);
	GetPrivateProfileString(CONFIG_TOKEN_APP_MASTER_SERVER, L"IP",		L"localhost",		szValue, 256, szFileName);	m_strMasterServerIP = szValue;
	m_nMasterServerPort				= GetPrivateProfileInt(CONFIG_TOKEN_APP_MASTER_SERVER, L"PORT",			DEFAULT_MASTER_SERVER_PORT,	szFileName);
	
	// path
	m_strSystemPath = GetPathString(szFileName, L"SYSTEM");
	m_strFieldPath = GetPathString(szFileName, L"FIELD");
	m_strZonePath = GetPathString(szFileName, L"ZONE");


	// Pmang	
	m_nPmangToleranceSeconds = GetPrivateProfileInt(CONFIG_TOKEN_APP_PMANG, L"TOLERANCE_SECONDS", 3600, szFileName);
	m_bPMSEnable	= GetPrivateProfileBool(CONFIG_TOKEN_APP_SERVER_INFO,	L"PMS_ENABLE",	0,	szFileName);
	m_SSN				= GetPrivateProfileInt(CONFIG_TOKEN_APP_SERVER_INFO,	L"PMANG_SSN",	0,	szFileName);
	m_bPmangDBEncrypt	= GetPrivateProfileBool(CONFIG_TOKEN_APP_SERVER_INFO,	L"PMANG_DBENCRYPT",	0,	szFileName);	


	// const
	m_bWindowVisible		= GetPrivateProfileBool(CONFIG_TOKEN_APP_CONST,	L"WINDOW_VISIBLE",		true,	szFileName);
	m_nMoveServerTimeout	= GetPrivateProfileInt(CONFIG_TOKEN_APP_CONST,	L"MOVE_SERVER_TIMEOUT",	60000,	szFileName);
	m_nCommandTimeout		= GetPrivateProfileInt(CONFIG_TOKEN_APP_CONST,	L"COMMAND_TIMEOUT",		30000,	szFileName);
	m_isAllowInsertNewAccount = GetPrivateProfileBool(CONFIG_TOKEN_APP_CONST, L"ALLOW_INSERT_NEW_ACCOUNT", true, szFileName);

	// debug
	GetPrivateProfileString(CONFIG_TOKEN_APP_DEBUG,	L"DUMP_DEST_SERVERIP",	L"localhost", szValue, 128, szFileName); m_strDumpDestServerIP = szValue;
	m_nDumpDestServerPort = GetPrivateProfileInt(CONFIG_TOKEN_APP_DEBUG,	L"DUMP_DEST_SERVERPORT",	21,			szFileName);
}



wstring LConfig::GetPathString( const wchar_t* szFileName, const wchar_t* szKeyName )
{
	wstring strRet;
	wchar_t szValue[512] = L"";

	GetPrivateProfileString(CONFIG_TOKEN_APP_PATH, szKeyName,	L"./", szValue, 512, szFileName);

	strRet = szValue;

	if (strRet.size() > 0)
	{
		if (strRet[strRet.size()-1] != L'/' &&
			strRet[strRet.size()-1] != L'\\')
		{
			strRet = strRet + L'/';
		}
	}

	return strRet;
}

void LConfig::InitAccountDB( const wchar_t* szFileName )
{
	wchar_t szValue[512];

	GetPrivateProfileString(CONFIG_TOKEN_APP_ACCOUNTDB, L"SERVER",		L"SH_DB",	szValue, 256, szFileName);	m_AccountDBConfig.strServer		= szValue;
	GetPrivateProfileString(CONFIG_TOKEN_APP_ACCOUNTDB, L"DATABASE",	L"Main",		szValue, 256, szFileName);	m_AccountDBConfig.strDBName		= szValue;
	GetPrivateProfileString(CONFIG_TOKEN_APP_ACCOUNTDB, L"USERNAME",	L"dev",		szValue, 256, szFileName);	m_AccountDBConfig.strUserName	= szValue;
	GetPrivateProfileString(CONFIG_TOKEN_APP_ACCOUNTDB, L"PASSWORD",	L"dev",		szValue, 256, szFileName);	m_AccountDBConfig.strPassword	= szValue;
}

void LConfig::InitGameDB( const wchar_t* szFileName )
{
	wchar_t szValue[512];

	GetPrivateProfileString(CONFIG_TOKEN_APP_DB, L"SERVER",		L"SH_DB",		szValue, 256, szFileName);	m_GameDBConfig.strServer	= szValue;
	GetPrivateProfileString(CONFIG_TOKEN_APP_DB, L"DATABASE",	L"SoulHuntDB",	szValue, 256, szFileName);	m_GameDBConfig.strDBName	= szValue;
	GetPrivateProfileString(CONFIG_TOKEN_APP_DB, L"USERNAME",	L"dev",			szValue, 256, szFileName);	m_GameDBConfig.strUserName	= szValue;
	GetPrivateProfileString(CONFIG_TOKEN_APP_DB, L"PASSWORD",	L"dev",			szValue, 256, szFileName);	m_GameDBConfig.strPassword	= szValue;
}

void LConfig::InitLogDB( const wchar_t* szFileName )
{
	wchar_t szValue[512];

	GetPrivateProfileString(CONFIG_TOKEN_APP_LOGDB, L"SERVER",		L"SH_DB",	szValue, 256, szFileName);	m_LogDBConfig.strServer		= szValue;
	GetPrivateProfileString(CONFIG_TOKEN_APP_LOGDB, L"DATABASE",	L"LogDB",	szValue, 256, szFileName);	m_LogDBConfig.strDBName		= szValue;
	GetPrivateProfileString(CONFIG_TOKEN_APP_LOGDB, L"USERNAME",	L"dev",		szValue, 256, szFileName);	m_LogDBConfig.strUserName	= szValue;
	GetPrivateProfileString(CONFIG_TOKEN_APP_LOGDB, L"PASSWORD",	L"dev",		szValue, 256, szFileName);	m_LogDBConfig.strPassword	= szValue;
}