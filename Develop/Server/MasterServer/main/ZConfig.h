#pragma once

#include "MTypes.h"
#include "string"
#include "SConfig.h"

#define CONFIG_AUTO_TEST_AI	L"ai"



/// 인게임 설정값
class ZConfig : public SConfig
{
private:
	static wstring GetPathString(const wchar_t* szFileName, const wchar_t* szKeyName);
public:
	static void Init_INI();

	// server info
	static int			m_nMaxUser;					///< 최대 접속자
	static std::wstring	m_strServerName;
	static uint8		m_nServerType;
	static int			m_SSN;						///< 피망 서비스 ID
	static int			m_nMyWorldID;
	static int			m_nMyServerID;
	static bool			m_bPMSEnable;
	static bool			m_bPmangDBEncrypt;
	static wstring		m_strOdbcDriver;
	static int			m_nSocketPoolSize;			///< 소켓 풀 사이즈
	static int			m_nSendPendingLimitCount;	///< 센드 커멘드 팬딩 제한
	
	// accountdb
	static std::wstring		m_strAccountDB_Server;			///< Account DB Server
	static std::wstring		m_strAccountDB_DatabaseName;	///< Account DB Name
	static std::wstring		m_strAccountDB_UserName;		///< Account DB Username
	static std::wstring		m_strAccountDB_Password;		///< Account DB Password

	// db
	static std::wstring		m_strGameDB_Server;			///< DB Server
	static std::wstring		m_strGameDB_DatabaseName;	///< DB Name
	static std::wstring		m_strGameDB_UserName;		///< DB Username
	static std::wstring		m_strGameDB_Password;		///< DB Password

	// logdb
	static std::wstring		m_strLogDB_Server;			///< DB Server
	static std::wstring		m_strLogDB_DatabaseName;	///< DB Name
	static std::wstring		m_strLogDB_UserName;		///< DB Username
	static std::wstring		m_strLogDB_Password;		///< DB Password

	// config
	static bool		m_bWindowVisible;				///< 서버창을 보일지 여부
	static int		m_nPort;						///< 접속 포트

	// path
	static std::wstring	m_strSystemPath;			///< 시스템 폴더
	static std::wstring	m_strFieldPath;				///< Field 폴더
	
	// etc
	static wstring			m_strDumpDestServerIP;
	static int				m_nDumpDestServerPort;
};
