#include "stdafx.h"
#include "LConfig.h"
#include "LDef.h"
#include "CSNet.h"
#include "MFileSystem.h"

char LConfig::m_szDB_DNS[128] = "";
char LConfig::m_szDB_UserName[128] = "";
char LConfig::m_szDB_Password[128] = "";
bool LConfig::m_bWindowVisible = true;
int LConfig::m_nPort = DEFAULT_MASTER_SERVER_PORT;

std::string LConfig::m_strSystemPath;
std::string	LConfig::m_strFieldPath;
std::string	LConfig::m_strZonePath;

#define CONFIG_TOKEN_APP_DB					"DB"
#define CONFIG_TOKEN_APP_CONFIG				"CONFIG"
#define CONFIG_TOKEN_APP_DEBUG				"DEBUG"
#define CONFIG_TOKEN_APP_PATH				"PATH"

void LConfig::Init_INI()
{
	const char* szFileName = FILENAME_CONFIG_REL;
	if (MIsExistFile(FILENAME_CONFIG)) szFileName = FILENAME_CONFIG;

	// server info


	// config
	m_bWindowVisible = GetPrivateProfileBool(CONFIG_TOKEN_APP_CONFIG,	"WINDOW_VISIBLE",	true,						szFileName);
	m_nPort = GetPrivateProfileInt(CONFIG_TOKEN_APP_CONFIG,				"PORT",				DEFAULT_MASTER_SERVER_PORT,	szFileName);

	// db
	GetPrivateProfileString(CONFIG_TOKEN_APP_DB, "DNS",		"SH_DB", m_szDB_DNS, 64, szFileName);
	GetPrivateProfileString(CONFIG_TOKEN_APP_DB, "USERNAME", "dev", m_szDB_UserName, 64, szFileName);
	GetPrivateProfileString(CONFIG_TOKEN_APP_DB, "PASSWORD", "dev", m_szDB_Password, 64, szFileName);

	char szValue[512] = "";

	// path
	m_strSystemPath = GetPathString(szFileName, "SYSTEM");
	m_strFieldPath = GetPathString(szFileName, "FIELD");
	m_strZonePath = GetPathString(szFileName, "ZONE");
}


bool LConfig::GetPrivateProfileBool(const char* szAppName, const char* szKeyName, 
						   bool bDefault, const char* szFileName)
{
	int nDefault = bDefault ? 1 : 0;

	int n;
	n = GetPrivateProfileInt(szAppName, szKeyName, nDefault, szFileName);
	if (n == 0) return false;
	return true;
}

string LConfig::GetPathString( const char* szFileName, const char* szKeyName )
{
	string strRet;
	char szValue[512] = "";

	GetPrivateProfileString(CONFIG_TOKEN_APP_PATH, szKeyName,	"./", szValue, 512, szFileName);

	strRet = szValue;

	if (strRet.size() > 0)
	{
		if (strRet[strRet.size()-1] != '/' &&
			strRet[strRet.size()-1] != '\\')
		{
			strRet = strRet + '/';
		}
	}

	return strRet;
}
