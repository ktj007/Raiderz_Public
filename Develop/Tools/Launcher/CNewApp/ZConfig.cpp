#include "stdafx.h"
#include "ZConfig.h"


bool	ZConfig::m_bHideWebPage = true;
string	ZConfig::m_strWebPageURL = "";

bool	ZConfig::m_bStaticLaunch = true;
string	ZConfig::m_strStaticServerName = "SoulHunterZ Internal Pre-Test";
string	ZConfig::m_strStaticIPAddress = "198.50.217.200";
bool	ZConfig::m_bSkipSelectWorldForPWE = true;

bool	ZConfig::m_bSkipUpdate = true;
string	ZConfig::m_strPatchServAddr = "";
string	ZConfig::m_strPatchServDir = "";

#define CONFIG_APP_NAME_CONFIG			"CONFIG"
#define CONFIG_APP_NAME_STATIC_LAUNCH	"STATIC_LAUNCH"
#define CONFIG_APP_NAME_PATCH			"PATCH"

bool ZConfig::Init()
{
	string strConfigFileName = "./Launcher.ini";

	/*char szTemp[512] = "";

	/// CONFIG
	m_bHideWebPage = (0 != GetPrivateProfileInt(CONFIG_APP_NAME_CONFIG, "HIDE_WEBPAGE", 0, strConfigFileName.c_str()));
	GetPrivateProfileString(CONFIG_APP_NAME_CONFIG, "WEBPAGE_URL", "",	szTemp, MAX_PATH, strConfigFileName.c_str());	m_strWebPageURL = szTemp;

	/// LAUNCH
	m_bStaticLaunch = (0 != GetPrivateProfileInt(CONFIG_APP_NAME_STATIC_LAUNCH, "ENABLE", 0, strConfigFileName.c_str()));
	GetPrivateProfileString(CONFIG_APP_NAME_STATIC_LAUNCH, "SERVER_NAME", "",	szTemp, MAX_PATH, strConfigFileName.c_str());	m_strStaticServerName = szTemp;
	GetPrivateProfileString(CONFIG_APP_NAME_STATIC_LAUNCH, "IP_ADDRESS", "",	szTemp, MAX_PATH, strConfigFileName.c_str());	m_strStaticIPAddress = szTemp;
	m_bSkipSelectWorldForPWE = (0 != GetPrivateProfileInt(CONFIG_APP_NAME_STATIC_LAUNCH, "SKIP_SELECT_WORLD_FOR_PWE", 0, strConfigFileName.c_str()));

	/// PATCH
	m_bSkipUpdate = (0 != GetPrivateProfileInt(CONFIG_APP_NAME_PATCH, "SKIP", 0, strConfigFileName.c_str()));

	GetPrivateProfileString(CONFIG_APP_NAME_PATCH, "ADDRESS", "",	szTemp, MAX_PATH, strConfigFileName.c_str());	m_strPatchServAddr = szTemp;
	GetPrivateProfileString(CONFIG_APP_NAME_PATCH, "DIR", "",		szTemp, MAX_PATH, strConfigFileName.c_str());	m_strPatchServDir = szTemp;*/

	return true;
}
