#include "StdAfx.h"
#include "TQuestConfig.h"
#include "TDef.h"

char TQuestConfig::m_szDB_DNS[128] = "";
char TQuestConfig::m_szDB_UserName[128] = "";
char TQuestConfig::m_szDB_Password[128] = "";

#define CONFIG_TOKEN_APP_DB					"DB"
#define CONFIG_TOKEN_APP_CONFIG				"CONFIG"

bool TQuestConfig::Init()
{
	// db
	GetPrivateProfileString(CONFIG_TOKEN_APP_DB, "DNS",		"SH_DB", m_szDB_DNS, 64, FILENAME_QUEST_CONFIG);
	GetPrivateProfileString(CONFIG_TOKEN_APP_DB, "USERNAME", "dev", m_szDB_UserName, 64, FILENAME_QUEST_CONFIG);
	GetPrivateProfileString(CONFIG_TOKEN_APP_DB, "PASSWORD", "dev", m_szDB_Password, 64, FILENAME_QUEST_CONFIG);

	return true;
}


bool TQuestConfig::GetPrivateProfileBool(String^ strAppName, String^ strKeyName, 
									bool bDefault, String^ strFileName)
{
	int nDefault = bDefault ? 1 : 0;

	int n;
	n = GetPrivateProfileInt(MStringToCharPointer(strAppName), MStringToCharPointer(strKeyName), bDefault, MStringToCharPointer(strFileName));
	if (n == 0) return false;
	return true;
}
