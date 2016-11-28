#include "stdafx.h"
#include "ZConfig.h"
#include "ZDef.h"
#include "CSNet.h"
#include "MFileSystem.h"

std::wstring ZConfig::m_strServerName = DEFAULT_SERVER_NAME;
int ZConfig::m_nMaxUser = DEFAULT_MAX_PLAYER;
uint8 ZConfig::m_nServerType = DEFAULT_SERVER_TYPE;
int ZConfig::m_SSN = 0;
int ZConfig::m_nMyWorldID = 0;
int ZConfig::m_nMyServerID = 0;
bool ZConfig::m_bPMSEnable = false;
bool ZConfig::m_bPmangDBEncrypt = false;
wstring ZConfig::m_strOdbcDriver = L"SQL Server";
int ZConfig::m_nSocketPoolSize = 2000;
int ZConfig::m_nSendPendingLimitCount = 200000;		// 20만개. 서버간에는 너그럽게 처리합니다.

bool ZConfig::m_bWindowVisible = true;
int ZConfig::m_nPort = DEFAULT_MASTER_SERVER_PORT;

ZDBConfig ZConfig::m_AccountDBConfig;
ZDBConfig ZConfig::m_GameDBConfig;
ZDBConfig ZConfig::m_LogDBConfig;

std::wstring ZConfig::m_strSystemPath;
std::wstring	ZConfig::m_strFieldPath;

wstring ZConfig::m_strDumpDestServerIP = L"localhost";
int ZConfig::m_nDumpDestServerPort = m_nDumpDestServerPort;


#define CONFIG_TOKEN_APP_ACCOUNTDB			L"ACCOUNTDB"
#define CONFIG_TOKEN_APP_DB					L"DB"
#define CONFIG_TOKEN_APP_LOGDB				L"LOGDB"
#define CONFIG_TOKEN_APP_CONFIG				L"CONFIG"
#define CONFIG_TOKEN_APP_DEBUG				L"DEBUG"
#define CONFIG_TOKEN_APP_PATH				L"PATH"
#define CONFIG_TOKEN_APP_SERVER_INFO		L"SERVER_INFO"


void ZConfig::Init_INI()
{	
	const wchar_t* szFileName = FILENAME_CONFIG_REL;
	if (MIsExistFile(FILENAME_CONFIG)) szFileName = FILENAME_CONFIG;

	SConfig::Init_INI_Common(szFileName);

	wchar_t szValue[256] = L"";

	// server info
	GetPrivateProfileString(CONFIG_TOKEN_APP_SERVER_INFO, L"NAME",	DEFAULT_SERVER_NAME,	szValue, 256, szFileName);	m_strServerName = szValue;
	m_nServerType			= SERVER_MASTER;
	m_nMyWorldID		= GetPrivateProfileInt(CONFIG_TOKEN_APP_SERVER_INFO,	L"WORLD_ID",		0,	szFileName);
	m_SSN				= GetPrivateProfileInt(CONFIG_TOKEN_APP_SERVER_INFO,	L"PMANG_SSN",	0,	szFileName);
	m_nMyServerID		= GetPrivateProfileInt(CONFIG_TOKEN_APP_SERVER_INFO,	L"SERVER_ID",	0,	szFileName);
	m_bPMSEnable		= GetPrivateProfileBool(CONFIG_TOKEN_APP_SERVER_INFO,	L"PMS_ENABLE",	0,	szFileName);
	m_bPmangDBEncrypt	= GetPrivateProfileBool(CONFIG_TOKEN_APP_SERVER_INFO,	L"PMANG_DBENCRYPT",	0,	szFileName);	
	GetPrivateProfileString(CONFIG_TOKEN_APP_SERVER_INFO, L"ODBC", L"SQL Server", szValue, 256, szFileName); m_strOdbcDriver = szValue;
	m_nSocketPoolSize	= GetPrivateProfileInt(CONFIG_TOKEN_APP_SERVER_INFO,	L"SOCKET_POOL_SIZE",	2000,	szFileName);
	m_nSendPendingLimitCount = GetPrivateProfileInt(CONFIG_TOKEN_APP_SERVER_INFO, L"SEND_PENDING_LIMIT_COUNT", 200000, szFileName);
	
	// config
	m_bWindowVisible= GetPrivateProfileBool(CONFIG_TOKEN_APP_CONFIG,	L"WINDOW_VISIBLE",	true,						szFileName);
	m_nPort			= GetPrivateProfileInt(CONFIG_TOKEN_APP_CONFIG,		L"PORT",				DEFAULT_MASTER_SERVER_PORT,	szFileName);

	InitAccountDB(szFileName);
	InitGameDB(szFileName);
	InitLogDB(szFileName);

	// path
	m_strSystemPath	= GetPathString(szFileName, L"SYSTEM");
	m_strFieldPath	= GetPathString(szFileName, L"FIELD");

	// debug
	GetPrivateProfileString(CONFIG_TOKEN_APP_DEBUG,	L"DUMP_DEST_SERVERIP",	L"localhost", szValue, 128, szFileName); m_strDumpDestServerIP = szValue;
	m_nDumpDestServerPort = GetPrivateProfileInt(CONFIG_TOKEN_APP_DEBUG,	L"DUMP_DEST_SERVERPORT",	21,			szFileName);

}


