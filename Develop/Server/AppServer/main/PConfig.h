#pragma once

#include "MTypes.h"
#include <string>

#include "SConfig.h"

/// 인게임 설정값
class PConfig : public SConfig
{
private:
	static wstring GetPathString( const wchar_t* szFileName, const wchar_t* szKeyName );

public:
	static void Init_INI();

	// config
	static bool		m_bWindowVisible;			///< 서버창을 보일지 여부
	static int		m_nMyNetworkCardID;			///< Listen Socket에서 사용할 네트워크 카드 ID
	static int		m_nPort;					///< 접속 포트

	// ServerInfo
	static wstring	m_strServerName;			///< 서버이름
	static uint8	m_nServerType;				///< 서버타입
	static int		m_nMyServerID;				///< 서버ID
	static int		m_nMyWorldID;				///< 월드ID
	static int		m_SSN;						///< 피망 서비스 ID
	static bool		m_bPmangDBEncrypt;
	static wstring	m_strOdbcDriver;			///< ODBC 드라이버 이름
	static int		m_nSocketPoolSize;			///< 소켓 풀 사이즈
	static int		m_nSendPendingLimitCount;		///< 센드 커멘드 팬딩 제한

	// MasterServer
	static int		m_nMasterServer_NetworkCardID;	///< 마스터 서버 통신에 사용할 네트워크 카드 ID
	static wstring	m_strMasterServerIP;		///< 접속 IP
	static int		m_nMasterServerPort;		///< 접속 포트

	// db
	static wstring	m_strGameDB_Server;				///< DB Server
	static wstring	m_strGameDB_DatabaseName;		///< DB Name
	static wstring	m_strGameDB_UserName;			///< DB Username
	static wstring	m_strGameDB_Password;			///< DB Password

	static wstring	m_strLogDB_Server;				///< DB Server
	static wstring	m_strLogDB_DatabaseName;		///< DB Name
	static wstring	m_strLogDB_UserName;			///< DB Username
	static wstring	m_strLogDB_Password;			///< DB Password

	// path
	static wstring	m_strSystemPath;			///< 시스템 폴더

	// PMS
	static bool		m_bPMSEnable;

	// etc
	static wstring			m_strDumpDestServerIP;
	static int				m_nDumpDestServerPort;


};
