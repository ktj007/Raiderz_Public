#include "stdafx.h"
#include "PConfig.h"
#include "PDef.h"
#include "CSNet.h"
#include "MFileSystem.h"
#include "STypes.h"

bool	PConfig::m_bWindowVisible = true;
int		PConfig::m_nMyNetworkCardID = 0;
int		PConfig::m_nPort = DEFAULT_APPLICATION_SERVER_PORT;

wstring	PConfig::m_strServerName = L"AppServer";
uint8	PConfig::m_nServerType = 0;
int		PConfig::m_nMyServerID = 0;
int		PConfig::m_nMyWorldID = 0;
int		PConfig::m_SSN = 0;
bool	PConfig::m_bPmangDBEncrypt = false;
wstring PConfig::m_strOdbcDriver = L"SQL Server";
int		PConfig::m_nSocketPoolSize = 2000;
int		PConfig::m_nSendPendingLimitCount = 200000;		// 20만개. 서버간에는 너그럽게 처리합니다.

int		PConfig::m_nMasterServer_NetworkCardID = 0;
wstring	PConfig::m_strMasterServerIP = L"localhost";
int		PConfig::m_nMasterServerPort = DEFAULT_MASTER_SERVER_PORT;

PDBConfig	PConfig::m_AccountDBConfig;
PDBConfig	PConfig::m_GameDBConfig;
PDBConfig	PConfig::m_LogDBConfig;

wstring	PConfig::m_strSystemPath;

// PMS
bool PConfig::m_bPMSEnable = false;

wstring PConfig::m_strDumpDestServerIP = L"localhost";
int PConfig::m_nDumpDestServerPort = m_nDumpDestServerPort;

#define CONFIG_TOKEN_APP_CONFIG				L"CONFIG"
#define CONFIG_TOKEN_APP_INFO				L"SERVER_INFO"
#define CONFIG_TOKEN_APP_NET				L"MASTER_SERVER"
#define CONFIG_TOKEN_APP_ACCOUNTDB			L"ACCOUNTDB"
#define CONFIG_TOKEN_APP_DB					L"DB"
#define CONFIG_TOKEN_APP_LOGDB				L"LOGDB"
#define CONFIG_TOKEN_APP_PATH				L"PATH"
#define CONFIG_TOKEN_APP_DEBUG				L"DEBUG"

void PConfig::Init_INI()
{
	const wchar_t* szFileName = FILENAME_CONFIG_REL;
	if (MIsExistFile(FILENAME_CONFIG)) szFileName = FILENAME_CONFIG;

	SConfig::Init_INI_Common(szFileName);
	SConfig::InitNetworkCard(szFileName);

	wchar_t szValue[256] = L"";

	// config
	m_bWindowVisible = GetPrivateProfileBool(CONFIG_TOKEN_APP_CONFIG,	L"WINDOW_VISIBLE",	true,						szFileName);
	m_nPort = GetPrivateProfileInt(CONFIG_TOKEN_APP_CONFIG,				L"PORT",				DEFAULT_MASTER_SERVER_PORT,	szFileName);
	
	// server info
	GetPrivateProfileString(CONFIG_TOKEN_APP_INFO, L"NAME",	L"AppServer", szValue, 256, szFileName); m_strServerName = szValue;
	m_nServerType = SERVER_APPLICATION;
	m_nMyServerID = GetPrivateProfileInt(CONFIG_TOKEN_APP_INFO,	L"SERVER_ID",	0,	szFileName);
	m_nMyWorldID = GetPrivateProfileInt(CONFIG_TOKEN_APP_INFO,	L"WORLD_ID",		0,	szFileName);
	m_SSN			= GetPrivateProfileInt(CONFIG_TOKEN_APP_INFO,	L"PMANG_SSN",	0,	szFileName);
	m_bPMSEnable	= GetPrivateProfileBool(CONFIG_TOKEN_APP_INFO,	L"PMS_ENABLE",	0,	szFileName);	
	m_bPmangDBEncrypt	= GetPrivateProfileBool(CONFIG_TOKEN_APP_INFO,	L"PMANG_DBENCRYPT",	0,	szFileName);	
	GetPrivateProfileString(CONFIG_TOKEN_APP_INFO, L"ODBC", L"SQL Server", szValue, 256, szFileName); m_strOdbcDriver = szValue;
	m_nSocketPoolSize = GetPrivateProfileInt(CONFIG_TOKEN_APP_INFO,	L"SOCKET_POOL_SIZE",	2000,	szFileName);
	m_nSendPendingLimitCount = GetPrivateProfileInt(CONFIG_TOKEN_APP_INFO, L"SEND_PENDING_LIMIT_COUNT", 200000, szFileName);

	// master server
	GetPrivateProfileString(CONFIG_TOKEN_APP_NET, L"IP",	L"localhost",		szValue, 256, szFileName); m_strMasterServerIP = szValue;
	m_nMasterServerPort = GetPrivateProfileInt(CONFIG_TOKEN_APP_NET, L"PORT", DEFAULT_MASTER_SERVER_PORT, szFileName);	

	// db
	InitAccountDB(szFileName);
	InitGameDB(szFileName);
	InitLogDB(szFileName);

	// path
	m_strSystemPath	= GetPathString(szFileName, L"SYSTEM");

	// debug
	GetPrivateProfileString(CONFIG_TOKEN_APP_DEBUG,	L"DUMP_DEST_SERVERIP",	L"localhost", szValue, 128, szFileName); m_strDumpDestServerIP = szValue;
	m_nDumpDestServerPort = GetPrivateProfileInt(CONFIG_TOKEN_APP_DEBUG,	L"DUMP_DEST_SERVERPORT",	21,			szFileName);
}


