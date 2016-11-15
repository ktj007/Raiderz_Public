#ifndef _GCONFIG_H
#define _GCONFIG_H

#include "GFrameworkLib.h"
#include "MTypes.h"
#include "SConfig.h"
#include "GConfig_Arg.h"
#include "GConfig_Lua.h"

#define CONFIG_AUTO_TEST_AI			L"ai"
#define CONFIG_AUTO_TEST_QPVPEVENT	L"qpvpevent"
#define CONFIG_AUTO_TEST_AI_PROFILE	L"ai_profile"


/// 인게임 설정값
class GFRAMEWORK_API GConfig : public SConfig
{
private:
	static void InitLogDB(const wchar_t* szFileName);
public:
	static void Init_INI();
	static void Init_Arg(const GConfig_Arg& arg);
	static void Init_Lua(const GConfig_Lua& lua);

	static bool IsStandAloneMode(void)		{ return m_bStandAlone || m_bAutoTestActive;	}

	// server info
	static int			m_nMaxUser;					///< 최대 접속자
	static std::wstring	m_strServerName;
	static uint8		m_nServerType;
	static int			m_SSN;						///< 피망 서비스 ID
	static int			m_nMyWorldID;				///< 월드 ID
	static int			m_nMyServerID;				///< 서버 ID
	static wstring		m_strOdbcDriver;			///< ODBC
	static wstring		m_strPublicIP;				///< 외부 접속 IP
	static int			m_nSocketPoolSize;			///< 소켓 풀 사이즈
	static int			m_nSendPendingLimitCount;	///< 센드 커멘드 팬딩 제한

	// db
	static std::wstring		m_strGameDB_Server;			///< DB Server
	static std::wstring		m_strGameDB_DatabaseName;	///< DB Name
	static std::wstring		m_strGameDB_UserName;		///< DB Username
	static std::wstring		m_strGameDB_Password;		///< DB Password

	// LogDB
	static std::wstring		m_strLogDB_Server;			///< LogDB Server.
	static std::wstring		m_strLogDB_DatabaseName;	///< LogDB Name.
	static std::wstring		m_strLogDB_UserName;		///< LogDB Username.
	static std::wstring		m_strLogDB_Password;		///< LogDB Password.

	// config
	static bool		m_bWindowVisible;				///< 서버창을 보일지 여부
	static int		m_nMyNetworkCardID;				///< Listen Socket에서 사용할 네트워크 카드 ID
	static int		m_nPort;						///< 접속 포트
	static bool		m_bStandAlone;					///< 마스터 서버없이 실행할지 여부

	// heart beat
	static bool		m_bEnableHeartBeat;				///< HeartBeat 체크를 할 것인지 여부
	static int		m_nHeartBeatTickTime;			///< HeartBeat 체크 주기
	static int		m_nHeartBeatTimeout;			///< HeartBeat Timeout


	// network
	static int		m_nPrivateNetworkCardID;	///< 마스터 서버 통신에 사용할 네트워크 카드 ID
	static wstring	m_strMasterServerIP;
	static int		m_nMasterServerPort;

	// debug
	static wchar_t	m_szTestSuite[4][128];			///< 테스트할 Suite, 아무것도 없으면 모두 테스트한다.
	static DWORD	m_dwPacketDelayTime;			///< 패킷을 보내고 받는데 대기되는 시간 (테스트용)
	static bool		m_bDBTaskCount;
	static bool		m_bNoDB;
	static bool		m_bLogPoolCount;				///< 메모리 풀 갯수 로그로 남길지 여부

	static bool				m_bExportUpdateDB;
	static bool				m_bExportWriteFile;
	static wstring			m_strExportLocale;

	// autotest
	static bool				m_bAutoTestActive;
	static wstring			m_strAutoTestType;
	static GAITestType		m_nAITestType;
	static GCommandRecordingType m_nCommandRecordingType;
	static int				m_nRepeat;
	static DWORD			m_dwTime;	
	static int				m_nSpeed;
	static float			m_fLimitFrame;
	static int				m_nNPCCount;
	static int				m_nFieldID;
	static int				m_nQPVPEventID;

	// 게임서버 프로파일러
	static wstring			m_strAutoTestFieldlistFileName;

	// 서버 실행시간
	static bool				m_bLimitRunTime;
	static DWORD			m_dwRunTime;

	// PMS
	static bool				m_bPMSEnable;

	// GameGuard (참고 문서: CS_Auth26.pdf)
	static bool				m_bGGEnable;		
	static bool				m_bGGShowDebugLog; // (InitGameGuardAuth 사용)
	static int				m_nGGInitNumActive; // CS인증 모듈의 새버젼 최소 접속자수 (InitGameGuardAuth 사용)
	static int				m_nGGTimeLimit;		// 업데이트 조건 중 통계데이터를 수집하는 시간 (SetUpdateCondition 사용)
	static int				m_nGGCondition;		// 업데이트 조건 중 버젼 통데이터 중 선택할 조건 지정 (SetUpdateCondition 사용)
	static int				m_nGGAuthInterval;	// 인증 주기시간 (단위: 초) 


	// pmang db encrypt.
	static bool				m_bPmangDBEncrypt;

	// etc
	static bool				m_bFullMemoryCrashDump;
	static bool				m_bDumpEnable;
	static wstring			m_strDumpDestServerIP;
	static int				m_nDumpDestServerPort;

	// command profiler
	static tstring	m_strCommandProfiler_LogName_ForMasterServer;	// 커맨드 프로파일러 로그 파일 이름 - 마스터서버
	static tstring	m_strCommandProfiler_LogName_ForAppServer;		// 커맨드 프로파일러 로그 파일 이름 - 앱서버
};





#endif