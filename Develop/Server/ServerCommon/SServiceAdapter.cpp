#include "stdafx.h"
#include "SServiceAdapter.h"
#include "SServerApplication.h"
#include "MCommandLine.h"
#include "MLocale.h"

struct ServiceContext
{
	friend SServiceAdapter;

private:
	SServerApplication*		pApp;
	SServerAppType			applicationType;
	wstring					strServerName;
	wstring					strStartParameter;

	SERVICE_STATUS_HANDLE	serviceStatusHandle;
	SERVICE_STATUS			serviceStatus;

	HANDLE					finishEvent;
} g_serviceContext;

namespace {
	SServiceAdapter* g_serviceAdapter = NULL;
}

SServiceAdapter::SServiceAdapter(SServerApplication* pApplication, SServerAppType applicationType, wstring strServerName)
{
	g_serviceContext.pApp = pApplication;
	g_serviceContext.applicationType = applicationType;
	g_serviceContext.strServerName = strServerName;
	g_serviceAdapter = this;
}

SServiceAdapter::~SServiceAdapter()
{
	// do nothing
}

bool SServiceAdapter::Run()
{
	cml2::MCommandLine cmdLine;
	cmdLine.ParseFromCommandLine();

	// 서비스 설치
	if (cmdLine.HasSwitch("install"))
	{
		return Install();
	}

	// 서비스 제거
	if (cmdLine.HasSwitch("uninstall"))
	{
		return Uninstall();
	}
	
	// 실행
	return Start();	
}

bool SServiceAdapter::Install()
{
	// 서비스 관리자 요청 - 관리자 권한이 필요합니다.
	SC_HANDLE hSCManager = ::OpenSCManager(NULL, NULL, SC_MANAGER_CREATE_SERVICE);

	if (hSCManager == 0)
	{		
		mlog3("OpenSCManager failed(%d).\n", ::GetLastError());
		
		return false;
	}


	// 실행파일 위치
	wchar_t szModuleFileName[MAX_PATH];
	::GetModuleFileName(NULL, szModuleFileName, MAX_PATH);


	// 서비스 등록
	SC_HANDLE hService = CreateService(	
										hSCManager,
										g_serviceContext.strServerName.c_str(),
										g_serviceContext.strServerName.c_str(),
										SERVICE_ALL_ACCESS,
										SERVICE_WIN32_OWN_PROCESS | SERVICE_INTERACTIVE_PROCESS,
										SERVICE_DEMAND_START,
										SERVICE_ERROR_NORMAL,
										szModuleFileName,
										NULL,
										NULL,
										NULL,
										NULL,
										NULL
									);

	if (hService == 0)
	{
		mlog3("Create service failed(%d).\n", ::GetLastError());		

		CloseServiceHandle(hSCManager);
		
		return false;
	}


	// 정리
	CloseServiceHandle(hService);
	CloseServiceHandle(hSCManager);

	return true;
}

bool SServiceAdapter::Uninstall()
{
	// 서비스 관리자 요청 - 관리자 관한이 필요합니다.
	SC_HANDLE hSCManager = OpenSCManager(NULL, NULL, SC_MANAGER_CREATE_SERVICE);

	if (hSCManager == 0)
	{
		mlog3("OpenSCManager failed(%d).\n", ::GetLastError());
		return false;
	}


	// 서비스 찾기
	SC_HANDLE hService = OpenService(hSCManager, g_serviceContext.strServerName.c_str(), SERVICE_ALL_ACCESS);

	if (hService == 0)
	{
		mlog3("Open service failed(%d).\n", ::GetLastError());

		CloseServiceHandle(hSCManager);
		
		return false;
	}


	// 서비스 등록해제
	if (!DeleteService(hService))
	{
		mlog3("Delete service failed(%d).\n", ::GetLastError());

		CloseServiceHandle(hService);
		CloseServiceHandle(hSCManager);

		return false;
	}


	// 정리
	CloseServiceHandle(hService);
	CloseServiceHandle(hSCManager);

	return true;
}

bool SServiceAdapter::Start()
{	
	// 서비스 준비
	wchar_t szServiceName[500];
	wcsncpy_s(szServiceName, g_serviceContext.strServerName.c_str(), _TRUNCATE);	// SERVICE_TABLE_ENTRY가 const 문자열을 받지 못한다.

	SERVICE_TABLE_ENTRY serviceTable[] =
	{
		{szServiceName,	RunService},
		{NULL, NULL}
	};
	

	// 서비스 시작
	if (StartServiceCtrlDispatcher(serviceTable))
	{		
		// 서비스가 정상적으로 실행.
		// do nothing
	}
	else
	{
		// 등록된 서비스가 없다. 일반 모드 실행으로 취급한다.
		mlog1("Start application without service mode\n");
		
		if (!BeginApplicationService())
		{
			return false;
		}
	
	}

	// 이벤트 무한대기
	for(;;)	
	{
		DWORD waitResult = WaitForSingleObject(g_serviceContext.finishEvent, INFINITE);

		if (waitResult == WAIT_FAILED || waitResult == WAIT_OBJECT_0)
		{
			break;
		}

		if (waitResult == WAIT_ABANDONED)
		{
			ResetEvent(g_serviceContext.finishEvent);
		}			
	}		

	return true;
}

