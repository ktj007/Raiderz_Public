#include "stdafx.h"
#include "GConfig.h"
#include "GDef.h"
#include "CSNet.h"
#include "MFileSystem.h"

char GConfig::m_szDB_DNS[128] = "";
char GConfig::m_szDB_UserName[128] = "";
char GConfig::m_szDB_Password[128] = "";
int GConfig::m_nMaxUser = DEFAULT_MAX_PLAYER;
bool GConfig::m_bWindowVisible = true;
int GConfig::m_nPort = DEFAULT_GAME_SERVER_PORT;

#define CONFIG_TOKEN_APP_DB					"DB"
#define CONFIG_TOKEN_APP_CONFIG				"CONFIG"
#define CONFIG_TOKEN_APP_DEBUG				"DEBUG"

void GConfig::Init_INI()
{
	const char* szFileName = FILENAME_SERVER_CONFIG_REL;
	if (MIsExistFile(FILENAME_SERVER_CONFIG)) szFileName = FILENAME_SERVER_CONFIG;

	// server info


	// config
	m_bWindowVisible = GetPrivateProfileBool(CONFIG_TOKEN_APP_CONFIG,	"WINDOW_VISIBLE",	true,						szFileName);
	m_nPort = GetPrivateProfileInt(CONFIG_TOKEN_APP_CONFIG,				"PORT",				DEFAULT_GAME_SERVER_PORT,	szFileName);

	// db
	GetPrivateProfileString(CONFIG_TOKEN_APP_DB, "DNS",		"SH_DB", m_szDB_DNS, 64, szFileName);
	GetPrivateProfileString(CONFIG_TOKEN_APP_DB, "USERNAME", "dev", m_szDB_UserName, 64, szFileName);
	GetPrivateProfileString(CONFIG_TOKEN_APP_DB, "PASSWORD", "dev", m_szDB_Password, 64, szFileName);
}


bool GConfig::GetPrivateProfileBool(const char* szAppName, const char* szKeyName, 
						   bool bDefault, const char* szFileName)
{
	int nDefault = bDefault ? 1 : 0;

	int n;
	n = GetPrivateProfileInt(szAppName, szKeyName, nDefault, szFileName);
	if (n == 0) return false;
	return true;
}