wstring ZConfig::GetPathString(const wchar_t* szFileName, const wchar_t* szKeyName )
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

void ZConfig::InitAccountDB(const wchar_t* szFileName)
{
	wchar_t szValue[256];

	GetPrivateProfileString(CONFIG_TOKEN_APP_ACCOUNTDB, L"SERVER",	 L"SH_DB",			szValue, 256, szFileName);	m_AccountDBConfig.strServer			= szValue;
	GetPrivateProfileString(CONFIG_TOKEN_APP_ACCOUNTDB, L"DATABASE", L"RZ_ACCOUNTDB",	szValue, 256, szFileName);	m_AccountDBConfig.strDatabaseName	= szValue;
	GetPrivateProfileString(CONFIG_TOKEN_APP_ACCOUNTDB, L"USERNAME", L"dev",			szValue, 256, szFileName);	m_AccountDBConfig.strUserName		= szValue;
	GetPrivateProfileString(CONFIG_TOKEN_APP_ACCOUNTDB, L"PASSWORD", L"dev",			szValue, 256, szFileName);	m_AccountDBConfig.strPassword		= szValue;
}

void ZConfig::InitGameDB(const wchar_t* szFileName)
{
	wchar_t szValue[256];

	GetPrivateProfileString(CONFIG_TOKEN_APP_DB, L"SERVER",	  L"SH_DB",		szValue, 256, szFileName);	m_GameDBConfig.strServer		= szValue;
	GetPrivateProfileString(CONFIG_TOKEN_APP_DB, L"DATABASE", L"RZ_GAMEDB", szValue, 256, szFileName);	m_GameDBConfig.strDatabaseName	= szValue;
	GetPrivateProfileString(CONFIG_TOKEN_APP_DB, L"USERNAME", L"dev",		szValue, 256, szFileName);	m_GameDBConfig.strUserName		= szValue;
	GetPrivateProfileString(CONFIG_TOKEN_APP_DB, L"PASSWORD", L"dev",		szValue, 256, szFileName);	m_GameDBConfig.strPassword		= szValue;
}

void ZConfig::InitLogDB(const wchar_t* szFileName)
{
	wchar_t szValue[256];

	GetPrivateProfileString(CONFIG_TOKEN_APP_LOGDB, L"SERVER",	 L"SH_DB",		szValue, 256, szFileName);	m_LogDBConfig.strServer			= szValue;
	GetPrivateProfileString(CONFIG_TOKEN_APP_LOGDB, L"DATABASE", L"RZ_LOGDB",	szValue, 256, szFileName);	m_LogDBConfig.strDatabaseName	= szValue;
	GetPrivateProfileString(CONFIG_TOKEN_APP_LOGDB, L"USERNAME", L"dev",		szValue, 256, szFileName);	m_LogDBConfig.strUserName		= szValue;
	GetPrivateProfileString(CONFIG_TOKEN_APP_LOGDB, L"PASSWORD", L"dev",		szValue, 256, szFileName);	m_LogDBConfig.strPassword		= szValue;
}
