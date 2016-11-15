
#include "stdafx.h"
#include "XAntiHackModuleGameGuardKor.h"
#include "XAppErrorCollector.h"


// Include libraries
#if defined( XANTIHACK)  &&  defined( _KOREA)

#pragma comment( lib, "ws2_32.lib")
#pragma comment( lib, "winmm.lib")
#pragma comment( lib, "../../Extern/nProtect/Korea/Client/NPGameLib_78_MT.lib")

#endif






// XAntiHackAuthInfoGameGuardKor
XAntiHackAuthInfoGameGuardKor::XAntiHackAuthInfoGameGuardKor()
{
}


// XAntiHackAuthInfoGameGuardKor
XAntiHackAuthInfoGameGuardKor::XAntiHackAuthInfoGameGuardKor( void* pAuthInfo)
{
#if defined( XANTIHACK)  &&  defined( _KOREA)

	if ( pAuthInfo != NULL)		memcpy( &m_AuthInfo, pAuthInfo, sizeof( GG_AUTH_DATA));

#endif
}


// ~XAntiHackAuthInfoGameGuardKor
XAntiHackAuthInfoGameGuardKor::~XAntiHackAuthInfoGameGuardKor()
{
}








#if defined( XANTIHACK)  &&  defined( _KOREA)

// Global variables
static XAntiHackModuleGameGuardKor* g_pGameGuardKorModule = NULL;


// Callback
BOOL CALLBACK NPGameMonCallback( DWORD dwMsg, DWORD dwArg)
{
	switch ( dwMsg)
	{
	case NPGAMEMON_COMM_ERROR :
	case NPGAMEMON_COMM_CLOSE :
	case NPGAMEMON_INIT_ERROR :
	case NPGAMEMON_SPEEDHACK :
	case NPGAMEMON_GAMEHACK_KILLED :
	case NPGAMEMON_GAMEHACK_DETECT :
	case NPGAMEMON_GAMEHACK_DOUBT :
		if ( g_pGameGuardKorModule != NULL)
		{
			g_pGameGuardKorModule->SetError( dwMsg);
			return FALSE;
		}

	case NPGAMEMON_CHECK_CSAUTH2 :
		if ( g_pGameGuardKorModule != NULL)
		{
			XAntiHackAuthInfoGameGuardKor _auth_info;
			memcpy( &_auth_info.m_AuthInfo, (PVOID)dwArg, sizeof( GG_AUTH_DATA));
			XGetAntiHack()->CheckedAuthority( &_auth_info);
		}
		return TRUE;

	case NPGAMEMON_GAMEHACK_REPORT :
		if ( g_pGameGuardKorModule != NULL)
		{
			// to do...

			return TRUE;
		}
	}

	return TRUE;
}

#endif




// XAntiHackModuleGameGuardKor
XAntiHackModuleGameGuardKor::XAntiHackModuleGameGuardKor()
{
	m_bCreated = false;
	m_dwErrorCode = NULL;
	m_dwTimer = 0;

#if defined( XANTIHACK)  &&  defined( _KOREA)

	g_pGameGuardKorModule = this;

#endif
}


// ~XAntiHackModuleGameGuardKor
XAntiHackModuleGameGuardKor::~XAntiHackModuleGameGuardKor()
{
#if defined( XANTIHACK)  &&  defined( _KOREA)

	if ( m_bCreated == true)		CloseNPGameMon();

	g_pGameGuardKorModule = NULL;

#endif
}


// GetLastErrorCode
DWORD XAntiHackModuleGameGuardKor::GetLastErrorCode()
{
	return m_dwErrorCode;
}


// GetLastErrorMsg
const wchar_t* XAntiHackModuleGameGuardKor::GetLastErrorMsg()
{
	return m_strErrorMsg.c_str();
}