unsigned int SServiceAdapter::RunApplication(void* pArg)
{
	const wchar_t* szServerName = g_serviceContext.strServerName.c_str();

	// 초기화
	bool isCreated = false;

	switch(g_serviceContext.applicationType)
	{
	case APPTYPE_WINDOW:
		// WinApp는 작동 확인 못해봤습니다. [2010/5/31 praptor]
		isCreated = g_serviceContext.pApp->Create(0, 0, 0, 0, szServerName, szServerName, NULL, NULL);			
		break;
	case APPTYPE_CONSOLE:
		isCreated = g_serviceContext.pApp->CreateConsole(szServerName, g_serviceContext.strStartParameter);
		break;
	default:
		isCreated = g_serviceContext.pApp->CreateConsole(szServerName);
		break;
	}

	// 실행
	if (isCreated)
	{
		MIOCP* const iocp = static_cast<MIOCP*>(pArg);
		g_serviceContext.pApp->Run(iocp);
	}

	SetEvent(g_serviceContext.finishEvent);

	return 0;
}

void SServiceAdapter::RunService(DWORD dwArgc, LPTSTR *lpszArgv)
{
	// 시작 매개변수
	for(DWORD i = 0; i < dwArgc; i++)
	{
		g_serviceContext.strStartParameter += lpszArgv[i];
		g_serviceContext.strStartParameter += L" ";
	}
	
	mlog("service StartParameter: %s\n", MLocale::ConvUTF16ToAnsi(g_serviceContext.strStartParameter.c_str()).c_str());


	// 서비스 상태 초기값 설정
	SERVICE_STATUS&	status = g_serviceContext.serviceStatus;
	SERVICE_STATUS_HANDLE& handle = g_serviceContext.serviceStatusHandle;
	const wchar_t* szServerName = g_serviceContext.strServerName.c_str();
	
	status.dwServiceType				= SERVICE_WIN32; 
	status.dwCurrentState       		= SERVICE_START_PENDING; 
	status.dwControlsAccepted   		= SERVICE_ACCEPT_STOP | SERVICE_ACCEPT_SHUTDOWN | SERVICE_ACCEPT_PAUSE_CONTINUE;
	status.dwWin32ExitCode      		= 0;
	status.dwServiceSpecificExitCode	= 0; 
	status.dwCheckPoint         		= 0; 
	status.dwWaitHint           		= 0; 


	// 서비스 이벤트 핸들러 등록
	handle = ::RegisterServiceCtrlHandler(szServerName, ServiceHandler);

	if (handle == 0) 
	{
		mlog3("Regist service control handler failed(error %d)\n", ::GetLastError());
	} 


	// 어플리케이션 실행
	if (handle != 0 && BeginApplicationService())
	{
		status.dwCurrentState  = SERVICE_RUNNING; 
	}
	else
	{		
		status.dwCurrentState  = SERVICE_STOPPED; 
	}

	::SetServiceStatus(handle, &status);
}

void SServiceAdapter::ServiceHandler(DWORD dwControl)
{
	SERVICE_STATUS&	status = g_serviceContext.serviceStatus;
	SERVICE_STATUS_HANDLE& handle = g_serviceContext.serviceStatusHandle;
	SServerApplication*	pApp = g_serviceContext.pApp;

	switch(dwControl) 
	{
	case SERVICE_CONTROL_PAUSE:
		status.dwCurrentState = SERVICE_PAUSED; 
		break;
	case SERVICE_CONTROL_CONTINUE:
		status.dwCurrentState = SERVICE_RUNNING; 
		break;
	case SERVICE_CONTROL_STOP:
	case SERVICE_CONTROL_SHUTDOWN:
		status.dwWin32ExitCode = 0; 
		status.dwCurrentState  = SERVICE_STOPPED; 
		status.dwCheckPoint    = 0; 
		status.dwWaitHint      = 0;

		pApp->CloseServer();

		break; 
	case SERVICE_CONTROL_INTERROGATE:
		// do nothing
		break;
	default:
		// do nothing
		break;
	};


	SetServiceStatus(handle,  &status);
}

bool SServiceAdapter::StartServiceThread()
{
	return 0 != _beginthreadex(NULL, 0, RunApplication, NULL, 0, NULL);
}

bool SServiceAdapter::BeginApplicationService()
{
	// 서버 애플리케이션 실행 - 별도 쓰레드 생성
	g_serviceContext.finishEvent = CreateEvent(NULL, TRUE, FALSE, NULL);

	if (false == g_serviceAdapter->StartServiceThread())
	{
		mlog3("Create application thread failed.\n");
		return false;
	}
	return true;
}
