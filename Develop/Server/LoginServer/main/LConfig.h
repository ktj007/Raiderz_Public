#pragma once

#include "MTypes.h"
#include <string>
#include "SConfig.h"

#define CONFIG_AUTO_TEST_AI	"ai"


struct LDBConfig
{
	std::wstring strServer;
	std::wstring strDBName;
	std::wstring strUserName;
	std::wstring strPassword;
};


/// 인게임 설정값
class LConfig : public SConfig
{
private:
	static wstring GetPathString(const wchar_t* szFileName, const wchar_t* szKeyName);

	static void InitAccountDB(const wchar_t* szFileName);
	static void InitGameDB(const wchar_t* szFileName);
	static void InitLogDB(const wchar_t* szFileName);


public:
	static void Init_INI();

	// db
	static LDBConfig		m_AccountDBConfig;
	static LDBConfig		m_GameDBConfig;
	static LDBConfig		m_LogDBConfig;

	// config
	static int				m_nMyNetworkCardID;				///< Listen Socket에서 사용할 네트워크 카드 ID
	static int				m_nPort;						///< 접속 포트
	static int				m_nUDPPort;						///< UDP 포트
	static bool				m_bStandAlone;					///< 마스터서버 없이 실행할지 여부

	// path
	static std::wstring		m_strSystemPath;				///< 시스템 폴더
	static std::wstring		m_strFieldPath;					///< Field 폴더
	static std::wstring		m_strZonePath;					///< Zone 폴더

	// server info
	static std::wstring		m_strServerName;				///< 서버 이름	
	static uint8			m_nServerType;
	static int				m_nServerID;					///< 서버 ID
	static int				m_nWorldID;						///< 월드 ID
	static int				m_SSN;							///< 피망 서비스 ID
	static wstring			m_strOdbcDriver;				///< ODBC
	static int				m_nSocketPoolSize;				///< 소켓 풀 사이즈
	static int				m_nSendPendingLimitCount;		///< 센드 커멘드 팬딩 제한

	// master server
	static int				m_nMasterServer_NetworkCardID;	///< 마스터 서버 통신에 사용할 네트워크 카드 ID
	static wstring			m_strMasterServerIP;			///< 마스터 서버 IP
	static int				m_nMasterServerPort;			///< 마스터 서버 Port	

	// const
	static bool				m_bWindowVisible;				///< 서버창을 보일지 여부
	static int				m_nMoveServerTimeout;			///< 서버이동 접속종료 타임아웃(ms)
	static int				m_nCommandTimeout;				///< 커맨드 응답 타임아웃
	static bool				m_isAllowInsertNewAccount;		///< 새로운 계정일 경우 등록허용

	// Pmang
	static wstring			m_strPmangServerKey;				///< 피망 서버키
	static uint32			m_nPmangToleranceSeconds;		///< 피망 로그인 시간 허용오차

	// PMS
	static bool				m_bPMSEnable;

	// pmang db encrypt.
	static bool				m_bPmangDBEncrypt;

	// etc
	static wstring			m_strDumpDestServerIP;
	static int				m_nDumpDestServerPort;
};