// SetError
void XAntiHackModuleGameGuardKor::SetError( DWORD dwErrorCode)
{
	m_dwErrorCode = dwErrorCode;

#if defined( XANTIHACK)  &&  defined( _KOREA)

	switch ( dwErrorCode)
	{
		// Callback Message
	case NPGAMEMON_UNDEFINED :				m_strErrorMsg = L"Undefined message";						break;
	case NPGAMEMON_COMM_ERROR :				m_strErrorMsg = L"Communication error";						break;
	case NPGAMEMON_COMM_CLOSE :				m_strErrorMsg = L"Communication closing";					break;
	case NPGAMEMON_SPEEDHACK :				m_strErrorMsg = L"스피드핵이 감지되었습니다.";				break;
	case NPGAMEMON_GAMEHACK_KILLED :		m_strErrorMsg = L"게임핵이 발견되었습니다.";				break;
	case NPGAMEMON_GAMEHACK_DETECT :		m_strErrorMsg = L"게임핵이 발견되었습니다.";				break;
	case NPGAMEMON_INIT_ERROR :				m_strErrorMsg = L"게임가드 초기화 에러";					break;
	case NPGAMEMON_GAMEHACK_DOUBT :			m_strErrorMsg = L"게임이나 게임가드가 변조되었습니다.";		break;
	case NPGAMEMON_CHECK_CSAUTH :			m_strErrorMsg = L"CSAuth";									break;
	case NPGAMEMON_CHECK_CSAUTH2 :			m_strErrorMsg = L"CSAuth2";									break;
	case NPGAMEMON_GAMEHACK_REPORT :		m_strErrorMsg = L"GameHack report";							break;

		// Error Code  110 - 300
	case NPGAMEMON_ERROR_EXIST :			m_strErrorMsg = L"게임가드가 실행 중 입니다. 잠시 후나 재부팅 후에 다시 실행해보시기 바랍니다.";												break;
	case NPGAMEMON_ERROR_CREATE :			m_strErrorMsg = L"GameGuard Directory Create Error";																							break;
	case NPGAMEMON_ERROR_NPSCAN :			m_strErrorMsg = L"바이러스 및 해킹툴 검사 모듈 로딩에 실패 했습니다. 메모리 부족이거나 바이러스에 의한 감염일 수 있습니다.";					break;
	case NPGAMEMON_ERROR_THREAD :			m_strErrorMsg = L"CreateThread Error";																											break;
	case NPGAMEMON_ERROR_INIT :				m_strErrorMsg = L"게임가드 초기화 에러입니다. 재부팅 후 다시 실행해보거나 충돌할 수 있는 다른 프로그램들을 종료한 후 실행해 보시기 바랍니다.";	break;
	case NPGAMEMON_ERROR_GAME_EXIST :		m_strErrorMsg = L"게임이 중복 실행되었거나 게임가드가 이미 실행 중 입니다. 게임 종료 후 다시 실행해보시기 바랍니다.";							break;
	case NPGAMEMON_ERROR_AUTH_INI :			m_strErrorMsg = L"게임가드 파일이 없거나 변조되었습니다. 게임가드 셋업 파일을 설치해보시기 바랍니다.";											break;
	case NPGAMEMON_ERROR_AUTH_NPGMUP :		m_strErrorMsg = L"npgmup.des Authentication Fail";																								break;
	case NPGAMEMON_ERROR_AUTH_GAMEMON :		m_strErrorMsg = L"GameMon.des Authentication Fail";																								break;
	case NPGAMEMON_ERROR_AUTH_NEWUP :		m_strErrorMsg = L"npgmup.des.new Auth Fail";																									break;
	case NPGAMEMON_ERROR_AUTH_GAMEGUARD :	m_strErrorMsg = L"게임가드 파일이 없거나 변조되었습니다. 게임가드 셋업 파일을 설치해보시기 바랍니다.";											break;
	case NPGAMEMON_ERROR_AUTH_DLL :			m_strErrorMsg = L"NPGameDLL.dll Authentication Fail";																							break;
	case NPGAMEMON_ERROR_DECRYPT :			m_strErrorMsg = L".ini File Decryption Fail";																									break;
	case NPGAMEMON_ERROR_CORRUPT_INI :		m_strErrorMsg = L"Corrupt ini file Error";																										break;
	case NPGAMEMON_ERROR_CORRUPT_INI2 :		m_strErrorMsg = L"Not match GameName in ini file Error";																						break;
	case NPGAMEMON_ERROR_NFOUND_INI :		m_strErrorMsg = L"게임가드 파일이 없거나 변조되었습니다. 게임가드 셋업 파일을 설치해보시기 바랍니다.";											break;
	case NPGAMEMON_ERROR_NFOUND_NPGMUP :	m_strErrorMsg = L"npgmup.des not found";																										break;
	case NPGAMEMON_ERROR_NFOUND_NEWUP :		m_strErrorMsg = L"npgmup.des.new not found";																									break;
	case NPGAMEMON_ERROR_NFOUND_GG :		m_strErrorMsg = L"게임가드 파일이 없거나 변조되었습니다. 게임가드 셋업 파일을 설치해보시기 바랍니다.";											break;
	case NPGAMEMON_ERROR_NFOUND_GM :		m_strErrorMsg = L"GameMon.des not found";																										break;
	case NPGAMEMON_ERROR_CRYPTOAPI :		m_strErrorMsg = L"윈도우의 일부 시스템 파일이 손상되었습니다. 인터넷 익스플로러(IE)를 다시 설치해보시기 바랍니다.";								break;
	case NPGAMEMON_ERROR_COMM :				m_strErrorMsg = L"Communication Init Error";																									break;
	case NPGAMEMON_ERROR_EXECUTE :			m_strErrorMsg = L"게임가드 실행에 실패했습니다. 게임가드 셋업 파일을 다시 설치해보시기 바랍니다.";												break;
	case NPGAMEMON_ERROR_EVENT :			m_strErrorMsg = L"GameMon Event Create Error";																									break;
	case NPGAMEMON_ERROR_NPGMUP :			m_strErrorMsg = L"npgmup.dll Error";																											break;
	case NPGAMEMON_ERROR_MOVE_INI :			m_strErrorMsg = L"Move ini Error";																												break;
	case NPGAMEMON_ERROR_MOVE_NEWUP :		m_strErrorMsg = L"Move npgmup.des.new Error";																									break;
	case NPGAMEMON_ERROR_ILLEGAL_PRG :		m_strErrorMsg = L"불법 프로그램이 발견되었습니다. 불필요한 프로그램을 종료한 후 다시 실행해보시기 바랍니다.";									break;
	case NPGAMEMON_ERROR_GAMEMON :			m_strErrorMsg = L"GameMon Init Error";																											break;
	case NPGAMEMON_ERROR_SPEEDCHECK :		m_strErrorMsg = L"SpeedCheck Init Error";																										break;
	case NPGAMEMON_ERROR_GAMEGUARD :		m_strErrorMsg = L"게임가드 초기화 에러 또는 구버젼의 게임가드 파일입니다. 게임가드 셋업파일을 다시 설치하고 게임을 실행해봅니다.";				break;

		// Update Error Code
	case NPGMUP_ERROR_PARAM :				m_strErrorMsg = L"ini 파일이 없거나 변조되었습니다. 게임가드 셋업 파일을 설치하면 해결할 수 있습니다.";											break;
	case NPGMUP_ERROR_INIT :				m_strErrorMsg = L"npgmup.des 초기화 에러입니다. 게임가드폴더를 삭제후 다시 게임실행을 해봅니다.";												break;
	case NPGMUP_ERROR_DOWNCFG :				m_strErrorMsg = L"게임가드 업데이트 서버 접속에 실패하였습니다. 잠시 후 재시도 해보거나, 개인 방화벽이 있다면 설정을 조정해 보시기 바랍니다.";	break;
	case NPGMUP_ERROR_ABORT :				m_strErrorMsg = L"게임가드 업데이트를 취소하셨습니다. 접속이 계속 되지 않을 경우 인터넷 및 개인 방화벽 설정을 조정해 보시기 바랍니다.";			break;
	case NPGMUP_ERROR_AUTH :				m_strErrorMsg = L"게임가드 업데이트를 완료하지 못 했습니다. 바이러스 백신을 일시 중시 시킨 후 재시도 해보시거나, PC 관리 프로그램을 사용하시면 설정을 조정해 보시기 바랍니다.";		break;
	case NPGMUP_ERROR_AUTH_INI :			m_strErrorMsg = L".ini Authentication Error";																									break;
	case NPGMUP_ERROR_DECRYPT :				m_strErrorMsg = L".ini Decrypt Error";																											break;
	case NPGMUP_ERROR_CONNECT :				m_strErrorMsg = L"게임가드 업데이트 서버 접속에 실패하였습니다. 잠시 후 다시 접속하거나, 네트웍 상태를 점검해봅니다.";							break;
	case NPGMUP_ERROR_INI :					m_strErrorMsg = L".ini is corrupted";																											break;

		// Collision Error Code
	case NPGG_ERROR_COLLISION :				m_strErrorMsg = L"게임가드와 충돌 프로그램이 발견되었습니다.";																					break;

		// Run hack tool
	case 620 :								m_strErrorMsg = L"해킹툴 실행이 감지되었습니다. 해당 해킹툴의 사용을 중단하시기 바랍니다.";														break;

		// Default
	default :								m_strErrorMsg = L"알 수 없는 오류가 발생하였습니다.";																							break;
	}


	wchar_t _buff[ 512];
	swprintf_s( _buff, L"보안 오류가 발생하였습니다.\n\n에러 코드=%d\n에러 메시지=%s\n\n바이러스나 해킹툴에 의해서 파일이 변조되었을 수 있습니다. 백신 소프트웨어로 검사해보시기 바랍니다.",
		m_dwErrorCode, m_strErrorMsg.c_str());
	XGetAppErrorCollector()->SetLastError( ERRORTYPE_ANTIHACK, _buff);

#endif
}