wstring PConfig::GetPathString( const wchar_t* szFileName, const wchar_t* szKeyName )
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

void PConfig::InitAccountDB(const wchar_t* szFileName)
{
	wchar_t szValue[256];

	GetPrivateProfileString(CONFIG_TOKEN_APP_ACCOUNTDB, L"SERVER",	 L"SH_DB",			szValue, 256, szFileName);	m_AccountDBConfig.strServer			= szValue;
	GetPrivateProfileString(CONFIG_TOKEN_APP_ACCOUNTDB, L"DATABASE", L"RZ_ACCOUNTDB",	szValue, 256, szFileName);	m_AccountDBConfig.strDatabaseName	= szValue;
	GetPrivateProfileString(CONFIG_TOKEN_APP_ACCOUNTDB, L"USERNAME", L"dev",			szValue, 256, szFileName);	m_AccountDBConfig.strUserName		= szValue;
	GetPrivateProfileString(CONFIG_TOKEN_APP_ACCOUNTDB, L"PASSWORD", L"dev",			szValue, 256, szFileName);	m_AccountDBConfig.strPassword		= szValue;
}

void PConfig::InitGameDB(const wchar_t* szFileName)
{
	wchar_t szValue[256];

	GetPrivateProfileString(CONFIG_TOKEN_APP_DB, L"SERVER",	  L"SH_DB",		szValue, 256, szFileName);	m_GameDBConfig.strServer		= szValue;
	GetPrivateProfileString(CONFIG_TOKEN_APP_DB, L"DATABASE", L"RZ_GAMEDB",	szValue, 256, szFileName);	m_GameDBConfig.strDatabaseName	= szValue;
	GetPrivateProfileString(CONFIG_TOKEN_APP_DB, L"USERNAME", L"dev",		szValue, 256, szFileName);	m_GameDBConfig.strUserName		= szValue;
	GetPrivateProfileString(CONFIG_TOKEN_APP_DB, L"PASSWORD", L"dev",		szValue, 256, szFileName);	m_GameDBConfig.strPassword		= szValue;
}

void PConfig::InitLogDB(const wchar_t* szFileName)
{
	wchar_t szValue[256];

	GetPrivateProfileString(CONFIG_TOKEN_APP_LOGDB, L"SERVER",	 L"SH_DB",		szValue, 256, szFileName);	m_LogDBConfig.strServer			= szValue;
	GetPrivateProfileString(CONFIG_TOKEN_APP_LOGDB, L"DATABASE", L"RZ_LOGDB",	szValue, 256, szFileName);	m_LogDBConfig.strDatabaseName	= szValue;
	GetPrivateProfileString(CONFIG_TOKEN_APP_LOGDB, L"USERNAME", L"dev",		szValue, 256, szFileName);	m_LogDBConfig.strUserName		= szValue;
	GetPrivateProfileString(CONFIG_TOKEN_APP_LOGDB, L"PASSWORD", L"dev",		szValue, 256, szFileName);	m_LogDBConfig.strPassword		= szValue;
}
