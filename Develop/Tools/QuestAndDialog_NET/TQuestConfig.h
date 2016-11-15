#ifndef _TQUEST_CONFIG_
#define _TQUEST_CONFIG_

#include "MTypes.h"


/// 퀘스트 툴의 설정값
class TQuestConfig
{
private:
	static bool GetPrivateProfileBool(String^ szAppName, String^ szKeyName, bool bDefault, String^ szFileName);
public:
	static bool Init();

	// db
	static char		m_szDB_DNS[128];			///< DB DNS
	static char		m_szDB_UserName[128];		///< DB Username
	static char		m_szDB_Password[128];		///< DB Password
};

#endif //_TQUEST_CONFIG_