// OnCreate
bool XAntiHackModuleGameGuardKor::OnCreate()
{
	if ( m_bCreated == true)		return false;

#if defined( XANTIHACK)  &&  defined( _KOREA)

	// Create GameGuard module
	DWORD dwErrorCode = PreInitNPGameMon( L"RaiderzKR");
	if ( dwErrorCode != NPGAMEMON_SUCCESS)
	{
		// An error occurred
		SetError( dwErrorCode);
		return false;
	}


	// Initialize module
	dwErrorCode = InitNPGameMon();
	if ( dwErrorCode != NPGAMEMON_SUCCESS)
	{
		// An error occurred
		SetError( dwErrorCode);
		return false;
	}

#endif

	m_bCreated = true;
	return true;
}


// OnDestroy
bool XAntiHackModuleGameGuardKor::OnDestroy()
{
	if ( m_bCreated == false)		return false;

#if defined( XANTIHACK)  &&  defined( _KOREA)
	
	CloseNPGameMon();

#endif

	m_bCreated = false;
	return true;
}


// OnCreatedWindow
void XAntiHackModuleGameGuardKor::OnCreatedWindow( HWND hWnd)
{
	if ( m_bCreated == true)
	{

#if defined( XANTIHACK)  &&  defined( _KOREA)
	
		SetHwndToGameMon( hWnd);

#endif

	}
}


