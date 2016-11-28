#pragma once

#include "MTypes.h"
#include "string"
#include "SConfig.h"

#define CONFIG_AUTO_TEST_AI	L"ai"



struct ZDBConfig
{
	std::wstring	strServer;
	std::wstring	strDatabaseName;
	std::wstring	strUserName;
	std::wstring	strPassword;
};

/// 인게임 설정값
class ZConfig : public SConfig
{
private:
	static wstring GetPathString(const wchar_t* szFileName, const wchar_t* szKeyName);

	static void InitAccountDB(const wchar_t* szFileName);
	static void InitGameDB(const wchar_t* szFileName);
	static void InitLogDB(const wchar_t* szFileName);
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

	// database
	static ZDBConfig	m_AccountDBConfig;
	static ZDBConfig	m_GameDBConfig;
	static ZDBConfig	m_LogDBConfig;

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
