#ifndef _GCONFIG_H
#define _GCONFIG_H

#include "MTypes.h"

#define CONFIG_AUTO_TEST_AI	"ai"



/// 인게임 설정값
class GConfig
{
private:
	static bool GetPrivateProfileBool(const char* szAppName, const char* szKeyName, bool bDefault, const char* szFileName);
public:
	static void Init_INI();

	// server info
	static int		m_nMaxUser;					///< 최대 접속자

	// db
	static char		m_szDB_DNS[128];				///< DB DNS
	static char		m_szDB_UserName[128];		///< DB Username
	static char		m_szDB_Password[128];		///< DB Password

	// config
	static bool		m_bWindowVisible;					///< 서버창을 보일지 여부
	static int		m_nPort;							///< 접속 포트
};





#endif