// OnPlayerLogin
bool XAntiHackModuleGameGuardKor::OnPlayerLogin( const wchar_t* szID)
{
	if ( m_bCreated == true)
	{

#if defined( XANTIHACK)  &&  defined( _KOREA)

		BOOL bResult = SendUserIDToGameMon( szID);
		if ( bResult == FALSE)
		{
			// An error occurred
			return false;
		}

#endif

	}

	return true;
}


// OnPlayerLogout
bool XAntiHackModuleGameGuardKor::OnPlayerLogout( const wchar_t* szID)
{

	return true;
}


// OnUpdate
bool XAntiHackModuleGameGuardKor::OnUpdate()
{
	if ( m_bCreated == true)
	{

#if defined( XANTIHACK)  &&  defined( _KOREA)

		// Check wake-up time
		{
			DWORD dwCurr = timeGetTime();
			DWORD dwElapsed = dwCurr - m_dwTimer;
			if ( dwElapsed < 5000 /*5sec*/)		return true;

			m_dwTimer = dwCurr;
		}


		// Check GameMon
		DWORD dwErrorCode = CheckNPGameMon();
		if ( dwErrorCode != NPGAMEMON_SUCCESS)
		{
			// An error occurred
			SetError( dwErrorCode);
			return false;
		}

#endif

	}

	return true;
}


// OnCheckAuthority
bool XAntiHackModuleGameGuardKor::OnCheckAuthority( XAntiHackAuthInfo* pAuthInfo)
{
	if ( m_bCreated == true)
	{

#if defined( XANTIHACK)  &&  defined( _KOREA)

		if ( pAuthInfo == NULL)
		{
			// An error occurred
			SetError( NPGAMEMON_ERROR_AUTH_GAMEGUARD);
			return false;
		}

		XAntiHackAuthInfoGameGuardKor* _auth_info = (XAntiHackAuthInfoGameGuardKor*)pAuthInfo;

		BOOL bResult = SendCSAuth2ToGameMon( &_auth_info->m_AuthInfo);
		if ( bResult == FALSE)
		{
			// An error occurred
			SetError( NPGAMEMON_ERROR_AUTH_GAMEGUARD);
			return false;
		}

#endif

	}

	return true;
}


// OnCheckedAuthority
bool XAntiHackModuleGameGuardKor::OnCheckedAuthority( XAntiHackAuthInfo* pAuthInfo)
{
	if ( m_bCreated == true)
	{

#if defined( XANTIHACK)  &&  defined( _KOREA)

		if ( pAuthInfo == NULL)
		{
			// An error occurred
			SetError( NPGAMEMON_ERROR_AUTH_GAMEGUARD);
			return false;
		}

		XAntiHackAuthInfoGameGuardKor* _auth_info = (XAntiHackAuthInfoGameGuardKor*)pAuthInfo;

		// Send authority info to server
		XPOSTCMD1( MC_GG_AUTH_RESPOND, MCommandParameterSingleBlob( &_auth_info->m_AuthInfo, sizeof( GG_AUTH_DATA)));

#endif

	}

	return true;
}


// OnShowErrorMessageBox
void XAntiHackModuleGameGuardKor::OnShowErrorMessageBox()
{
	if ( m_bCreated == true)
	{

#if defined( XANTIHACK)  &&  defined( _KOREA)

	if ( m_dwErrorCode == NULL  ||  m_dwErrorCode == NPGAMEMON_SUCCESS)
		return;

	wchar_t szBuff[ 1024];
	swprintf_s( szBuff, L"%s (errorcode=%u)", m_strErrorMsg.c_str(), m_dwErrorCode);

	MessageBox( NULL, szBuff, L"보안 오류", MB_OK);

#endif

	}
}
