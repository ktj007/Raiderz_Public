#pragma once

#include "MTypes.h"
#include <string>

#define CONFIG_AUTO_TEST_AI	"ai"



/// 인게임 설정값
class LConfig
{
private:
	static bool GetPrivateProfileBool(const char* szAppName, const char* szKeyName, bool bDefault, const char* szFileName);
	static string GetPathString(const char* szFileName, const char* szKeyName);
public:
	static void Init_INI();

	// db
	static char		m_szDB_DNS[128];				///< DB DNS
	static char		m_szDB_UserName[128];		///< DB Username
	static char		m_szDB_Password[128];		///< DB Password

	// config
	static bool		m_bWindowVisible;					///< 서버창을 보일지 여부
	static int		m_nPort;							///< 접속 포트

	// path
	static std::string	m_strSystemPath;			///< 시스템 폴더
	static std::string	m_strFieldPath;				///< Field 폴더
	static std::string	m_strZonePath;				///< Zone 폴더
	
};

